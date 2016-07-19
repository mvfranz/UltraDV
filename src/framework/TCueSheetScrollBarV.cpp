//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollBarV.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Vertical scroll bar to scroll CueSheet
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

#include "TCueSheetScrollBarV.h"
#include "TCueSheetWindow.h"
#include "THeaderContainerView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueSheetScrollBarV::TCueSheetScrollBarV( TCueSheetWindow *parent, BRect frame, BView *target, float min, float max ) :
	BScrollBar( frame, "VCueScroll", target, min, max, B_VERTICAL)
{
	fCueSheetWindow = parent;

	// Do default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Contruct from an archive
//

TCueSheetScrollBarV::TCueSheetScrollBarV(BMessage *data ) : BScrollBar(data)
{
	fCueSheetWindow = NULL;

	// Do default initialization
	Init();
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetScrollBarV::~TCueSheetScrollBarV()
{

}

#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueSheetScrollBarV::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TCueSheetScrollBarV") )
		return new TCueSheetScrollBarV(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetScrollBarV::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BScrollBar::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueSheetScrollBarV");

		// Add our member variables to the archive

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TCueSheetScrollBarV::Init()
{
	//      Get current scrollvalue and save it in our tracking variable.
	//	We use this value to scroll the HeaderZone in concert with
	//	the CueSheetView
	fLastValue = Value();
}

//---------------------------------------------------------------------
//	ValueChanged
//---------------------------------------------------------------------
//
//

void TCueSheetScrollBarV::ValueChanged(float newValue)
{
	// Pass message to base class
	BScrollBar::ValueChanged(newValue);

	// We need to offset the HeaderContainer as well...
	if (newValue != fLastValue) {
		if (newValue > fLastValue)
			fCueSheetWindow->GetHeaderContainer()->ScrollBy(0, newValue - fLastValue);
		else
			fCueSheetWindow->GetHeaderContainer()->ScrollBy(0, -(fLastValue - newValue));

		// Save value for next compare
		fLastValue = newValue;
	}
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

void TCueSheetScrollBarV::AttachedToWindow()
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
void TCueSheetScrollBarV::SetParent(TCueSheetWindow *parent)
{
	fCueSheetWindow = parent;
}