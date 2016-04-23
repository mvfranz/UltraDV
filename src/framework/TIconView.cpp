//---------------------------------------------------------------------
//
//	File:	TIconView.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	This view shows the current media list as icons which are
//			freely positionable by the user. It is a child view to the 
//			Media tab in the Browser window.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TIconView.h"

#include <assert.h>		// For assert()

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TIconView::TIconView(BRect bounds) : 
		BView(bounds, "IconView", B_FOLLOW_ALL | B_FRAME_EVENTS, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TIconView::~TIconView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TIconView::Init()
{
}

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TIconView::Draw(BRect updateRect)
{
	// TODO: actually implement icon view drawing... (mo - 08/11/98)
	SetHighColor(255, 0, 0, 0);
	FillRect(updateRect);
}

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TIconView::FrameResized(float w, float h)
{
	AdjustScrollBars();	
	BView::FrameResized(w, h);	
}

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TIconView::TargetedByScrollView(BScrollView *view)
{
	AdjustScrollBars();
}


//---------------------------------------------------------------------
//	AdjustScrollBars
//---------------------------------------------------------------------
//
//

void TIconView::AdjustScrollBars()
{
	if (Parent() == 0)
		return;
		
	// if we have been targetted, look for a BScrollView parent.
	BScrollView* scroll = dynamic_cast<BScrollView*>(Parent());
	if (scroll == 0)
		return;
	
	// TODO: Set scroll bars based on overall size in relation to the Bounds()
	BScrollBar* bar = scroll->ScrollBar(B_HORIZONTAL);
	bar->SetRange(0, 100);

	bar = scroll->ScrollBar(B_VERTICAL);
	bar->SetRange(0, 100);
}
