//---------------------------------------------------------------------
//
//	File:	TCueButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Button object for use in cue palettes
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

#include "TPaletteStatusView.h"

#include "TCueButton.h"



// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueButton::TCueButton(BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BMessage *message, short ID) : BView(bounds, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	// Set MouseDown/MouseUp flag
	m_MouseDown = false;
	
	// Store bitmaps
	m_OffBitmap = offBitmap;
	m_OnBitmap = onBitmap;
	
	// Store message
	m_Message = message;
	
	// Set Cue ID
	m_ID = ID;
	
	// Default state is off
	m_State = false;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueButton::~TCueButton()
{
	if (m_Message)		
		delete m_Message;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TCueButton::Init()
{ 
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);   	
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state
	// If m_State is true, draw on bitmap
	if (m_State)
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

void TCueButton::MouseDown(BPoint where)
{
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

void TCueButton::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueButton::MouseMoved( BPoint where, uint32 code, const BMessage *message )
{

	// Check to see if button is down
	uint32 	buttons = 0;
	BPoint	point;
					
	Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);
	if (buttons && m_MouseDown)
	{
		// 	If the mouse button is down, they want to drag a cue off the palette
		//	and most likely onto the cue sheet.
		// We use the DragMessage function to achieve this
		BMessage message(CUE_ICON_DRAG_MSG);
		BPoint centerPt( Bounds().Width() / 2, Bounds().Height() / 2 );
		message.AddInt16("CueIconID", m_ID);
		
		// Create a copy of the bitmap for dragging
		BBitmap *dragBitmap = new BBitmap( m_OnBitmap->Bounds(), m_OnBitmap->ColorSpace() );
		dragBitmap->SetBits( m_OnBitmap->Bits(), m_OnBitmap->BitsLength(), 0, m_OnBitmap->ColorSpace() );
		DragMessage(&message, dragBitmap, centerPt);
		
		// Set our flag to indicate mouse up happened
		m_MouseDown = false; 
	}
	// Show button depressed state
	else
	{
		// Do nothing if mouse button is depressed
		uint32 	buttons = 0;
		GetMouse(&where, &buttons, true);		
		if (buttons)
			return;
		
		switch (code)
		{
			case B_ENTERED_VIEW:
				{
					m_State = true;
					Invalidate();
					BMessage *theMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
					theMessage->AddString("CueName", Name());
					if (Window())
						Window()->PostMessage(theMessage);
					delete theMessage;
				}
				break;
				
			case B_EXITED_VIEW :
				{
					m_State = false;
					Invalidate();
					BMessage *theMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
					theMessage->AddString("CueName", "");
					if (Window())
						Window()->PostMessage(theMessage);
					delete theMessage;
				}
				break;
				
			default:
				break;	
		}
	}
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TCueButton::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TCueButton::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TCueButton::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TCueButton::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TCueButton::FrameResized(float new_width, float new_height)
{
}

