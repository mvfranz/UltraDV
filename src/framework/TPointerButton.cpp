//---------------------------------------------------------------------
//
//	File:	TPointerButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
///	Desc:	Pointer Toolbar button.  Clicking restes cursor to pointer 
//			functionality
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
#include "ResourceManager.h"

#include "TPointerButton.h"
#include "TToolbar.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPointerButton::TPointerButton(TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Parent = parent;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
	// Store bitmaps
	m_OffBitmap = offBitmap;
	m_OnBitmap 	= onBitmap;
		
	// Store target and handler
	m_Handler = handler;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPointerButton::TPointerButton(BMessage *data) : BView(data)
{
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
	m_OffBitmap = GetIcon16FromResource("PointerDown");
	m_OnBitmap  = GetIcon16FromResource("PointerUp");
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPointerButton::~TPointerButton()
{
	// Free data
	delete m_OffBitmap;
	delete m_OnBitmap;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TPointerButton::Init()
{        	
}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TPointerButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TPointerButton") ) 
		return new TPointerButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TPointerButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TPointerButton");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{		
	
		}
	}
	
	Looper()->Unlock();
	
	return myErr;
}	

#pragma mark -
#pragma mark === Drawing Routines


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TPointerButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_MouseDown)
		DrawBitmap(m_OffBitmap, B_ORIGIN);		
	else
		DrawBitmap(m_OnBitmap, B_ORIGIN);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TPointerButton::MouseDown(BPoint where)
{
				
	// Force redraw to reflect new state
	//Invalidate();
	
	// Create and send message to channel and header		
	//BMessage *message = new BMessage(CHANNEL_EXPAND_MSG);
	//(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, m_Channel);
	//delete message;
		
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	if (m_MouseDown)
		m_MouseDown = false; 
	else
		m_MouseDown = true; 
		
	Invalidate();
}

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TPointerButton::AttachedToWindow()
{			
	if(m_Parent == NULL)
	{
		m_Parent 	= (TToolbar *)Parent();
		m_Handler 	= (TToolbar *)Parent();					
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}
