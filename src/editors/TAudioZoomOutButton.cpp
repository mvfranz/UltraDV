//---------------------------------------------------------------------
//
//	File:	TAudioZoomOutButton.cpp
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

#include "TAudioZoomOutButton.h"
#include "TAudioEditorToolbar.h"
#include "TAudioEditor.h"
#include "TAudioEditorView.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioZoomOutButton::TAudioZoomOutButton(TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fParent = parent;

	// Save handler
	fHandler = handler;

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
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioZoomOutButton::~TAudioZoomOutButton()
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

void TAudioZoomOutButton::Init()
{
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioZoomOutButton::Draw(BRect updateRect)
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

void TAudioZoomOutButton::MouseDown(BPoint where)
{
	// Handle button click
	DoClick();
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioZoomOutButton::MouseUp(BPoint where)
{

	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	//fMouseDown = false;
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioZoomOutButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioZoomOutButton::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioZoomOutButton::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioZoomOutButton::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioZoomOutButton::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioZoomOutButton::FrameResized(float new_width, float new_height)
{
}

#pragma mark -
#pragma mark === Button Handling ===


//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Zoom Out button has been clicked
//

void TAudioZoomOutButton::DoClick()
{
	// Force redraw to reflect new state
	//Invalidate();

	// We will always be in the up position when we start.
	// First, set the button state to down and force a redraw...
	fMouseDown = true;
	Draw(Bounds());

	// Now send message to the view that the user is zooming out...
	BMessage *message = new BMessage(AUDIO_ZOOMOUT_BUTTON_MSG);
	fParent->Window()->PostMessage(message, (( TAudioEditor *)fParent->Window())->GetEditorView() );
	delete message;

	// Wait a short while before restoring to up position
	snooze(100 * 1000);

	// Restore and invalidate
	fMouseDown = false;
	Draw(Bounds());
}