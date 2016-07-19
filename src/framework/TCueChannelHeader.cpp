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
	fChannel = channel;
	
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
	fChannel = NULL;
	
	//	Find child views
	fName			= (TChannelNameView *)FindView("ChannelName");
	fSoloButton	= (TSoloButton *)FindView("SoloButton");
	fMuteButton	= (TMuteButton *)FindView("MuteButton");
	fExpandButton	= (TExpandButton *)FindView("ExpandButton");
	fLockButton	= (TLockButton *)FindView("LockButton");	
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
	fIsHighlighted = false;
	
	// Create label text
	BRect bounds(kNameOffset, kNameOffset, Bounds().right-kNameOffset, kNameOffset*3);	
	sprintf( IDStr, " [%d]", fChannel->GetID() );	
	strcpy(nameStr, fChannel->GetName()); 
	strcat(nameStr, IDStr);
	fName = new TChannelNameView(fChannel, bounds, "ChannelName", nameStr);
	AddChild(fName);
	fName->SetViewColor(kHeaderGrey);
	fName->SetHighColor(kBlack);
	fName->SetLowColor(kHeaderGrey);
	fName->Show();
		
	// Set target
	TCueSheetWindow *theTarget = static_cast<MuseumApp *>(be_app)->GetCueSheet();
		
	// Create Mute button
	BBitmap *muteUp = GetAppIcons()->fMuteUp;
	BBitmap *muteDn = GetAppIcons()->fMuteDown;
	BRect textBounds = fName->Bounds();
	bounds.Set( kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, kButtonWidth+kButtonLeftOffset, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	fMuteButton = new TMuteButton(fChannel, bounds, "MuteButton", muteUp, muteDn, theTarget); 
	AddChild(fMuteButton);
	fMuteButton->Show();	
	
	// Create Solo button
	// Place it to the right of the Mute button
	BBitmap *soloUp = GetAppIcons()->fSoloUp;
	BBitmap *soloDn = GetAppIcons()->fSoloDown;
	BRect buttonBounds = fMuteButton->Frame();
	bounds.Set( buttonBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+buttonBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	fSoloButton = new TSoloButton(fChannel, bounds, "SoloButton", soloUp, soloDn, theTarget); 
	AddChild(fSoloButton);
	fSoloButton->Show();	
	
	// Create Expand button
	// Place it to the right of the Solo button
	BBitmap *expand = GetAppIcons()->fExpand;
	BBitmap *contract = GetAppIcons()->fContract;	
	BRect expandBounds = fSoloButton->Frame();
	bounds.Set( expandBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+expandBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	fExpandButton = new TExpandButton(fChannel, bounds, "ExpandButton", expand, contract, theTarget); 
	AddChild(fExpandButton);
	fExpandButton->Show();		
	
	// Create Lock button
	// Place it to the right of the Expand button	
	BBitmap *unlock = GetAppIcons()->fUnlockDark;
	BBitmap *lock 	= GetAppIcons()->fLockDark;
	BRect lockBounds = fExpandButton->Frame();
	bounds.Set( lockBounds.right + kButtonLeftOffset, textBounds.bottom + kButtonTopOffset, 
				kButtonWidth+kButtonLeftOffset+lockBounds.right, (textBounds.bottom + kButtonTopOffset + kButtonHeight) -1 );
	fLockButton = new TLockButton(fChannel, bounds, "LockButton", unlock, lock, theTarget); 
	AddChild(fLockButton);
	fLockButton->Show();		

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
	//if (fIsHighlighted)
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
	
	const BRect	bounds = fChannel->Bounds();
		
	// Check for mouse down
	Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons); 
	GetMouse(&point, &buttons, true); 
	
	if (buttons) 
	{ 			
		// They must be moving the channel...
		BMessage message(CHANNEL_DRAG_MSG);
		message.AddPointer("ChannelView", fChannel);
				
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
						if ( fChannel->CanInsertCue( cueView, insertTime, true) )
						{													
							// Remove cue from old location
							fChannel->RemoveCue(cueView);
							
							// Insert cue into channel at new location							
							fChannel->InsertCue(cueView, insertTime);
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
						if (insertChannel != fChannel)									
						{
							// Now insert it into the CueSheets list of channels at the right spot
							fChannel->GetCueSheet()->DragInsertChannel(insertChannel, fChannel);
						}
					}
				}
				break;
		}
						
		// Highlight header to indicate cue icon entering channel
		case CUE_ICON_DRAG_IN_MSG:	
			fIsHighlighted = true;
			Invalidate();				
			break;
			
		// Unhighlight header to indicate cue icon leaving channel
		case CUE_ICON_DRAG_OUT_MSG:
			fIsHighlighted = false;
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
	if(fChannel == NULL)
	{
		//fChannel = (TCueChannel *)Parent();		
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
//	the channel's fName memeber variable has already been set.
//

void TCueChannelHeader::UpdateChannelName()
{

	char nameStr[255];
	char IDStr[255];
	
	BLooper *looper = Looper();
	looper->Lock();
	
	// Set label text
	sprintf( IDStr, " [%d]", fChannel->GetID() );	
	strcpy(nameStr, fChannel->GetName()); 
	strcat(nameStr, IDStr);
	fName->SetText(nameStr);
	
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
	fChannel = channel;
	
	//	Update children
	fName->SetChannel(channel);
	fSoloButton->SetChannel(channel);
	fMuteButton->SetChannel(channel);	
	fExpandButton->SetChannel(channel);
	fLockButton->SetChannel(channel);			
}
