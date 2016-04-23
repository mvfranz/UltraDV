//---------------------------------------------------------------------
//
//	File:	TPauseCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	
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
#include "MuseumUtils.h"
#include "ResourceManager.h"

#include "TCueView.h"
#include "TCueChannel.h"
#include "TPauseCue.h"
// ABH does not exist
#include "TPauseSetup.h"
#include "TPlaybackEngine.h"
#include "TTimer.h"
#include "TBitmapView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPauseCue::TPauseCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point,  long startTime) : 
			TCueView(id, parent, bounds, point, startTime, "PauseCue")
{	
	// Perform default initialization	
	Init();
	
	OpenEditor();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a CueChunk

TPauseCue::TPauseCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds) : 
			TCueView(theChunk, parent, bounds, "PauseCue")
{	
	// Perform default initialization
	Init(theChunk);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TPauseCue::TPauseCue(BMessage *data) : TCueView(data)
{	
	// Load cue icon
	LoadCueIcon();	
	
	// Set up default settings
	m_IsLocked 		= false;	
	m_IsSelected 	= false;
	m_Editor		= NULL;
	
	// Add member variables
	data->FindInt16("PauseType", &m_PauseType);
	data->FindInt32("PauseDuration", &m_PauseDuration);
	//data->FindInt8("Key", &m_Key);
	data->FindInt32("Message", &m_Message);
	data->FindBool("ShowCursor", &m_ShowCursor);				

}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPauseCue::~TPauseCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TPauseCue::Init()
{	
	
	// Set up default settings
	m_IsLocked = false;
	
	m_IsSelected = false;
	
	m_LowColor 		= kWhite;	
	m_HighColor 	= kBlack;	
	m_IsPrepared 	= false;
	m_IsPlaying 	= false;
	
	// Set up default duration
	m_Duration 			= 1000;
	m_HasDuration 		= false;
	
	m_CanStretch		= false;	// true if cue is stretchable
	m_CanWindow			= false;	// true if cue can window into file
	m_CanLoop			= false;	// true if cue can loop
	m_CanEnvelope		= false;	// true if cue can volume envelope
	m_HasDuration		= true;		// true if cue has a duration
	m_IsVisible 		= false;	// true if cue is visual
	m_CanCrop			= false;	// true if cue can visual crop
	m_CanTransition 	= false;	// true if cue can transition
	m_CanPath			= false;	// true if cue can path
	m_HasEditor			= true;		// true if cue has internal editor
		
	// Pause cue defaults
	m_PauseType			= kSecondsPause;
	m_PauseDuration		= 1000;
	m_ShowCursor 		= true;
	m_Key				= 'A';
	m_Message			= NULL;	
	m_PauseTimer		= NULL;
	
	// Default initialization
	TCueView::Init();
	
	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue(this, m_InsertPoint, true))
	{
		m_Channel->AddChild(this);
		m_Channel->InsertCue(this, m_InsertPoint, m_InsertTime);		
		Select();								
	}

	// We are now fully instantiated
	m_IsInstantiated = true;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Init from BaseCueChunk	
//

void TPauseCue::Init(BaseCueChunk *theChunk)
{
	TCueView::Init(theChunk);	
}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TPauseCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TPauseCue") ) 
		return new TPauseCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TPauseCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TPauseCue");
		
		// Add member variables
		data->AddInt16("PauseType", m_PauseType);
		data->AddInt32("PauseDuration", m_PauseDuration);
		data->AddInt8("Key", m_Key);
		data->AddInt32("Message", m_Message);
		data->AddBool("ShowCursor", m_ShowCursor);				
	}
	
	Looper()->Unlock();
		
	return myErr;
}

#pragma mark -
#pragma mark === Drawing Functions ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TPauseCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();
			
	// Restore color
	SetHighColor(saveColor);
	
	// Pass up to parent
	TCueView::Draw(updateRect);

}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TPauseCue::MouseDown(BPoint where)
{
	// Pass up to parent
	TCueView::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TPauseCue::MouseUp(BPoint where)
{
	// Pass up to parent
	TCueView::MouseUp(where);
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TPauseCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Pass up to parent
	TCueView::MouseMoved(where, code, a_message);
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TPauseCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TPauseCue::KeyDown(const char *bytes, int32 numBytes)
{	
	TCueView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TPauseCue::KeyUp(const char *bytes, int32 numBytes)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TPauseCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{	
		case PAUSE_DONE_MSG:
			TPlaybackEngine *theEngine;
			message->FindPointer("PlaybackEngine", (void**)&theEngine);
			theEngine->Resume();
			break;
			
		default:
			TCueView::MessageReceived(message);						
			break;
			
	}
}

#pragma mark -
#pragma mark === Cue Editor ===

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TPauseCue::OpenEditor()
{
		printf("PauseSettingsWindow not implemented - see TPauseCue.cpp\n");
#ifdef ABH

	// If editor is already open, bring it to front
	if (m_EditorOpen)
	{

		if (m_Editor)
		{	
			m_Editor->Show();
			m_Editor->Activate(true);
		}

	}
	else
	{		
		BRect bounds(50, 50, 350, 350);
		m_EditorOpen = true;
		
		BMessage *theMessage = GetWindowFromResource("PauseSettingsWindow");
		ASSERT(theMessage);
		m_Editor = new TPauseSetup(this, theMessage);
		
		if (m_Editor)
		{
			CenterWindow((BWindow *)m_Editor);
			m_Editor->Show();
		}
		else
			m_EditorOpen = false;
	}
#endif

}




#pragma mark -
#pragma mark === Playback Routines ====

//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//	Prepare cue for playback
//

void TPauseCue::Preroll(TPlaybackEvent *playbackEvent, double currentTime)
{
	
	// Do we have an event at this time?  Check for the following:
	//	PAUSE_EVENT
					
	// Get times
	double cueStartTime = m_StartTime;
	double cueEndTime 	 = m_StartTime + m_Duration;
		
	//	If the startTime == currentTime, check for pause duration
	if (cueStartTime == currentTime)
	{																			
		// Set up cue event
		TCueEvent *cueEvent = new TCueEvent;
		cueEvent->m_Time = currentTime;
		cueEvent->m_Cue = this;
		
		BMessage *pauseMsg = new BMessage(PAUSE_EVENT);
		
		pauseMsg->AddInt16("PauseType", m_PauseType); 
		pauseMsg->AddInt32("PauseDuration", m_PauseDuration); 
				
		cueEvent->m_Events.AddItem( pauseMsg);
				
		playbackEvent->m_CueEvents.AddItem(cueEvent);
	}
						
}


//---------------------------------------------------------------------
//	Play
//---------------------------------------------------------------------
//
//	Tell cue we are playing
//

void TPauseCue::Play()
{
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Tell cue we are pausing
//

void TPauseCue::Pause()
{
}


//---------------------------------------------------------------------
//	DoPause
//---------------------------------------------------------------------
//
//	Main pause routine that halts playback.  This differs fron Pause(),
//	as Pause() is reactive.  DoPause() is proactive
//

void TPauseCue::DoPause(TPlaybackEngine *theEngine)
{
	if (m_PauseTimer)
	{
		delete m_PauseTimer;
		m_PauseTimer = NULL;
	}
		
	switch(m_PauseType)
	{
		case kSecondsPause:
			{
				theEngine->Pause();
				snooze(m_PauseDuration * 1000);
				theEngine->Resume();
				/*
				BMessage *timerMsg = new BMessage(PAUSE_DONE_MSG);
				if (timerMsg)
				{
					timerMsg->AddPointer("PlaybackEngine", theEngine);
					theEngine->Pause();
					m_PauseTimer = new TTimer(this, timerMsg, m_PauseDuration * 1000);
				}
				*/
			}
			break;
			
		case kMousePause:
			break;
			
		case kKeyPause:
			break;
			
		case kMessagePause:
			break;
					
		default:
			break;
	}
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Tell cue we are resuming playback
//

void TPauseCue::Resume()
{
}

//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Tell cue we are stopping
//

void TPauseCue::Stop()
{
}

#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TPauseCue::LoadCueIcon()
{
	BBitmap *cueIcon = NULL;
	
	cueIcon = GetCicnFromResource("PauseCue");

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "PauseCue",cueIcon, false);
		AddChild(m_CueIcon);		
	}	
}
