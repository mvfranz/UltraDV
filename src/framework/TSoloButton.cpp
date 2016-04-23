//---------------------------------------------------------------------
//
//	File:	TSoloButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
///	Desc:	Solo button object.  The m_IsMuted flag in the Cue Channel acts as 
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

#include "TSoloButton.h"
#include "TCueSheetWindow.h"
#include "TCueChannel.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSoloButton::TSoloButton(TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Channel = parent;
	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
	// Store bitmaps
	m_OffBitmap = offBitmap;
	m_OnBitmap  = onBitmap;
		
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

TSoloButton::TSoloButton(BMessage *data) : BView(data)
{	
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;

	// Set bitmaps
	m_OffBitmap	= GetAppIcons()->m_SoloUp;
	m_OnBitmap 	= GetAppIcons()->m_SoloDown;
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSoloButton::~TSoloButton()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TSoloButton::Init()
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

BArchivable *TSoloButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TSoloButton") ) 
		return new TSoloButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TSoloButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TSoloButton");
		
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

void TSoloButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_Channel->GetSolo())
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

void TSoloButton::MouseDown(BPoint where)
{
		
	// Force redraw to reflect new state
	Invalidate();
	
	// Create and send message		
	BMessage *soloMessage = new BMessage(SOLO_BUTTON_MSG);
	short id = m_Channel->GetID();
	soloMessage->AddInt16("ChannelID", m_Channel->GetID());								
	(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(soloMessage, m_Channel);
	delete soloMessage;
		
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

void TSoloButton::MouseUp(BPoint where)
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

void TSoloButton::AttachedToWindow()
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

void TSoloButton::SetChannel(TCueChannel *channel)
{
	m_Channel = channel;	
}