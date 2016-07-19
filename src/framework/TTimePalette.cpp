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

TTimePalette::TTimePalette(BRect bounds):BWindow( bounds, "Locator", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
									  B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE),
									  BMediaNode("LocatorNode")
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

	//	Quit service thread
	if (write_port_etc(fPort, 0x60000000, NULL, 0, B_TIMEOUT, DEFAULT_TIMEOUT))
		kill_thread(fServiceThread);

	status_t result;
	wait_for_thread(fServiceThread, &result);

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
	fTimeToQuit	= false;
	fIsPlaying 	= false;
	fIsStopping	= false;

	// Create MediaCueView and add it to the window
	fTimeView = new TTimePaletteView(Bounds());

	// Add view to frame
	AddChild(fTimeView);

	//	Create our port
	fPort = create_port(2, "LocatorPort");

	//	Create port service thread
	fServiceThread = spawn_thread(service_routine, "Locator:Service", B_NORMAL_PRIORITY, this);
	resume_thread(fServiceThread);

	//	Create run thread
	fRunThread = spawn_thread(run_routine, "Locator::Run", B_NORMAL_PRIORITY, this);
	resume_thread(fRunThread);

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

void TTimePalette::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
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

bool TTimePalette::QuitRequested()
{
	Hide();
	return false;
}

#pragma mark -
#pragma mark === MediaNode Functions ===

//------------------------------------------------------------------
//	ControlPort
//------------------------------------------------------------------
//
//	Return nodes control port
//

port_id TTimePalette::ControlPort() const
{
	return fPort;
}


//------------------------------------------------------------------
//	ControlPort
//------------------------------------------------------------------
//
//	No an addon.  Return NULL
//

BMediaAddOn	*TTimePalette::AddOn(int32 * internal_id) const
{
	return NULL;
}


#pragma mark -
#pragma mark === Thread Functions ===

//-------------------------------------------------------------------
//	service_routine
//-------------------------------------------------------------------
//
//	Static service thread function
//

status_t TTimePalette::service_routine(void * data)
{
	((TTimePalette *)data)->ServiceRoutine();

	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void TTimePalette::ServiceRoutine()
{
	while (!fTimeToQuit)
	{
		//	Read message
		status_t 		err  = 0;
		int32 			code = 0;
		char 			msg[B_MEDIA_MESSAGE_SIZE];

		err = read_port_etc(fPort, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);

		if (err == B_TIMED_OUT)
			continue;

		if (err < B_OK)
		{
			printf("TTimePalette::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
			continue;
		}

		// dispatch message
		if (code == 0x60000000)
			break;

		HandleMessage(code, &msg, err);
	}
}


//-------------------------------------------------------------------
//	run_routine
//-------------------------------------------------------------------
//
//	Static run thread function
//

status_t TTimePalette::run_routine(void *data)
{
	((TTimePalette *)data)->RunRoutine();
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

	while(!fTimeToQuit)
	{
		snooze(50000);

		//	Update media_server
		if (TimeSource()->IsRunning())
		{
			//	Update text
			TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), text, false);
			Lock();
			fTimeView->GetTimeText()->SetText(text);
			fTimeView->GetTimeText()->Sync();
			Unlock();
		}
	}
}
