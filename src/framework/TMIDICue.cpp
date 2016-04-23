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

TMIDICue::TMIDICue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) : 
			TCueView(id, parent, bounds, startTime, "MIDICue")
{	
	// Init member variables 
	m_Editor	= NULL;

	m_TrackList = new BList();
	
	// Load MIDI file
	ShowPanel();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TMIDICue::TMIDICue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime) : 
	TCueView(id, parent, bounds, startTime, "MIDICue")
{	
	// Init member variables 
	m_Editor	= NULL;
		
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
	if ( IsMIDI(nodeInfo) )
	{
		// 	Create a BMessage that includes the entry_ref to send to our open routine
		BMessage *theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);
		
		bool retVal = LoadMIDIFile(theMessage);
		
		// We are succesful.  Init the cue
		if (retVal)
		{
			Init();
		}
		// If we have an error, ask the user to locate a new data file
		else
		{
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
	if ( m_EditorOpen && m_Editor)
	{
		m_Editor->Hide();
		m_Editor->Lock();
		m_Editor->Quit();
	}
	
	// Stop playback if neccessary
	m_IsPlaying = false;
	
	// Close and delete TMIDIFile	
	if (m_MIDIFile)
		delete m_MIDIFile;	
		
	// Close and free TMIDIStore
	if (m_MIDIStore)
		delete m_MIDIStore;
	
	// Close and free TMIDIEngine
	if (m_MIDIEngine)
		delete m_MIDIEngine;
			
	// Free track list
	if (m_TrackList)
	{
		// Delete tracks in list
		for (int32 index = 0; index < m_TrackList->CountItems(); index++)
		{
			TMIDITrack	*theTrack =  (TMIDITrack *)m_TrackList->ItemAt(index);
			
			if (theTrack)
				delete theTrack;						
		}
				
		delete m_TrackList;	
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
	m_EditorOpen = false;
	
	// Set up default settings	
	m_IsLocked 			= false;	
	m_IsSelected 		= false;	
	m_LowColor 			= kWhite;	
	m_HighColor 		= kBlack;	
	m_IsPrepared 		= false;
	m_IsPlaying 		= false;	
	m_IsVisible			= false;	
	m_HasDuration 		= true;					
	m_CanLoop			= true;
	m_CanStretch		= true;		
	m_CanEnvelope		= true;
	m_HasEditor 		= true;		
	m_CanWindow			= true;
	m_CanTransition		= false;
	m_CanPath			= false;
					
	// Default initialization
	TCueView::Init();
	
	// Create MIDIStore and MIDIEngine
	m_MIDIStore = new BMidiStore();
	assert(m_MIDIStore);
	m_MIDIStore->Import(&m_FileRef);
	m_MIDIEngine = new TMIDIEngine(m_MIDIStore);
		
	// Set cues m_File to point to the m_MIDIFile
	m_File = m_MIDIFile;
		
	// Calcaluate duration
	CalcDuration();	
	
	// Adjust bounds based on cue duration
	int32 newWidth = TimeToPixels(m_Duration, GetCurrentTimeFormat(), GetCurrentResolution());
	ResizeBy( newWidth - Bounds().Width(), 0);
	
	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue(this, m_InsertTime, true))
	{
		m_Channel->InsertCue(this, m_InsertTime);		
		Select();
		
		// We are now fully instantiated
		m_IsInstantiated = true;
	}
	
	// Set expanded status
	if (m_Channel->IsExpanded())
	{
		m_IsExpanded = false;
		Expand();
	}
	else
	{
		m_IsExpanded = true;
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
	uint32 	buttons = 0;				
	Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);
	
	// Determine which button has been clicked
	switch(buttons)
	{
		case B_PRIMARY_MOUSE_BUTTON:		
			//m_MIDIEngine->Play();
			break;
			
		case B_SECONDARY_MOUSE_BUTTON:
			//m_MIDIEngine->Stop();
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
void TMIDICue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{		
		case B_OK:
		case B_REFS_RECEIVED:
			{
				m_Panel->Hide();
				
				// Attempt to load MIDI file
				if ( LoadMIDIFile(message) )
					Init();						
			}
			break;
													
		case MIDI_PLAYBACK_DONE_MSG:
			{
				m_SynthFile->Fade();
				m_SynthFile->Stop();
				m_SynthFile->UnloadFile();
				m_IsPlaying = false;
				kill_thread(m_PlaybackThread);
				m_PlaybackThread = NULL;
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
 		
 	// 	Create messenger to send panel messages to our channel.  We cannot send it to 
	//  ourself as we are not part of the view heirarchy.
 	BMessenger *messenger = new BMessenger( m_Channel,  ((MuseumApp *)be_app)->GetCueSheet());
 		
 	// Create message containing pointer to ourself
	BMessage *message = new BMessage();
	message->AddPointer("TheCue", this);
	
 	// Construct a file panel and set it to modal
 	m_Panel = new BFilePanel( B_OPEN_PANEL, messenger, NULL, B_FILE_NODE, false, message, NULL, true, true );
 
 	// Set it to application's home directory
 	app_info appInfo;
 	be_app->GetAppInfo(&appInfo); 	
 	BEntry entry(&appInfo.ref);
 	BDirectory parentDir;
 	entry.GetParent(&parentDir);
 	m_Panel->SetPanelDirectory(&parentDir);
 		
	// Center Panel
	CenterWindow(m_Panel->Window());
	m_Panel->Show();
	
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
	if(m_Panel)
	{
		delete m_Panel;
		m_Panel = NULL;
	}
	
	TCueView::HidePanel();
}

//---------------------------------------------------------------------
//	LoadMIDIFile
//---------------------------------------------------------------------
//
//	Load a MIDI file using BSoundFile
//

bool TMIDICue::LoadMIDIFile(BMessage *message)
{
	
	TMIDITrack	*curTrack;
	int32		time, totalTime;
	int16		tempoRatio = kTempoDivisor;
		
	// Load MIDI file
	message->FindRef("refs", 0, &m_FileRef);
	
	// Resolve possible symlink...
	BEntry entry(&m_FileRef, true);
	entry.GetRef(&m_FileRef);
		 	
	// Create and load midi file	
	if (!m_MIDIFile) 
	{				
		//	Load and read MIDI file
		m_MIDIFile = new TMIDIFile(&m_FileRef, B_READ_ONLY);			
		if (!m_MIDIFile)
			return false;
			
		// 	Make sure it is a MIDI file.  If not, alert user and return false
		//if ( strcmp((char *)&m_MIDIFile->m_MidiChunkHeader.chunkType), "MThd") != 0)
		//{
		//	UnhandledMIDIFormat(&m_FileRef);
		//	return false;
		//}
									
		// Create conductor 
		m_MIDIConductor = new TMIDIConductor(this);		
		if (!m_MIDIConductor)
			return false;
				
		if (m_MIDIFile->m_Header.format == 0) 
		{		
			// Allocate data track
			curTrack = new TMIDITrack(this);
			totalTime = m_MIDIFile->ReadType0(m_MIDIConductor, curTrack);
			curTrack->ExtractInfo();
			
			// Add to track list
			m_TrackList->AddItem(curTrack);			
			
		} 
		else 
		{
			// Read in conductor
			totalTime = m_MIDIFile->ReadConductor(m_MIDIConductor);
						
			// Now read tracks
			for (int32 index = 0; index < m_MIDIFile->m_Header.numTracks; index++) 
			{
				// Allocate next track
				curTrack = new TMIDITrack(this);			
				time = m_MIDIFile->ReadNextTrack(curTrack);
				curTrack->ExtractInfo();
				
				// Add to track list
				m_TrackList->AddItem(curTrack);			
				
				if (time > totalTime)
					totalTime = time;
			}						
		}
		
		// Complete setup of conductor track
		m_MIDIConductor->SetEndTime(totalTime);
		m_MIDIConductor->SetTempoRatio(tempoRatio);
		m_MIDIConductor->CalcMarkerTimes();

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
	while( (m_IsPlaying == true) )
	{
		// Pause for a while...
		snooze(1000000);
		
		// If we are at the end, break out of loop
		if ( m_SynthFile->IsFinished() )
			m_IsPlaying = false;					
	}
	
	// Clean up file
	delete m_SynthFile;
	
	return true;
}


//---------------------------------------------------------------------
//	play_midi_file
//---------------------------------------------------------------------
//
//	Static thread function
//

int32 TMIDICue::play_midi_file(void *arg)
{			
	TMIDICue *obj = (TMIDICue *)arg;
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
	m_IsPlaying = true;
	
	// Create synth file
	m_SynthFile = new BMidiSynthFile();
	
	// Load file, set completion hook and start playback
	status_t myErr = m_SynthFile->LoadFile(&m_FileRef); 
	m_SynthFile->Start(); 
	
	// Create thread and let it do the playback...
    m_PlaybackThread = spawn_thread( play_midi_file, "MIDIPlayer", B_REAL_TIME_DISPLAY_PRIORITY, (void *) this);
     						 
    if ( m_PlaybackThread == B_NO_ERROR )
   		resume_thread(m_PlaybackThread);
   		   			
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Pause playback of MIDI file
//

void TMIDICue::PauseCue()
{
	m_SynthFile->Pause();
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Resume playback of MIDI file
//

void TMIDICue::ResumeCue()
{
	m_SynthFile->Resume(); 	
}

//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop playback of MIDI file
//

void TMIDICue::StopCue(uint32 theTime)
{
	m_IsPlaying = false;
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
		m_FileTime = m_MIDIConductor->GetTotalTimeMSec();
		
		// Scale time by tempo ratio
		m_FileTime *= m_MIDIConductor->GetTempoRatio();
		m_FileTime /= kTempoDivisor;
		m_Duration = m_FileTime;
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
	BBitmap *cueIcon = GetAppIcons()->m_MIDIUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(m_CueIcon);		
	}	
	
	//	Pass up to parent
	TCueView::LoadCueIcon();	
	
}
