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
const int16	kMsecsPerSecond = 1000;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPlaybackEngine::TPlaybackEngine(TCueSheetView *cueSheet) : BMediaNode("PlaybackEngine")
{				
	//	Stash parent cue sheet
	m_CueSheet = cueSheet;
	
	//	Set up member variables
	m_TimeToQuit	= false;
	m_IsPlaying 	= false;
	m_IsStopping	= false;
		
	//	Create our port
	m_Port = create_port(1, "PlaybackEnginePort");
			
	//	Create port service thread
	m_ServiceThread = spawn_thread(service_routine, "PlaybackEngine:Service", B_NORMAL_PRIORITY, this);
	resume_thread(m_ServiceThread);	
	
	//	Create run thread
	int runPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 1000);
	m_RunThread = spawn_thread(run_routine, "PlaybackEngine::Run", runPrio, this);	
	resume_thread(m_RunThread);

	//	Set up time source
	bool retVal = BMediaRoster::Roster()->GetSystemTimeSource(&m_TimeSource);
		
}



//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPlaybackEngine::~TPlaybackEngine()
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
	uint32 currentTime 	= GetCurrentTime();
	uint32 clipTime  	= (int32)(currentTime) % kMsecsPerSecond;	
	uint32 endTime 		= (currentTime + kPrerollTime) - clipTime; 
	Preroll( currentTime, endTime);
	
	//	Set our first time flag.  We use this to track prerolls that start at an
	//	odd time withing the cue sheet, such as 1023, 3044, etc.
	m_FirstTime = true;
	
	// Start Ticker	
	if (m_Ticker)
		m_Ticker->Start();
	else
	{
		// Create ticker
		m_Ticker = new TTicker( GetFPSValue( GetCurrentTimeFormat() ), "playbackTick", B_REAL_TIME_PRIORITY);
		
		// Reconnect all chasers to ticker port
		ReconnectAll();
				
		// Set to proper timecode type 
		SetTimecode( GetCurrentTimeFormat() );	
		
		// Begin playback
		m_Ticker->Start();
	}
	*/
	
	//	Are we playing?  If so, ignore play request
	if (m_IsPlaying)
		return;
		
	//	Seek time source
	status_t retVal;
	retVal = BMediaRoster::Roster()->SeekNode(m_TimeSource, performance_time);
	retVal = BMediaRoster::Roster()->StartNode(m_TimeSource, performance_time);
	
	//	Get m_LastTime and convert to milliseconds
	m_LastTime = TimeSource()->Now();
	m_LastTime /= 1000;
	
	//	Start TimeSource at current time
	m_IsPlaying = true;

			
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
	if (m_IsPlaying)
	{		
		m_IsPlaying = false;
		
		status_t retVal = BMediaRoster::Roster()->StopNode(m_TimeSource, performance_time);		
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
	if (m_IsPlaying)
	{
		m_IsPlaying = false;
		
	}
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//

void TPlaybackEngine::Resume()
{
	if (!m_IsPlaying)
	{
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

status_t TPlaybackEngine::HandleMessage(int32 message, const void *data, size_t size)
{
	switch(message)
	{
				
		//	Call derived classes
		default:
			{
				if ( BMediaNode::HandleMessage(message, data, size) )
				{				
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
		if ( m_PlaybackVector[listIndex] )
			delete m_PlaybackVector[listIndex];	
		
		// Set to NULL
		m_PlaybackVector[listIndex] = 0;
	}
	
	// Increment by a millisecond
	uint32 	incrementor = 1.0;
	uint32 	currentTime;
	int32		arrayIndex;
	
	// Reset array index
	m_ArrayIndex = 0;
	
	// Iterate from start to end.  Increment by milliseconds...
	for ( currentTime = startTime, arrayIndex = 0; currentTime < endTime; currentTime += incrementor, arrayIndex++)
	{			
		// 	Start building the playback list.  Go through all channels 
		//	and check for valid cues
		for ( int32 index = 0; index < m_CueSheet->GetTotalChannels(); index++)
		{
			TCueChannel *theChannel = (TCueChannel *)m_CueSheet->GetChannelList()->ItemAt(index);
			
			// We have a valid channel.  Get cue at this time slice...
			if (theChannel)
			{
				TCueView *theCue = theChannel->GetCueAtTime(currentTime);
																		
				// 	We have a cue at this time.  Inform it to Preroll and add a pointer
				//	to it to our PlaybackVector
				if (theCue && theCue->IsMuted() == false)
				{																																					
					// Preroll cue if start time matched current time
					if (theCue->StartTime() == currentTime)
					{
						theCue->Preroll(currentTime);
					}
					
					if ( m_PlaybackVector[arrayIndex] == NULL)
					{										
						// Create a list to store pointer in 
						m_PlaybackVector[arrayIndex] = new BList();
						m_PlaybackVector[arrayIndex]->AddItem(theCue);
					}
					else
					{
						m_PlaybackVector[arrayIndex]->AddItem(theCue);
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
	return m_Port;
}


//---------------------------------------------------------------------
//	AddOn
//---------------------------------------------------------------------
//
//	We are not an add0n,  Return NULL
//


BMediaAddOn* TPlaybackEngine::AddOn( int32 *internal_id) const
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

status_t TPlaybackEngine::service_routine(void * data)
{
	((TPlaybackEngine *)data)->ServiceRoutine();
	
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

status_t TPlaybackEngine::run_routine(void *data)
{
	((TPlaybackEngine *)data)->RunRoutine();
	
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
	
	while(!m_TimeToQuit)
	{
		snooze(20*1000);
	
		//	We are stopping.  Inform media_server
		if (m_IsStopping == true)
		{
			m_IsStopping 	= false;
			m_IsPlaying		= false;	
		}
		
		//	Update current time
		if (m_IsPlaying)
		{			
			curTime = TimeSource()->Now();
			
			//	Convert to milliseconds
			curTime /= 1000;
			
			uint32 diff = curTime - m_LastTime;			
			
			uint32 newTime = m_CueSheet->GetCurrentTime() + diff;
			
			//	Stop playback if we are past our duration
			if(newTime > m_CueSheet->StartTime() + m_CueSheet->Duration())
			{
				newTime = m_CueSheet->StartTime() + m_CueSheet->Duration();
				Stop(0, true);				
			}
			
			m_CueSheet->SetCurrentTime(newTime);
			m_LastTime = curTime;
			
			//printf("Current Time: %.4f\n", (double)m_CueSheet->GetCurrentTime()/1000000.);
		}
	}
}
