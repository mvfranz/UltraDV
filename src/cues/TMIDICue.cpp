//---------------------------------------------------------------------
//
//	File:	TMIDICue.cpp
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
#include <assert.h>
#include <MidiKit.h>

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TMIDICue.h"
#include "TTimeTextView.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"

#include "TMIDIFile.h"
#include "TMIDIEditor.h"
#include "TMIDIEngine.h"
#include "TMIDIConductor.h"
#include "TMIDITrack.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDICue::TMIDICue(int16 id, TCueChannel* parent, BRect bounds, uint32 startTime) :
	TCueView(id, parent, bounds, startTime, "MIDICue")
{
	// Init member variables
	fEditor = NULL;

	fTrackList = new BList();

	// Load MIDI file
	ShowPanel();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TMIDICue::TMIDICue(entry_ref &theRef, int16 id,  TCueChannel* parent, BRect bounds, uint32 startTime) :
	TCueView(id, parent, bounds, startTime, "MIDICue")
{
	// Init member variables
	fEditor = NULL;

	//
	// Attempt to load data file
	//

	BNode theNode(&theRef);
	if (theNode.InitCheck() != B_OK)
		return;

	BNodeInfo nodeInfo(&theNode);
	if (nodeInfo.InitCheck() != B_OK)
		return;

	// First, make sure we have a valid ref
	if ( IsMIDI(nodeInfo) ) {
		//      Create a BMessage that includes the entry_ref to send to our open routine
		BMessage* theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);

		bool retVal = LoadMIDIFile(theMessage);

		// We are succesful.  Init the cue
		if (retVal) {
			Init();
		}
		// If we have an error, ask the user to locate a new data file
		else{
			ShowPanel();
		}
	}
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDICue::~TMIDICue()
{
	// Close editor if open...
	if ( fEditorOpen && fEditor) {
		fEditor->Hide();
		fEditor->Lock();
		fEditor->Quit();
	}

	// Stop playback if neccessary
	fIsPlaying = false;

	// Close and delete TMIDIFile
	if (fMIDIFile)
		delete fMIDIFile;

	// Close and free TMIDIStore
	if (fMIDIStore)
		delete fMIDIStore;

	// Close and free TMIDIEngine
	if (fMIDIEngine)
		delete fMIDIEngine;

	// Free track list
	if (fTrackList) {
		// Delete tracks in list
		for (int32 index = 0; index < fTrackList->CountItems(); index++) {
			TMIDITrack* theTrack =  (TMIDITrack*)fTrackList->ItemAt(index);

			if (theTrack)
				delete theTrack;
		}

		delete fTrackList;
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TMIDICue::Init()
{

	// Editor is closed
	fEditorOpen = false;

	// Set up default settings
	fIsLocked                       = false;
	fIsSelected             = false;
	fLowColor                       = kWhite;
	fHighColor              = kBlack;
	fIsPrepared             = false;
	fIsPlaying              = false;
	fIsVisible                      = false;
	fHasDuration            = true;
	fCanLoop                        = true;
	fCanStretch             = true;
	fCanEnvelope            = true;
	fHasEditor              = true;
	fCanWindow                      = true;
	fCanTransition          = false;
	fCanPath                        = false;

	// Default initialization
	TCueView::Init();

	// Create MIDIStore and MIDIEngine
	fMIDIStore = new BMidiStore();
	assert(fMIDIStore);
	fMIDIStore->Import(&fFileRef);
	fMIDIEngine = new TMIDIEngine(fMIDIStore);

	// Set cues fFile to point to the fMIDIFile
	fFile = fMIDIFile;

	// Calcaluate duration
	CalcDuration();

	// Adjust bounds based on cue duration
	int32 newWidth = TimeToPixels(fDuration, GetCurrentTimeFormat(), GetCurrentResolution());
	ResizeBy( newWidth - Bounds().Width(), 0);

	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertTime, true)) {
		fChannel->InsertCue(this, fInsertTime);
		Select();

		// We are now fully instantiated
		fIsInstantiated = true;
	}

	// Set expanded status
	if (fChannel->IsExpanded()) {
		fIsExpanded = false;
		Expand();
	} else {
		fIsExpanded = true;
		Contract();
	}
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TMIDICue::Draw(BRect updateRect)
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

void TMIDICue::MouseDown(BPoint where)
{
	// Play midi file
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (long*)&buttons);

	// Determine which button has been clicked
	switch(buttons)
	{
	case B_PRIMARY_MOUSE_BUTTON:
		//fMIDIEngine->Play();
		break;

	case B_SECONDARY_MOUSE_BUTTON:
		//fMIDIEngine->Stop();
		break;

	}

	// Pass up to parent
	TCueView::MouseDown(where);
}

#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TMIDICue::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
	case B_OK:
	case B_REFS_RECEIVED:
	{
		fPanel->Hide();

		// Attempt to load MIDI file
		if ( LoadMIDIFile(message) )
			Init();
	}
	break;

	case MIDI_PLAYBACK_DONE_MSG:
	{
		fSynthFile->Fade();
		fSynthFile->Stop();
		fSynthFile->UnloadFile();
		fIsPlaying = false;
		kill_thread(fPlaybackThread);
		fPlaybackThread = NULL;
	}
	break;

	default:
		TCueView::MessageReceived(message);
		break;
	}
}


#pragma mark -
#pragma mark === File Handling ===

//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TMIDICue::ShowPanel()
{

	TCueView::ShowPanel();

	//      Create messenger to send panel messages to our channel.  We cannot send it to
	//  ourself as we are not part of the view heirarchy.
	BMessenger* messenger = new BMessenger( fChannel,  ((MuseumApp*)be_app)->GetCueSheet());

	// Create message containing pointer to ourself
	BMessage* message = new BMessage();
	message->AddPointer("TheCue", this);

	// Construct a file panel and set it to modal
	fPanel = new BFilePanel( B_OPEN_PANEL, messenger, NULL, B_FILE_NODE, false, message, NULL, true, true );

	// Set it to application's home directory
	app_info appInfo;
	be_app->GetAppInfo(&appInfo);
	BEntry entry(&appInfo.ref);
	BDirectory parentDir;
	entry.GetParent(&parentDir);
	fPanel->SetPanelDirectory(&parentDir);

	// Center Panel
	CenterWindow(fPanel->Window());
	fPanel->Show();

	// Clean up
	delete messenger;
	delete message;

}



//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TMIDICue::HidePanel()
{
	if(fPanel) {
		delete fPanel;
		fPanel = NULL;
	}

	TCueView::HidePanel();
}

//---------------------------------------------------------------------
//	LoadMIDIFile
//---------------------------------------------------------------------
//
//	Load a MIDI file using BSoundFile
//

bool TMIDICue::LoadMIDIFile(BMessage* message)
{

	TMIDITrack* curTrack;
	int32 time, totalTime;
	int16 tempoRatio = kTempoDivisor;

	// Load MIDI file
	message->FindRef("refs", 0, &fFileRef);

	// Resolve possible symlink...
	BEntry entry(&fFileRef, true);
	entry.GetRef(&fFileRef);

	// Create and load midi file
	if (!fMIDIFile) {
		//	Load and read MIDI file
		fMIDIFile = new TMIDIFile(&fFileRef, B_READ_ONLY);
		if (!fMIDIFile)
			return false;

		//      Make sure it is a MIDI file.  If not, alert user and return false
		//if ( strcmp((char *)&fMIDIFile->fMidiChunkHeader.chunkType), "MThd") != 0)
		//{
		//	UnhandledMIDIFormat(&fFileRef);
		//	return false;
		//}

		// Create conductor
		fMIDIConductor = new TMIDIConductor(this);
		if (!fMIDIConductor)
			return false;

		if (fMIDIFile->fHeader.format == 0) {
			// Allocate data track
			curTrack = new TMIDITrack(this);
			totalTime = fMIDIFile->ReadType0(fMIDIConductor, curTrack);
			curTrack->ExtractInfo();

			// Add to track list
			fTrackList->AddItem(curTrack);

		} else {
			// Read in conductor
			totalTime = fMIDIFile->ReadConductor(fMIDIConductor);

			// Now read tracks
			for (int32 index = 0; index < fMIDIFile->fHeader.numTracks; index++) {
				// Allocate next track
				curTrack = new TMIDITrack(this);
				time = fMIDIFile->ReadNextTrack(curTrack);
				curTrack->ExtractInfo();

				// Add to track list
				fTrackList->AddItem(curTrack);

				if (time > totalTime)
					totalTime = time;
			}
		}

		// Complete setup of conductor track
		fMIDIConductor->SetEndTime(totalTime);
		fMIDIConductor->SetTempoRatio(tempoRatio);
		fMIDIConductor->CalcMarkerTimes();

		// Close file

	}

	return true;
}


#pragma mark -
#pragma mark === File Playback ===

//---------------------------------------------------------------------
//	PlayMidiFile
//---------------------------------------------------------------------
//
//	Internal PlayMidiFile that works with thread callback
//

int32 TMIDICue::PlayMidiFile()
{
	// Do nothing until we stop playback or file is done playing
	while( (fIsPlaying == true) ) {
		// Pause for a while...
		snooze(1000000);

		// If we are at the end, break out of loop
		if ( fSynthFile->IsFinished() )
			fIsPlaying = false;
	}

	// Clean up file
	delete fSynthFile;

	return true;
}


//---------------------------------------------------------------------
//	play_midi_file
//---------------------------------------------------------------------
//
//	Static thread function
//

int32 TMIDICue::play_midi_file(void* arg)
{
	TMIDICue* obj = (TMIDICue*)arg;
	return(obj->PlayMidiFile() );
}


//---------------------------------------------------------------------
//	Play
//---------------------------------------------------------------------
//
//	Function used within thread to play back MIDI file
//


void TMIDICue::PlayCue(uint32 theTime)
{

	// We are playing
	fIsPlaying = true;

	// Create synth file
	fSynthFile = new BMidiSynthFile();

	// Load file, set completion hook and start playback
	status_t myErr = fSynthFile->LoadFile(&fFileRef);
	fSynthFile->Start();

	// Create thread and let it do the playback...
	fPlaybackThread = spawn_thread( play_midi_file, "MIDIPlayer", B_REAL_TIME_DISPLAY_PRIORITY, (void*) this);

	if ( fPlaybackThread == B_NO_ERROR )
		resume_thread(fPlaybackThread);

}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Pause playback of MIDI file
//

void TMIDICue::PauseCue()
{
	fSynthFile->Pause();
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Resume playback of MIDI file
//

void TMIDICue::ResumeCue()
{
	fSynthFile->Resume();
}

//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop playback of MIDI file
//

void TMIDICue::StopCue(uint32 theTime)
{
	fIsPlaying = false;
}



//---------------------------------------------------------------------
//	CompletionHook
//---------------------------------------------------------------------
//
//	Function called at completion of MIDI file playback
//

void TMIDICue::CompletionHook(int32 arg)
{
}


//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TMIDICue::OpenEditor()
{

}


#pragma mark === Time Utilities ===

//---------------------------------------------------------------------
//	CalcDuration
//---------------------------------------------------------------------
//
//	Sets the file time (and initial duration)
//

void TMIDICue::CalcDuration()
{
//	if (waitingForRecord)
//	{
//		itsDuration = itsRecordTask->GetDefaultLength();
//		itsFileTime = itsDuration;
//	}
//	else
	{
		fFileTime = fMIDIConductor->GetTotalTimeMSec();

		// Scale time by tempo ratio
		fFileTime *= fMIDIConductor->GetTempoRatio();
		fFileTime /= kTempoDivisor;
		fDuration = fFileTime;
	}
}


#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TMIDICue::LoadCueIcon()
{
	BBitmap* cueIcon = GetAppIcons()->fMIDIUpIcon;

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(fCueIcon);
	}

	//	Pass up to parent
	TCueView::LoadCueIcon();

}
