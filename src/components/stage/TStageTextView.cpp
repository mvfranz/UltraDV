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
