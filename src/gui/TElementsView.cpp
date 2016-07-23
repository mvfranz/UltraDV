//---------------------------------------------------------------------
//
//	File:	TElementsView.cpp
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
#include "AppMessages.h"
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"
#include "TMouseWatcher.h"

#include "TSorterContainer.h"

#include "TElementsSorter.h"
#include "TElementsScrollBarH.h"
#include "TElementsScrollBarV.h"

#include "TElementsView.h"
#include <assert.h>

// Constants
const int32 kDragRectWidth      = 14;
const int32 kDragRectHeight = 14;
const rgb_color kDkGrey = {100, 100, 100, 255};


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TElementsView::TElementsView(BRect bounds) : BView(bounds, "ElementsView",
	                                           B_FOLLOW_ALL|B_FRAME_EVENTS, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TElementsView::~TElementsView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TElementsView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);

	// Create Tracker Sorter
	BRect sorterRect = Bounds();
	sorterRect.right        -= kScrollWidth+1;
	sorterRect.bottom       -= kScrollHeight+1;
	fSorter = new TElementsSorter(sorterRect, this);
	AddChild(fSorter);

	//
	// Create scroll bars
	//

	// Horizontal
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.left, scrollRect.bottom-kScrollHeight, scrollRect.right-kScrollWidth, scrollRect.bottom);
	fHScroll = new TElementsScrollBarH(scrollRect, "HElementsScroll", fSorter, 0, 100);
	AddChild(fHScroll);


	// Vertical
	scrollRect = Bounds();
	scrollRect.Set(scrollRect.right-kScrollWidth, scrollRect.top, scrollRect.right, scrollRect.bottom-kScrollHeight);
	fVScroll = new TElementsScrollBarV(scrollRect, "VElementsScroll", fSorter, 0, 100);
	AddChild(fVScroll);

	AdjustScrollBars();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TElementsView::MessageReceived(BMessage* theMessage)
{

	switch( theMessage->what)
	{
	// Mouse moved. Send resize message to parent
	case MW_MOUSE_MOVED:
	{
		BPoint mousePt;
		if ( theMessage->FindPoint("where", &mousePt) == B_OK) {
			// Conver mousePt to screen coordinate
			ConvertToScreen(&mousePt);

			// Get new window bounds
			BRect frame = Window()->Frame();
			frame.right             = mousePt.x;
			frame.bottom    = mousePt.y;

			// Adjust size to keep mouse centered in DragRect
			frame.right     += kDragRectWidth/2;
			frame.bottom    += kDragRectHeight/2;

			// Resize
			Window()->ResizeTo(frame.Width(), frame.Height());
		}
	}
	break;

	// A sorter is being dragged.  Adjust scroll bars...
	case SORTER_RESIZE_MSG:
		AdjustScrollBars();
		break;

	case SORTER_SELECT_MSG:
	case SORTER_INVOKE_MSG:
		fSorter->MessageReceived(theMessage);
		break;

	// Add the ref to our browser
	case ADD_REF_MSG:
		fSorter->MessageReceived(theMessage);
		break;

	default:
		BView::MessageReceived(theMessage);
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

void TElementsView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	// Draw simulated DragRect
	DrawDragRect();
}


//---------------------------------------------------------------------
//	DrawDragRect
//---------------------------------------------------------------------
//
//	Simulate standard Be DragRect

void TElementsView::DrawDragRect()
{
	PushState();

	BRect dragRect;
	GetDragRect(&dragRect);

	// Fill with background color
	SetHighColor(kBeGrey);
	FillRect(dragRect);

	// Only draw texture if we are active

	if ( Window()->IsActive() ) {
		// Draw shadow dots
		BPoint startPt, endPt;
		SetHighColor(kDkGrey);

		// Top row
		startPt.Set(dragRect.right-1, dragRect.top+4);
		endPt.Set(dragRect.right-1, dragRect.top+4);
		StrokeLine(startPt, endPt);

		// Second Row
		startPt.Set(dragRect.right-1, dragRect.top+7);
		endPt.Set(dragRect.right-1, dragRect.top+7);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Third Row
		startPt.Set(dragRect.right-1, dragRect.top+10);
		endPt.Set(dragRect.right-1, dragRect.top+10);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Bottom Row
		startPt.Set(dragRect.left+4, dragRect.top+13);
		endPt.Set(dragRect.left+4, dragRect.top+13);
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);

		// Draw Hilight dots
		SetHighColor(kWhite);

		// Top row
		startPt.Set(dragRect.right, dragRect.top+5);
		endPt.Set(dragRect.right, dragRect.top+5);
		StrokeLine(startPt, endPt);

		// Second Row
		startPt.Set(dragRect.right, dragRect.top+8);
		endPt.Set(dragRect.right, dragRect.top+8);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Third Row
		startPt.Set(dragRect.right, dragRect.top+11);
		endPt.Set(dragRect.right, dragRect.top+11);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Bottom Row
		startPt.Set(dragRect.left+5, dragRect.top+14);
		endPt.Set(dragRect.left+5, dragRect.top+14);
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);

		startPt.x       += 3;
		endPt.x         += 3;
		StrokeLine(startPt, endPt);
	}

	PopState();
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TElementsView::MouseDown(BPoint where)
{

	// Check and see if we are in our DragRect
	BRect dragRect;
	GetDragRect(&dragRect);

	if ( dragRect.Contains(where) ) {
		StartMouseWatcher(this);
	}

}


#pragma mark -
#pragma mark === Window Routines ===

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Force a redraw of our DragRect
//

void TElementsView::WindowActivated(bool state)
{
	BRect dragRect;
	GetDragRect(&dragRect);

	Invalidate(dragRect);

	// Inform parent
	BView::WindowActivated(state);
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TElementsView::FrameResized(float w,float h)
{
	AdjustScrollBars();
	DrawDragRect();

	BView::FrameResized(w, h);
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	GetDragRect
//---------------------------------------------------------------------
//
//	Return location of DragRect
//

void TElementsView::GetDragRect(BRect* dragRect)
{
	*dragRect               = Bounds();
	dragRect->left  = dragRect->right - kDragRectWidth;
	dragRect->top   = dragRect->bottom - kDragRectHeight;
}


//---------------------------------------------------------------------
//	AdjustScrollBars
//---------------------------------------------------------------------
//
//

void TElementsView::AdjustScrollBars()
{
	BRect scrollRect = Bounds();
	BRect scrollArea = fSorter->GetScrollArea();

	// Adjust horizontal scrollbar
	AdjustScrollBar( fHScroll, scrollRect.Width(), 200, scrollArea.Width(), scrollRect.left);

	/*
	   //   Vertical scrollbar area is total combined height of all channels
	   //	Iterate through all of the channels and calculate height
	   int32 totalHeight = 0;
	   for (int32 index = 0; index < fCueSheetView->GetTotalChannels(); index++)
	   {
	        TCueChannel *theChannel = (TCueChannel *)fCueSheetView->GetChannelList()->ItemAt(index);
	        if (theChannel)
	                totalHeight += theChannel->Bounds().Height();
	   }
	 */

	// Adjust vertical scrollbar
	//AdjustScrollBar( fVScroll, scrollRect.Height()-(kTimelineHeight+kToolbarHeight+kMenuHeight+10), kTickSpacing, totalHeight+kTimelineHeight+kToolbarHeight+kMenuHeight+1, scrollRect.top);

}

//---------------------------------------------------------------------
//	MakeRoomForButtons
//---------------------------------------------------------------------
//
//

void TElementsView::MakeRoomForButtons(float w)
{
	// This doesn't need to be called again!
	assert(fHScroll->Bounds().left == 0);
	// make room for the buttons
	fHScroll->ResizeBy(-w, 0);
	fHScroll->MoveBy(w, 0);
}

