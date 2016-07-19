//---------------------------------------------------------------------
//
//	File:	TMarkerCue.cpp
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
#include "MuseumUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TMarkerCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMarkerCue::TMarkerCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point,  long startTime) : 
			TCueView(id, parent, bounds, point, startTime, "MarkerCue")
{	
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a CueChunk

TMarkerCue::TMarkerCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds) : 
				TCueView(theChunk, parent, bounds, "MarkerCue")
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

TMarkerCue::TMarkerCue(BMessage *theMessage) : TCueView(theMessage)
{	
	// Load cue icon
	LoadCueIcon();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMarkerCue::~TMarkerCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TMarkerCue::Init()
{	
		
	// Set up default duration
	fDuration = 1000;
	
	// Set up default settings
	fIsSelected	= false;
	fIsLocked 		= false;	
	fLowColor 		= kWhite;	
	fHighColor 	= kBlack;	
	fIsPrepared 	= false;
	fIsPlaying 	= false;
	
	// Marker cues have no duration
	fHasDuration 		= false;
	
	fCanStretch 		= true;		// true if cue is stretchable
	fCanWindow 		= false;	// true if cue can window into file
	fCanLoop 			= false;	// true if cue can loop
	fCanEnvelope 		= false;	// true if cue can volume envelope
	fIsVisible 		= false;	// true if cue is visual
	fCanCrop 			= false;	// true if cue can visual crop
	fCanTransition 	= false;	// true if cue can transition
	fCanPath 			= false;	// true if cue can path
	fHasEditor 		= false;	// true if cue has internal editor
			
	// Default initialization
	TCueView::Init();
			
	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertPoint, true))
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

void TMarkerCue::Init(BaseCueChunk *theChunk)
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

BArchivable *TMarkerCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TMarkerCue") ) 
		return new TMarkerCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TMarkerCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TMarkerCue");						
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

void TMarkerCue::Draw(BRect updateRect)
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

void TMarkerCue::MouseDown(BPoint where)
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

void TMarkerCue::MouseUp(BPoint where)
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

void TMarkerCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
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

void TMarkerCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TMarkerCue::KeyDown(const char *bytes, int32 numBytes)
{
	TCueView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TMarkerCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TMarkerCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		default:
			TCueView::MessageReceived(message);						
			break;
	}
}

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TMarkerCue::OpenEditor()
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

void TMarkerCue::LoadCueIcon()
{
	BBitmap *cueIcon = NULL;
	
	cueIcon = GetCicnFromResource("MarkerCue");

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		fCueIcon = new TBitmapView(area, "MarkerCue",cueIcon, false);
		AddChild(fCueIcon);		
	}	
}
