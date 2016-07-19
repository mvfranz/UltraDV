//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Horizontal scroll bar to scroll CueSheet
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

#include "TCueSheetScrollBarH.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"
#include "TExportZone.h"
#include "TTimelineView.h"



// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueSheetScrollBarH::TCueSheetScrollBarH( TCueSheetWindow *parent, BRect frame, BView *target, float min, float max ) :
	BScrollBar( frame, "HCueScroll", target, min, max, B_HORIZONTAL)
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

TCueSheetScrollBarH::TCueSheetScrollBarH(BMessage *data ) : BScrollBar(data)
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

TCueSheetScrollBarH::~TCueSheetScrollBarH()
{

}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueSheetScrollBarH::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TCueSheetScrollBarH") )
		return new TCueSheetScrollBarH(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetScrollBarH::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BScrollBar::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueSheetScrollBarH");

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

void TCueSheetScrollBarH::Init()
{
	//      Get current scroll value and save it in our tracking variable.
	//	We use this value to scroll the HeaderZone in concert with
	//	the CueSheetView
	fLastValue = Value();
}

//---------------------------------------------------------------------
//	ValueChanged
//---------------------------------------------------------------------
//
//

void TCueSheetScrollBarH::ValueChanged(float newValue)
{
	// Pass message to base class
	BScrollBar::ValueChanged(newValue);

	// We need to offset the Export Zone, TimeLine and CueChannels...
	if (newValue != fLastValue) {
		if (newValue > fLastValue) {
			fCueSheetWindow->GetExportZone()->ScrollBy(newValue - fLastValue, 0);
			fCueSheetWindow->GetTimeline()->ScrollBy(newValue - fLastValue, 0);

			//	Handle cue channels
			BList *channelList = fCueSheetWindow->GetCueSheetView()->GetChannelList();
			for (int32 channelNum = 0; channelNum < channelList->CountItems(); channelNum++) {
				TCueChannel *theChannel = (TCueChannel *)channelList->ItemAt(channelNum);
				if (theChannel)
					theChannel->ScrollBy(newValue - fLastValue, 0);
			}
		} else   {
			fCueSheetWindow->GetExportZone()->ScrollBy(-(fLastValue - newValue), 0);
			fCueSheetWindow->GetTimeline()->ScrollBy(-(fLastValue - newValue), 0);

			//	Handle cue channels
			BList *channelList = fCueSheetWindow->GetCueSheetView()->GetChannelList();
			for (int32 channelNum = 0; channelNum < channelList->CountItems(); channelNum++) {
				TCueChannel *theChannel = (TCueChannel *)channelList->ItemAt(channelNum);
				if (theChannel)
					theChannel->ScrollBy(-(fLastValue - newValue), 0);
			}

		}

		// Force a redraw of the items
		fCueSheetWindow->GetExportZone()->Invalidate();
		BRect bounds = fCueSheetWindow->GetTimeline()->Bounds();
		fCueSheetWindow->GetTimeline()->Invalidate(bounds);

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

void TCueSheetScrollBarH::AttachedToWindow()
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
void TCueSheetScrollBarH::SetParent(TCueSheetWindow *parent)
{
	fCueSheetWindow = parent;
}