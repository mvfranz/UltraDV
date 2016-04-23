//---------------------------------------------------------------------
//
//	File:	TChannelCueButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.17.98
//
///	Desc:	Abstract cue button class.  
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

#include "TChannelCueButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueView.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChannelCueButton::TChannelCueButton(TCueView *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler, uint32 flags) : BView(bounds, name, flags, B_WILL_DRAW)
{
	// Save parent view
	m_Cue = parent;
	
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

TChannelCueButton::TChannelCueButton(const TChannelCueButton *theButton) : BView(theButton->Bounds(), theButton->Name(), B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Create copy
	m_Cue 		= theButton->m_Cue;
	m_MouseDown = false;
	m_OffBitmap = theButton->m_OffBitmap;
	m_OnBitmap 	= theButton->m_OnBitmap;
	m_Handler 	= theButton->m_Handler;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChannelCueButton::TChannelCueButton(BMessage *data) : BView(data)
{
	
	// Find our member variables in the BMessage
	data->FindBool("MouseDown", &m_MouseDown);
	
	data->FindPointer("OffBitmap", (void **)&m_OffBitmap);
	data->FindPointer("OffBitmap", (void **)&m_OnBitmap);
		
	/*
	// Save parent view	
	m_Cue = parent;
			
	// Store target and handler
	m_Handler = handler;
	*/
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TChannelCueButton::~TChannelCueButton()
{
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TChannelCueButton::Init()
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

BArchivable *TChannelCueButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TChannelCueButton") ) 
		return new TChannelCueButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TChannelCueButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{				
		// Add our class name to the archive
		data->AddString("class", "TChannelCueButton");
		
		// Add our member variables to the archive
		data->AddBool("MouseDown", m_MouseDown);
		
		data->AddPointer("OffBitmap", m_OffBitmap);
		data->AddPointer("OffBitmap", m_OnBitmap);
		
		//TCueView	*m_Cue;
		//BLooper		*m_Target;
		//BHandler	*m_Handler;
				
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

void TChannelCueButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if m_State is true, draw on bitmap
	if ( m_MouseDown )
		DrawBitmap(m_OnBitmap, B_ORIGIN);
	else
		DrawBitmap(m_OffBitmap, B_ORIGIN);
}



#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TChannelCueButton::MouseDown(BPoint where)
{
	DoClick();
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TChannelCueButton::MouseUp(BPoint where)
{
	
	// Set flag that we have been clicked. When the MouseUp method
	// is implimented we can remove this		
	m_MouseDown = false; 
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TChannelCueButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Pass up to parent
	ConvertToParent(&where);
	Parent()->MouseMoved(where, code, a_message);
}


#pragma mark -
#pragma mark === Click Handling ===

//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Button has been clicked
//

void TChannelCueButton::DoClick()
{	
	// We will always be in the up position when we start.
	// First, set the button state to down and force a redraw...
	m_MouseDown = true; 
	Draw(Bounds()); 
			
	// Trap mouse while it is down
	uint32 	buttons;
	BPoint 	mousePt, savePt;
	BRect 	bounds = Bounds();
	GetMouse(&mousePt, &buttons, true);
	
	while(buttons)
	{
		GetMouse(&mousePt, &buttons, true);
		
		if (savePt != mousePt)
		{
			if ( bounds.Contains(mousePt) )
			{
				if (m_MouseDown == false)
				{
					m_MouseDown = true;
					Draw(bounds);	
				}
			}
			// If the mouse is outside of the button bounds draw it's up state
			else 
			{
				if (m_MouseDown == true)
				{
					m_MouseDown = false;
					Draw(bounds);
				}
			} 			
			savePt = mousePt;			
			
			snooze(20 * 1000);
		}
		
	} 
	
	if ( bounds.Contains(mousePt) )
	{		
		// Restore and invalidate
		m_MouseDown = false; 
		Draw(bounds);
	}
}
