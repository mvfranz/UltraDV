//---------------------------------------------------------------------
//
//	File:	TVideoCue.cpp
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
#include <assert.h>

#include "AppConstants.h"
#include "AppMessages.h"
// ABH #include "MuseumUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TVideoCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoCue::TVideoCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point,  long startTime) :
	TCueView(id, parent, bounds, point, startTime, "VideoCue")
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a CueChunk

TVideoCue::TVideoCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds) :
	TCueView(theChunk, parent, bounds, "VideoCue")
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

TVideoCue::TVideoCue(BMessage *theMessage) : TCueView(theMessage)
{
	// Load cue icon
	LoadCueIcon();
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoCue::~TVideoCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoCue::Init()
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

	/*
	   bool			fCanStretch;			// true if cue is stretchable
	   bool			fCanWindow;			// true if cue can window into file
	   bool			fCanLoop;				// true if cue can loop
	   bool			fCanEnvelope;			// true if cue can volume envelope
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
// ABH old bad	if ( fChannel->CanInsertCue(this, fInsertPoint, true))
	if ( fChannel->CanInsertCue(this, fInsertTime, true)) {
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

void TVideoCue::Init(BaseCueChunk *theChunk)
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

BArchivable *TVideoCue::Instantiate(BMessage *archive)
{
	if ( validate_instantiation(archive, "TVideoCue") )
		return new TVideoCue(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TVideoCue::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TVideoCue");
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

void TVideoCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();

	// Restore color
	SetHighColor(saveColor);

	// Pass up to parent
	TCueView::Draw(updateRect);

}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TVideoCue::MouseDown(BPoint where)
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

void TVideoCue::MouseUp(BPoint where)
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

void TVideoCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
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

void TVideoCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TVideoCue::KeyDown(const char *bytes, int32 numBytes)
{
	TCueView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TVideoCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TVideoCue::MessageReceived(BMessage *message)
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

void TVideoCue::OpenEditor()
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

void TVideoCue::LoadCueIcon()
{
	BBitmap *cueIcon = NULL;

	cueIcon = GetCicnFromResource("VideoCue");

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
// ABH old		fCueIcon = new TBitmapView(area, cueIcon, false);
		printf("Added 'VideoCue' to TBitmapView text\n");
		fCueIcon = new TBitmapView(area, "VideoCue", cueIcon, false);
		AddChild(fCueIcon);
	}
}
