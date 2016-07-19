//---------------------------------------------------------------------
//
//	File:	TAudioPlayButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
///	Desc:	Audio Editor Play button. Clicking cause adio file to be played
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

#include "TAudioPlayButton.h"
#include "TAudioEditorToolbar.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioPlayButton::TAudioPlayButton(TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fParent = parent;

	// Save handler
	fHandler = handler;

	// button state flag
	fButtonState = false;

	// Store bitmaps
	fOffBitmap = offBitmap;
	fOnBitmap = onBitmap;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioPlayButton::~TAudioPlayButton()
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

void TAudioPlayButton::Init()
{
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioPlayButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if fState is true, draw on bitmap
	if (fButtonState)
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

void TAudioPlayButton::MouseDown(BPoint where)
{

	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	if (fButtonState) {
		// Tell sound to stop
		BMessage *message = new BMessage(AUDIO_STOP_BUTTON_MSG);
		//fParent->Window()->PostMessage(message, fHandler);
		fParent->Window()->PostMessage(message, NULL);
		delete message;
		fButtonState = false;
	} else   {
		// Tell sound to play
		BMessage *message = new BMessage(AUDIO_PLAY_BUTTON_MSG);
		//fParent->Window()->PostMessage(message, fHandler);
		fParent->Window()->PostMessage(message, NULL);
		delete message;
		fButtonState = true;
	}

	// Force redraw to reflect new state
	Invalidate();
}
