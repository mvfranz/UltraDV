//---------------------------------------------------------------------
//
//	File:	TMuteButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	Mute button object.  The m_IsMuted flag in the Cue Channel acts as 
//			the state flag
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

#include "TMuteButton.h"
#include "TCueSheetWindow.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMuteButton::TMuteButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
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

TMuteButton::TMuteButton(BMessage *data) : BView(data)
{	
	m_Handler = NULL;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;

	// Set bitmaps
	m_OffBitmap	= GetAppIcons()->m_MuteUp;
	m_OnBitmap 	= GetAppIcons()->m_MuteDown;
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMuteButton::~TMuteButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TMuteButton::Init()
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

BArchivable *TMuteButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TMuteButton") ) 
		return new TMuteButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TMuteButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TMuteButton");
		
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

void TMuteButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_Channel->GetMute())
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

void TMuteButton::MouseDown(BPoint where)
{	
	// Force redraw to reflect new state
	Invalidate();
	
	// Create and send message		
	BMessage *muteMessage = new BMessage(MUTE_BUTTON_MSG);
	short id = m_Channel->GetID();
	muteMessage->AddInt16("ChannelID", m_Channel->GetID());								
	(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(muteMessage, m_Channel);	
	// Clean up
	delete muteMessage;
		
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

void TMuteButton::MouseUp(BPoint where)
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

void TMuteButton::AttachedToWindow()
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

void TMuteButton::SetChannel(TCueChannel *channel)
{
	m_Channel = channel;	
}