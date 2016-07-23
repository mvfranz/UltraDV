//---------------------------------------------------------------------
//
//	File:	TTimePalette.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	This palette displays the playback and cue movement
//			time.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <stdio.h>

#include <Application.h>
#include <TimeSource.h>


#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "TTimePalette.h"
#include "TTimeText.h"
#include "TTimePaletteView.h"

//------------------------------------------------------------------
//	TTimePalette
//------------------------------------------------------------------
//
//

TTimePalette::TTimePalette(BRect bounds) : BWindow( bounds, "Locator", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
	                                            B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)
{
	Init();
}

//------------------------------------------------------------------
//
//	Function:	~TTimePalette()
//
//	Desc:		Destructor
//
//------------------------------------------------------------------
//
//

TTimePalette::~TTimePalette()
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

void TTimePalette::Init()
{
	Lock();

	//	Set up member variables
	fIsPlaying      = false;
	fIsStopping     = false;

	// Create MediaCueView and add it to the window
	fTimeView = new TTimePaletteView(Bounds());

	// Add view to frame
	AddChild(fTimeView);

	Unlock();
}


//------------------------------------------------------------------
//	QuitRequested
//------------------------------------------------------------------
//
//

bool TTimePalette::QuitRequested()
{
	Hide();
	return false;
}
