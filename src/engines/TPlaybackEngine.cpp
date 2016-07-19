//---------------------------------------------------------------------
//
//	File:	TPlaybackEngine.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.10.98
//
///	Desc:	Main playback thread that coordinates the playback of
//			all cues
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Debug.h>
#include <MediaRoster.h>
#include <TimeSource.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "MuseumApp.h"

#include "TStageWindow.h"
#include "TStageView.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"
#include "TCueView.h"
#include "TCueChannel.h"

#include "TPlaybackEngine.h"

// Constants
const int16 kMsecsPerSecond = 1000;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPlaybackEngine::TPlaybackEngine(TCueSheetView* cueSheet) : BMediaNode("PlaybackEngine")
{
	//	Stash parent cue sheet
	fCueSheet = cueSheet;

	//	Set up member variables
	fTimeToQuit     = false;
	fIsPlaying      = false;
	fIsStopping     = false;

	//	Create our port
	fPort = create_port(1, "PlaybackEnginePort");

	//	Create port service thread
	fServiceThread = spawn_thread(service_routine, "PlaybackEngine:Service", B_NORMAL_PRIORITY, this);
	resume_thread(fServiceThread);

	//	Create run thread
	int runPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 1000);
	fRunThread = spawn_thread(run_routine, "PlaybackEngine::Run", runPrio, this);
	resume_thread(fRunThread);

	//	Set up time source
	bool retVal = BMediaRoster::Roster()->GetSystemTimeSource(&fTimeSource);

}



//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPlaybackEngine::~TPlaybackEngine()
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


#pragma mark -
#pragma mark === Playback Handling ===

//---------------------------------------------------------------------
//	Start
//---------------------------------------------------------------------
//
//	Start the timesource
//

void TPlaybackEngine::Start(bigtime_t performance_time)
{
	/*
	   // Do initial preroll
	   uint32 currentTime   = GetCurrentTime();
	   uint32 clipTime      = (int32)(currentTime) % kMsecsPerSecond;
	   uint32 endTime               = (currentTime + kPrerollTime) - clipTime;
	   Preroll( currentTime, endTime);

	   //	Set our first time flag.  We use this to track prerolls that start at an
	   //	odd time withing the cue sheet, such as 1023, 3044, etc.
	   fFirstTime = true;

	   // Start Ticker
	   if (fTicker)
	        fTicker->Start();
	   else
	   {
	        // Create ticker
	        fTicker = new TTicker( GetFPSValue( GetCurrentTimeFormat() ), "playbackTick", B_REAL_TIME_PRIORITY);

	        // Reconnect all chasers to ticker port
	        ReconnectAll();

	        // Set to proper timecode type
	        SetTimecode( GetCurrentTimeFormat() );

	        // Begin playback
	        fTicker->Start();
	   }
	 */

	//	Are we playing?  If so, ignore play request
	if (fIsPlaying)
		return;

	//	Seek time source
	status_t retVal;
	retVal = BMediaRoster::Roster()->SeekNode(fTimeSource, performance_time);
	retVal = BMediaRoster::Roster()->StartNode(fTimeSource, performance_time);

	//	Get fLastTime and convert to milliseconds
	fLastTime = TimeSource()->Now();
	fLastTime /= 1000;

	//	Start TimeSource at current time
	fIsPlaying = true;


	return;
}

//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//

void TPlaybackEngine::Stop(bigtime_t performance_time, bool immediate)
{
	//	Are we playing?
	if (fIsPlaying) {
		fIsPlaying = false;

		status_t retVal = BMediaRoster::Roster()->StopNode(fTimeSource, performance_time);
	}
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//
//

void TPlaybackEngine::Pause()
{
	//	Are we playing?
	if (fIsPlaying) {
		fIsPlaying = false;

	}
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//

void TPlaybackEngine::Resume()
{
	if (!fIsPlaying) {
		//Play();
	}
}


//---------------------------------------------------------------------
//	Seek
//---------------------------------------------------------------------
//
//

void TPlaybackEngine::Seek(bigtime_t media_time, bigtime_t performance_time)
{

}

//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//

void TPlaybackEngine::Preroll()
{
}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	HandleMessage
//-------------------------------------------------------------------
//
//

status_t TPlaybackEngine::HandleMessage(int32 message, const void* data, size_t size)
{
	switch(message)
	{

	//	Call derived classes
	default:
	{
		if ( BMediaNode::HandleMessage(message, data, size) ) {
			BMediaNode::HandleBadMessage(message, data, size);
		}
	}
	break;
	}

	return B_OK;
}


#pragma mark -
#pragma mark === Playback List Handling ===


//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//	Prepare the playback list.  Travel from start to end time constucting
//	an array of all cues that have an event at that time.
//

/*
   void TPlaybackEngine::Preroll( uint32 startTime, uint32 endTime)
   {
        // Clean up old array
        for (int32 listIndex = 0; listIndex < kPlaybackItems; listIndex++)
        {
                // Free list
                if ( fPlaybackVector[listIndex] )
                        delete fPlaybackVector[listIndex];

                // Set to NULL
                fPlaybackVector[listIndex] = 0;
        }

        // Increment by a millisecond
        uint32  incrementor = 1.0;
        uint32  currentTime;
        int32		arrayIndex;

        // Reset array index
        fArrayIndex = 0;

        // Iterate from start to end.  Increment by milliseconds...
        for ( currentTime = startTime, arrayIndex = 0; currentTime < endTime; currentTime += incrementor, arrayIndex++)
        {
                //      Start building the playback list.  Go through all channels
                //	and check for valid cues
                for ( int32 index = 0; index < fCueSheet->GetTotalChannels(); index++)
                {
                        TCueChannel *theChannel = (TCueChannel *)fCueSheet->GetChannelList()->ItemAt(index);

                        // We have a valid channel.  Get cue at this time slice...
                        if (theChannel)
                        {
                                TCueView *theCue = theChannel->GetCueAtTime(currentTime);

                                //      We have a cue at this time.  Inform it to Preroll and add a pointer
                                //	to it to our PlaybackVector
                                if (theCue && theCue->IsMuted() == false)
                                {
                                        // Preroll cue if start time matched current time
                                        if (theCue->StartTime() == currentTime)
                                        {
                                                theCue->Preroll(currentTime);
                                        }

                                        if ( fPlaybackVector[arrayIndex] == NULL)
                                        {
                                                // Create a list to store pointer in
                                                fPlaybackVector[arrayIndex] = new BList();
                                                fPlaybackVector[arrayIndex]->AddItem(theCue);
                                        }
                                        else
                                        {
                                                fPlaybackVector[arrayIndex]->AddItem(theCue);
                                        }
                                }
                        }
                }
        }
   }
 */

#pragma mark -
#pragma mark === BMediaNode Routines ===

//---------------------------------------------------------------------
//	ControlPort
//---------------------------------------------------------------------
//
//	Return our port ID
//

port_id TPlaybackEngine::ControlPort() const
{
	return fPort;
}


//---------------------------------------------------------------------
//	AddOn
//---------------------------------------------------------------------
//
//	We are not an add0n,  Return NULL
//


BMediaAddOn* TPlaybackEngine::AddOn( int32* internal_id) const
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

status_t TPlaybackEngine::service_routine(void* data)
{
	((TPlaybackEngine*)data)->ServiceRoutine();

	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void TPlaybackEngine::ServiceRoutine()
{
	while (!fTimeToQuit) {
		//	Read message
		status_t err  = 0;
		int32 code = 0;
		char msg[B_MEDIA_MESSAGE_SIZE];

		err = read_port_etc(fPort, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);

		if (err == B_TIMED_OUT)
			continue;

		if (err < B_OK) {
			printf("TPlaybackEngine::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
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

status_t TPlaybackEngine::run_routine(void* data)
{
	((TPlaybackEngine*)data)->RunRoutine();

	return 0;
}



//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void TPlaybackEngine::RunRoutine()
{
	uint32 curTime;

	while(!fTimeToQuit) {
		snooze(20*1000);

		//	We are stopping.  Inform media_server
		if (fIsStopping == true) {
			fIsStopping     = false;
			fIsPlaying              = false;
		}

		//	Update current time
		if (fIsPlaying) {
			curTime = TimeSource()->Now();

			//	Convert to milliseconds
			curTime /= 1000;

			uint32 diff = curTime - fLastTime;

			uint32 newTime = fCueSheet->GetCurrentTime() + diff;

			//	Stop playback if we are past our duration
			if(newTime > fCueSheet->StartTime() + fCueSheet->Duration()) {
				newTime = fCueSheet->StartTime() + fCueSheet->Duration();
				Stop(0, true);
			}

			fCueSheet->SetCurrentTime(newTime);
			fLastTime = curTime;

			//printf("Current Time: %.4f\n", (double)fCueSheet->GetCurrentTime()/1000000.);
		}
	}
}
