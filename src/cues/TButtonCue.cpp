//---------------------------------------------------------------------
//
//	File:	TButtonCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:
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
#include "BaseCueChunk.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TButtonCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TButtonCue::TButtonCue(int16 id, TCueChannel* parent, BRect bounds, BPoint point,  long startTime) :
	TCueView(id, parent, bounds, point, startTime, "ButtonCue")
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a CueChunk

TButtonCue::TButtonCue(BaseCueChunk* theChunk, TCueChannel* parent, BRect bounds) : TCueView(theChunk, parent, bounds, "ButtonCue")
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

TButtonCue::TButtonCue(BMessage* theMessage) : TCueView(theMessage)
{
	// Load cue icon
	LoadCueIcon();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TButtonCue::~TButtonCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TButtonCue::Init()
{
	// Set up default duration
	fDuration = 1000;

	// Set up default settings
	fIsLocked = false;

	fIsSelected = false;

	fLowColor               = kWhite;
	fHighColor      = kBlack;
	fIsPrepared     = false;
	fIsPlaying      = false;

	fHasDuration    = true;
	fCanStretch     = true;

	fCanWindow              = false;
	fCanLoop                = false;
	fCanEnvelope    = false;
	/*
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
	if ( fChannel->CanInsertCue((TCueView*)this, fInsertPoint, true)) {
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

void TButtonCue::Init(BaseCueChunk* theChunk)
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

BArchivable* TButtonCue::Instantiate(BMessage* archive)
{
	if ( validate_instantiation(archive, "TButtonCue") )
		return new TButtonCue(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TButtonCue::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TButtonCue");
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

void TButtonCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();

	// Restore color
	SetHighColor(saveColor);

	// Pass up to parent
	TCueView::Draw(updateRect);

}

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TButtonCue::OpenEditor()
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

void TButtonCue::LoadCueIcon()
{
	BBitmap* cueIcon = NULL;

	cueIcon = GetCicnFromResource("ButtonCue");

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon = new TBitmapView(area, "ButtonCue", cueIcon, false);
		AddChild(fCueIcon);
	}
}
