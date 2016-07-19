//---------------------------------------------------------------------
//
//	File:	TElementsScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Custom Horizontal scroll bar to scroll Elements View
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

#include "TElementsScrollBarH.h"



// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TElementsScrollBarH::TElementsScrollBarH(BRect frame, const char *name, BView *target, float min, float max ) :
	BScrollBar( frame, name, target, min, max, B_HORIZONTAL)
{

	// Do default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TElementsScrollBarH::~TElementsScrollBarH()
{

}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::Init()
{
	//      Get current scroll value and save it in our tracking variable.
	//	We use this value to scroll the HeaderZone in concert with
	//	the CueSheetView
	fLastValue = Value();
}



//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::AttachedToWindow()
{
}


//---------------------------------------------------------------------
//	ValueChanged
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::ValueChanged(float newValue)
{
	// Pass message to base class
	BScrollBar::ValueChanged(newValue);

	/*
	   // Offset the cue channels

	   // We need to offset the TimeLine as well...
	   if (newValue != fLastValue)
	   {
	        if (newValue > fLastValue)
	                fCueSheetWindow->GetTimeline()->ScrollBy(newValue - fLastValue, 0);
	        else
	                fCueSheetWindow->GetTimeline()->ScrollBy(-(fLastValue - newValue), 0);

	        // Force a redraw
	        BRect bounds = fCueSheetWindow->GetTimeline()->Bounds();
	        bounds.top = fCueSheetWindow->GetTimeline()->GetIndicatorZone().bottom+1;
	        fCueSheetWindow->GetTimeline()->Invalidate(bounds);

	        // Save value for next compare
	        fLastValue = newValue;
	   }
	 */
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::MessageReceived(BMessage *msg)
{
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::MouseDown(BPoint pt)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::MouseUp(BPoint pt)
{
}



//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
}



//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::DetachedFromWindow()
{
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::Draw(BRect updateRect)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::FrameMoved(BPoint new_position)
{
}



//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TElementsScrollBarH::FrameResized(float new_width, float new_height)
{
}



