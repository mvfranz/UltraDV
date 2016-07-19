//---------------------------------------------------------------------
//
//	File:	TSizeScrollView.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.18.98
//
//	Desc:	This class is derived from BScrollView, with an additional
//			wrinkle: it has a size control and sends sizing messages
//			to its container window. Other interface behaves like
//			BScrollView.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TSizeScrollView.h"
#include "TMouseWatcher.h"      // for MW_MOUSE_MOVED, et al

// Constants
static const rgb_color kDkGrey = {100, 100, 100, 255};
static const rgb_color kWhite   = {255, 255, 255, 255};
static const rgb_color kBeGrey  = {216, 216, 216, 255};


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSizeScrollView::TSizeScrollView(const char *name, BView *target,
                                 uint32 resizeMask, uint32 flags, bool horizontal,
                                 bool vertical, border_style border) :
	// defaults:
	//		resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP
	//              flags = 0
	//		horizontal = false
	//		vertical = false
	//		border_style border = B_FANCY_BORDER

	BScrollView(name, target, resizeMask, flags, horizontal,
	            vertical, border)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSizeScrollView::~TSizeScrollView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TSizeScrollView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TSizeScrollView::MessageReceived(BMessage *theMessage)
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
			BRect dragRect;
			GetDragRect(&dragRect);

			frame.right += dragRect.Width() / 2;
			frame.bottom += dragRect.Height() / 2;

			// Resize
			Window()->ResizeTo(frame.Width(), frame.Height());
		}
	}
	break;

	default:
		BView::MessageReceived(theMessage);
		break;

	}
}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TSizeScrollView::MouseDown(BPoint where)
{

	// Check and see if we are in our DragRect
	BRect dragRect;
	GetDragRect(&dragRect);

	if ( dragRect.Contains(where) ) {
		StartMouseWatcher(this);
	}

}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TSizeScrollView::Draw(BRect updateRect)
{
	BScrollView::Draw(updateRect);
	// Draw simulated DragRect
	DrawDragRect();
}

//---------------------------------------------------------------------
//	DrawDragRect
//---------------------------------------------------------------------
//
//	Simulate standard Be DragRect

void TSizeScrollView::DrawDragRect()
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
		startPt.Set(dragRect.right-2, dragRect.top+3);
		endPt.Set(dragRect.right-2, dragRect.top+3);
		StrokeLine(startPt, endPt);

		// Second Row
		startPt.Set(dragRect.right-2, dragRect.top+6);
		endPt.Set(dragRect.right-2, dragRect.top+6);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Third Row
		startPt.Set(dragRect.right-2, dragRect.top+9);
		endPt.Set(dragRect.right-2, dragRect.top+9);
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		startPt.x       -= 3;
		endPt.x         -= 3;
		StrokeLine(startPt, endPt);

		// Bottom Row
		startPt.Set(dragRect.left+3, dragRect.top+12);
		endPt.Set(dragRect.left+3, dragRect.top+12);
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
	}

	PopState();
}

#pragma mark -
#pragma mark === Support Routines ===

//---------------------------------------------------------------------
//	GetDragRect
//---------------------------------------------------------------------
//
//	Return location of DragRect
//

void TSizeScrollView::GetDragRect(BRect *dragRect)
{
	*dragRect               = Bounds();
	// TODO: respect the size of the scroll bars for this
	static const int kDragRectWidth = 14;
	static const int kDragRectHeight = 14;

	dragRect->left  = dragRect->right - kDragRectWidth;
	dragRect->top   = dragRect->bottom - kDragRectHeight;
}
