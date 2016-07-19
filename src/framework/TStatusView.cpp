//---------------------------------------------------------------------
//
//	File:	TStatusView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.09.98
//
//	Desc:	Display status text
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

#include "TStatusView.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStatusView::TStatusView(BRect bounds) : BView(bounds, "StatusView", B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStatusView::~TStatusView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TStatusView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Set up the bounding rectangles for the Text Display and Tick Indicator areas
	BRect area = Bounds();

	// Set up status text
	strcpy(fText, "");

}

#pragma mark -
#pragma mark === Message Handling ===

//------------------------------------------------------------------
//	MessageReceived
//------------------------------------------------------------------
//
//

void TStatusView::MessageReceived(BMessage* message)
{

	switch (message->what)
	{
	// Update the status bar to display name of media cue
	case UPDATE_STATUS_TEXT_MSG:
		char *theStr;
		message->FindString("StatusText",(const char **) &theStr);
		strcpy(fText, theStr);
		Invalidate();
		break;

	default:
		BView::MessageReceived(message);
		break;
	}

}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TStatusView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;

	// Save old colors
	PushState();

	BRect bounds = Bounds();

	// Fill
	SetHighColor(kBlueGrey);
	FillRect( bounds );

	// Outline entire area
	SetHighColor(kDarkGrey);
	StrokeRect(bounds);

	// Draw highlight at top
	SetHighColor(kBeGrey);
	startPt.Set(bounds.left+1, bounds.bottom-1);
	endPt.Set(bounds.left+1, bounds.top+1);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left+1, bounds.top+1);
	endPt.Set(bounds.right-1, bounds.top+1);
	StrokeLine(startPt, endPt);

	// Draw text
	SetFont(be_plain_font);
	SetHighColor(kBlack);
	SetLowColor(kBlueGrey);
	endPt.Set( Bounds().left+5, (Bounds().Height() / 2) + 4 );
	DrawString(fText, endPt);

	// Restore environment
	PopState();
}
