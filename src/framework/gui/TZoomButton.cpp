//---------------------------------------------------------------------
//
//	File:	TZoomButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
///	Desc:	Zoom button.  Sets cursor to magnify icon and allows
//			user to zoom in or out of cue channels.
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
#include "ResourceManager.h"

#include "TZoomButton.h"
#include "TToolbar.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TZoomButton::TZoomButton(TToolbar* parent, BRect bounds, const char* name, BBitmap* offBitmap, BBitmap* onBitmap, BHandler* handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fParent = parent;

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

TZoomButton::TZoomButton(BMessage* data) : BView(data)
{
	// Set MouseDown/MouseUp flag
	fMouseDown = false;

	fOffBitmap = GetIcon16FromResource("ZoomDown");
	fOnBitmap  = GetIcon16FromResource("ZoomUp");
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TZoomButton::~TZoomButton()
{
	// Free data
	delete fOffBitmap;
	delete fOnBitmap;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TZoomButton::Init()
{
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TZoomButton::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TZoomButton") )
		return new TZoomButton(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TZoomButton::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TZoomButton");

		// Add our member variables to the archive

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}

#pragma mark -
#pragma mark === Drawing Routines

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TZoomButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if fState is true, draw on bitmap
	if (fMouseDown)
		DrawBitmap(fOffBitmap, B_ORIGIN);
	else
		DrawBitmap(fOnBitmap, B_ORIGIN);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TZoomButton::MouseDown(BPoint where)
{

	// Force redraw to reflect new state
	//Invalidate();

	// Create and send message to channel and header
	//BMessage *message = new BMessage(CHANNEL_EXPAND_MSG);
	//(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, fChannel);
	//delete message;

	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	if (fMouseDown)
		fMouseDown = false;
	else
		fMouseDown = true;

	Invalidate();
}

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TZoomButton::AttachedToWindow()
{
	if(fParent == NULL) {
		fParent         = (TToolbar*)Parent();
		fHandler        = (TToolbar*)Parent();
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}


