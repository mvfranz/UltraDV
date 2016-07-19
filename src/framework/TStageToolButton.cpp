//---------------------------------------------------------------------
//
//	File:	TStageToolButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.13.98
//
///	Desc:	Stage Tool button
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

#include "TCueSheetWindow.h"
#include "TStageToolsView.h"
#include "TStageWindow.h"
#include "TStageView.h"

#include "TStageToolButton.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageToolButton::TStageToolButton(TStageToolsView *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BMessage *message) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fParent = parent;

	// Save message
	fMessage = message;

	// button state flag
	fButtonState = false;

	// Store bitmaps
	fOffBitmap = offBitmap;
	fOnBitmap 	= onBitmap;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageToolButton::~TStageToolButton()
{
	//	Free data
	delete fOffBitmap;
	delete fOnBitmap;

	//	Free message
	delete fMessage;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TStageToolButton::Init()
{
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TStageToolButton::Draw(BRect updateRect)
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

void TStageToolButton::MouseDown(BPoint where)
{
	if (fButtonState == false)
		DoClick();

	/*
	//	Only accept mouse down if button is not selected.  This is a tool palette button
	//	and only one button can be depressed at a time
	if (fButtonState == false)
	{
		fParent->Window()->PostMessage(fMessage, NULL);
		fButtonState = true;
	}

	// Force redraw to reflect new state
	Invalidate();
	*/
}


//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Button has been clicked
//

void TStageToolButton::DoClick()
{

	// We will always be in the up position when we start.
	// First, set the button state to down and force a redraw...
	fButtonState = true;

	BRect 	bounds = Bounds();
	Draw(bounds);

	// Trap mouse while it is down
	uint32 	buttons;
	BPoint 	mousePt, savePt;
	GetMouse(&mousePt, &buttons, true);

	while(buttons)
	{
		GetMouse(&mousePt, &buttons, true);

		if (savePt != mousePt)
		{
			if ( bounds.Contains(mousePt) )
			{
				if (fButtonState == false)
				{
					fButtonState = true;
					Draw(bounds);
				}
			}
			// If the mouse is outside of the button bounds draw it's up state
			else
			{
				if (fButtonState == true)
				{
					fButtonState = false;
					Draw(bounds);
				}
			}
			savePt = mousePt;
			snooze(20 * 1000);
		}

	}

	if ( bounds.Contains(mousePt) )
	{
		//	Send message to parent and stage
		fParent->Window()->PostMessage(fMessage, fParent);

		MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
		theApp->GetCueSheet()->GetStage()->PostMessage(fMessage, theApp->GetCueSheet()->GetStage()->GetStageView());

		//	Draw
		Draw(bounds);
	}
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	Activate
//---------------------------------------------------------------------
//
//	Set the state of the button and redraw
//

void TStageToolButton::Activate(bool theState)
{
	if (theState == true)
	{
		if (fButtonState == false)
		{
			fButtonState = true;
			Invalidate();
		}
	}
	else
	{
		if (fButtonState == true)
		{
			fButtonState = false;
			Invalidate();
		}
	}
}