//---------------------------------------------------------------------
//
//	File:	TCueChannel.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Cue channel object.  This object holds other cue items and 
//			can be resized and repositioned within the Cue Sheet 
//			document.  
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>
#include <TimeCode.h>

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "MenuMessages.h"
#include "MuseumApp.h"

#include "TPlaybackEngine.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TTimelineView.h"
#include "TCueChannelHeader.h"
#include "THeaderContainerView.h"
#include "TCueView.h"
#include "TVisualCue.h"
#include "TMuteButton.h"
#include "TSoloButton.h"
#include "TExpandButton.h"
#include "TLockButton.h"
#include "TStageWindow.h"
#include "TStageView.h"

#include "TCueChannel.h"

// Cues
#include "TAnimationCue.h"
#include "TAudioCue.h"
#include "TMIDICue.h"
#include "TMovieCue.h"
#include "TPictureCue.h"
#include "TTextCue.h"

// Constants
const int8 kChannelLedgeHeight = 3;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueChannel::TCueChannel(BRect bounds, TCueSheetView *parent, int32 ID) : 
			 BView(bounds, "CueChannel", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_WILL_DRAW)
{
	
	// Save unique ID
	m_ID = ID;
	
	// Set up name
	sprintf(m_Name, "Untitled");
	
	// Save the parent cue sheet view
	m_CueSheet = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Contruct from a BMessage

TCueChannel::TCueChannel(BMessage *data) : BView(data)
{

	m_CueSheet 	= NULL;
	m_Header	= NULL;

	// Create cue item list
	m_CueList = new BList();
	ASSERT(m_CueList);

	//	Find our member variables in the archive		
	data->FindInt32("ID", &m_ID);		
	
	const char *theStr;
	data->FindString("Name", &theStr);		
	strcpy(m_Name, theStr);	
	
	data->FindBool("IsMuted", &m_IsMuted);
	data->FindBool("IsSoloed", &m_IsSoloed);
	data->FindBool("IsExpanded", &m_IsExpanded);
	data->FindBool("IsLocked", &m_IsLocked);
	
	//	Set up environment
	SetDrawingMode(B_OP_COPY);
	SetPenSize(1.0);
		
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueChannel::~TCueChannel()
{
	FreeCueList();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks
//

void TCueChannel::Init()
{			
	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);
		
	// Create cue item list
	m_CueList = new BList();
	ASSERT(m_CueList);
	
	// 	Create header.  This is where the mute and solo buttons, text label and id number are displayed.
	//	The header is actually a child of the CueSheetWindow.  We do this so it remains fixed
	//	when the channels are being scrolled.	
	CreateHeader( m_CueSheet->GetParent()->GetHeaderContainer(), Bounds().Height());
	
	// Set member variables
	m_IsMuted 		= false;
	m_IsSoloed 		= false;
	m_IsExpanded 	= false;
	m_IsLocked		= false;
	
	//	Init tick points
	m_TickPt.Set(-1, -1);
	m_OldTickPt = m_TickPt;
	
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueChannel::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TCueChannel") ) 
		return new TCueChannel(archive); 
		
	return NULL; 
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueChannel::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TCueChannel");
		
		// Add our member variables to the archive		
		data->AddInt32("ID", m_ID);				
		data->AddString("Name", m_Name);		
		data->AddBool("IsMuted", m_IsMuted);
		data->AddBool("IsSoloed", m_IsSoloed);
		data->AddBool("IsExpanded", m_IsExpanded);
		data->AddBool("IsLocked", m_IsLocked);
				
		// Add attached views
		if (deep)
		{		
		
		}		
	}
	
	Looper()->Unlock();	
	
	return myErr;
}

//---------------------------------------------------------------------
//	CreateHeader
//---------------------------------------------------------------------
//
//	Create channel header.  Used when instantiating from an archive
//

void TCueChannel::CreateHeader(THeaderContainerView *container, float height)
{

	// 	Create header.  This is where the mute and solo buttons, text label and id number are displayed.
	//	The header is actually a child of the CueSheetWindow.  We do this so it remains fixed
	//	when the channels are being scrolled.
	BRect bounds = Frame();
	bounds.Set(m_CueSheet->Bounds().left , bounds.top+1, m_CueSheet->Bounds().left+kHeaderWidth, bounds.top+(height+1));
	m_Header = new TCueChannelHeader(bounds, this);
	container->AddChild(m_Header);
	m_Header->Show();	
}
		
#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueChannel::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
		return;
	
	m_CueSheet->DeselectAllCues();
		
	// We have the focus
	MakeFocus(true);
	Window()->Activate(true);
	
	// Draw marching ants.  Select all cues that come in contact with 
	// tracking rect.
	BRect	trackRect(where.x, where.y, where.x, where.y);
	BPoint 	mousePt;
	uint32	buttons;
	
	// Wait a while before tracking.  The user may just want to deselect all cues.
	snooze(100 * 1000);
	GetMouse(&mousePt, &buttons);
	
	if (buttons)
	{
		BeginRectTracking(trackRect, B_TRACK_RECT_CORNER);
		
		do
		{
			snooze(20 * 1000);
			GetMouse(&mousePt, &buttons);	
		}
		while(buttons);
		
		EndRectTracking();
	}
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TCueChannel::MouseUp(BPoint where)
{
	// Do nothing if we are playing
	if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
		return;
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueChannel::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Do nothing if we are playing
	if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
		return;
	
	// Exit if panel is open
	if ( m_CueSheet->GetParent()->IsPanelOpen())
		return;
			
	// Update Timeline.  Clip the location of where.x to be within the bounds
	// of the channel bounds
	if (where.x > Bounds().right)
		where.x = Bounds().right;

	//	Here is how it works.  If the user is dragging a cue icon into the channel,
	//	we send a message to the cue header and it highlights in response.  When the
	//	cue icon leaves the channel, we tell the header to dehighlight.
	switch(code)
	{
		// Turn on highlight
		case B_ENTERED_VIEW:			
			break;
			
		// Ignore	
		case B_INSIDE_VIEW:		
			break;
		
		// Turn off highlight
		case B_EXITED_VIEW:	
			// Send tick off the screen
			where.x = 0;
			break;
	}	
			
	//  If a cue is being dragged, we need to do a special tick update based on left edge of cue
	BPoint mousePt;
	uint32	buttons;
	GetMouse(&mousePt, &buttons);
	
	if (buttons)
	{
		// Check and see if this is a cue drag
		if (static_cast<MuseumApp *>(be_app)->m_IsCueDrag)
		{
			where.x -= static_cast<MuseumApp *>(be_app)->m_CueDragDelta;
		}
	}
	
	BMessage *message = new BMessage(UPDATE_TIMELINE_MSG);
	message->AddPoint("Where", where);
	GetCueSheet()->GetParent()->Lock();
	GetCueSheet()->GetParent()->GetTimeline()->MessageReceived(message);
	GetCueSheet()->GetParent()->Unlock();
	delete message;
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TCueChannel::KeyDown(const char *bytes, int32 numBytes)
{
	m_CueSheet->KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TCueChannel::KeyUp(const char *bytes, int32 numBytes)
{
	m_CueSheet->KeyDown(bytes, numBytes);
}


#pragma mark -
#pragma mark === Scroll Handling ===

//---------------------------------------------------------------------
//	ScrollBy
//---------------------------------------------------------------------
//
//

void TCueChannel::ScrollBy(float horz, float vert)
{
	BView::ScrollBy(horz, vert);
}


#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//

void TCueChannel::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		
		TCueView *theCue;
		
		// These messages come from the BFilePanel that a cue displays when it is ready
		// to load a media element.  Force the message into the cue.  We have to do this
		// because the cue is not yet part of the visual heiracrchy and is not
		// receiving messages.
		case B_OK:
		case B_REFS_RECEIVED:
			{
				// Get cue object from the message				
				if ( !message->FindPointer("TheCue", (void **)&theCue) )
					theCue->MessageReceived(message);
			}
			break;
		
		case B_CANCEL:
			{				
				// Get cue object from the message			
				if ( !message->FindPointer("TheCue", (void **)&theCue) )
				{						
					// Hide file panel
					if (theCue->GetPanel() )
						theCue->HidePanel();
					
					// Delete cue if not instantiated
					if (theCue->IsInstantiated() == false)
						delete theCue;					
				}
			}
			break;
						
		case MUTE_BUTTON_MSG:
			HandleMuteMessage();
			break;
			
		case SOLO_BUTTON_MSG:
			HandleSoloMessage();
			break;
			
		case CHANNEL_EXPAND_MSG:		
			HandleExpandMessage();					
			break;
			
		case LOCK_CHANNEL_MSG:
			HandleLockMessage();
			break;
								
		// Check for dropped messages.  Do nothing if we are playing
		if ( message->WasDropped() )
		{			
			// Was this cue icon dropped?
			case CUE_ICON_DRAG_MSG:						
				{
					// Do nothing if we are playing
					if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
						return;
					
					// Make sure we aren't locked
					if ( m_IsLocked )
						return;
						
					//	Extract drop point
					BPoint where = message->DropPoint();
					ConvertFromScreen(&where);
													
					// Get cue icon type and create the cue
					short cueIconID;
					if ( message->FindInt16("CueIconID", &cueIconID) == B_OK )						
						CreateCue(where, cueIconID);
				}
				break;
			
			// A file is being dragged in from the Browser			
			case BROWSER_ICON_DRAG_MSG:
				{
					// Do nothing if we are playing
					if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
						return;

					// Make sure we aren't locked
					if ( m_IsLocked )
						return;
										
					// Get drop point
					BPoint where = message->DropPoint();
					ConvertFromScreen(&where);

					// Get ref from message and create cue
					entry_ref theRef;
					if ( message->FindRef("FileRef", &theRef) == B_OK )
						CreateCue(where, theRef);	
				}
				break;
				

			// A cue is being moved in the channel
			case CUE_DRAG_MSG:
				{
					// We are done dragging a cue
					static_cast<MuseumApp *>(be_app)->m_IsCueDrag = false;

					// Do nothing if we are playing
					if ( m_CueSheet->GetParent()->GetPlaybackEngine()->IsPlaying() )
						return;

					// Make sure we aren't locked
					if ( m_IsLocked )
						return;

					// Get drop point
					BPoint where = message->DropPoint();
					ConvertFromScreen(&where);
					
					// Find out which cue was moved.  Was it one from this channel
					// or is it being moved in from another?
					TCueView *cueView;
					message->FindPointer("CueView", (void **)&cueView);						
					if (&cueView)
					{
						// Get cue delta
						int32 delta = message->FindInt32("CueDelta");
						where.x -= delta;

						//	Don't allow drag past channel start
						if (where.x < 0)
							where.x = 0;
							
						//	Don't allow drag past channel end
						//float cueWidth = cueView->Bounds().Width();
						//if (where.x > ( Bounds().right - cueWidth) )
						//	where.x = ( Bounds().right - cueWidth);

						//	Calculate insertion time
						uint32 insertTime = PixelsToTime( where.x, GetCurrentTimeFormat(), GetCurrentResolution());
						
						// Check for collisions with other cues
						if (CanInsertCue(cueView, insertTime, true))
						{													
							// Remove cue from old location
							RemoveCue(cueView);
							
							// Insert cue into channel at new location							
							InsertCue(cueView, insertTime);
						}
					}
				}
				break;	
			}				
			
		case CUE_SETUP_MSG:
		case CUE_TIMEBASE_MSG:
		case CUE_ZOOMIN_MSG:
		case CUE_ZOOMOUT_MSG:		
			break;
			
		case CUE_SHOWDURATIONS_MSG:
		case CUE_SHOWSTOPTIMES_MSG:
			SendMessageToAllCues(message);
			break;
		
		//	Pass message to channel
		case CUE_EXPAND_MSG:
			HandleEffectExpandMessage();			
			break;
				
		default:
			BView::MessageReceived(message);						
			break;
	}
}


//---------------------------------------------------------------------
//	HandleSoloMessage
//---------------------------------------------------------------------
//
//	We have recieved a message that the user wants to solo or unsolo 
//	the channel. If the channel is already soloed, unsolo the channel 
//	and mute it.  If is muted or in default mode, set this channel to solo.  
//	To do this we need to unmute the channel if it is muted, set it to solo
// 	and inform the cue sheet to mute all other channels that aren't 
//	also in solo mode.
//

void TCueChannel::HandleSoloMessage()
{
	// Unsolo the channel
	if (m_IsSoloed)
	{
		if (IsOptionKeyDown())
			m_CueSheet->UnsoloAllChannels();
		else
			Unsolo();			
	}
	// Solo the channel
	else
	{
		if (IsOptionKeyDown())
			m_CueSheet->SoloAllChannels();
		else
			Solo();
	}
	
	//	Have stage redraw
	UpdateStage();	

}

//---------------------------------------------------------------------
//	HandleMuteMessage
//---------------------------------------------------------------------
//
//	We have recieved a message that the user wants to mute or unmute 
//	the channel.  If the channel is muted, unmute

void TCueChannel::HandleMuteMessage()
{
	// Unmute the channel
	if (m_IsMuted)
	{
		if (IsOptionKeyDown())
			m_CueSheet->UnmuteAllChannels();
		else
			Unmute();			
	}
	// Mute the channel
	else
	{
		if (IsOptionKeyDown())
			m_CueSheet->MuteAllChannels();
		else
			Mute();
	}
	
	//	Have stage redraw
	UpdateStage();	
}


//---------------------------------------------------------------------
//	HandleExpandMessage
//---------------------------------------------------------------------
//
//	We have received a message that the user wants to expand or contract
//  the channel.  Respond with appropriate action.
//	If we are contracting, we need to notify the channel header and all 
// 	attached cues to contract.  Otherwise, expand the channel, it's header
// 	and all attached cues.  If the option key is down, expand or contract
//	all channels.
//

void TCueChannel::HandleExpandMessage()
{
	// Contract the channel
	if (m_IsExpanded)
	{
		if (IsOptionKeyDown())
			m_CueSheet->ContractAllChannels();
		else
		{
			Contract();			
			m_CueSheet->AdjustChannelPositions(this);
		}
	}
	// Expand the channel
	else
	{
		if (IsOptionKeyDown())
		{
			m_CueSheet->ExpandAllChannels();
		}
		else
		{
			Expand();
			m_CueSheet->AdjustChannelPositions(this);
		}			
	}
	
	// Adjust scroll bars
	static_cast<MuseumApp *>(be_app)->GetCueSheet()->AdjustScrollBars();
}


//---------------------------------------------------------------------
//	HandleLockMessage
//---------------------------------------------------------------------
//
//	We have received a message that the user wants to expand or contract
//  the channel.  Respond with appropriate action.
//	If we are contracting, we need to notify the channel header and all 
// 	attached cues to contract.  Otherwise, expand the channel, it's header
// 	and all attached cues.  If the option key is down, expand or contract
//	all channels.
//

void TCueChannel::HandleLockMessage()
{
	// Unlock the channel
	if (m_IsLocked)
	{
		if (IsOptionKeyDown())
			m_CueSheet->UnlockAllChannels();
		else
		{
			UnlockChannel();			
		}
	}
	// Lock the channel
	else
	{
		if (IsOptionKeyDown())
			m_CueSheet->LockAllChannels();
		else
		{
			LockChannel();
		}
	}

}


//---------------------------------------------------------------------
//	HandleEffectExpandMessage
//---------------------------------------------------------------------
//
//	We have received a message that the user wants to expand or contract
//  the cue effect tray.  Respond with appropriate action.
//

void TCueChannel::HandleEffectExpandMessage()
{
	//	Hide all cue effect trays
	if (m_IsExpanded)
	{				
		bool collapsed = false;
		
		for ( int32 index = 0; index < m_CueList->CountItems(); index++)
		{
			TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
						
			if (theCue->IsEffectsVisible())
			{
				//	Resize channel and header to accomodate effects channel
				if (collapsed == false)
				{
					ResizeTo(Bounds().Width(), kChannelMaxHeight);										
					m_Header->ResizeTo( m_Header->Bounds().Width(), kChannelMaxHeight);	
					collapsed = true;
				}				
				theCue->HideEffects();
			}
			else
			{
				//	Resize channel and header to accomodate effects channel				
				if (collapsed == false)
				{
					ResizeTo(Bounds().Width(), Bounds().Height() + kEffectsTrayHeight);
					m_Header->ResizeTo( m_Header->Bounds().Width(), m_Header->Bounds().Height() + kEffectsTrayHeight);
					collapsed = true;
				}
				theCue->ShowEffects();
			}
		}

		//	Adjust positions of other channels
		m_CueSheet->AdjustChannelPositions(this);
	}

	//	Redraw
	Invalidate();
	m_Header->Invalidate();

	// Adjust scroll bars
	static_cast<MuseumApp *>(be_app)->GetCueSheet()->AdjustScrollBars();
}


#pragma mark -
#pragma mark === Cue List Handling ====

//---------------------------------------------------------------------
//	AddCueToList
//---------------------------------------------------------------------
//
//	Add the cue to the list in it's proper position.  We want the list
//	to be ordered sequentially from start to end times
//

void TCueChannel::AddCueToList(TCueView *insertCue)
{
	
	TCueView	*theCue;
	BPoint		endPt;
	
	// Get total items in list
	int32 totalItems = m_CueList->CountItems();

	// If totalItems == 0, it is the first item in the list.  Just add it.
	if (totalItems == 0)
	{
		m_CueList->AddItem(insertCue, 0);
		return;
	}
	// Iterate through the list of cues
	else
	{
		for (int32 index = 0; index < totalItems; index++)
		{
			theCue = (TCueView *)m_CueList->ItemAt(index);
			if(theCue)
			{			
				// Check and see if the insert cues start time is before the
				// start time of the found cue.  If so, we have our insertion point
				int32 insertStartTime = insertCue->StartTime();
				int32 cueStartTime = theCue->StartTime();
												
				if ( insertStartTime > cueStartTime)
				{
					// Are we at the end of the list?  If so, add the cue to the end.
					// If we aren't at the end, keep on iterating through the list					
					if (index+1 == totalItems)
					{
						m_CueList->AddItem(insertCue);
						return;
					}
				}				
				else
				{
					// Insert cue into list
					//m_CueList->AddItem(insertCue, index-1);
					m_CueList->AddItem(insertCue, index);
					return;
				}
			}	
		}	
	}
}


//---------------------------------------------------------------------
//	RemoveCueFromList
//---------------------------------------------------------------------
//
//	Remove the cue object from the cue list
//
void TCueChannel::RemoveCueFromList(TCueView *cueView)
{
	if (m_CueList->HasItem(cueView))
	{
		bool retVal;
		
		int32 index = m_CueList->IndexOf(cueView);
		retVal = m_CueList->RemoveItem(cueView);
	}
}

//---------------------------------------------------------------------
//	FreeCueList
//---------------------------------------------------------------------
//
//	Deallocate cue list
//
#pragma warn_possunwant off

void TCueChannel::FreeCueList()
{
	void *anItem; 
   	
   	for ( int32 i = m_CueList->CountItems(); anItem = m_CueList->ItemAt(i); i++ ) 
   	{
		delete anItem; 
	}
   	
   	delete m_CueList;

} 
#pragma warn_possunwant reset


//---------------------------------------------------------------------
//	GetCueAtTime
//---------------------------------------------------------------------
//
//	Locate a cue in the channel at specified time
//

TCueView *TCueChannel::GetCueAtTime(uint32 theTime)
{
   	
   	for ( int32 index = 0; index < m_CueList->CountItems(); index++ ) 
   	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
		
		// We have a cue
		if (theCue)
		{
			const uint32 startTime = theCue->StartTime();
			const uint32 endTime	= startTime + theCue->Duration();
			if ( theTime >= startTime && theTime <= endTime )
			{				
				return theCue;
			}
		}
	}
	
	return NULL;
} 


//---------------------------------------------------------------------
//	GetVisualCueAtTime
//---------------------------------------------------------------------
//
//	Locate a visual cue in the channel at specified time
//

TVisualCue *TCueChannel::GetVisualCueAtTime(uint32 theTime)
{
   	
   	for ( int32 index = 0; index < m_CueList->CountItems(); index++ ) 
   	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
		
		// We have a cue
		if (theCue)
		{
			if (theCue->IsVisible() == true)
			{
				const uint32 startTime = theCue->StartTime();
				const uint32 endTime	= startTime + theCue->Duration();
				if ( theTime >= startTime && theTime <= endTime )
				{				
					return ((TVisualCue *)theCue);
				}
			}
		}
	}
	
	return NULL;
} 


#pragma mark -
#pragma mark === Cue Creation ===

//---------------------------------------------------------------------
//	CreateCue
//---------------------------------------------------------------------
//
//	Create a new cue based on cueIconID
//

void TCueChannel::CreateCue(BPoint point, int16 cueIconID)
{
	// Check for cue collision
	
	// Set up bounds
	BRect bounds 	= Bounds();
	bounds.left 	= point.x;
	bounds.top 		+= kCueInset;	
	// Set default duration to be one second in milliseconds
	bounds.right 	=  bounds.left + TimeToPixels(1000, GetTimeFormat(), GetResolution());
	bounds.bottom 	-= kCueInset;	
		
	// Deselect other cues
	m_CueSheet->DeselectAllCues();

	// Get insert time based on point.x
	uint32 insertTime = PixelsToTime( point.x, GetCurrentTimeFormat(), GetCurrentResolution());
			
	// Create cue based on type and add it to the channel's cue list at the correct position		
	TCueView *theCue;
	
	switch(cueIconID)
	{				
		case kActionCue:
			//theCue = new TActionCue(cueIconID, this, bounds, point, insertTime);	
			break;
			
		case kButtonCue:
			//theCue = new TButtonCue(cueIconID, this, bounds, point, insertTime);			
			break;
					
		case kCDCue:
			//theCue = new TCDCue(cueIconID, this, bounds, point, insertTime);			
			break;		
			
		case kControlCue:
			//theCue = new TControlCue(cueIconID, this, bounds, point, insertTime);			
			break;
					
		case kMarkerCue:
			//theCue = new TMarkerCue(cueIconID, this, bounds, point, insertTime);			
			break;
					
		case kPauseCue:
			//theCue = new TPauseCue(cueIconID, this, bounds, point, insertTime);			
			break;
					
		case kVideoCue:
			//theCue = new TVideoCue(cueIconID, this, bounds, point, insertTime);			
			break;		

		case kAnimationCue:
			theCue = new TAnimationCue(cueIconID, this, bounds, insertTime);			
			break;
			
		case kAudioCue:
			theCue = new TAudioCue(cueIconID, this, bounds, insertTime);			
			break;
			
		case kMIDICue:
			theCue = new TMIDICue(cueIconID, this, bounds, insertTime);			
			break;
			
		case kMovieCue:
			theCue = new TMovieCue(cueIconID, this, bounds, insertTime);			
			break;
			
		case kPictureCue:
			theCue = new TPictureCue(cueIconID, this, bounds, insertTime);			
			break;
			
		case kSlideCue:
			//theCue = new TSlideCue(cueIconID, this, bounds, point, insertTime);			
			break;
			
		case kTextCue:
			theCue = new TTextCue(cueIconID, this, bounds, insertTime);							
			break;
			
		default:
			break;						
	}		
}


//---------------------------------------------------------------------
//	CreateCue
//---------------------------------------------------------------------
//
//	Create a new cue based on an entry_ref
//

void TCueChannel::CreateCue(BPoint point, entry_ref &theRef)
{
	
	// Set up bounds
	BRect bounds 	= Bounds();
	bounds.left 	+= point.x;
	bounds.top 		+= kCueInset;	
	// Set default duration to be one second
	bounds.right 	=  bounds.left + TimeToPixels(1000, GetTimeFormat(), GetResolution());
	bounds.bottom 	-= kCueInset;	
		
	// Create BNode from entry ref
	BNode theNode(&theRef);
	if (theNode.InitCheck() != B_NO_ERROR)
		return;
		
	// Create BNodeInfo from our node
	BNodeInfo nodeInfo(&theNode);
	if (nodeInfo.InitCheck() != B_NO_ERROR)
		return;
	
	// Deselect other cues
	m_CueSheet->DeselectAllCues();

	// Get insert time based on point.x
	uint32 insertTime = PixelsToTime( point.x, GetCurrentTimeFormat(), GetCurrentResolution());
			
	// Create cue based on type and add it to the channel's cue list at the correct position		
	TCueView *theCue;
	
	//
	//	Determine type of cue to create
	//
	
	// Do we create a PictureCue?
	if (IsImage(nodeInfo))
		theCue = new TPictureCue(theRef, kPictureCue, this, bounds, insertTime);
	// AudioCue?
	else if (IsAudio(nodeInfo))
		theCue = new TAudioCue(theRef, kAudioCue, this, bounds, insertTime);
	// MIDI Cue
	else if (IsMIDI(nodeInfo))		
		theCue = new TMIDICue(theRef, kMIDICue, this, bounds, insertTime);
	// Video cue
	else if (IsVideo(nodeInfo))		
		theCue = new TMovieCue(theRef, kMovieCue, this, bounds, insertTime);
		
	/*
	switch(cueIconID)
	{				
		case kAnimationCue:
			theCue = new TAnimationCue(cueIconID, this, bounds, point, insertTime);			
			break;
			
		case kAudioCue:
			theCue = new TAudioCue(cueIconID, this, bounds, point, insertTime);			
			break;
			
		case kMIDICue:
			theCue = new TMIDICue(cueIconID, this, bounds, point, insertTime);			
			break;
			
		case kMovieCue:
			theCue = new TMovieCue(cueIconID, this, bounds, point, insertTime);			
			break;
			
		case kPictureCue:
			theCue = new TPictureCue(cueIconID, this, bounds, point, insertTime);			
			break;
						
		case kTextCue:
			theCue = new TTextCue(cueIconID, this, bounds, point, insertTime);							
			break;

		default:
			break;						
	}	
	*/	
}


//---------------------------------------------------------------------
//	CanInsertCue
//---------------------------------------------------------------------
//
//	Check for cue position conflicts with other cues in the channel.
//	To do this we need to check the start time and duration.  We must check
// 	for any cues that will conflict with the desired cue to be inserted.
//	If there is a conflict, we can do a couple of things:  Inform the user
//	that there is a conflict and cancel the operation or shift the affected
// 	cues in time to allow the insert.  For now, lets just tell the user.
//

bool TCueChannel::CanInsertCue(TCueView *insertCue, uint32 insertStartTime, bool showAlert)
{
	Looper()->Lock();
	
	//	Calculate cue startTime and endTime
	uint32 insertEndTime = insertStartTime + insertCue->Duration();

	// Iterate through the list of cues
	for (int32 i = 0; i < m_CueList->CountItems(); i++)
	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(i);
		
		uint32 cueStartTime = theCue->StartTime();
		uint32 cueEndTime = theCue->StartTime() + theCue->Duration();
		
		if(theCue)
		{			
			//	Do our times overlap?
			if ( (insertStartTime >= cueStartTime) && (insertStartTime <= cueEndTime) )
			{
				// We have an intersection.  This can mean a couple of things.  One is that the cue has 
				// overlapped in it's own space.  We handle this as follows...
				if (theCue == insertCue)
				{					
					TCueView *neighborCue;
					
					// Make sure we aren't overlapping any neighbors on the left
					neighborCue = GetNeighborLeft(insertCue);
					if (neighborCue)
					{			
						// Check and see if start time of cue ovelaps onto ending time
						// of neighboring cue
						uint32 neighborEndTime = neighborCue->StartTime() + neighborCue->Duration();
						if ( insertStartTime < neighborEndTime )
						{
							if (showAlert)
								CueBoundsAlert();
								
							Looper()->Unlock();
							return false;
						}
					}
					
					// Make sure we aren't overlapping any neighbors on the right
					neighborCue = GetNeighborRight(insertCue);
					if (neighborCue)
					{
						// Check and see if ending time of cue ovelaps onto start time of neighboring cue
						uint32 neighborStartTime = neighborCue->StartTime();
						
						uint32	neighborStartClipTime =  ClipTime(neighborStartTime, GetCurrentTimeFormat(), GetCurrentResolution());
						if (  insertEndTime > neighborStartClipTime )
						{
							if (showAlert)
								CueBoundsAlert();
							Looper()->Unlock();
							return false;
						}
						
					}
					Looper()->Unlock();									
					return true;	
				}
				else
				{
					if (showAlert)
						CueBoundsAlert();
					Looper()->Unlock();
					return false;
				}
			}
		}	
	}	
	
	Looper()->Unlock();
	return true;
}

//
//
// ABH missing function
//
//
bool TCueChannel::CanInsertCue(TCueView *insertCue, BPoint insertPoint, bool showAlert){
	// ABH should check insertPoint validity - fix this
	printf("CanInsertCue: called with insertPoint, fix this\n");
//	return CanInsertCue(insertCue, insertStartTime, showalert);	
	return (bool)1;
}





//---------------------------------------------------------------------
//	AddCue
//---------------------------------------------------------------------
//
//	Routines used to add cues when cues are read in from a file
//	in a batch.  We already know info about the cues that preempt
//	the sanity checks the other routines provide
//

void TCueChannel::AddCue(TCueView *cueView)
{
	
	// Set the cue to ownership by this channel	
	AddChild(cueView);
	cueView->SetChannel(this);
	
	// Get insert point
	BPoint movePt;
	movePt.x = TimeToPixels(cueView->StartTime(), GetTimeFormat(), GetResolution());
	movePt.y = Bounds().top + kCueInset;	
	cueView->MoveTo(movePt);
							
	// Rezize to channel dimensions
	int32 height = Frame().Height() - kCueInset*2;
	cueView->ResizeTo( cueView->Bounds().Width(), height);					
				
	// Add to the channel's cue list
	AddCueToList(cueView);
				
	// Make it visible
	cueView->Show();			
}


//---------------------------------------------------------------------
//	InsertCue
//---------------------------------------------------------------------
//
//	Insert cue at location and time specified.  Insert it into cue list
//	and adjust its position
//

void TCueChannel::InsertCue(TCueView *cueView, uint32 time)
{	
	Looper()->Lock();
	
	const BRect bounds = Bounds();
	
	BPoint movePt;
	movePt.y = bounds.top + kCueInset;
		
	//	Get time position in pixels
	uint32 position = TimeToPixels(time, GetCurrentTimeFormat(), GetCurrentResolution());
	
	//	Possible bug: Offset move point to take into account scrolled view
	movePt.x = position;
	movePt.x -= bounds.left;		

	//	Update internal start time
	cueView->SetStartTime(time);
		
	//	Set ownership to this channel and add as child of view
	if ( this != cueView->GetChannel() || cueView->Parent() == NULL)
	{
		AddChild(cueView);
		cueView->SetChannel(this);
		
		//	Resize to channel dimensions
		int32 height = Frame().Height() - kCueInset*2;
		cueView->ResizeTo( cueView->Bounds().Width(), height);					
	}
	
	//	Move to insert point	
	cueView->MoveTo(movePt);
				
	//	Reorder and add to the channel's cue list
	AddCueToList(cueView);
				
	//	Update the cue
	cueView->ChannelInsert();
	
	//	Make it visible and set focus
	cueView->Show();
	cueView->MakeFocus(true);
	
	// 	Check for channel expanded or contracted state.  Show or hide cue controls
	//	depending on state
	if ( m_IsExpanded )
		cueView->Expand();
	else
		cueView->Contract();	
		
	// Update stage appearence
	BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
	theMessage->AddInt32("TheTime", GetCurrentTime());
	static_cast<MuseumApp *>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView());
	delete theMessage;		
	
	Looper()->Unlock();
}

// ABH - missing function
// InsertCue
//
void TCueChannel::InsertCue(TCueView *cueView, BPoint insertPoint, uint32 insertTime){
	
	Looper()->Lock();
	
	const BRect bounds = Bounds();
	
	BPoint movePt;
	movePt.y = bounds.top + kCueInset;
		
	//	Get time position in pixels
	uint32 position = TimeToPixels((long unsigned int)time, GetCurrentTimeFormat(), GetCurrentResolution());
	
	//	Possible bug: Offset move point to take into account scrolled view
	movePt.x = position;
	movePt.x -= bounds.left;		

	//	Update internal start time
	cueView->SetStartTime(insertTime);
		
	//	Set ownership to this channel and add as child of view
	if ( this != cueView->GetChannel() || cueView->Parent() == NULL)
	{
		AddChild(cueView);
		cueView->SetChannel(this);
		
		//	Resize to channel dimensions
		int32 height = Frame().Height() - kCueInset*2;
		cueView->ResizeTo( cueView->Bounds().Width(), height);					
	}
	
	//	Move to insert point	
	cueView->MoveTo(movePt);
				
	//	Reorder and add to the channel's cue list
	AddCueToList(cueView);
				
	//	Update the cue
	cueView->ChannelInsert();
	
	//	Make it visible and set focus
	cueView->Show();
	cueView->MakeFocus(true);
	
	// 	Check for channel expanded or contracted state.  Show or hide cue controls
	//	depending on state
	if ( m_IsExpanded )
		cueView->Expand();
	else
		cueView->Contract();	
		
	// Update stage appearence
	BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
	theMessage->AddInt32("TheTime", GetCurrentTime());
	static_cast<MuseumApp *>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView());
	delete theMessage;		
					
	
	Looper()->Unlock();
}






//---------------------------------------------------------------------
//	RemoveCue
//---------------------------------------------------------------------
//
//	Remove cue from parent channel and cue list.
//

void TCueChannel::RemoveCue(TCueView *cueView)
{
	if ( this == cueView->GetChannel())
	{
		//cueView->Hide();
		//theChannel->RemoveChild(cueView);	
				
		cueView->GetChannel()->RemoveCueFromList(cueView);
		
		//	Possible bug: Reset cue bounds to zero
		cueView->ScrollTo(0, 0);
	}
	else
	{
		// Remove view from parent channel
		cueView->Hide();
		cueView->GetChannel()->RemoveChild(cueView);	
		cueView->GetChannel()->RemoveCueFromList(cueView);
		
		//	Possible bug: Reset cue bounds to zero
		cueView->ScrollTo(0, 0);
	}
}


//---------------------------------------------------------------------
//	GetNeighborRight
//---------------------------------------------------------------------
//
//	If the cue has a neighbor to the right, return true.  
//

TCueView  *TCueChannel::GetNeighborRight(TCueView *theView)
{
	TCueView 	*theCue;
		
	// Get the cues index into the cue list
	int32 index = m_CueList->IndexOf(theView);
	
	// If the index is negative, the item is not in the list
	if (index < 0 )
		return NULL;
		
	// If the index is equal to the total number of items in the list, 
	//	it is the last cue in the list and has no neighbor to the right
	int32 numItems = m_CueList->CountItems();
	if (index == numItems)
		return NULL;
		
	// Any other cue has a neighbor directly to the right of it
	theCue = (TCueView *)m_CueList->ItemAt(index+1);
	
	return (theCue);
	
}


//---------------------------------------------------------------------
//	GetNeighborLeft
//---------------------------------------------------------------------
//
//	If the cues has a neighbor to the left, return it. 
//

TCueView  *TCueChannel::GetNeighborLeft(TCueView *theView)
{
	TCueView 	*theCue;
		
	// Get the cues index into the cue list
	int32 index = m_CueList->IndexOf(theView);
	
	// If the index is negative, the item is not in the list
	if (index < 0 )
		return NULL;
		
	// If the index is zero, it is the first cue in the list and has no 
	// neighbor to the left
	if (index == 0)
		return NULL;
		
	// Any other cue has a neighbor directly to the left of it
	theCue = (TCueView *)m_CueList->ItemAt(index-1);
	
	return (theCue);
	
}



//---------------------------------------------------------------------
//	GetNeighborRightPoint
//---------------------------------------------------------------------
//
//	If the cue has a neighbor to the right, return it.  
//

bool TCueChannel::GetNeighborRightPoint(TCueView *theView, BPoint *where)
{
	TCueView 	*theCue;
	BPoint		rightPt;
	
	// Get right neighbor
	theCue = GetNeighborRight(theView);
	
	if (theCue)
	{
		where->Set( theCue->Frame().left + Bounds().left, theCue->Frame().top ); 
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------------------------------------------------
//	GetNeighborLeftPoint
//---------------------------------------------------------------------
//
//	If the cues has a neighbor to the left, return true.  Also return
//	its left point position in the where argument.		
//

bool TCueChannel::GetNeighborLeftPoint(TCueView *theView, BPoint *where)
{
	TCueView 	*theCue;
	BPoint		leftPt;
	
	// Get left neighbor
	theCue = GetNeighborLeft(theView);
	
	if (theCue)
	{
		where->Set( theCue->Frame().right + Bounds().left, theCue->Frame().top ); 
		return true;
	}
	else
	{
		return false;
	}
}


#pragma mark -
#pragma mark === Cue Handling ===


//---------------------------------------------------------------------
//	SendMessageToAllCues
//---------------------------------------------------------------------
//
//	Send message to all cues
//

void TCueChannel::SendMessageToAllCues(BMessage *theMessage)
{
	TCueView *theView;
	
	for( int32 i = 0; i < m_CueList->CountItems(); i++)
	{
		theView = (TCueView *)m_CueList->ItemAt(i);
		
		if(theView)
		{
			theView->MessageReceived(theMessage);
		}
	}
}

//---------------------------------------------------------------------
//	SendMessageToAllSelectedCues
//---------------------------------------------------------------------
//
//	Send message to all selected cues
//

void TCueChannel::SendMessageToAllSelectedCues(BMessage *theMessage)
{
	Looper()->Lock();
	
	if ( HasSelectedCues() )
	{
		//  Create alist of all selected cues
		BList *cueList = GetSelectedCues();
		
		// Send message to all cues in list
		if (cueList)
		{		
			for (int32 index = 0; index < cueList->CountItems(); index++)
			{
				TCueView *theCue = (TCueView *)cueList->ItemAt(index);
				
				if (theCue)
				{
					//	Send message to cue
					theCue->MessageReceived(theMessage);
				}
			}
			
			// All done.  Delete the cue list
			delete cueList;
		}	
	}
	
	Looper()->Unlock();
}




//---------------------------------------------------------------------
//	DeleteSelectedCues
//---------------------------------------------------------------------
//
//	Delete all selected cues
//

void TCueChannel::DeleteSelectedCues()
{
	Looper()->Lock();
	
	if ( HasSelectedCues() )
	{
		//  Create alist of all selected cues
		BList *cueList = GetSelectedCues();
		
		// Delete all cues in list
		if (cueList)
		{		
			for (int32 index = 0; index < cueList->CountItems(); index++)
			{
				TCueView *theCue = (TCueView *)cueList->ItemAt(index);
				
				if (theCue)
				{
					// Remove cue and delete it	
					theCue->Hide();
					RemoveChild(theCue);	
					RemoveCueFromList(theCue);	
					delete theCue;
				}
			}
			
			// All done.  Delete the cue list
			delete cueList;
			
			//	Update stage appearence
			UpdateStage();			
		}	
	}
		
	Looper()->Unlock();
}


//---------------------------------------------------------------------
//	NudgeSelectedCues
//---------------------------------------------------------------------
//
//	Nudge all selected cues left, earlier in time
//

void TCueChannel::NudgeSelectedCues(bool nudgeLeft)
{
	//	Are cue selected?
	if ( HasSelectedCues() )
	{
		//  Create alist of all selected cues
		BList *cueList = GetSelectedCues();
		
		// Nudge all all cues in list on resolution click earlier
		if (cueList)
		{		
			for (int32 index = 0; index < cueList->CountItems(); index++)
			{
				TCueView *theCue = (TCueView *)cueList->ItemAt(index);
				
				if (theCue)
				{
					uint32 insertTime 	= theCue->StartTime();
					uint32 unitMSec		= GetUnitMSec( GetCurrentTimeFormat(), GetCurrentResolution() );
					float  increment 	= unitMSec / GetFPSValue(GetCurrentTimeFormat());
					
					// Get new cue position.  Clip it to the nearest resolution
					if (nudgeLeft)
					{
						//	Decrement start time
						insertTime -= increment;
																	
						//  Don't move ahead of the cue sheet start time
						if (insertTime < m_CueSheet->StartTime())
							insertTime = m_CueSheet->StartTime();
					}
					else
					{
						//	Increment start time
						insertTime += increment;

						//  Don't move after the cue sheet end time
						if (insertTime > (m_CueSheet->StartTime() + m_CueSheet->Duration()) - theCue->Duration())
							insertTime = (m_CueSheet->StartTime() + m_CueSheet->Duration()) - theCue->Duration();
					}
					
					// Check for collisions with other cues
					if (CanInsertCue(theCue, insertTime, false))
					{													
						// Remove cue from old location
						RemoveCue(theCue);
						
						// Insert cue into channel at new location						
						InsertCue(theCue, insertTime);
					}
					// Cue cannot be nudged anymore.  Deselect it.
					//else
					//{
					//	theCue->Deselect();	
					//}					
				}
			}
			
			// All done.  Delete the cue list
			delete cueList;
		}	
	}
}

#pragma mark -
#pragma mark === Time and Resolution Handling ===


//---------------------------------------------------------------------
//	GetTimeFormat
//---------------------------------------------------------------------
//
//	Return cue sheets m_TimeFormat
//

timecode_type TCueChannel::GetTimeFormat()
{
	return m_CueSheet->GetTimeFormat();
}


//---------------------------------------------------------------------
//	GetResolution
//---------------------------------------------------------------------
//
//	Return cue sheets m_Resolution
//

short TCueChannel::GetResolution()
{
	return m_CueSheet->GetResolution();
}

	
//---------------------------------------------------------------------
//	TimeFormatChanged
//---------------------------------------------------------------------
//
//	The time format has been changed resize channel according to the 
//	new cue sheet format and inform all attached cues to resize as well
//

void TCueChannel::TimeFormatChanged()
{
	// Resize channel according to new format
	
	// Resize all cues
	//ResizeCuesToTimeFormat();
}


//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//	The cue sheet has a new resolution.  Resize channel to fit new resolution
//	and inform all attached cues as well.

void TCueChannel::ResolutionChanged(int32 resizePixels)
{		
	// Resize all cues
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
		
		if(theCue)
		{
			theCue->ResolutionChanged(m_CueSheet->GetTimeFormat(), m_CueSheet->GetResolution());
		}
	}
	
	// Force redraw
	Invalidate();
}

#pragma mark -
#pragma mark === Selection Routines ===


//---------------------------------------------------------------------
//	HasCues
//---------------------------------------------------------------------
//
//	Check to see if channel has any cues in it
//

bool TCueChannel::HasCues()
{
	TCueView *theView;
	
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		theView = (TCueView *)m_CueList->ItemAt(index);
		
		if(theView)
			return true;			
	}
	
	return false;
}


//---------------------------------------------------------------------
//	SelectAllCues
//---------------------------------------------------------------------
//
//	Select every cue in channel
//

void TCueChannel::SelectAllCues()
{
	TCueView *theView;
	
	for( int32 i = 0; i < m_CueList->CountItems(); i++)
	{
		theView = (TCueView *)m_CueList->ItemAt(i);
		
		if(theView)
		{
			// Check state to avoid uneccessary redraw
			if(!theView->Selected())
			{				
				theView->Select();			
			}
		}
	}
}


//---------------------------------------------------------------------
//	DeselectAllCues
//---------------------------------------------------------------------
//
//	Deselect every cue in channel
//

void TCueChannel::DeselectAllCues()
{
	TCueView *theView;
	
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		theView = (TCueView *)m_CueList->ItemAt(index);
		
		if(theView)
		{
			// Check state to avoid uneccessary redraw
			if(theView->Selected())
			{
				theView->Deselect();							
			}
		}
	}
}


//---------------------------------------------------------------------
//	DeselectAllCues
//---------------------------------------------------------------------
//
//	Deselect every cue in channel, except cue passed in
//

void TCueChannel::DeselectAllCues(TCueView *theCue)
{
	TCueView *theView;
	
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		theView = (TCueView *)m_CueList->ItemAt(index);
		
		if(theView != theCue)
		{
			// Check state to avoid uneccessary redraw
			if(theView->Selected())
			{
				theView->Deselect();							
			}
		}
	}
}

//---------------------------------------------------------------------
//	HasSelectedCues
//---------------------------------------------------------------------
//
//	Check to see if channel has any selected cues in it
//

bool TCueChannel::HasSelectedCues()
{
	TCueView *theView;
	
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		theView = (TCueView *)m_CueList->ItemAt(index);
		
		if(theView)
		{
			if(theView->Selected())
				return true;
		}
	}
	
	return false;
}


//---------------------------------------------------------------------
//	GetSelectedCues
//---------------------------------------------------------------------
//
//	Return a BList of all selected cues in a channel.  The caller is
//	responsible for freeing the list.
//

BList *TCueChannel::GetSelectedCues()
{
	BList *cueList = new BList();
	
	TCueView *theView;
	
	for( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		theView = (TCueView *)m_CueList->ItemAt(index);
		
		if(theView)
		{
			if(theView->Selected())
				cueList->AddItem(theView);
		}
	}
	
	return cueList;
}



#pragma mark -
#pragma mark === Channel Utilities ===

//---------------------------------------------------------------------
//	SetChannelName
//---------------------------------------------------------------------
//
//	Update channel name 
//

void TCueChannel::SetChannelName(const char *theName)
{
	// Copy the new name into our member variable
	strcpy(m_Name, theName);
	
	// Update the Channel Header's StringView
	m_Header->UpdateChannelName();	
}


//---------------------------------------------------------------------
//	SetChannelID
//---------------------------------------------------------------------
//
//	Update channel ID 
//

void TCueChannel::SetChannelID(short ID)
{
	// Copy the new name into our member variable
	m_ID = ID;
	
	// Update the Channel Header's StringView
	m_Header->UpdateChannelName();	
}


//---------------------------------------------------------------------
//	Solo
//---------------------------------------------------------------------
//
//	We are soloing the channel.  
//

void TCueChannel::Solo()
{
	if (!m_IsSoloed)
	{
		if (m_IsMuted)
		{
			// Unmute the channel
			m_IsMuted = false;
			m_Header->GetMuteButton()->Invalidate();		
		}
				
		// Solo the channel
		m_IsSoloed = true;
		m_Header->GetSoloButton()->Invalidate();
		
		// Mute all other channels that aren't also soloed
		m_CueSheet->MuteAllUnsoloedChannels();						
	}
}


//---------------------------------------------------------------------
//	Unsolo
//---------------------------------------------------------------------
//
//	We are unsoloing the channel.  
//

void TCueChannel::Unsolo()
{
	if (m_IsSoloed)
	{
		// Unsolo the channel
		m_IsSoloed = false;
		m_Header->GetSoloButton()->Invalidate();		
		
		// Mute the channel if other channels are soloed
		if ( m_CueSheet->AreOtherChannelsSoloed(this) )
			Mute();
	}
}


//---------------------------------------------------------------------
//	Mute
//---------------------------------------------------------------------
//
//	We are muting the channel.  
//

void TCueChannel::Mute()
{
	if (!m_IsMuted)
	{
		// Mute the channel
		m_IsMuted = true;
		m_Header->GetMuteButton()->Draw( m_Header->GetMuteButton()->Bounds() );
				
		// Unsolo the channel
		if (m_IsSoloed)
		{
			m_IsSoloed = false;
			m_Header->GetSoloButton()->Draw( m_Header->GetSoloButton()->Bounds() );
		}
		
		// Mute all cues in channel
		for (int32 index = 0; index < m_CueList->CountItems(); index++)
		{	
			TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
			
			if (theCue)
				theCue->Mute();
		}
	}
}


//---------------------------------------------------------------------
//	Unmute
//---------------------------------------------------------------------
//
//	We are unmuting the channel.  
//

void TCueChannel::Unmute()
{
	if (m_IsMuted)
	{
		// Unmute the channel
		m_IsMuted = false;
		m_Header->GetMuteButton()->Draw( m_Header->GetMuteButton()->Bounds() );
				
/*		// Unsolo the channel
		if (m_IsSoloed)
		{
			m_IsSoloed = false;
			m_Header->GetSoloButton()->Invalidate();		
		}*/
		
		// Unmute all cues in channel
		for (int32 index = 0; index < m_CueList->CountItems(); index++)
		{	
			TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
			
			if (theCue)
				theCue->Unmute();
		}
	}
}


//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
//	We are shrinking the height of the channel.  Update the channel,
//	header and cues to new height
//

void TCueChannel::Contract(bool force)
{
	if (m_IsExpanded == true || force == true)
	{
		m_IsExpanded = false;
		// For now, set to default contracted height
		ResizeTo( Bounds().Width(), kChannelMinHeight);		
		m_Header->ResizeTo( m_Header->Bounds().Width(), kChannelMinHeight);
		m_Header->Invalidate();		
						
		// Inform cues of contracted state
		ContractAllCues();
		UpdateAllResizeZones();
		RedrawAllCues();
		
		m_Header->GetExpandButton()->Invalidate();
		
		// Force redraw
		Invalidate();
	}
}


//---------------------------------------------------------------------
//	Expand
//---------------------------------------------------------------------
//
//	We are expanding the height of the channel.  Update the channel,
//	header and cues to new height
//

void TCueChannel::Expand(bool force)
{
	if (m_IsExpanded == false || force == true)
	{
		m_IsExpanded = true;
		
		// For now, set to default expanded height
		// gzr: to do... If the user has set up a custom
		// size, restore it to that
		
		// Resize channel, header and all attached cues
		ResizeTo( Bounds().Width(), kChannelMaxHeight);	
		m_Header->ResizeTo( m_Header->Bounds().Width(), kChannelMaxHeight);	
		m_Header->Invalidate();	
							
		// Inform cues of expanded state
		ExpandAllCues();					
		UpdateAllResizeZones();
		RedrawAllCues();	
			
		m_Header->GetExpandButton()->Invalidate();
		
		// Force redraw
		Invalidate();
	}
}


//---------------------------------------------------------------------
//	ContractAllCues
//---------------------------------------------------------------------
//
//

void TCueChannel::ContractAllCues()
{
	for ( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
		
		if (theCue)
			theCue->Contract();
	}	
}


//---------------------------------------------------------------------
//	ExpandAllCues
//---------------------------------------------------------------------
//
//

void TCueChannel::ExpandAllCues()
{
	for ( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		TCueView *theCue = (TCueView *)m_CueList->ItemAt(index);
		
		if (theCue)
			theCue->Expand();
	}	
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueChannel::Draw(BRect updateRect)
{
	// Save 
	PushState();
	
	const BRect bounds = Bounds();
		
	// Draw main area
	SetHighColor(kLightGrey);
	FillRect(updateRect);

	// Draw timeline guides
	DrawTimelineGuides(updateRect);
	
	const float kTop 	= bounds.top;
	const float kBottom = bounds.bottom;
	
	BPoint startPt, endPt;
	
	// 	Draw main outline.  Don't bother drawing left side of outline, as the header
	//	already draws an outline there.
	SetHighColor(kBlack);
	startPt.Set( updateRect.left, kTop);
	endPt.Set( updateRect.right, kTop);
	StrokeLine(startPt, endPt);

	// Draw bottom line
	startPt.Set( updateRect.right-1, kBottom);
	endPt.Set( updateRect.left+1, kBottom);
	StrokeLine(startPt, endPt);
	
	// Draw top highlite
	SetHighColor(kWhite);
	startPt.Set( updateRect.right-1, kTop+1);
	endPt.Set( updateRect.left+1, kTop+1);
	StrokeLine(startPt, endPt);

	// Draw top ledge
	SetHighColor(kMediumGrey);
	startPt.Set( updateRect.right, kTop+kChannelLedgeHeight);
	endPt.Set( updateRect.left, kTop+kChannelLedgeHeight);
	StrokeLine(startPt, endPt);

	// Draw bottom ledge
	SetHighColor(kWhite);
	startPt.Set( updateRect.right, kBottom-kChannelLedgeHeight);
	endPt.Set( updateRect.left, kBottom-kChannelLedgeHeight);
	StrokeLine(startPt, endPt);

	// Draw bottom shadow
	SetHighColor(kMediumGrey);
	startPt.Set( updateRect.right, kBottom-1);
	endPt.Set( updateRect.left, kBottom-1);
	StrokeLine(startPt, endPt);
	
	// Draw right side outline
	//SetHighColor(kBlack);
	//startPt.Set( bounds.right, kTop);
	//endPt.Set( bounds.right, kBottom);
	//if (updateRect.Contains(endPt) )
	//	StrokeLine(startPt, endPt);	
		
	// Draw playback indicator tick
	if (IsPlaying() == false)
	{
		SetHighColor(kBlack);
		startPt.Set(m_TickPt.x, kTop+1);
		endPt.Set(m_TickPt.x, kBottom-1);
		StrokeLine(startPt, endPt);
	}

	// Restore
	PopState();
}



//---------------------------------------------------------------------
//	DrawIndicatorTick
//---------------------------------------------------------------------
//
//	This draws the playback indicator tick mark at the appropriate
//	location.  It also cleans up the last postion of the tick mark.
//

void TCueChannel::DrawIndicatorTick(BPoint drawPt)
{					
	BPoint 		startPt, endPt;
	const BRect bounds = Bounds();
		
	// Set up the environment	
	PushState();
		
	// Save point into tracking member variable points
	m_TickPt = drawPt;

	// If point is at new location, go ahead and draw
	if (m_TickPt.x != m_OldTickPt.x)
	{			
		// Erase last indicator tick. Clip out the outline lines 
		//SetHighColor(kLightGrey);
		startPt.Set(m_OldTickPt.x, bounds.top+2);
		endPt.Set(m_OldTickPt.x, bounds.bottom-2);
		StrokeLine(startPt, endPt);
		BRect invalRect(startPt, endPt);
		Draw(invalRect);
		
		// Draw the new indicator tick. Clip out the outline lines 
		SetHighColor(kBlack);
		startPt.Set(m_TickPt.x, bounds.top+1);
		endPt.Set(m_TickPt.x, bounds.bottom-1);
		StrokeLine(startPt, endPt);
		
		// Save tick location for next compare
		m_OldTickPt = m_TickPt;
	}
	
	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	DrawTimelineGuides
//---------------------------------------------------------------------
//
//	Draw the light timeline indicator lines that extend down from the
//	timeline indicating timeline ticks
//

void TCueChannel::DrawTimelineGuides(BRect updateRect)
{
	// Set up environment
	PushState();
	SetHighColor(kSteelGrey);   	
	
	// updateRect.left should be divisible by kTickSpacing.  Force it to be...
	int32 left = updateRect.left;
	int32 mod = left % kTickSpacing;
	updateRect.left -= mod;  
	
	// Set up variables here to increase efficiency in loop
	//
		
	BPoint startPt, endPt;
	BRect	bounds = Bounds();
	
	//	Inset rect by 2 so as not to draw over bevel		
	bounds.top 		+= (kChannelLedgeHeight+1);
	bounds.bottom 	-= (kChannelLedgeHeight+1);
	
	for (int32 index = updateRect.left / kTickSpacing * kTickSpacing;  index < updateRect.right + kTickSpacing; index += kTickSpacing)
	{										
		// Draw ticks...  
		startPt.Set( index, bounds.top);
		endPt.Set( startPt.x, bounds.bottom);
		StrokeLine(startPt, endPt);			
	}
	
	PopState();
}



//---------------------------------------------------------------------
//	RedrawAllCues
//---------------------------------------------------------------------
//
//	Invalidate all cues in channel cue list and force a redraw
//

void TCueChannel::RedrawAllCues()
{
	for ( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		TCueView *theView = (TCueView *)m_CueList->ItemAt(index);
		
		if (theView)
			theView->Invalidate();
	}	
}


//---------------------------------------------------------------------
//	UpdateResizeZones
//---------------------------------------------------------------------
//
//	Update all resize zones
//

void TCueChannel::UpdateAllResizeZones()
{
	for ( int32 index = 0; index < m_CueList->CountItems(); index++)
	{
		TCueView *theView = (TCueView *)m_CueList->ItemAt(index);
		
		if (theView)
			theView->UpdateResizeZones();
	}	
}


#pragma mark -
#pragma mark === Locking Routines ====


//---------------------------------------------------------------------
//	Lock
//---------------------------------------------------------------------
//
//	We are locking the channel.
//

void TCueChannel::LockChannel()
{
	if (! m_IsLocked)
	{
		m_IsLocked = true;
		DeselectAllCues();
		m_Header->GetLockButton()->Invalidate();
		LockAllCues();
	}
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//	We are unlocking the channel.
//

void TCueChannel::UnlockChannel()
{
	if(m_IsLocked)
	{
		m_IsLocked = false;
		m_Header->GetLockButton()->Invalidate();
		UnlockAllCues();
	}
}


//---------------------------------------------------------------------
//	LockAllCues
//---------------------------------------------------------------------
//
//	Lock every cue in channel
//

void TCueChannel::LockAllCues()
{
	TCueView *theView;
	
	for( int32 i = 0; i < m_CueList->CountItems(); i++)
	{
		theView = (TCueView *)m_CueList->ItemAt(i);
		
		if(theView)
		{
			// Check state to avoid uneccessary redraw
			if(!theView->IsLocked())
			{
				theView->LockCue();			
				theView->Invalidate();
			}
		}
	}
}


//---------------------------------------------------------------------
//	UnlockAllCues
//---------------------------------------------------------------------
//
//	Unlock every cue in channel
//

void TCueChannel::UnlockAllCues()
{
	TCueView *theView;
	
	// Iterate through all cues in the list
	for( int32 i = 0; i < m_CueList->CountItems(); i++)
	{
		theView = (TCueView *)m_CueList->ItemAt(i);
		
		if(theView)
		{
			// Check state to avoid uneccessary redraw
			if(theView->IsLocked())
			{
				theView->UnlockCue();			
				theView->Invalidate();
			}
		}
	}
}

#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TCueChannel::AttachedToWindow()
{			
	if (m_CueSheet == NULL)
	{
		m_CueSheet 	= (TCueSheetView *)Parent();		
	
		//	Add ourselves to cue list
printf("TCueChannel::AttachedToWindow: GetChannelList\n");
		BList *channelList = m_CueSheet->GetChannelList();		
		if ( channelList->HasItem(this) == false)
			channelList->AddItem(this);			
	}
			
	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Member Variable Handling ===

//---------------------------------------------------------------------
//	SetID
//---------------------------------------------------------------------
//
//	Set channel m_ID to theID
//

void TCueChannel::SetID(int32 theID)
{
	m_ID = theID;
}

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//	Set channel m_CueSheet to parent
//

void TCueChannel::SetParent(TCueSheetView *parent)
{
	m_CueSheet = parent;
}

//---------------------------------------------------------------------
//	SetHeader
//---------------------------------------------------------------------
//
//

void TCueChannel::SetHeader(TCueChannelHeader *header)
{
	m_Header = header;
}
