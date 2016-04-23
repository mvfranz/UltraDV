//---------------------------------------------------------------------
//
//	File:	TMediaCuePalette.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.19.98
//
//	Desc:	This palette contains the stage tools.	These are:
//				--	Move
//				--	Rotate
//				--	Scale
//				--	Crop
//				--	Shear
//				--	Skew
//				--	Perspectivr
//				--	Mirror
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <stdio.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "TPaletteStatusView.h"

#include "TMediaCuePalette.h"
#include "TMediaCuePaletteView.h"


// Constants
const int32 kStatusRectHeight = 14;

// Set the window size
const BRect kMediaCueBounds(0, 0, ( kCueIconWidth * 2) + (kCueButtonOffset*2), ( kCueIconHeight * 3) + (kCueButtonOffset*4) + kStatusRectHeight);



//------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------
//
//
	        							        								
TMediaCuePalette::TMediaCuePalette():BWindow( kMediaCueBounds, "Media", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL, 
									  B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)									   	
{
	Init();
}

//------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------
//
//

TMediaCuePalette::~TMediaCuePalette()
{
}


//------------------------------------------------------------------
//	Init
//------------------------------------------------------------------
//
//

void TMediaCuePalette::Init()
{
	Lock();
		
	// Create MediaCueView and add it to the window
	BRect viewRect = Bounds();
	viewRect.bottom -= kStatusRectHeight;
	m_CueView = new TMediaCuePaletteView(viewRect);
	
	// Add view to frame
	AddChild(m_CueView);
	
	// Create status bar
	BRect bounds = Bounds();
	bounds.top = bounds.bottom - kStatusRectHeight;
	m_CueStatusView = new TPaletteStatusView(bounds);
	AddChild(m_CueStatusView);
	
	Unlock();
	
	// Show window
	//Show();
}



//------------------------------------------------------------------
//	MessageReceived
//------------------------------------------------------------------
//
//

void TMediaCuePalette::MessageReceived(BMessage* message)
{	
	
	switch (message->what)
	{
		// Update the status bar to diplay name of media cue
		case UPDATE_STATUS_TEXT_MSG:
			m_CueStatusView->MessageReceived(message);
			break;
			
		default:
			BWindow::MessageReceived(message);						
			break;
	}

}	


//------------------------------------------------------------------
//	QuitRequested
//------------------------------------------------------------------
//
//

bool TMediaCuePalette::QuitRequested()
{		
	//be_app->PostMessage(MEDIA_PALETTE_CLOSE_MSG);
	//return true;	
	Hide();	
	return false;
}	
