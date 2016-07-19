//---------------------------------------------------------------------
//
//	File:	TStageTextView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.28.98
//
//	Desc:	Visual representation of a text cue on the stage
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "TStageTextView.h"

// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageTextView::TStageTextView(BRect bounds, char* name, TCueView* theCue) : TStageCue(bounds, name, theCue)
{

}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageTextView::~TStageTextView()
{

}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TStageTextView::Draw(BRect updateRect)
{


}



#pragma mark -
#pragma mark === Mouse Handling ===


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TStageTextView::MouseDown(BPoint where)
{

	// Wait a short while before dragging
	snooze(100 * 1000);

	// Is button down?  They are dragging or resizing the view...
	// Check to see if button is down
	uint32 buttons = 0;
	BPoint mousePt, savePt;

	GetMouse(&mousePt, &buttons, true);
	ConvertToParent(&mousePt);
	savePt = mousePt;

	while (buttons) {
		if (mousePt != savePt) {
			// Convert to parents coordinate system
			ConvertToParent(&mousePt);

			MoveBy( (mousePt.x - savePt.x), (mousePt.y - savePt.y) );

			// Save mouse location for next compare
			savePt = mousePt;
		}

		GetMouse(&mousePt, &buttons, true);
	}

	// Update the rects
	fChannelCue->SetArea(Frame());
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TStageTextView::MouseUp(BPoint where)
{

}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TStageTextView::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TStageTextView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TStageTextView::KeyDown(const char* bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TStageTextView::KeyUp(const char* bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TStageTextView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TStageTextView::FrameResized(float new_width, float new_height)
{
}


