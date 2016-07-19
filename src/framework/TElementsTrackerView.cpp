//---------------------------------------------------------------------
//
//	File:	TElementsTrackerView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Child view of TBrowserTabView.  Handles display and sorting
//			of project media elements
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "ResourceManager.h"

#include "TElementsView.h"

#include "TElementsTrackerView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TElementsTrackerView::TElementsTrackerView(BRect bounds, TElementsView *parent) : BView(bounds, "ElementsTrackerView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	fParent = parent;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TElementsTrackerView::~TElementsTrackerView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TElementsTrackerView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TElementsTrackerView::Draw(BRect updateRect)
{

	BView::Draw(updateRect);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TElementsTrackerView::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TElementsTrackerView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TElementsTrackerView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}

