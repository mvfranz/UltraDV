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

	//	Signal threads to quit
	fTimeToQuit = true;

	status_t result;
	//	Wait for Run thread
	wait_for_thread(fRunThread, &result);
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
	fTimeToQuit     = false;
	fIsPlaying      = false;
	fIsStopping     = false;

	// Create MediaCueView and add it to the window
	fTimeView = new TTimePaletteView(Bounds());

	// Add view to frame
	AddChild(fTimeView);

	//	Create run thread
	fRunThread = spawn_thread(run_routine, "Locator::Run", B_NORMAL_PRIORITY, this);
	resume_thread(fRunThread);

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

#pragma mark -
#pragma mark === Thread Functions ===


//-------------------------------------------------------------------
//	run_routine
//-------------------------------------------------------------------
//
//	Static run thread function
//

status_t TTimePalette::run_routine(void* data)
{
	((TTimePalette*)data)->RunRoutine();
	return 0;
}



//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void TTimePalette::RunRoutine()
{
	char text[12];

	while(!fTimeToQuit) {
		snooze(50000);
		//	Update text
		TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), text, false);
		Lock();
		fTimeView->GetTimeText()->SetText(text);
		fTimeView->GetTimeText()->Sync();
		Unlock();
	}
}
