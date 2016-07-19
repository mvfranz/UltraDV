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
	fIsLocked               = false;
	fIsSelected     = false;
	fEditor         = NULL;

	// Add member variables
	data->FindInt16("PauseType", &fPauseType);
	data->FindInt32("PauseDuration", &fPauseDuration);
	//data->FindInt8("Key", &fKey);
	data->FindInt32("Message", &fMessage);
	data->FindBool("ShowCursor", &fShowCursor);

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
	fIsLocked = false;

	fIsSelected = false;

	fLowColor               = kWhite;
	fHighColor      = kBlack;
	fIsPrepared     = false;
	fIsPlaying      = false;

	// Set up default duration
	fDuration                       = 1000;
	fHasDuration            = false;

	fCanStretch             = false;        // true if cue is stretchable
	fCanWindow                      = false;        // true if cue can window into file
	fCanLoop                        = false;        // true if cue can loop
	fCanEnvelope            = false;        // true if cue can volume envelope
	fHasDuration            = true;         // true if cue has a duration
	fIsVisible              = false;        // true if cue is visual
	fCanCrop                        = false;        // true if cue can visual crop
	fCanTransition  = false;        // true if cue can transition
	fCanPath                        = false;        // true if cue can path
	fHasEditor                      = true;         // true if cue has internal editor

	// Pause cue defaults
	fPauseType                      = kSecondsPause;
	fPauseDuration          = 1000;
	fShowCursor             = true;
	fKey                            = 'A';
	fMessage                        = NULL;
	fPauseTimer             = NULL;

	// Default initialization
	TCueView::Init();

	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertPoint, true)) {
		fChannel->AddChild(this);
		fChannel->InsertCue(this, fInsertPoint, fInsertTime);
		Select();
	}

	// We are now fully instantiated
	fIsInstantiated = true;
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

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TPauseCue");

		// Add member variables
		data->AddInt16("PauseType", fPauseType);
		data->AddInt32("PauseDuration", fPauseDuration);
		data->AddInt8("Key", fKey);
		data->AddInt32("Message", fMessage);
		data->AddBool("ShowCursor", fShowCursor);
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
	if (fEditorOpen) {

		if (fEditor) {
			fEditor->Show();
			fEditor->Activate(true);
		}

	} else   {
		BRect bounds(50, 50, 350, 350);
		fEditorOpen = true;

		BMessage *theMessage = GetWindowFromResource("PauseSettingsWindow");
		ASSERT(theMessage);
		fEditor = new TPauseSetup(this, theMessage);

		if (fEditor) {
			CenterWindow((BWindow *)fEditor);
			fEditor->Show();
		} else
			fEditorOpen = false;
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
	double cueStartTime = fStartTime;
	double cueEndTime        = fStartTime + fDuration;

	//	If the startTime == currentTime, check for pause duration
	if (cueStartTime == currentTime) {
		// Set up cue event
		TCueEvent *cueEvent = new TCueEvent;
		cueEvent->fTime = currentTime;
		cueEvent->fCue = this;

		BMessage *pauseMsg = new BMessage(PAUSE_EVENT);

		pauseMsg->AddInt16("PauseType", fPauseType);
		pauseMsg->AddInt32("PauseDuration", fPauseDuration);

		cueEvent->fEvents.AddItem( pauseMsg);

		playbackEvent->fCueEvents.AddItem(cueEvent);
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
	if (fPauseTimer) {
		delete fPauseTimer;
		fPauseTimer = NULL;
	}

	switch(fPauseType)
	{
	case kSecondsPause:
	{
		theEngine->Pause();
		snooze(fPauseDuration * 1000);
		theEngine->Resume();
		/*
		   BMessage *timerMsg = new BMessage(PAUSE_DONE_MSG);
		   if (timerMsg)
		   {
		        timerMsg->AddPointer("PlaybackEngine", theEngine);
		        theEngine->Pause();
		        fPauseTimer = new TTimer(this, timerMsg, fPauseDuration * 1000);
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

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon = new TBitmapView(area, "PauseCue",cueIcon, false);
		AddChild(fCueIcon);
	}
}
