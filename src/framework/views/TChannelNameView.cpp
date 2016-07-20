//---------------------------------------------------------------------
//
//	File:	TChannelNameView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.04.98
//
//	Desc:	This is an derived version of BStringView that is responsible
//			for the display of the Channel name in the header area.
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
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TPlaybackEngine.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TChannelNameView.h"
#include "TCueChannel.h"
#include "TChannelName.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChannelNameView::TChannelNameView( TCueChannel* channel, BRect bounds, const char* name, const char* text, uint32 resizeFlags, uint32 flags) :
	BStringView(bounds, name, text, resizeFlags, flags)
{
	fChannel = channel;

	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChannelNameView::TChannelNameView(BMessage* message) : BStringView(message)
{
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TChannelNameView::~TChannelNameView()
{
}



#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TChannelNameView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TChannelNameView") )
		return new TChannelNameView(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TChannelNameView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BStringView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TChannelNameView");

		// Add our member variables to the archive

		// Add attached views
		if (deep) {

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

void TChannelNameView::Draw(BRect bounds)
{
	PushState();

	// Fill background
	SetHighColor(kHeaderGrey);
	FillRect(bounds);

	// Draw text
	SetHighColor(kBlack);
	SetLowColor(kHeaderGrey);
	BStringView::Draw(bounds);

	PopState();
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TChannelNameView::MessageReceived(BMessage* message)
{

	BStringView::MessageReceived(message);

}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TChannelNameView::MouseDown(BPoint pt)
{
	// Do nothing if we are playing
	if ( ((TCueSheetWindow*)fChannel->Window())->GetPlaybackEngine()->IsPlaying() )
		return;

	// If the option key is down, allow user to change the text
	if ( IsOptionKeyDown() ) {
		// Create the channel name dialog from a resource archive
		BMessage* theMessage = GetWindowFromResource("ChannelNameWindow");
		TChannelName* theDialog = new TChannelName(theMessage, fChannel);
		ASSERT(theDialog);

		// Move it under the mouse
		ConvertToScreen(&pt);
		theDialog->MoveTo(pt.x, pt.y);

		// Show the dialog
		theDialog->Show();
	}
	// Otherwise, select all the cues in the channel
	else{
		if ( IsShiftKeyDown() == false )
			fChannel->GetCueSheet()->DeselectAllCues();

		fChannel->SelectAllCues();
	}
}

#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//

void TChannelNameView::SetChannel(TCueChannel* channel)
{
	fChannel = channel;
}

