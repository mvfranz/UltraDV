//---------------------------------------------------------------------
//
//	File:	TVideoEditorShuttle.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//	Desc:	Video editor shuttle control
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>

#include "AppConstants.h"

#include "TVideoEditor.h"

#include "TVideoEditorShuttle.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditorShuttle::TVideoEditorShuttle(TVideoEditor* editor, BMessage* message) : BView(message)
{
	fEditor = editor;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoEditorShuttle::~TVideoEditorShuttle()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoEditorShuttle::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


#pragma mark -
#pragma mark === Message Handling ===


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoEditorShuttle::Draw(BRect updateRect)
{
	PushState();

	const BRect bounds = Bounds();

	// Fill Rect
	SetHighColor(kBeGrey);
	FillRect(bounds);

	// Draw outline
	SetHighColor(kTextHilite);
	BPoint startPt, endPt;
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left+1, bounds.bottom);
	StrokeLine(startPt, endPt);
	SetHighColor(kTextShadow);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right-1, bounds.top);
	StrokeLine(startPt, endPt);

	// Draw black rect
	BRect blackRect = bounds;
	blackRect.InsetBy(1, 1);
	SetHighColor(kBlack);
	StrokeRect(blackRect);

	//      Draw nubbies.  A nubbie is a 4 pixel block consisting of a black, white, light grey and
	//	medium grey line.
	int32 top        = bounds.top+2;
	int32 bottom = bounds.bottom-2;
	for (int32 width = bounds.left+2; width < bounds.right-2; width++) {
		SetHighColor(kBlack);
		startPt.Set(width, top);
		endPt.Set(width, bottom);
		StrokeLine(startPt, endPt);
		width++;
		SetHighColor(kWhite);
		startPt.Set(width, top);
		endPt.Set(width, bottom);
		StrokeLine(startPt, endPt);
		width++;
		SetHighColor(kLightGrey);
		startPt.Set(width, top);
		endPt.Set(width, bottom);
		StrokeLine(startPt, endPt);
		width++;
		SetHighColor(kMediumGrey);
		startPt.Set(width, top);
		endPt.Set(width, bottom);
		StrokeLine(startPt, endPt);
	}

	PopState();
}


