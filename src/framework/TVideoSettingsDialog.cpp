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
	fParameterWeb = NULL;
	
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
	
	roster->StopNode(fFrom.node, 0, true);
	roster->StopNode(fTo.node, 0, true);
	roster->Disconnect(fFrom.node.node, fFrom.source, fTo.node.node, fTo.destination);
	roster->ReleaseNode(fFrom.node);
	roster->ReleaseNode(fTo.node);
	roster->ReleaseNode(fTimeSource);

	//	Stop our VideoNode
	//fPreviewView->RemoveSelf();
	//delete fPreviewView;
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
	fPreviewView = new TVideoPreviewView(previewBounds, this);
	mediaRoster->RegisterNode(fPreviewView);
					
	//	Get TimeSource
	status_t err = mediaRoster->GetTimeSource(&fTimeSource);
	if (err < B_OK) 
	{
		//error("Can't get TimeSource!", err);
		printf("Can't get TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	printf("TimeSource found: %d (%x)\n", fTimeSource.node, fTimeSource.port);
	
	//	Find video source
	err = mediaRoster->GetVideoInput(&fVideoSource);
	if (err < B_OK) 
	{		
		//error("Can't find the video input!", err);
		printf("Can't find the video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	printf("input found: %d (%x)\n", fVideoSource.node, fVideoSource.port);

	//	Find output from video source
	int32 cnt = 0;
	err = mediaRoster->GetFreeOutputsFor(fVideoSource, &fFrom, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		//error("The video input is busy!", err);
		printf("The video input is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	//	Get parameter web for video source
	err = mediaRoster->GetParameterWebFor(fVideoSource, &fParameterWeb);
	if (err != B_OK) 	
	{
		printf("Unable to get VideoSource ParameterWeb!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Find free input
	cnt = 0;
	err = mediaRoster->GetFreeInputsFor(fPreviewView->Node(), &fTo, 1, &cnt, B_MEDIA_RAW_VIDEO);
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
	err = mediaRoster->Connect(fFrom.source, fTo.destination, &format, &fFrom, &fTo);
	if (err < B_OK) 
	{
		//error("Couldn't connect video input to video display!", err);
		printf("Couldn't connect video input to video display!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Set time source for output node
	err = mediaRoster->SetTimeSourceFor( fPreviewView->Node().node, fTimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video display!", err);
		printf("Couldn't set TimeSource for video display!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Set time source for video input
	err = mediaRoster->SetTimeSourceFor( fVideoSource.node, fTimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video input!", err);
		printf("Couldn't set TimeSource for video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	BTimeSource *source = mediaRoster->MakeTimeSourceFor(fTimeSource);

	//	Start them
	bigtime_t start_delay = 0;
	mediaRoster->GetStartLatencyFor(fTimeSource, &start_delay);
	start_delay += estimate_max_scheduling_latency(find_thread(NULL));

	bool running = source->IsRunning();
	printf("running: %s\n", running ? "true" : "false");
	bigtime_t perf = source->Now() + start_delay;
	bigtime_t real = BTimeSource::RealTime();
	source->Release();

	printf("perf = %.4f real = %.4f\n", (double)perf/1000000., (double)real/1000000.);
	
	err = mediaRoster->StartNode(fTo.node, perf);
	if (err < B_OK) 
	{
		//error("Couldn't start video displayer!", err);
		printf("Couldn't start video displayer!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	err = mediaRoster->StartNode(fFrom.node, perf);
	if (err < B_OK) 
	{
		//error("Couldn't start video input!", err);
		printf("Couldn't start video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	//	Workaround for possibly broken Now()
	err = mediaRoster->SeekNode(fTimeSource, perf-start_delay, real);
	if (err < B_OK) 
	{
		//error("Couldn't Seek() TimeSource!", err);
		printf("Couldn't Seek() TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	err = mediaRoster->StartNode(fTimeSource, real);
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
		fTempVideoSettings = thePrefs->fVideoSettings;		
	}
	
	// Create background view
	fBGView = new TVideoSettingsView(Bounds(), this); 
	AddChild(fBGView);
			
	//
	// Add tabs to the window
	//
	
	// VideoSettings
	BRect bounds(0, 0, Bounds().Width(), 200);
	fTabView = new TVideoSettingsTabView(fBGView, bounds, "VideoSettingsTabView"); 
	ASSERT(fTabView);
	fTabView->SetViewColor(kBeGrey); 
		
	fBGView->AddChild(fTabView);
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
					thePrefs->fVideoSettings = fTempVideoSettings;
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
	//BTimeSource *source = BMediaRoster::Roster()->MakeTimeSourceFor(fTimeSource);
	status_t err = BMediaRoster::Roster()->StartNode(fTimeSource, 0);
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
	status_t err = BMediaRoster::Roster()->StopNode(fTimeSource, BTimeSource::RealTime());
	if (err < B_OK) 
	{
		//error("Couldn't start TimeSource!", err);
		printf("Couldn't stop TimeSource!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}	
}	