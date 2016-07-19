//---------------------------------------------------------------------
//
//	File:	TCueSheetTimeView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
///	Desc:	Displays the cue sheet start time.  If the user clicks this
//			view, the cue sheet settings appear
//
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
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueSheetTimeView.h"
#include "TProjectSettings.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueSheetTimeView::TCueSheetTimeView(TCueSheetWindow *parent, BRect bounds) : BView(bounds, "TimeZoneView", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fCueSheetWindow = parent;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

TCueSheetTimeView::TCueSheetTimeView(BMessage *message) : BView(message)
{
	fCueSheetWindow = NULL;

	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetTimeView::~TCueSheetTimeView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TCueSheetTimeView::Init()
{

	// Set up memeber variables
	fProjectSettings = false;

}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueSheetTimeView::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TCueSheetTimeView") )
		return new TCueSheetTimeView(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetTimeView::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueSheetTimeView");

		// Add our member variables to the archive

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueSheetTimeView::Draw(BRect updateRect)
{
	// Set up environment
	PushState();

	const BRect bounds = Bounds();

	BPoint startPt, endPt;

	// Draw TimeRect frame
	SetHighColor(kLightGrey);
	FillRect(bounds);

	//	Draw black outline
	SetHighColor(kBlack);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);


	//	Draw shadow
	SetHighColor(kMediumGrey);
	startPt.Set(bounds.right-1, bounds.top);
	endPt.Set(bounds.right-1, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right-1, bounds.bottom);
	endPt.Set(bounds.left+1, bounds.bottom);
	StrokeLine(startPt, endPt);

	//	Draw highlight
	SetHighColor(kWhite);
	startPt.Set(bounds.left+1, bounds.bottom - 2);
	endPt.Set(bounds.left+1, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left+1, bounds.top);
	endPt.Set(bounds.right-2, bounds.top);
	StrokeLine(startPt, endPt);

	// Draw cue sheet start time
	BFont font;
	GetFont(&font);
	SetFont(be_bold_font);
	SetHighColor(kBlack);

	BPoint textPt;
	char timeStr[256];
	int32 startTime = fCueSheetWindow->GetCueSheetView()->StartTime();
	textPt.Set(bounds.left + 9, bounds.top + 16);
	TimeToString(startTime, fCueSheetWindow->GetCueSheetView()->GetTimeFormat(), timeStr, FALSE);
	DrawString(timeStr, textPt);

	// Restore environment
	PopState();
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueSheetTimeView::MouseDown(BPoint where)
{
	// Open up the cue sheet settings dialog
	ShowProjectSettingDialog(where);
}

#pragma mark -
#pragma mark === Dialog Routines ===

//---------------------------------------------------------------------
//	ShowProjectSettingDialog
//---------------------------------------------------------------------
//
//	Display the project setign dialog.  This allows the user to set the
//	time resolution, the start, duration and end times.
//

void TCueSheetTimeView::ShowProjectSettingDialog(BPoint where)
{

	// If we have created the dialog bring it to the front and show it
	if( fProjectSettings) {
		if (fProjectWindow) {
			fProjectWindow->Show();
			fProjectWindow->Activate(true);
		}
	}
	// Create the Project Settings dialog from a resource archive
	else{
		BMessage *theMessage = GetWindowFromResource("ProjectSettingsWindow");
		fProjectWindow = new TProjectSettings(this, fCueSheetWindow->GetCueSheetView(), theMessage);

		// Move it under the mouse
		//ConvertToScreen(&where);
		//fProjectWindow->MoveTo(where.x, where.y);
		CenterWindow(fProjectWindow);

		// Show the dialog
		fProjectWindow->Show();
	}
	return;
}

#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TCueSheetTimeView::AttachedToWindow()
{
	if(fCueSheetWindow == NULL) {
		fCueSheetWindow = (TCueSheetWindow *)Window();
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//

void TCueSheetTimeView::SetParent(TCueSheetWindow *parent)
{
	fCueSheetWindow = parent;
}