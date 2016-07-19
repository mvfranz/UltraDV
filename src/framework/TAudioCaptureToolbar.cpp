//---------------------------------------------------------------------
//
//	File:	TAudioCaptureToolbar.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.07.98
//
//	Desc:	Audio Capture toolbar.  Contains the following:
//
//				-- Record
//				-- Stop
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapButton.h"

#include "TAudioCaptureWindow.h"

#include "TAudioCaptureToolbar.h"



// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioCaptureToolbar::TAudioCaptureToolbar(BRect bounds) : BView(bounds, "VidCapToolbar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{

	// Default Initialization
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Create toolbar buttons
//

const short kButtonLeftOffset   = 5;
const short kButtonTopOffset    = 4;
const short kToolButtonWidth    = 16;
const short kToolButtonHeight   = 16;

void TAudioCaptureToolbar::Init()
{
	//
	// Create the toolbar buttons
	//

	BRect bounds;

	// Create Play button
	BBitmap* recordUp = GetIcon16FromResource("Record");
	BBitmap* recordDn = GetIcon16FromResource("AudioStopUp");
	bounds = Bounds();
	bounds.Set( kButtonLeftOffset, bounds.top + kButtonTopOffset, kToolButtonWidth+kButtonLeftOffset,
	            bounds.top + kButtonTopOffset + kToolButtonHeight );
	fRecordButton = new TBitmapButton( bounds, "RecordButton", recordUp, recordDn,
	                                   this, new BMessage(AUDCAP_RECORD_MSG), true, B_TWO_STATE_BUTTON);
	AddChild(fRecordButton);
	fRecordButton->Show();
	fRecordButton->SetTarget(Parent());
}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Fill with grey background
//

void TAudioCaptureToolbar::Draw(BRect updateRect)
{
	// Setup environment
	rgb_color saveColor = HighColor();

	// Fill background
	//SetHighColor(kLightGrey);
	SetHighColor(kBlueGrey);
	FillRect(Bounds());

	// Frame it
	BPoint endPt;
	SetHighColor(kWhite);
	MovePenTo(Bounds().left, Bounds().top+1);
	endPt.Set( Bounds().right, Bounds().top+1 );
	StrokeLine(endPt);

	SetHighColor(kMediumGrey);
	MovePenTo(Bounds().left, Bounds().bottom-1);
	endPt.Set( Bounds().right, Bounds().bottom-1 );
	StrokeLine(endPt);

	SetHighColor(kBlack);
	MovePenTo(Bounds().left, Bounds().top);
	endPt.Set( Bounds().right, Bounds().top );
	StrokeLine(endPt);
	MovePenTo(Bounds().left, Bounds().bottom);
	endPt.Set( Bounds().right, Bounds().bottom );
	StrokeLine(endPt);

	// Restore Environment
	SetHighColor(saveColor);
}
