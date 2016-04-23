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
	m_Duration = 1000;

	// Set up default settings
	m_IsLocked 		= false;
	
	m_IsSelected 	= false;
	
	m_LowColor 		= kWhite;	
	m_HighColor 	= kBlack;	
	m_IsPrepared 	= false;
	m_IsPlaying 	= false;
	
	m_HasDuration 	= true;
	
	/*
	bool			m_CanStretch;			// true if cue is stretchable
	bool			m_CanWindow;			// true if cue can window into file
	bool			m_CanLoop;				// true if cue can loop
	bool			m_CanEnvelope;			// true if cue can volume envelope
	bool			m_HasDuration;			// true if cue has a duration
	bool			m_IsVisible;			// true if cue is visual
	bool			m_CanCrop;				// true if cue can visual crop
	bool			m_CanTransition;		// true if cue can transition
	bool			m_CanTransitionOut;		// true if cue can transition out
	bool			m_CanPath;				// true if cue can path
	bool			m_HasEditor;			// true if cue has internal editor
		
	BRect			m_OriginalArea;			// Visual size of untouched image
	BRect			m_Area;					// Total visual area (in stage coords).
	BRect			m_CroppedArea;			// Crop area (in itsArea coords).
	*/
	
	// Default initialization
	TVisualCue::Init();
	
	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue(this, m_InsertTime, true))
	{
		m_Channel->InsertCue(this, m_InsertTime);		
		Select();
		
		// We are now fully instantiated
		m_IsInstantiated = true;
	}
	
	// Set expanded status
	if (m_Channel->IsExpanded())
	{
		m_IsExpanded = false;
		Expand();
	}
	else
	{
		m_IsExpanded = true;
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
	BBitmap *cueIcon = GetAppIcons()->m_AnimationUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "AnimationBitmap", cueIcon, false);
		AddChild(m_CueIcon);		
	}

	//	Pass up to parent
	TCueView::LoadCueIcon();	

}

