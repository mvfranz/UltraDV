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
const float kVidCapViewWidth     = 319;
const float kVidCapViewHeight    = 239;
const int32 kStatusRectHeight    = 14;
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
	status_t retVal = roster->StopNode(fTimeSource, 0, true);

	//	Close running connections
	roster->StopNode(fFrom.node, 0, true);
	roster->StopNode(fTo.node, 0, true);
	roster->Disconnect(fFrom.node.node, fFrom.source, fTo.node.node, fTo.destination);
	roster->ReleaseNode(fFrom.node);
	roster->ReleaseNode(fTo.node);
	roster->ReleaseNode(fTimeSource);

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
	fToolbar = new TVideoCaptureToolbar(bounds);
	AddChild(fToolbar);

	//  Create VidCap View
	BRect vidCapRect = Bounds();
	vidCapRect.top = fToolbar->Frame().bottom;
	vidCapRect.bottom -= kStatusRectHeight;
	fVidCapView = new TVideoCaptureView(vidCapRect, this);
	AddChild(fVidCapView);

	// Create Statusbar
	BRect statusRect = Bounds();
	statusRect.top = statusRect.bottom - kStatusRectHeight;
	fStatusBar = new TStatusView(statusRect);
	AddChild(fStatusBar);

	// Set status text
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	fStatusBar->MessageReceived(textMessage);
	delete textMessage;


	//
	//	Set up nodes
	//

	//	Get MediaRoster
	BMediaRoster *mediaRoster = BMediaRoster::Roster();
	if (!mediaRoster) {
		(new BAlert("", "Cannot connect to the media server!", "Quit"))->Go();
		PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Register VideoPreviewView node and get output
	mediaRoster->RegisterNode(fVidCapView);

	//	Get TimeSource
	status_t err = mediaRoster->GetTimeSource(&fTimeSource);
	if (err < B_OK) {
		//error("Can't get TimeSource!", err);
		printf("Can't get TimeSource!\n");
		return;
	}
	printf("TimeSource found: %d (%x)\n", fTimeSource.node, fTimeSource.port);

	//	Find video source
	err = mediaRoster->GetVideoInput(&fVideoSource);
	if (err < B_OK) {
		//error("Can't find the video input!", err);
		printf("Can't find the video input!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
	printf("input found: %d (%x)\n", fVideoSource.node, fVideoSource.port);

	//	Find output from video source
	int32 cnt = 0;
	err = mediaRoster->GetFreeOutputsFor(fVideoSource, &fFrom, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) {
		//error("The video input is busy!", err);
		printf("The video input is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Get parameter web for video source
	err = mediaRoster->GetParameterWebFor(fVideoSource, &fParameterWeb);
	if (err != B_OK) {
		printf("Unable to get VideoSource ParameterWeb!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Find free input
	cnt = 0;
	err = mediaRoster->GetFreeInputsFor(fVidCapView->Node(), &fTo, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) {
		//error("The video output is busy!", err);
		printf("The video output is busy!\n");
		return;
	}

	//	Connect them
	media_format format;
	format.type = B_MEDIA_RAW_VIDEO;
	format.u.raw_video = media_raw_video_format::wildcard;
	err = mediaRoster->Connect(fFrom.source, fTo.destination, &format, &fFrom, &fTo);
	if (err < B_OK) {
		//error("Couldn't connect video input to video display!", err);
		printf("Couldn't connect video input to video display!\n");
		return;
	}

	//	Set time source for output node
	err = mediaRoster->SetTimeSourceFor( fVidCapView->Node().node, fTimeSource.node);
	if (err < B_OK) {
		//error("Couldn't set TimeSource for video display!", err);
		printf("Couldn't set TimeSource for video display!\n");
		return;
	}

	//	Set time source for video input
	err = mediaRoster->SetTimeSourceFor( fVideoSource.node, fTimeSource.node);
	if (err < B_OK) {
		//error("Couldn't set TimeSource for video input!", err);
		printf("Couldn't set TimeSource for video input!\n");
		return;
	}

	//	Start them
	bigtime_t start_delay = 0;
	mediaRoster->GetStartLatencyFor(fTimeSource, &start_delay);
	start_delay += estimate_max_scheduling_latency(find_thread(NULL));

	BTimeSource *source = BMediaRoster::Roster()->MakeTimeSourceFor(fTimeSource);
	bool running = source->IsRunning();
	printf("running: %s\n", running ? "true" : "false");
	bigtime_t perf = source->Now() + start_delay;
	bigtime_t real = BTimeSource::RealTime();
	source->Release();

	printf("perf = %.4f real = %.4f\n", (double)perf/1000000., (double)real/1000000.);

	err = mediaRoster->StartNode(fVidCapView->Node(), perf);
	if (err < B_OK) {
		printf("Couldn't start video displayer!\n");
		return;
	}

	err = mediaRoster->StartNode(fVideoSource, perf);
	if (err < B_OK) {
		printf("Couldn't start video input!\n");
		return;
	}

	err = mediaRoster->StartNode(fTimeSource, real);
	if (err < B_OK) {
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
	        TVideoSettings	vidSettings = thePrefs->fVideoSettings;

	        // VideoImageSettings
	        fVideoSource->VideoControls()->SetBrightness( vidSettings.fVideoImageSettings.fBrightnessValue);
	        fVideoSource->VideoControls()->SetContrast(vidSettings.fVideoImageSettings.fContrastValue);
	        fVideoSource->VideoControls()->SetHue(vidSettings.fVideoImageSettings.fHueValue);
	        fVideoSource->VideoControls()->SetSaturation(vidSettings.fVideoImageSettings.fSaturationValue);

	        //
	        // VideoSourceSettings
	        //

	        //VideoDigitizer	fDigitizer;

	        // VideoInput
	        switch(vidSettings.fVideoSourceSettings.fInput)
	        {
	                case kComposite:
	                        fVideoSource->VideoMux()->SetSource(B_COMPOSITE_0);
	                        fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
	                        break;

	                case kTuner:
	                        fVideoSource->VideoMux()->SetSource(B_TUNER_0);
	                        fVideoSource->AudioMux()->SetSource(B_TUNER_AUDIO_0);
	                        break;

	                case kSVideo:
	                        fVideoSource->VideoMux()->SetSource(B_SVIDEO_0);
	                        fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
	                        break;

	                case kColorBars:
	                        fVideoSource->VideoMux()->SetSource(B_COLOR_BARS);
	                        fVideoSource->AudioMux()->SetSource(B_MUTE_AUDIO);
	                        break;

	                case kCamera:
	                        fVideoSource->VideoMux()->SetSource(B_COMPOSITE_1);
	                        fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
	                        break;

	                default:
	                        break;
	        }

	        // VideoFormat
	        switch(vidSettings.fVideoSourceSettings.fFormat)
	        {
	                case kNTSC_M:
	                        fVideoSource->SetVideoFormat(B_NTSC_M);
	                        break;

	                case kNTSC_J:
	                        fVideoSource->SetVideoFormat(B_NTSC_J);
	                        break;

	                case kPAL_BG:
	                        fVideoSource->SetVideoFormat(B_PAL_M);
	                        break;

	                case kPAL_I:
	                        fVideoSource->SetVideoFormat(B_PAL_N);
	                        break;

	                case kSECAM:
	                        fVideoSource->SetVideoFormat(B_SECAM);
	                        break;

	                default:
	                        fVideoSource->SetVideoFormat(B_NTSC_M);
	                        break;

	        }

	        // VideoFilters
	        fVideoSource->VideoControls()->SetGammaCorrectionRemoval(vidSettings.fVideoSourceSettings.fFilters.fGammaCorrection);
	        fVideoSource->VideoControls()->SetLumaCoring(vidSettings.fVideoSourceSettings.fFilters.fLumaCoring);
	        fVideoSource->VideoControls()->SetChromaCombFilter(vidSettings.fVideoSourceSettings.fFilters.fChromaComb);
	        fVideoSource->VideoControls()->SetLumaCombFilter(vidSettings.fVideoSourceSettings.fFilters.fLumaComb);
	        fVideoSource->VideoControls()->SetErrorDiffusion(vidSettings.fVideoSourceSettings.fFilters.fErrorDiffusion);
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
		fVidCapView->MessageReceived(message);
		break;
	/*
	   if(fIsRecording)
	   {
	        fIsRecording = false;

	        // Update status bar
	        BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	        textMessage->AddString("StatusText", "Ready to Capture...");
	        fStatusBar->MessageReceived(textMessage);
	        delete textMessage;
	   }
	   else
	   {
	        fIsRecording = true;
	        Record();
	   }
	   break;*/

	default:
		BWindow::MessageReceived(message);
		break;
	}
}
