//---------------------------------------------------------------------
//
//	File:	TExpandButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
///	Desc:	Expand button.  Clikcing will either expand or contract the
//			channel depending upon it's current state
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
#include "TMuseumIcons.h"
#include "ResourceManager.h"

#include "TExpandButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExpandButton::TExpandButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Channel = parent;
	
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

TExpandButton::TExpandButton(BMessage *message) : BView(message)
{
	m_Handler = NULL;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;

	// Set bitmaps
	m_OffBitmap = GetAppIcons()->m_Expand;
	m_OnBitmap 	= GetAppIcons()->m_Contract;	
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TExpandButton::~TExpandButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TExpandButton::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);        	
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TExpandButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TExpandButton") ) 
		return new TExpandButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TExpandButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TExpandButton");
		
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
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TExpandButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_Channel->IsExpanded())
		DrawBitmap(m_OnBitmap, B_ORIGIN);
	else
		DrawBitmap(m_OffBitmap, B_ORIGIN);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TExpandButton::MouseDown(BPoint where)
{
				
	// Force redraw to reflect new state
	Invalidate();
	
	// Create and send message to channel, header and window		
	BMessage *message = new BMessage(CHANNEL_EXPAND_MSG);
	(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, m_Channel);
	delete message;
		
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	m_MouseDown = true; 
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TExpandButton::MouseUp(BPoint where)
{
	
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this		
	m_MouseDown = false; 
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

void TExpandButton::AttachedToWindow()
{			
	if (m_Handler == NULL)
	{
		// Set target
		m_Handler = (TCueSheetWindow *)Window();

	}	
	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//

void TExpandButton::SetChannel(TCueChannel *channel)
{
	m_Channel = channel;	
}