//---------------------------------------------------------------------
//
//	File:	TRazorButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
///	Desc:	Razor Toolbar button.  Sets cursor to razor cursor and allows
//			user to cut cues into segments.
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

#include "TRazorButton.h"
#include "TToolbar.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TRazorButton::TRazorButton(TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Parent = parent;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
	// Store bitmaps
	m_OffBitmap = offBitmap;
	m_OnBitmap = onBitmap;
		
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

TRazorButton::TRazorButton(BMessage *data) : BView(data)
{
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
 	m_OffBitmap = GetIcon16FromResource("RazorDown");
	m_OnBitmap  = GetIcon16FromResource("RazorUp");
	
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TRazorButton::~TRazorButton()
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

void TRazorButton::Init()
{        	
}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TRazorButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TRazorButton") ) 
		return new TRazorButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TRazorButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TRazorButton");
		
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

void TRazorButton::Draw(BRect updateRect)
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

void TRazorButton::MouseDown(BPoint where)
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

void TRazorButton::AttachedToWindow()
{			
	if(m_Parent == NULL)
	{
		m_Parent 	= (TToolbar *)Parent();
		m_Handler 	= (TToolbar *)Parent();					
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}

