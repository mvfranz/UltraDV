//---------------------------------------------------------------------
//
//	File:	TAudioCaptureWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.07.98
//
//	Desc:	Audio capture window
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppAlerts.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "TMuseumIcons.h"

#include "TAudioCaptureToolbar.h"
#include "TVideoCaptureWindow.h"
#include "TAudioLevelsView.h"
#include "TStatusView.h"
#include "TBrowserWindow.h"
#include "TAudioEditor.h"
#include "TSoundRecorder.h"

#include "TAudioCaptureWindow.h"


char audio_buffer[2048];

// Local prototypes
int32	_updater_thread(void *arg);
void 	adc_reader(void *arg, bigtime_t, const void* buf, size_t count, const media_raw_audio_format & format);

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioCaptureWindow::TAudioCaptureWindow(BMessage *theMessage) : BWindow(theMessage)
{			
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioCaptureWindow::~TAudioCaptureWindow()
{
	
	// Exit from all threads
	status_t status;
	m_IsRecording = false;
	wait_for_thread(m_UpdaterThread, &status);
		
	// Dispose of BFilePanel
	HidePanel();
	
	// Close any open files
	if (m_CaptureSettings.audioFile)
	{
		m_CaptureSettings.audioFile->Unset();
		delete m_CaptureSettings.audioFile;
	}
	
	if (m_CaptureSettings.audioBuf)
	{
		delete m_CaptureSettings.audioBuf;
	}	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TAudioCaptureWindow::Init()
{	
	
	//	Init variables
	m_IsRecording 	= false;
	m_RecordNode 	= NULL;
	
	// Create tool bar
	BView *toolbarView = (BView *)FindView("AudioCaptureToolbarView");
	m_Toolbar = new TAudioCaptureToolbar(toolbarView->Bounds());
	RemoveChild(toolbarView);
	AddChild(m_Toolbar);
	m_Toolbar->Show();
	delete toolbarView;
			
	// Find text items
	m_FileSize 			= (BStringView *)FindView("FileSizeString");
	m_SpaceRemaining 	= (BStringView *)FindView("SpaceRemainingString");
		
	// Find Levels Box
	BBox *levelsBox = (BBox *)FindView("LevelsBox");
	
	//  Create AudCap View
	m_AudCapView = (BView *)FindView("AudioCaptureView");
	
	// Add levels view
	BView *levelsView = (BView *)FindView("LevelsView");
	m_LevelsView = new TAudioLevelsView(levelsView->Frame());
	levelsBox->RemoveChild(levelsView);
	levelsBox->AddChild(m_LevelsView);	
	m_LevelsView->Show();
	delete levelsView;

	// Find status view
	BView *statusView = (BView *)FindView("StatusView");
	m_StatusView = new TStatusView(statusView->Frame());
	RemoveChild(statusView);
	delete statusView;
	AddChild(m_StatusView);
	m_StatusView->Show();
			
	// Set up m_CaptureSettings
	m_CaptureSettings.captureMethod 	= kCaptureToDisk;
	m_CaptureSettings.compressMethod 	= kPostCompress;
	m_CaptureSettings.audioBuf			= new BMallocIO();
	m_CaptureSettings.audioFile			= NULL;
	m_CaptureSettings.audioWindow		= this;
	
	// Set panel to NULL
	m_Panel = NULL;
	
	// Set indicators
	m_FileSize->SetText("0k");
	m_SpaceRemaining->SetText("0k");
	
	// Set status text
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	m_StatusView->MessageReceived(textMessage);
	delete textMessage;
	
	// Find temp directory on boot volume and create temp file
	CreateTempFile();

}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioCaptureWindow::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{
		case AUDCAP_RECORD_MSG:			
			{
				if (m_IsRecording)
					StopRecording();
				else
					StartRecording();
			}
			break;
			
		// Select file
		case AUDCAP_FILE_MSG:
			ShowPanel();
			break;
			
		case AUDCAP_PANEL_MSG:
			SelectAudioFile(message);
			break;
												
		// Capture to Disk
		case AUDCAP_DISK_MSG:
			m_CaptureSettings.captureMethod = kCaptureToDisk;
			break;
			
		// Capture to RAM
		case AUDCAP_RAM_MSG:
			m_CaptureSettings.captureMethod = kCaptureToRAM;
			break;
			
		// Post compress
		case AUDCAP_POST_MSG:
			{
				BHandler *handler; 
				if ( message->FindPointer("source", (void **)&handler) ) 
				{ 
           			BCheckBox *box = cast_as(handler, BCheckBox); 
           			if ( box ) 
					{ 
						if ( box->Value() == B_CONTROL_ON)
							m_CaptureSettings.compressMethod = kPostCompress;
						else
							m_CaptureSettings.compressMethod = kRealTimeCompress;
					} 					
				}				
			}
			break;

		default:
			BWindow::MessageReceived(message);						
			break;
	}
}	


#pragma mark -
#pragma mark === Capture Routines ===

//---------------------------------------------------------------------
//	StartRecording
//---------------------------------------------------------------------
//
//	Record audio data to disk
//

void TAudioCaptureWindow::StartRecording()
{		
	// Check for a valid file first
	if (m_CaptureSettings.audioFile == NULL)
		CreateTempFile();
	
	//	Set up record node
	m_RecordNode = new TSoundRecorder("Sound Input");
	SetupRecordNode(m_RecordNode, m_Input, m_Output, m_Timesource);

	if (!m_Timesource) 
	{
		(new BAlert("", "Error configuring sound hardware!", "Stop"))->Go();
		BMediaRoster::Roster()->StopNode(m_Output.node, 0, true);
		m_RecordNode->Release();
		return;
	}
				
	// Set record flag to true
	m_IsRecording = true;
	
	//	Set up hooks and start recording
	m_RecordNode->SetHooks(adc_reader, NULL, &m_CaptureSettings);
	StartRecordNode(m_RecordNode, m_Input, m_Output, m_Timesource);

	// Update status bar
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Capturing Audio...");
	m_StatusView->MessageReceived(textMessage);
	delete textMessage;

	// Spawn updater thread
	m_UpdaterThread = spawn_thread(_updater_thread, "UpdaterThread", B_REAL_TIME_PRIORITY, this);
	
	if ( ValidThread(m_UpdaterThread) )
	{
		resume_thread(m_UpdaterThread);	
		
		// Update status bar
		BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
		textMessage->AddString("StatusText", "Capturing Audio...");
		m_StatusView->MessageReceived(textMessage);
		delete textMessage;	
	}
}


//---------------------------------------------------------------------
//	StopRecording
//---------------------------------------------------------------------
//
//	Stop recording audio data to disk
//

void TAudioCaptureWindow::StopRecording()
{		
	m_IsRecording = false;
	
	//	Stop RecordNode
	StopRecordNode();
	
	// If we have captured to RAM, save file out.
	if (m_CaptureSettings.captureMethod == kCaptureToRAM)
	{
		m_CaptureSettings.audioFile->Write( m_CaptureSettings.audioBuf->Buffer(), m_CaptureSettings.audioBuf->BufferLength() );	
	}
	
	// Reset indicators
	Lock();

	m_FileSize->SetText("0k");
	m_SpaceRemaining->SetText("0k");
	
	// Update status bar
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	m_StatusView->MessageReceived(textMessage);
	delete textMessage;

	Unlock();

	// Set the mimetype
	BNodeInfo info(m_CaptureSettings.audioFile);
	info.SetType("audio/raw");
	
	// Set icons
	BBitmap *smallIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_AudioIcon16;
	info.SetIcon( smallIcon, B_MINI_ICON);	
	
	BBitmap *largeIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_AudioIcon32;	
	info.SetIcon( largeIcon, B_LARGE_ICON);
		
	// Clean up
	m_CaptureSettings.audioFile->Unset();
	delete m_CaptureSettings.audioFile;
	m_CaptureSettings.audioFile = NULL;
		
	// Open file in new AudioEditor
	//float maxWidthHeight = kAudioEditorHeight+kAudioToolbarHeight+kTimelineHeight;
	//BRect bounds(50, 50, 300, 50+maxWidthHeight);
	//TAudioEditor *theEditor = new TAudioEditor(bounds, &m_FileRef, NULL);
	
	//if (theEditor)
	//	theEditor->Show();
	
	// Clear our copy buffer
	m_CaptureSettings.audioBuf->SetSize(0);
	
	//	Play...
	/*media_raw_audio_format my_format;
	my_format.frame_rate = 44100.0;
	my_format.format = media_raw_audio_format::B_AUDIO_FLOAT;
	my_format.channel_count = 2;
	#if B_HOST_IS_LENDIAN
		my_format.byte_order = B_MEDIA_LITTLE_ENDIAN;
	#else
		my_format.byte_order = B_MEDIA_BIG_ENDIAN;
	#endif
	my_format.buffer_size = 4096;
	BSoundPlayer sound_player(&my_format, "SoundTest", dac_writer, NULL, &aud);
	sound_player.Start();
	sound_player.SetHasData(true);*/
	
	BSoundPlayer player;
	BSoundPlayer::play_id id;
	
	BSound *sound = new BSound(&m_FileRef);
	if (sound->InitCheck() == B_OK) 
	{
		printf("Have a BSound!\n");
		
		printf("BSoundPlayer::Start\n");
		player.Start();
		printf("BSoundPlayer::SetVolume\n");
		player.SetVolume(1.0);
		printf("BSoundPlayer::StartPlaying\n");
		id = player.StartPlaying(sound);
		printf("BSoundPlayer::ReleaseRef\n");
		sound->ReleaseRef();
	}
	
	printf("BSoundPlayer:: -EXIT-\n");
}

#pragma mark -
#pragma mark === Node Routines ===

//---------------------------------------------------------------------
//	SetupRecordNode
//---------------------------------------------------------------------
//
//	Set up record node and connect to media_server
//

void TAudioCaptureWindow::SetupRecordNode( BMediaNode *recorder, media_input &input,
										  media_output &output, BTimeSource *&timesource)
{
	status_t err = B_OK;
	
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->RegisterNode(recorder);
		
	media_node soundin;
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->GetAudioInput(&soundin);
		
	int32 count;
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->GetFreeOutputsFor(soundin, &output, 1, &count, B_MEDIA_RAW_AUDIO);
	
	if (err >= B_OK && count < 1) 
		err = B_MEDIA_NODE_BUSY;
		
	BTimeSource * ts = NULL;
	if (err >= B_OK) 
		ts = BMediaRoster::Roster()->MakeTimeSourceFor(soundin);
		
	if (!ts) 
		err = B_ERROR;
		
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->GetFreeInputsFor(recorder->Node(), &input, 1, &count, B_MEDIA_RAW_AUDIO);
	
	if (err >= B_OK && count < 1) 
		err = B_MEDIA_NODE_BUSY;
		
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->SetTimeSourceFor(recorder->Node().node, ts->ID());
		
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->SetTimeSourceFor(soundin.node, ts->ID());
		
	if (output.format.type < 1) 
		output.format.type = B_MEDIA_RAW_AUDIO;
		
	if (!output.format.u.raw_audio.buffer_size) 
		output.format.u.raw_audio.buffer_size = 2048;
		
	if (!output.format.u.raw_audio.frame_rate) 
		output.format.u.raw_audio.frame_rate = 44100.0;
		
	if (!output.format.u.raw_audio.format) 
		output.format.u.raw_audio.format = media_raw_audio_format::B_AUDIO_SHORT;
		
#if B_HOST_IS_LENDIAN
	if (!output.format.u.raw_audio.byte_order) 
		output.format.u.raw_audio.byte_order = B_MEDIA_LITTLE_ENDIAN;
#else
	if (!output.format.u.raw_audio.byte_order) 
		output.format.u.raw_audio.byte_order = B_MEDIA_BIG_ENDIAN;
#endif

	if (!output.format.u.raw_audio.channel_count) 
		output.format.u.raw_audio.channel_count = 2;
		
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->Connect(output.source, input.destination, &output.format, &output, &input);
	
	timesource = ts;
	if (err < B_OK) 
	{
		fprintf(stderr, "Error in setup: %x\n", err);
	}
}


//---------------------------------------------------------------------
//	StartRecordNode
//---------------------------------------------------------------------
//
//	Start the recording nodes
//

void TAudioCaptureWindow::StartRecordNode( BMediaNode *node, media_input &input,
										  media_output & output, BTimeSource *timesource)
{
	status_t err;
	
	bigtime_t then = BTimeSource::RealTime()+50000;	
	bigtime_t perf = timesource->PerformanceTimeFor(then);
	
	if (output.node.node == timesource->ID()) 
	{
		err = BMediaRoster::Roster()->StartNode(output.node, then);
	}
	else 
	{
		err = BMediaRoster::Roster()->SeekNode(output.node, 0, perf);
		if (err >= B_OK) 
			err = BMediaRoster::Roster()->StartNode(output.node, perf);
		
		if (!timesource->IsRunning()) 
		{
			BMediaRoster::Roster()->StartNode(timesource->Node(), BTimeSource::RealTime());
		}
	}
	
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->SetRunModeNode(input.node, BMediaNode::B_RECORDING);
		
	if (err >= B_OK) 
		err = BMediaRoster::Roster()->StartNode(input.node, perf);
		
	if (err < B_OK) 
		fprintf(stderr, "Error starting: %x\n", err);
}

//---------------------------------------------------------------------
//	StopRecordNode
//---------------------------------------------------------------------
//
//	Stop the recording nodes
//

void TAudioCaptureWindow::StopRecordNode()
{
	status_t err;
	
	//	Stop nodes
	err = BMediaRoster::Roster()->StopNode(m_Timesource->Node(), 0, true);
	err = BMediaRoster::Roster()->StopNode(m_Output.node, 0, true);
	err = BMediaRoster::Roster()->StopNode(m_Input.node, 0, true);
	
	if (err >= B_OK || err == B_TIMED_OUT)
	{
		printf("StopRecordNode::Disconnect-\n");
		err = BMediaRoster::Roster()->Disconnect( m_Output.node.node, m_Output.source, 
										  		  m_Input.node.node, m_Input.destination);
	}
												  
	if (err >= B_OK || err == B_TIMED_OUT) 
		err = BMediaRoster::Roster()->StopNode(m_Input.node, 0, true);
			
	//	All done
	delete m_RecordNode;
	m_RecordNode = NULL;
	
	if (err < B_OK) 
		fprintf(stderr, "Error stopping: %x\n", err);		
}

#pragma mark -
#pragma mark === Thread Routines ===

//---------------------------------------------------------------------
//	adc_reader
//---------------------------------------------------------------------
//
//	Read data from ADCStream
//

void adc_reader(void *arg, bigtime_t time, const void *buffer, size_t count, const media_raw_audio_format& ) 
{		
	AudCapPtr audCap = static_cast<AudCapPtr>(arg);
	
	// Write file to disk
	if (audCap->captureMethod == kCaptureToDisk)
	{	
		// Get copy of buffer and write it out
		//memcpy(audio_buffer, buffer, count);		
		if ( audCap->audioFile->Write(buffer, count) < 0)
		//audCap->audioBuf->Write(buffer, count);
		//if (audCap->audioFile->Write(audCap->audioBuf->Buffer(), audCap->audioBuf->BufferLength()) < 0)
		{			
			audCap->audioWindow->AbortRecording();
			ErrorAlert("Destination Disk Full!");
			// Barrett: This triggered an error --
			//return false;
		}	
		
		// Clear our copy buffer
		//audCap->audioBuf->SetSize(0);		
	}
	// Write file to RAM
	else
	{
		// Get copy of buffer and write it out
		audCap->audioBuf->Write(buffer, count);		
	}
} 


//---------------------------------------------------------------------
//	_updater_thread
//---------------------------------------------------------------------
//
//	Updater thread function
//

int32 _updater_thread(void *arg)
{	
	// Get window instance
	TAudioCaptureWindow	*audCapWindow = (TAudioCaptureWindow *)arg;
	
	// Get capture settings
	AudCapPtr audCap = audCapWindow->GetCaptureSettings();
						
	// Set up file display parameters
	off_t 	fileSize;
	off_t 	freeSpace;
	BEntry 	theEntry(audCapWindow->GetFileRef());		
	BVolume theVol;
	theVol.SetTo(audCapWindow->GetFileRef()->device);
	
	//	File size tracking variable
	int32 fileKBytes = 0;
	
	// Update display while recording
	while ( audCapWindow->IsRecording() )
	{		
		char sizeStr[65];
		
		// Update file size		
		theEntry.GetSize(&fileSize);
		fileKBytes = fileSize / 1024;
				
		// Check to display K or MB		
		//if (fileKBytes > 1023)
		//{
		//	kBytes /= 1024;
		//	sprintf(sizeStr, "%dMB", fileKBytes);
		//}
		//else
		sprintf(sizeStr, "%dK", fileKBytes);
			
		if (audCapWindow->Lock())
		{
			audCapWindow->GetFileSize()->SetText(sizeStr);
			audCapWindow->Unlock();
		}
		
		// Update space remaining
		freeSpace = theVol.FreeBytes();
		int32 freeKBytes = freeSpace /1024;
		sprintf(sizeStr, "%dK", freeKBytes);
		
		if (audCapWindow->Lock())
		{
			audCapWindow->GetSpaceRemaining()->SetText(sizeStr);
			audCapWindow->Unlock();
		}

		snooze(20*1000);				
	}
			
	return false;
}

#pragma mark -
#pragma mark === Data Routines ===

//---------------------------------------------------------------------
//	SelectAudioFile
//---------------------------------------------------------------------
//
//	Create audio capture file
//

void TAudioCaptureWindow::SelectAudioFile(BMessage *theMessage)
{
	
	// Check and see if we have an open file already.  Is so , clean it up
	if (m_CaptureSettings.audioFile)
	{
		// Clean up
		m_CaptureSettings.audioFile->Unset();
		delete m_CaptureSettings.audioFile;
	}
	
	// Extract info from message
	entry_ref dirRef;
	if ( theMessage->FindRef("directory", &dirRef) == B_OK)
	{
		char *theName = NULL;
		char fileName[B_FILE_NAME_LENGTH];
						
		// Get name of file to be saved as
		theMessage->FindString((const char *)"name", (const char **)&theName);
		strcpy(fileName, theName);
		
		// Create new file
		BDirectory saveDir(&dirRef);
		if (saveDir.InitCheck() == B_OK)
		{
			m_CaptureSettings.audioFile = new BFile();
			if ( saveDir.CreateFile(fileName, m_CaptureSettings.audioFile, false) == B_OK)
			{			
				if (m_CaptureSettings.audioFile->InitCheck() != B_OK)
				{
					DebugAlert("Error creating file.");
					return;
				}	
				
				// Stash files entry ref
				BEntry theEntry(&saveDir, fileName);
				theEntry.GetRef(&m_FileRef);
							
				// Update indicators
				m_FileSize->SetText("0k");
				m_SpaceRemaining->SetText("0k");	
				
				// Update status bar
				BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
				textMessage->AddString("StatusText", "Ready to Capture...");
				m_StatusView->MessageReceived(textMessage);
				delete textMessage;
			
			}
			else
			{
				DebugAlert("Error creating file.");
				return;
			}
		}
		else
		{
			DebugAlert("Error creating file.");	
			return;
		}
	}
	else
	{
		DebugAlert("Error creating file.");
		return;
	}	
}

//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TAudioCaptureWindow::ShowPanel()
{
	// Show panel if already created
	if (m_Panel)
		m_Panel->Show();
	// Create new panel
	else
	{
		// 	Create messenger to send panel messages to our channel.  We cannot send it to 
		//  ourself as we are not part of the view heirarchy.
	 	//BMessenger *messenger = new BMessenger( m_Channel,  ((MuseumApp *)be_app)->GetCueSheet());
	 	BMessenger *messenger = new BMessenger(this);
		
		// Create message containing pointer to ourself
		BMessage *message = new BMessage(AUDCAP_PANEL_MSG);
		 	
	 	// Construct a file panel and set it to modal
	 	m_Panel = new BFilePanel( B_SAVE_PANEL, messenger, NULL, B_FILE_NODE, false, message, NULL, true, true );
	 
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
}


//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TAudioCaptureWindow::HidePanel()
{	
	if(m_Panel)
	{
		delete m_Panel;
		m_Panel = NULL;
	}
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	CreateTempFile
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TAudioCaptureWindow::CreateTempFile()
{	
	BPath pathObj;
	if ( find_directory(B_SYSTEM_TEMP_DIRECTORY, &pathObj) != B_OK)
		DebugAlert("No Temp Directory Found.");
	
	entry_ref ref; 
	get_ref_for_path( pathObj.Path(), &ref);
   
	BDirectory fileDir(&ref);
	m_CaptureSettings.audioFile = new BFile();
	
	//if ( fileDir.CreateFile("tempCapFile.raw", m_CaptureSettings.audioFile, false) == B_OK)
	// Create temp file named the current system time
	char fileName[256];
	uint32 time = real_time_clock ();
	sprintf(fileName, "%ldaudio.raw", time);
	
	if ( fileDir.CreateFile(fileName, m_CaptureSettings.audioFile, false) == B_OK)
	{			
		if (m_CaptureSettings.audioFile->InitCheck() != B_OK)
			DebugAlert("Error creating file.");
		
		// Stash files entry ref
		BEntry theEntry(&fileDir, fileName);
		theEntry.GetRef(&m_FileRef);
	}	         
}
