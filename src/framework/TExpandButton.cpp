//---------------------------------------------------------------------
//
//	File:	TExpandButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
///	Desc:	Expand button.  Clikcing will either expand or contract the
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
#include "AppUtils.h"
#include "TMuseumIcons.h"
#include "ResourceManager.h"

#include "TExpandButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExpandButton::TExpandButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
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

TExpandButton::TExpandButton(BMessage *message) : BView(message)
{
	fHandler = NULL;

	// Set MouseDown/MouseUp flag
	fMouseDown = false;

	// Set bitmaps
	fOffBitmap = GetAppIcons()->fExpand;
	fOnBitmap 	= GetAppIcons()->fContract;
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TExpandButton::~TExpandButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TExpandButton::Init()
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

BArchivable *TExpandButton::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TExpandButton") )
		return new TExpandButton(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TExpandButton::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK)
	{
		// Add our class name to the archive
		data->AddString("class", "TExpandButton");

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

void TExpandButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if fState is true, draw on bitmap
	if (fChannel->IsExpanded())
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

void TExpandButton::MouseDown(BPoint where)
{

	// Force redraw to reflect new state
	Invalidate();

	// Create and send message to channel, header and window
	BMessage *message = new BMessage(CHANNEL_EXPAND_MSG);
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

void TExpandButton::MouseUp(BPoint where)
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

void TExpandButton::AttachedToWindow()
{
	if (fHandler == NULL)
	{
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

void TExpandButton::SetChannel(TCueChannel *channel)
{
	fChannel = channel;
}