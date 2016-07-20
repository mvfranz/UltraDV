//---------------------------------------------------------------------
//
//	File:	TTransportPalette.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	This palette contains the transport controls.  These include:
//				-- Play
//				-- Record
//				-- Pause
//				-- Rewind
//				-- Fast Forward
//				-- Go to Beginning
//				-- Go to End
//
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

#include "TTransportPaletteView.h"
#include "TTimeText.h"

#include "TTransportPalette.h"


//------------------------------------------------------------------
//
//	Function:	TTransportPalette()
//
//	Desc:		Constructor
//
//------------------------------------------------------------------
//
//

TTransportPalette::TTransportPalette(BRect bounds) : BWindow( bounds, "Transport", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
	                                                      B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)
{
	Init();
}

//------------------------------------------------------------------
//
//	Function:	~TTransportPalette()
//
//	Desc:		Destructor
//
//------------------------------------------------------------------
//
//

TTransportPalette::~TTransportPalette()
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

void TTransportPalette::Init()
{
	Lock();

	//	Set up member variables
	fTimeToQuit     = false;
	fIsPlaying      = false;
	fIsStopping     = false;

	//
	// Create background bitmap
	//

	// Get application info
	app_info info;

	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;

	size_t size;
	BBitmap* data;

	BResources res(&file);
	data = (BBitmap*)res.FindResource('bits', "Transport", &size);
	if (!data)
		return;

	BRect bitmapRect = Bounds();
	BBitmap* bitmap = new BBitmap(bitmapRect, B_COLOR_8_BIT);
	ASSERT(bitmap);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);

	// Create MediaCueView and add it to the window
	fTransportView = new TTransportPaletteView(Bounds(), bitmap);

	// Add view to frame
	AddChild(fTransportView);

	//	Create run thread
	fRunThread = spawn_thread(run_routine, "Transport::Run", B_NORMAL_PRIORITY, this);
	resume_thread(fRunThread);


	Unlock();

	// Show window
	//Show();
}

//------------------------------------------------------------------
//
//	QuitRequested()
//
//------------------------------------------------------------------
//
//

bool TTransportPalette::QuitRequested()
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

status_t TTransportPalette::run_routine(void* data)
{
	((TTransportPalette*)data)->RunRoutine();
	return 0;
}



//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void TTransportPalette::RunRoutine()
{
	char text[12];

	while(!fTimeToQuit) {
		snooze(50000);
		//	Update text
		TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), text, false);
		fTransportView->GetTransportText()->SetText(text);
	}
}
