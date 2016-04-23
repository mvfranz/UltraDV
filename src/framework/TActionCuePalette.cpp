//---------------------------------------------------------------------
//
//	File:	TActionCuePalette.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	This palette contains the media specific cues.  These are:
//				--Slide
//				--Text
//				--Picture
//				--Movie
//				--Animation
//				--Audio
//				--MIDI
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes


#include <app/Application.h>
#include <stdio.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"

#include "TActionCuePalette.h"
#include "TActionCuePaletteView.h"


// Set the window size
const BRect kMediaCueBounds(10, 50, 20+kCueIconWidth, 60+((kCueIconHeight+kCueTextOffset)*kTotalMediaCues));

//------------------------------------------------------------------
//
//	Function:	TActionCuePalette()
//
//	Desc:		Constructor
//
//------------------------------------------------------------------
//
//
	        							        								
TActionCuePalette::TActionCuePalette():BWindow( kMediaCueBounds, "Action", B_FLOATING_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL, 
									  B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)
{
	Init();
}

//------------------------------------------------------------------
//
//	Function:	~TActionCuePalette()
//
//	Desc:		Destructor
//
//------------------------------------------------------------------
//
//

TActionCuePalette::~TActionCuePalette()
{
}


//------------------------------------------------------------------
//
//	Function:	Init()
//
//	Desc:		
//
//------------------------------------------------------------------
//
//

void TActionCuePalette::Init()
{
	Lock();
		
	// Create MediaCueView and add it to the window
	m_CueView = new TActionCuePaletteView(Bounds());
	
	// Add view to frame
	AddChild(m_CueView);
	
	Unlock();
	
	// Show window
	Show();
}


//------------------------------------------------------------------
//
//	Function:	Refresh()
//
//	Desc:		
//
//------------------------------------------------------------------
//
//

void TActionCuePalette::Refresh()
{
	Lock();
	Unlock();	
}


//------------------------------------------------------------------
//
//	Function:	MessageReceived()
//
//	Desc:		
//
//------------------------------------------------------------------
//
//

void TActionCuePalette::MessageReceived(BMessage* message)
{		
	switch (message->what)
	{
	
		default:
		BWindow::MessageReceived(message);						
		break;

	}
}	


//------------------------------------------------------------------
//
//	QuitRequested()
//
//------------------------------------------------------------------
//
//

bool TActionCuePalette::QuitRequested()
{		
	be_app->PostMessage(ACTION_PALETTE_CLOSE_MSG);
	return true;
}	
