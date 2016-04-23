//---------------------------------------------------------------------
//
//	File:	TAudioZoomInButton.cpp
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

#include "TAudioZoomInButton.h"
#include "TAudioEditorToolbar.h"
#include "TAudioEditor.h"
#include "TAudioEditorView.h"
#include "TToolbar.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioZoomInButton::TAudioZoomInButton(TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_Parent = parent;
	
	// Save handler
	m_Handler = handler;

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
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioZoomInButton::~TAudioZoomInButton()
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

void TAudioZoomInButton::Init()
{        	
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioZoomInButton::Draw(BRect updateRect)
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

void TAudioZoomInButton::MouseDown(BPoint where)
{				
	// Handle button click
	DoClick();		
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioZoomInButton::MouseUp(BPoint where)
{
	
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this		
	//m_MouseDown = false; 
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioZoomInButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioZoomInButton::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioZoomInButton::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioZoomInButton::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioZoomInButton::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioZoomInButton::FrameResized(float new_width, float new_height)
{
}


#pragma mark -
#pragma mark === Button Handling ===


//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Zoom In button has been clicked
//

void TAudioZoomInButton::DoClick()
{
	
	// We will always be in the up osition when we start.
	// First, set the button state to down and force a redraw...
	m_MouseDown = true; 
	Draw(Bounds()); 
		
	// Now send message to the view that the user is zooming in...
	BMessage *message = new BMessage(AUDIO_ZOOMIN_BUTTON_MSG);
	m_Parent->Window()->PostMessage(message, (( TAudioEditor *)m_Parent->Window())->GetEditorView() );
	delete message;
	
	// Wait a short while before restoring to up position
	snooze(100 * 1000);
	
	// Restore and invalidate
	m_MouseDown = false; 
	Draw(Bounds()); 
}