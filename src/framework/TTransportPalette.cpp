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
	        							        								
TTransportPalette::TTransportPalette(BRect bounds):BWindow( bounds, "Transport", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL, 
									  B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE),
									  BMediaNode("TransportNode")
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
	m_TimeToQuit = true;
	
	//	Quit service thread
	if (write_port_etc(m_Port, 0x60000000, NULL, 0, B_TIMEOUT, DEFAULT_TIMEOUT))
		kill_thread(m_ServiceThread);
	
	status_t result;
	wait_for_thread(m_ServiceThread, &result);

	//	Wait for Run thread
	wait_for_thread(m_RunThread, &result);		
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
	m_TimeToQuit	= false;
	m_IsPlaying 	= false;
	m_IsStopping	= false;

	//
	// Create background bitmap
	//
	
	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;
	
	size_t 		size;
	BBitmap 	*data;
	
	BResources res(&file);
	data = (BBitmap *)res.FindResource('bits', "Transport", &size);
	if (!data)			
		return;

	BRect bitmapRect = Bounds();
	BBitmap *bitmap = new BBitmap(bitmapRect, B_COLOR_8_BIT);
	ASSERT(bitmap);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);

	// Create MediaCueView and add it to the window
	m_TransportView = new TTransportPaletteView(Bounds(), bitmap);
	
	// Add view to frame
	AddChild(m_TransportView);
	
	//	Create our port
	m_Port = create_port(3, "TransportPort");
			
	//	Create port service thread
	m_ServiceThread = spawn_thread(service_routine, "Transport:Service", B_NORMAL_PRIORITY, this);
	resume_thread(m_ServiceThread);	
	
	//	Create run thread
	m_RunThread = spawn_thread(run_routine, "Transport::Run", B_NORMAL_PRIORITY, this);	
	resume_thread(m_RunThread);

	
	Unlock();
	
	// Show window
	//Show();
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

void TTransportPalette::MessageReceived(BMessage* message)
{	
	switch(message->what)
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

bool TTransportPalette::QuitRequested()
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

port_id TTransportPalette::ControlPort() const
{
	return m_Port;
}


//------------------------------------------------------------------
//	ControlPort
//------------------------------------------------------------------
//	
//	No an addon.  Return NULL
//

BMediaAddOn	*TTransportPalette::AddOn(int32 * internal_id) const
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

status_t TTransportPalette::service_routine(void * data)
{
	((TTransportPalette *)data)->ServiceRoutine();
	
	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void TTransportPalette::ServiceRoutine()
{
	while (!m_TimeToQuit)
	{
		//	Read message
		status_t 		err  = 0;
		int32 			code = 0;
		char 			msg[B_MEDIA_MESSAGE_SIZE];
		
		err = read_port_etc(m_Port, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);		
		
		if (err == B_TIMED_OUT) 
			continue;
		
		if (err < B_OK)
		{
			printf("TTransportPalette::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
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

status_t TTransportPalette::run_routine(void *data)
{
	((TTransportPalette *)data)->RunRoutine();	
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
	
	while(!m_TimeToQuit)
	{
		snooze(50000);
	
		//	Update media_server
		if (TimeSource()->IsRunning())
		{
			//	Update text						
			TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), text, false);						
			m_TransportView->GetTransportText()->SetText(text);
		}		
	}
}
