//---------------------------------------------------------------------
//
//	File:	TLockButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
///	Desc:	Lock button.  Clicking will either lock or unlock the
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
#include "TMuseumIcons.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TLockButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TLockButton::TLockButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
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

TLockButton::TLockButton(BMessage *data) : BView(data)
{	
	m_Handler = NULL;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;

	// Set bitmaps
	m_OffBitmap	= GetAppIcons()->m_UnlockDark;
	m_OnBitmap 	= GetAppIcons()->m_LockDark;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TLockButton::~TLockButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TLockButton::Init()
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

BArchivable *TLockButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TLockButton") ) 
		return new TLockButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TLockButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TLockButton");
		
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

void TLockButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_Channel->IsLocked() )
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

void TLockButton::MouseDown(BPoint where)
{
				
	// Force redraw to reflect new state
	Invalidate();
	
	// Create and send message to channel		
	BMessage *message = new BMessage(LOCK_CHANNEL_MSG);
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

void TLockButton::MouseUp(BPoint where)
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

void TLockButton::AttachedToWindow()
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

void TLockButton::SetChannel(TCueChannel *channel)
{
	m_Channel = channel;	
}