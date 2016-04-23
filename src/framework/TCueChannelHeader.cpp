//---------------------------------------------------------------------
//
//	File:	TCueChannelHeader.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header that is part of each cue channel.  This contains
//			the mute and solo buttons, track text label and number.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TCueChannel.h"
#include "TCueView.h"
#include "TCueChannelHeader.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TChannelNameView.h"
#include "TMuteButton.h"
#include "TSoloButton.h"
#include "TExpandButton.h"
#include "TLockButton.h"


// Constants
	 
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueChannelHeader::TCueChannelHeader(BRect bounds, TCueChannel *channel) : BView(bounds, "CueChannelHeader", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	m_Channel = channel;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueChannelHeader::TCueChannelHeader(BMessage *message) : BView(message)
{
	m_Channel = NULL;
	
	//	Find child views
	m_Name			= (TChannelNameView *)FindView("ChannelName");
	m_SoloButton	= (TSoloButton *)FindView("SoloButton");
	m_MuteButton	= (TMuteButton *)FindView("MuteButton");
	m_ExpandButton	= (TExpandButton *)FindView("ExpandButton");
	m_LockButton	= (TLockButton *)FindView("LockButton");	
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueChannelHeader::~TCueChannelHeader()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

const short kNameOffset = 5;
const short kButtonTopOffset = 7;
const short kButtonLeftOffset = 2;
const short kButtonWidth = 15;
const short kButtonHeight = 15;

void TCueChannelHeader::Init()
{		
	
	char nameStr[255];
	char IDStr[255];
	
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	// Set highlight state to off
	m_IsHighlighted = false;
	
	// Create label text
	BRect bounds(kNameOffset, kNameOffset, Bounds().right-kNameOffset, kNameOffset*3);	
	sprintf( IDStr, " [%d]", m_Channel->GetID() );	
	strcpy(nameStr, m_Channel->GetName()); 
	strcat(nameStr, IDStr);
	m_Name = new TChannelNameView(m_Channel, bounds, "ChannelName", nameStr);
	AddChild(m_Name);
	m_Name->SetViewColor(kHeaderGrey);
	m_Name->SetHighColor(kBlack);
	m_Name->SetLowColor(kHeaderGrey);
	m_Name->Show();
		
	// Set target
	TCueSheetWindow *theTarget = static_cast<MuseumApp *>(be_app)->GetCueSheet();
		
	// Create Mute button
	BBitmap *muteUp = GetAppIcons()->m_MuteUp;
	BBitmap *muteDn = GetAppIcons()->m_MuteDown;
	BRect textBounds = m_Name->Bounds();
	bounds.Set( kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, kButtonWidth+kButtonLeftOffset, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	m_MuteButton = new TMuteButton(m_Channel, bounds, "MuteButton", muteUp, muteDn, theTarget); 
	AddChild(m_MuteButton);
	m_MuteButton->Show();	
	
	// Create Solo button
	// Place it to the right of the Mute button
	BBitmap *soloUp = GetAppIcons()->m_SoloUp;
	BBitmap *soloDn = GetAppIcons()->m_SoloDown;
	BRect buttonBounds = m_MuteButton->Frame();
	bounds.Set( buttonBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+buttonBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	m_SoloButton = new TSoloButton(m_Channel, bounds, "SoloButton", soloUp, soloDn, theTarget); 
	AddChild(m_SoloButton);
	m_SoloButton->Show();	
	
	// Create Expand button
	// Place it to the right of the Solo button
	BBitmap *expand = GetAppIcons()->m_Expand;
	BBitmap *contract = GetAppIcons()->m_Contract;	
	BRect expandBounds = m_SoloButton->Frame();
	bounds.Set( expandBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+expandBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	m_ExpandButton = new TExpandButton(m_Channel, bounds, "ExpandButton", expand, contract, theTarget); 
	AddChild(m_ExpandButton);
	m_ExpandButton->Show();		
	
	// Create Lock button
	// Place it to the right of the Expand button	
	BBitmap *unlock = GetAppIcons()->m_UnlockDark;
	BBitmap *lock 	= GetAppIcons()->m_LockDark;
	BRect lockBounds = m_ExpandButton->Frame();
	bounds.Set( lockBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+lockBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	m_LockButton = new TLockButton(m_Channel, bounds, "LockButton", unlock, lock, theTarget); 
	AddChild(m_LockButton);
	m_LockButton->Show();		

}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueChannelHeader::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TCueChannelHeader") ) 
		return new TCueChannelHeader(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueChannelHeader::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TCueChannelHeader");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{	
			
		}				
	}
	
	
	Looper()->Unlock();
	
	return myErr;
}

#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueChannelHeader::Draw(BRect updateRect)
{
	
	BPoint startPt, endPt;
	
	const BRect bounds = Bounds();
	
	PushState();
	
	// Draw main area
	//if (m_IsHighlighted)
	//	SetHighColor(kLightGrey);
	//else
	//	SetHighColor(kSteelBlue);
	SetHighColor(kHeaderGrey);	
	FillRect(updateRect);
						
	// Draw outline
	SetHighColor(kBlack);		
	StrokeRect(bounds);
	
	// Draw Highlite
	SetHighColor(kBlueGrey);
	startPt.Set(bounds.left+1, bounds.bottom-1);
	endPt.Set(bounds.left+1, bounds.top+1);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.right-1, bounds.top+1);
	StrokeLine(endPt);
	
	// Draw Shadow
	SetHighColor(kDarkGrey);
	endPt.Set(bounds.right-1, bounds.bottom-1);
	StrokeLine(endPt);
	endPt.Set(bounds.left+1, bounds.bottom-1);
	StrokeLine(endPt);
		
	PopState();	
}


#pragma mark -
#pragma mark === Mouse Handling ===


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueChannelHeader::MouseDown(BPoint where)
{
	
	// Do nothing if we are playing
	if (IsPlaying() )
		return;
	
	// 	While the mouse is down, create an outline of the cue channel
	//	to be displayed while dragging
	uint32 buttons = 0; 
	BPoint point, oldPt;
	
	const BRect	bounds = m_Channel->Bounds();
		
	// Check for mouse down
	Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons); 
	GetMouse(&point, &buttons, true); 
	
	if (buttons) 
	{ 			
		// They must be moving the channel...
		BMessage message(CHANNEL_DRAG_MSG);
		message.AddPointer("ChannelView", m_Channel);
				
		// Center the rect within the mouse for a better drag appearence		
		DragMessage(&message, bounds);					
	}	
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TCueChannelHeader::MessageReceived(BMessage *message)
{
	switch(message->what)
	{					
		// Check for dropped messages
		if ( message->WasDropped() )
		{
			// Someone has dragged a cue into the header.  Check and see if the cue can be inserted
			// at time 00:00:00:00 in the channel
			case CUE_DRAG_MSG:
				{	
					BPoint where = message->DropPoint();
					ConvertFromScreen(&where);
					where.x = Bounds().right+1;
					
					// Find out which cue was moved.  Was it one from this channel
					// or is it being moved in from another?
					TCueView *cueView;
					message->FindPointer("CueView", (void **)&cueView);						
					if (&cueView)
					{
						uint32 insertTime = PixelsToTime( where.x, GetCurrentTimeFormat(), GetCurrentResolution());
						
						// Check for collisions with other cues
						if ( m_Channel->CanInsertCue( cueView, insertTime, true) )
						{													
							// Remove cue from old location
							m_Channel->RemoveCue(cueView);
							
							// Insert cue into channel at new location							
							m_Channel->InsertCue(cueView, insertTime);
						}
					}
				}
				break;
				
			case CHANNEL_DRAG_MSG:
				{
					BPoint where = message->DropPoint();
					ConvertFromScreen(&where);
					
					// Unpack message
					TCueChannel *insertChannel;
					message->FindPointer("ChannelView", (void **)&insertChannel);						
					if (&insertChannel)
					{					
						// Make sure we aren't dragging the channel back into itself
						if (insertChannel != m_Channel)									
						{
							// Now insert it into the CueSheets list of channels at the right spot
							m_Channel->GetCueSheet()->DragInsertChannel(insertChannel, m_Channel);
						}
					}
				}
				break;
		}
						
		// Highlight header to indicate cue icon entering channel
		case CUE_ICON_DRAG_IN_MSG:	
			m_IsHighlighted = true;
			Invalidate();				
			break;
			
		// Unhighlight header to indicate cue icon leaving channel
		case CUE_ICON_DRAG_OUT_MSG:
			m_IsHighlighted = false;
			Invalidate();
			break;	
			
		default:
			BView::MessageReceived(message);						
			break;
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

void TCueChannelHeader::AttachedToWindow()
{			
	if(m_Channel == NULL)
	{
		//m_Channel = (TCueChannel *)Parent();		
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Utility Functions ===


//---------------------------------------------------------------------
//	UpdateChannelName
//---------------------------------------------------------------------
//
//	Set the text string to the new channel name.  We are assuming that 
//	the channel's m_Name memeber variable has already been set.
//

void TCueChannelHeader::UpdateChannelName()
{

	char nameStr[255];
	char IDStr[255];
	
	BLooper *looper = Looper();
	looper->Lock();
	
	// Set label text
	sprintf( IDStr, " [%d]", m_Channel->GetID() );	
	strcpy(nameStr, m_Channel->GetName()); 
	strcat(nameStr, IDStr);
	m_Name->SetText(nameStr);
	
	looper->Unlock();
}

#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//

void TCueChannelHeader::SetChannel(TCueChannel *channel)
{
	m_Channel = channel;
	
	//	Update children
	m_Name->SetChannel(channel);
	m_SoloButton->SetChannel(channel);
	m_MuteButton->SetChannel(channel);	
	m_ExpandButton->SetChannel(channel);
	m_LockButton->SetChannel(channel);			
}
