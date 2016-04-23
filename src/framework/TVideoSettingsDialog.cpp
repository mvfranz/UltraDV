//---------------------------------------------------------------------
//
//	File:	TVideoSettingsDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Video Setting Dialog
//
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

#include "TCueSheetWindow.h"

#include "TVideoSettings.h"
#include "TVideoSettingsView.h"
#include "TVideoSettingsTabView.h"
#include "TVideoPreviewView.h"

#include "TVideoSettingsDialog.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoSettingsDialog::TVideoSettingsDialog(BRect bounds) : BWindow( bounds, "Video Settings", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE) 
{			
	//	Defaults
	m_ParameterWeb = NULL;
	
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoSettingsDialog::~TVideoSettingsDialog()
{
	//	Close running connections
	BMediaRoster *roster = BMediaRoster::Roster();
	
	roster->StopNode(m_From.node, 0, true);
	roster->StopNode(m_To.node, 0, true);
	roster->Disconnect(m_From.node.node, m_From.source, m_To.node.node, m_To.destination);
	roster->ReleaseNode(m_From.node);
	roster->ReleaseNode(m_To.node);
	roster->ReleaseNode(m_TimeSource);

	//	Stop our VideoNode
	//m_PreviewView->RemoveSelf();
	//delete m_PreviewView;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoSettingsDialog::Init()
{		
	//	
	//	Set up nodes
	//
	
	//	Get MediaRoster
	BMediaRoster *mediaRoster = BMediaRoster::Roster();
	if (!mediaRoster) 
	{
		(new BAlert("", "Cannot connect to the media server!", "Quit"))->Go();
		PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Create VideoPreviewView node and get output
	BRect previewBounds(0, 0, 100, 100);
	m_PreviewView = new TVideoPreviewView(previewBounds, this);
	mediaRoster->RegisterNode(m_PreviewView);
					
	//	Get TimeSource
	status_t err = mediaRoster->GetTimeSource(&m_TimeSource);
	if (err < B_OK) 
	{
		//error("Can't get TimeSource!", err);
		printf("Can't get TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	printf("TimeSource found: %d (%x)\n", m_TimeSource.node, m_TimeSource.port);
	
	//	Find video source
	err = mediaRoster->GetVideoInput(&m_VideoSource);
	if (err < B_OK) 
	{		
		//error("Can't find the video input!", err);
		printf("Can't find the video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	printf("input found: %d (%x)\n", m_VideoSource.node, m_VideoSource.port);

	//	Find output from video source
	int32 cnt = 0;
	err = mediaRoster->GetFreeOutputsFor(m_VideoSource, &m_From, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		//error("The video input is busy!", err);
		printf("The video input is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	//	Get parameter web for video source
	err = mediaRoster->GetParameterWebFor(m_VideoSource, &m_ParameterWeb);
	if (err != B_OK) 	
	{
		printf("Unable to get VideoSource ParameterWeb!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Find free input
	cnt = 0;
	err = mediaRoster->GetFreeInputsFor(m_PreviewView->Node(), &m_To, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		//error("The video output is busy!", err);
		printf("The video output is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Connect them
	media_format format;
	format.type = B_MEDIA_RAW_VIDEO;
	format.u.raw_video = media_raw_video_format::wildcard;
	err = mediaRoster->Connect(m_From.source, m_To.destination, &format, &m_From, &m_To);
	if (err < B_OK) 
	{
		//error("Couldn't connect video input to video display!", err);
		printf("Couldn't connect video input to video display!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Set time source for output node
	err = mediaRoster->SetTimeSourceFor( m_PreviewView->Node().node, m_TimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video display!", err);
		printf("Couldn't set TimeSource for video display!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Set time source for video input
	err = mediaRoster->SetTimeSourceFor( m_VideoSource.node, m_TimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video input!", err);
		printf("Couldn't set TimeSource for video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	BTimeSource *source = mediaRoster->MakeTimeSourceFor(m_TimeSource);

	//	Start them
	bigtime_t start_delay = 0;
	mediaRoster->GetStartLatencyFor(m_TimeSource, &start_delay);
	start_delay += estimate_max_scheduling_latency(find_thread(NULL));

	bool running = source->IsRunning();
	printf("running: %s\n", running ? "true" : "false");
	bigtime_t perf = source->Now() + start_delay;
	bigtime_t real = BTimeSource::RealTime();
	source->Release();

	printf("perf = %.4f real = %.4f\n", (double)perf/1000000., (double)real/1000000.);
	
	err = mediaRoster->StartNode(m_To.node, perf);
	if (err < B_OK) 
	{
		//error("Couldn't start video displayer!", err);
		printf("Couldn't start video displayer!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	err = mediaRoster->StartNode(m_From.node, perf);
	if (err < B_OK) 
	{
		//error("Couldn't start video input!", err);
		printf("Couldn't start video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	//	Workaround for possibly broken Now()
	err = mediaRoster->SeekNode(m_TimeSource, perf-start_delay, real);
	if (err < B_OK) 
	{
		//error("Couldn't Seek() TimeSource!", err);
		printf("Couldn't Seek() TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	err = mediaRoster->StartNode(m_TimeSource, real);
	if (err < B_OK) 
	{
		//error("Couldn't start TimeSource!", err);
		printf("Couldn't start TimeSource!\n");
		return;
	}
	
	
	//
	//	Set up TabView
	//
	
	// Get app instance
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
	
	// get active cue sheet
	TCueSheetWindow *cueSheet = theApp->GetCueSheet();
	if (cueSheet)
	{
		// Create local copy of VideoSettings
		TCueSheetPrefs *thePrefs = cueSheet->GetCueSheetPrefs();		
		m_TempVideoSettings = thePrefs->m_VideoSettings;		
	}
	
	// Create background view
	m_BGView = new TVideoSettingsView(Bounds(), this); 
	AddChild(m_BGView);
			
	//
	// Add tabs to the window
	//
	
	// VideoSettings
	BRect bounds(0, 0, Bounds().Width(), 200);
	m_TabView = new TVideoSettingsTabView(m_BGView, bounds, "VideoSettingsTabView"); 
	ASSERT(m_TabView);
	m_TabView->SetViewColor(kBeGrey); 
		
	m_BGView->AddChild(m_TabView);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVideoSettingsDialog::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{
			
		case CANCEL_MSG:
			Lock();
			Quit();
			break;
			
		// Update preferences
		case OK_MSG:
			{
				// get app instance
				MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
				
				// get active cue sheet
				TCueSheetWindow *cueSheet = theApp->GetCueSheet();
				if (cueSheet)
				{
					TCueSheetPrefs *thePrefs = cueSheet->GetCueSheetPrefs();
					
					// Update the prefs
					thePrefs->m_VideoSettings = m_TempVideoSettings;
				}
				
				// Goodbye				
				Lock();
				Quit();
			}
			break;
							
		default:
			BWindow::MessageReceived(message);						
			break;
	}
}	


#pragma mark -
#pragma mark === Window Handling ===

//------------------------------------------------------------------
//	QuitRequested
//------------------------------------------------------------------
//
//

bool TVideoSettingsDialog::QuitRequested()
{		
	Hide();	
	return true;
}	


#pragma mark -
#pragma mark === Capture Routines ===
	
//------------------------------------------------------------------
//	StartCapture
//------------------------------------------------------------------
//
//

void TVideoSettingsDialog::StartCapture()
{	
	printf("StartCapture -ENTER-\n");
	
	//	Start TimeSource	
	//BTimeSource *source = BMediaRoster::Roster()->MakeTimeSourceFor(m_TimeSource);
	status_t err = BMediaRoster::Roster()->StartNode(m_TimeSource, 0);
	if (err < B_OK) 
	{
		//error("Couldn't start TimeSource!", err);
		printf("Couldn't start TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	printf("StartCapture -EXIT-\n");	
}	


//------------------------------------------------------------------
//	StopCapture
//------------------------------------------------------------------
//
//

void TVideoSettingsDialog::StopCapture()
{	
	//	Stop TimeSource	
	status_t err = BMediaRoster::Roster()->StopNode(m_TimeSource, BTimeSource::RealTime());
	if (err < B_OK) 
	{
		//error("Couldn't start TimeSource!", err);
		printf("Couldn't stop TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}	
}	