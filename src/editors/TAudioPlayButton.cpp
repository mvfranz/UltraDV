//---------------------------------------------------------------------
//
//	File:	TAudioPlayButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
///	Desc:	Audio Editor Play button. Clicking cause adio file to be played
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

#include "TAudioPlayButton.h"
#include "TAudioEditorToolbar.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioPlayButton::TAudioPlayButton(TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Parent = parent;
	
	// Save handler
	m_Handler = handler;
	
	// button state flag
	m_ButtonState = false;
	
	// Store bitmaps
	m_OffBitmap = offBitmap;
	m_OnBitmap = onBitmap;
			
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioPlayButton::~TAudioPlayButton()
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

void TAudioPlayButton::Init()
{        	
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioPlayButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if (m_ButtonState)
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

void TAudioPlayButton::MouseDown(BPoint where)
{
	
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this
	if (m_ButtonState)
	{
		// Tell sound to stop
		BMessage *message = new BMessage(AUDIO_STOP_BUTTON_MSG);
		//m_Parent->Window()->PostMessage(message, m_Handler);
		m_Parent->Window()->PostMessage(message, NULL);
		delete message;				
		m_ButtonState = false; 
	}
	else
	{	
		// Tell sound to play
		BMessage *message = new BMessage(AUDIO_PLAY_BUTTON_MSG);
		//m_Parent->Window()->PostMessage(message, m_Handler);
		m_Parent->Window()->PostMessage(message, NULL);
		delete message;
		m_ButtonState = true; 
	}
		
	// Force redraw to reflect new state
	Invalidate(); 
}
