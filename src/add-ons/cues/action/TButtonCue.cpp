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
//#include "MuseumUtils.h"
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

TButtonCue::TButtonCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point,  long startTime) : 
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

TButtonCue::TButtonCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds) : TCueView(theChunk, parent, bounds, "ButtonCue")
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

TButtonCue::TButtonCue(BMessage *theMessage) : TCueView(theMessage)
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
	m_Duration = 1000;

	// Set up default settings
	m_IsLocked = false;
	
	m_IsSelected = false;
	
	m_LowColor 		= kWhite;	
	m_HighColor 	= kBlack;	
	m_IsPrepared 	= false;
	m_IsPlaying 	= false;
	
	m_HasDuration 	= true;
	m_CanStretch   	= true;
	
	m_CanWindow		= false;
	m_CanLoop		= false;
	m_CanEnvelope	= false;
	/*
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
	TCueView::Init();

	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue((TCueView*)this, m_InsertPoint, true))
	{
		m_Channel->AddChild(this);
		m_Channel->InsertCue(this, m_InsertPoint, m_InsertTime);		
		Select();								
	}

	// We are now fully instantiated
	m_IsInstantiated = true;
	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Init from BaseCueChunk	
//

void TButtonCue::Init(BaseCueChunk *theChunk)
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

BArchivable *TButtonCue::Instantiate(BMessage *archive) 
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

status_t TButtonCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
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
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TButtonCue::MouseDown(BPoint where)
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

void TButtonCue::MouseUp(BPoint where)
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

void TButtonCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
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

void TButtonCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TButtonCue::KeyDown(const char *bytes, int32 numBytes)
{
	TCueView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TButtonCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TButtonCue::MessageReceived(BMessage *message)
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
	BBitmap *cueIcon = NULL;
	
	cueIcon = GetCicnFromResource("ButtonCue");

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "ButtonCue", cueIcon, false);
		AddChild(m_CueIcon);		
	}	
}
