//---------------------------------------------------------------------
//
//	File:	TLockButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
///	Desc:	Lock button.  Clicking will either lock or unlock the
//			channel depending upon it's current state
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
#include "TMuseumIcons.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TLockButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TLockButton::TLockButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fChannel = parent;

	// Set MouseDown/MouseUp flag
	fMouseDown = false;

	// Store bitmaps
	fOffBitmap = offBitmap;
	fOnBitmap = onBitmap;

	// Store target and handler
	fHandler = handler;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TLockButton::TLockButton(BMessage *data) : BView(data)
{
	fHandler = NULL;

	// Set MouseDown/MouseUp flag
	fMouseDown = false;

	// Set bitmaps
	fOffBitmap      = GetAppIcons()->fUnlockDark;
	fOnBitmap       = GetAppIcons()->fLockDark;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TLockButton::~TLockButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TLockButton::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TLockButton::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TLockButton") )
		return new TLockButton(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TLockButton::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TLockButton");

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

void TLockButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if fState is true, draw on bitmap
	if (fChannel->IsLocked() )
		DrawBitmap(fOnBitmap, B_ORIGIN);
	else
		DrawBitmap(fOffBitmap, B_ORIGIN);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TLockButton::MouseDown(BPoint where)
{

	// Force redraw to reflect new state
	Invalidate();

	// Create and send message to channel
	BMessage *message = new BMessage(LOCK_CHANNEL_MSG);
	(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, fChannel);
	delete message;

	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	fMouseDown = true;
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TLockButton::MouseUp(BPoint where)
{

	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	fMouseDown = false;
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

void TLockButton::AttachedToWindow()
{
	if (fHandler == NULL) {
		// Set target
		fHandler = (TCueSheetWindow *)Window();

	}
	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//

void TLockButton::SetChannel(TCueChannel *channel)
{
	fChannel = channel;
}