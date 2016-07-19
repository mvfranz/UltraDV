//---------------------------------------------------------------------
//
//	File:	TAnimationCue.cpp
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
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TVisualCue.h"
#include "TAnimationCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAnimationCue::TAnimationCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) : 
			TVisualCue(id, parent, bounds, startTime, "AnimationCue")
{	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TAnimationCue::TAnimationCue(BMessage *theMessage) : TVisualCue(theMessage)
{	
	// Load cue icon
	LoadCueIcon();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAnimationCue::~TAnimationCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TAnimationCue::Init()
{	

	// Set up default duration in milliseconds
	fDuration = 1000;

	// Set up default settings
	fIsLocked 		= false;
	
	fIsSelected 	= false;
	
	fLowColor 		= kWhite;	
	fHighColor 	= kBlack;	
	fIsPrepared 	= false;
	fIsPlaying 	= false;
	
	fHasDuration 	= true;
	
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
	TVisualCue::Init();
	
	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertTime, true))
	{
		fChannel->InsertCue(this, fInsertTime);		
		Select();
		
		// We are now fully instantiated
		fIsInstantiated = true;
	}
	
	// Set expanded status
	if (fChannel->IsExpanded())
	{
		fIsExpanded = false;
		Expand();
	}
	else
	{
		fIsExpanded = true;
		Contract();		
	}	
}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TAnimationCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TAnimationCue") ) 
		return new TAnimationCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TAnimationCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TVisualCue::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TAnimationCue");						
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

void TAnimationCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();
		
	// Restore color
	SetHighColor(saveColor);
	
	// Pass up to parent
	TVisualCue::Draw(updateRect);
}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAnimationCue::MouseDown(BPoint where)
{
	// Pass up to parent
	TVisualCue::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAnimationCue::MouseUp(BPoint where)
{
	// Pass up to parent
	TVisualCue::MouseUp(where);
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAnimationCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Pass up to parent
	TVisualCue::MouseMoved(where, code, a_message);
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAnimationCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAnimationCue::KeyDown(const char *bytes, int32 numBytes)
{
	TVisualCue::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAnimationCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TAnimationCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{	
		default:
			TVisualCue::MessageReceived(message);						
			break;
	}
}

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TAnimationCue::OpenEditor()
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

void TAnimationCue::LoadCueIcon()
{
	BBitmap *cueIcon = GetAppIcons()->fAnimationUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		fCueIcon = new TBitmapView(area, "AnimationBitmap", cueIcon, false);
		AddChild(fCueIcon);		
	}

	//	Pass up to parent
	TCueView::LoadCueIcon();	

}

