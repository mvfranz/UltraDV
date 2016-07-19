//---------------------------------------------------------------------
//
//	File:	TVideoEditorControlsView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.12.98
//
//	Desc:	Video controls area
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"

#include "TVideoEditorControlsView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditorControlsView::TVideoEditorControlsView(BMessage* message) : BView(message)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoEditorControlsView::~TVideoEditorControlsView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoEditorControlsView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


#pragma mark -
#pragma mark === Message Handling ===


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TVideoEditorControlsView::Draw(BRect updateRect)
{
	PushState();

	const BRect bounds = Bounds();

	BPoint startPt, endPt;

	// Fill backgoround
	SetHighColor(kBlueGrey);
	FillRect(updateRect);

	// Draw Highlite
	SetHighColor(kLightGrey);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.right, bounds.top);
	StrokeLine(endPt);

	// Draw Shadow
	SetHighColor(kDarkGrey);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(endPt);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(endPt);

	PopState();
}


