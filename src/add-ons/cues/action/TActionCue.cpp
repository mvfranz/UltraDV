//---------------------------------------------------------------------
//
//	File:	TActionCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Implilentation of the Action cue.  An action cue can perform the following
//			actions:
//				--None
//				--Pause/Continue
//				--Jump to Time
//				--Jump to Market
//				--Return From Last Jump
//				--Run Script
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
// ABH #include "MuseumUtils.h"
#include "BaseCueChunk.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TActionCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TActionCue::TActionCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point,  long startTime) :
	TCueView(id, parent, bounds, point, startTime, "ActionCue")
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a CueChunk

TActionCue::TActionCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds) :
	TCueView(theChunk, parent, bounds, "ActionCue")
{
	// Perform default initialization
	Init(theChunk);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TActionCue::TActionCue(BMessage *theMessage) : TCueView(theMessage)
{
	// Load cue icon
	LoadCueIcon();
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TActionCue::~TActionCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TActionCue::Init()
{

	// Set up default settings
	fIsLocked               = false;

	fIsSelected     = false;

	fLowColor               = kWhite;
	fHighColor      = kBlack;
	fIsPrepared     = false;
	fIsPlaying      = false;

	// Action cues have no duration
	fHasDuration    = false;
	fCanStretch     = true;

	fCanEnvelope    = false;

	/*
	   bool			fCanWindow;			// true if cue can window into file
	   bool			fCanLoop;				// true if cue can loop
	   bool			fHasDuration;			// true if cue has a duration
	   bool			fIsVisible;			// true if cue is visual
	   bool			fCanCrop;				// true if cue can visual crop
	   bool			fCanTransition;		// true if cue can transition
	   bool			fCanTransitionOut;		// true if cue can transition out
	   bool			fCanPath;				// true if cue can path
	   bool			fHasEditor;			// true if cue has internal editor

	   BRect			fOriginalArea;			// Visual size of untouched image
	   BRect			fArea;					// Total visual area (in stage coords).
	   BRect			fCroppedArea;			// Crop area (in itsArea coords).
	 */

	// Default initialization
	TCueView::Init();

	// Add the cue to the cue channel
// ABH
//	if ( fChannel->CanInsertCue(this, fInsertPoint, true))
//	if ( fChannel->CanInsertCue((TCueView*)this, fInsertPoint, true))
	{
		fChannel->AddChild(this);
		fChannel->InsertCue(this, fInsertPoint, fInsertTime);
		Select();
	}

	// We are now fully instantiated
	fIsInstantiated = true;

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Init from BaseCueChunk
//

void TActionCue::Init(BaseCueChunk *theChunk)
{
	TCueView::Init(theChunk);
}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TActionCue::Instantiate(BMessage *archive)
{
	if ( validate_instantiation(archive, "TActionCue") )
		return new TActionCue(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TActionCue::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TActionCue");
	}

	Looper()->Unlock();

	return myErr;
}


#pragma mark -
#pragma mark === Drawing Functions ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TActionCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();

	// Restore color
	SetHighColor(saveColor);

	TCueView::Draw(updateRect);
}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TActionCue::MouseDown(BPoint where)
{
	// Pass up to parent
	TCueView::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TActionCue::MouseUp(BPoint where)
{
	// Pass up to parent
	TCueView::MouseUp(where);
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TActionCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Pass up to parent
	TCueView::MouseMoved(where, code, a_message);
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TActionCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TActionCue::KeyDown(const char *bytes, int32 numBytes)
{
	TCueView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TActionCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TActionCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		TCueView::MessageReceived(message);
		break;
	}
}


#pragma mark -
#pragma mark === Editor ===

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TActionCue::OpenEditor()
{
}


#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TActionCue::LoadCueIcon()
{
	BBitmap *cueIcon = NULL;

	cueIcon = GetCicnFromResource("ActionCue");

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
// ABH		fCueIcon = new TBitmapView(area, cueIcon, false);
		fCueIcon = new TBitmapView(area, "ActionCue", cueIcon, false);
		AddChild(fCueIcon);
	}
}

