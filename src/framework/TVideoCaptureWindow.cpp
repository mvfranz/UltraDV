//---------------------------------------------------------------------
//
//	File:	TVideoCaptureWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.30.98
//
//	Desc:	Video capture window
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
#include "AppAlerts.h"
#include "DebugUtils.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"
#include "TMuseumIcons.h"

#include "TCueSheetPrefs.h"
#include "TCueSheetWindow.h"

#include "TVideoCaptureView.h"
#include "TVideoCaptureToolbar.h"
#include "TStatusView.h"
#include "TVideoEditor.h"

#include "TVideoCaptureWindow.h"

// Constants
const float kVidCapToolbarHeight = 23;
const float kVidCapViewWidth 	 = 319;
const float kVidCapViewHeight	 = 239;
const int32 kStatusRectHeight 	 = 14;
const BRect kVidCapBounds(0, 0, kVidCapViewWidth, kVidCapToolbarHeight+kVidCapViewHeight+kStatusRectHeight);

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoCaptureWindow::TVideoCaptureWindow() : BWindow(kVidCapBounds, "Capture Video", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE)
{			
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoCaptureWindow::~TVideoCaptureWindow()
{
	//	Get roster
	BMediaRoster *roster = BMediaRoster::Roster();
	
	//	Stop timesource
	status_t retVal = roster->StopNode(m_TimeSource, 0, true);

	//	Close running connections
	roster->StopNode(m_From.node, 0, true);
	roster->StopNode(m_To.node, 0, true);
	roster->Disconnect(m_From.node.node, m_From.source, m_To.node.node, m_To.destination);
	roster->ReleaseNode(m_From.node);
	roster->ReleaseNode(m_To.node);
	roster->ReleaseNode(m_TimeSource);
	
	// Exit from all threads
	status_t status;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoCaptureWindow::Init()
{	
	
	// Create tool bar
	BRect bounds = Bounds();
	bounds.bottom = bounds.top + kVidCapToolbarHeight;
	m_Toolbar = new TVideoCaptureToolbar(bounds);
	AddChild(m_Toolbar);
	
	//  Create VidCap View
	BRect vidCapRect = Bounds();
	vidCapRect.top = m_Toolbar->Frame().bottom;
	vidCapRect.bottom -= kStatusRectHeight;
	m_VidCapView = new TVideoCaptureView(vidCapRect, this);
	AddChild(m_VidCapView);
	
	// Create Statusbar
	BRect statusRect = Bounds();
	statusRect.top = statusRect.bottom - kStatusRectHeight ;
	m_StatusBar = new TStatusView(statusRect);
	AddChild(m_StatusBar);
		
	// Set status text
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	m_StatusBar->MessageReceived(textMessage);
	delete textMessage;
	
	
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

	//	Register VideoPreviewView node and get output
	mediaRoster->RegisterNode(m_VidCapView);
					
	//	Get TimeSource
	status_t err = mediaRoster->GetTimeSource(&m_TimeSource);
	if (err < B_OK) 
	{
		//error("Can't get TimeSource!", err);
		printf("Can't get TimeSource!\n");
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
	err = mediaRoster->GetFreeInputsFor(m_VidCapView->Node(), &m_To, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		//error("The video output is busy!", err);
		printf("The video output is busy!\n");
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
		return;
	}

	//	Set time source for output node
	err = mediaRoster->SetTimeSourceFor( m_VidCapView->Node().node, m_TimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video display!", err);
		printf("Couldn't set TimeSource for video display!\n");
		return;
	}

	//	Set time source for video input
	err = mediaRoster->SetTimeSourceFor( m_VideoSource.node, m_TimeSource.node);
	if (err < B_OK) 
	{
		//error("Couldn't set TimeSource for video input!", err);
		printf("Couldn't set TimeSource for video input!\n");
		return;
	}

	//	Start them
	bigtime_t start_delay = 0;
	mediaRoster->GetStartLatencyFor(m_TimeSource, &start_delay);
	start_delay += estimate_max_scheduling_latency(find_thread(NULL));

	BTimeSource *source = BMediaRoster::Roster()->MakeTimeSourceFor(m_TimeSource);
	bool running = source->IsRunning();
	printf("running: %s\n", running ? "true" : "false");
	bigtime_t perf = source->Now() + start_delay;
	bigtime_t real = BTimeSource::RealTime();
	source->Release();

	printf("perf = %.4f real = %.4f\n", (double)perf/1000000., (double)real/1000000.);
	
	err = mediaRoster->StartNode(m_VidCapView->Node(), perf);
	if (err < B_OK) 
	{
		printf("Couldn't start video displayer!\n");
		return;
	}
	
	err = mediaRoster->StartNode(m_VideoSource, perf);
	if (err < B_OK) 
	{
		printf("Couldn't start video input!\n");
		return;
	}
		
	err = mediaRoster->StartNode(m_TimeSource, real);
	if (err < B_OK) 
	{
		//error("Couldn't start TimeSource!", err);
		printf("Couldn't start TimeSource!\n");
		return;
	}
	
	
	/*
	// Configure capture based on cue sheet prefs
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
	if (theApp)
	{
		// Get cue sheet prefs
		TCueSheetPrefs *thePrefs = theApp->GetCueSheet()->GetCueSheetPrefs();
		TVideoSettings	vidSettings = thePrefs->m_VideoSettings;
						
		// VideoImageSettings		
		m_VideoSource->VideoControls()->SetBrightness( vidSettings.m_VideoImageSettings.m_BrightnessValue);
		m_VideoSource->VideoControls()->SetContrast(vidSettings.m_VideoImageSettings.m_ContrastValue);
		m_VideoSource->VideoControls()->SetHue(vidSettings.m_VideoImageSettings.m_HueValue);
		m_VideoSource->VideoControls()->SetSaturation(vidSettings.m_VideoImageSettings.m_SaturationValue);
		
		//
		// VideoSourceSettings
		//
		
		//VideoDigitizer	m_Digitizer;
		
		// VideoInput
		switch(vidSettings.m_VideoSourceSettings.m_Input)
		{
			case kComposite:
				m_VideoSource->VideoMux()->SetSource(B_COMPOSITE_0);
				m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
				break;	
					
			case kTuner:
				m_VideoSource->VideoMux()->SetSource(B_TUNER_0);
				m_VideoSource->AudioMux()->SetSource(B_TUNER_AUDIO_0);
				break;
						
			case kSVideo:
				m_VideoSource->VideoMux()->SetSource(B_SVIDEO_0);
				m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
				break;
						
			case kColorBars:
				m_VideoSource->VideoMux()->SetSource(B_COLOR_BARS);
				m_VideoSource->AudioMux()->SetSource(B_MUTE_AUDIO);
				break;	
					
			case kCamera:
				m_VideoSource->VideoMux()->SetSource(B_COMPOSITE_1);
				m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
				break;
			
			default:
				break;
		}		
		
		// VideoFormat
		switch(vidSettings.m_VideoSourceSettings.m_Format)
		{
			case kNTSC_M:
				m_VideoSource->SetVideoFormat(B_NTSC_M);
				break;
				
			case kNTSC_J:
				m_VideoSource->SetVideoFormat(B_NTSC_J);
				break;
				
			case kPAL_BG:
				m_VideoSource->SetVideoFormat(B_PAL_M);
				break;
				
			case kPAL_I:
				m_VideoSource->SetVideoFormat(B_PAL_N);
				break;
				
			case kSECAM:
				m_VideoSource->SetVideoFormat(B_SECAM);
				break;
			
			default:
				m_VideoSource->SetVideoFormat(B_NTSC_M);
				break;
		
		}
			
		// VideoFilters
		m_VideoSource->VideoControls()->SetGammaCorrectionRemoval(vidSettings.m_VideoSourceSettings.m_Filters.m_GammaCorrection);
		m_VideoSource->VideoControls()->SetLumaCoring(vidSettings.m_VideoSourceSettings.m_Filters.m_LumaCoring);
		m_VideoSource->VideoControls()->SetChromaCombFilter(vidSettings.m_VideoSourceSettings.m_Filters.m_ChromaComb);
		m_VideoSource->VideoControls()->SetLumaCombFilter(vidSettings.m_VideoSourceSettings.m_Filters.m_LumaComb);
		m_VideoSource->VideoControls()->SetErrorDiffusion(vidSettings.m_VideoSourceSettings.m_Filters.m_ErrorDiffusion);
	}
	*/
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVideoCaptureWindow::MessageReceived(BMessage* message)
{	
	status_t status;
	
	switch (message->what)
	{
		//	Pass message on to view...
		case VIDCAP_RECORD_MSG:
			m_VidCapView->MessageReceived(message);
			break;
			/*
			if(m_IsRecording)
			{
				m_IsRecording = false;
				
				// Update status bar
				BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
				textMessage->AddString("StatusText", "Ready to Capture...");
				m_StatusBar->MessageReceived(textMessage);
				delete textMessage;
			}
			else
			{
				m_IsRecording = true;
				Record();			
			}			
			break;*/
			
		default:
			BWindow::MessageReceived(message);						
			break;
	}
}	
