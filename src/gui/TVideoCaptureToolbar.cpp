//---------------------------------------------------------------------
//
//	File:	TVideoCaptureToolbar.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.30.98
//
//	Desc:	Video Capture toolbar.  Contains the following:
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
#include "TVideoCaptureWindow.h"

#include "TVideoCaptureToolbar.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoCaptureToolbar::TVideoCaptureToolbar(BRect bounds) : BView(bounds, "VidCapToolbar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{

	// Default Initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoCaptureToolbar::~TVideoCaptureToolbar()
{
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

void TVideoCaptureToolbar::Init()
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
	                                   this, new BMessage(VIDCAP_RECORD_MSG), true, B_TWO_STATE_BUTTON);

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

void TVideoCaptureToolbar::Draw(BRect updateRect)
{
	// Setup environment
	rgb_color saveColor = HighColor();

	// Fill background
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


#pragma mark -
#pragma mark === Keyboard Handling ===

//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TVideoCaptureToolbar::KeyDown(const char* bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVideoCaptureToolbar::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	default:
		BView::MessageReceived(message);
		break;
	}
}
