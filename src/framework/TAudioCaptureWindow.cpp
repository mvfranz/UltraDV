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
	fIsRecording = false;
	wait_for_thread(fUpdaterThread, &status);
		
	// Dispose of BFilePanel
	HidePanel();
	
	// Close any open files
	if (fCaptureSettings.audioFile)
	{
		fCaptureSettings.audioFile->Unset();
		delete fCaptureSettings.audioFile;
	}
	
	if (fCaptureSettings.audioBuf)
	{
		delete fCaptureSettings.audioBuf;
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
	fIsRecording 	= false;
	fRecordNode 	= NULL;
	
	// Create tool bar
	BView *toolbarView = (BView *)FindView("AudioCaptureToolbarView");
	fToolbar = new TAudioCaptureToolbar(toolbarView->Bounds());
	RemoveChild(toolbarView);
	AddChild(fToolbar);
	fToolbar->Show();
	delete toolbarView;
			
	// Find text items
	fFileSize 			= (BStringView *)FindView("FileSizeString");
	fSpaceRemaining 	= (BStringView *)FindView("SpaceRemainingString");
		
	// Find Levels Box
	BBox *levelsBox = (BBox *)FindView("LevelsBox");
	
	//  Create AudCap View
	fAudCapView = (BView *)FindView("AudioCaptureView");
	
	// Add levels view
	BView *levelsView = (BView *)FindView("LevelsView");
	fLevelsView = new TAudioLevelsView(levelsView->Frame());
	levelsBox->RemoveChild(levelsView);
	levelsBox->AddChild(fLevelsView);	
	fLevelsView->Show();
	delete levelsView;

	// Find status view
	BView *statusView = (BView *)FindView("StatusView");
	fStatusView = new TStatusView(statusView->Frame());
	RemoveChild(statusView);
	delete statusView;
	AddChild(fStatusView);
	fStatusView->Show();
			
	// Set up fCaptureSettings
	fCaptureSettings.captureMethod 	= kCaptureToDisk;
	fCaptureSettings.compressMethod 	= kPostCompress;
	fCaptureSettings.audioBuf			= new BMallocIO();
	fCaptureSettings.audioFile			= NULL;
	fCaptureSettings.audioWindow		= this;
	
	// Set panel to NULL
	fPanel = NULL;
	
	// Set indicators
	fFileSize->SetText("0k");
	fSpaceRemaining->SetText("0k");
	
	// Set status text
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	fStatusView->MessageReceived(textMessage);
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
				if (fIsRecording)
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
			fCaptureSettings.captureMethod = kCaptureToDisk;
			break;
			
		// Capture to RAM
		case AUDCAP_RAM_MSG:
			fCaptureSettings.captureMethod = kCaptureToRAM;
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
							fCaptureSettings.compressMethod = kPostCompress;
						else
							fCaptureSettings.compressMethod = kRealTimeCompress;
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
	if (fCaptureSettings.audioFile == NULL)
		CreateTempFile();
	
	//	Set up record node
	fRecordNode = new TSoundRecorder("Sound Input");
	SetupRecordNode(fRecordNode, fInput, fOutput, fTimesource);

	if (!fTimesource) 
	{
		(new BAlert("", "Error configuring sound hardware!", "Stop"))->Go();
		BMediaRoster::Roster()->StopNode(fOutput.node, 0, true);
		fRecordNode->Release();
		return;
	}
				
	// Set record flag to true
	fIsRecording = true;
	
	//	Set up hooks and start recording
	fRecordNode->SetHooks(adc_reader, NULL, &fCaptureSettings);
	StartRecordNode(fRecordNode, fInput, fOutput, fTimesource);

	// Update status bar
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Capturing Audio...");
	fStatusView->MessageReceived(textMessage);
	delete textMessage;

	// Spawn updater thread
	fUpdaterThread = spawn_thread(_updater_thread, "UpdaterThread", B_REAL_TIME_PRIORITY, this);
	
	if ( ValidThread(fUpdaterThread) )
	{
		resume_thread(fUpdaterThread);	
		
		// Update status bar
		BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
		textMessage->AddString("StatusText", "Capturing Audio...");
		fStatusView->MessageReceived(textMessage);
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
	fIsRecording = false;
	
	//	Stop RecordNode
	StopRecordNode();
	
	// If we have captured to RAM, save file out.
	if (fCaptureSettings.captureMethod == kCaptureToRAM)
	{
		fCaptureSettings.audioFile->Write( fCaptureSettings.audioBuf->Buffer(), fCaptureSettings.audioBuf->BufferLength() );	
	}
	
	// Reset indicators
	Lock();

	fFileSize->SetText("0k");
	fSpaceRemaining->SetText("0k");
	
	// Update status bar
	BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
	textMessage->AddString("StatusText", "Ready to Capture...");
	fStatusView->MessageReceived(textMessage);
	delete textMessage;

	Unlock();

	// Set the mimetype
	BNodeInfo info(fCaptureSettings.audioFile);
	info.SetType("audio/raw");
	
	// Set icons
	BBitmap *smallIcon	= static_cast<MuseumApp *>(be_app)->fMuseumIcons->fAudioIcon16;
	info.SetIcon( smallIcon, B_MINI_ICON);	
	
	BBitmap *largeIcon	= static_cast<MuseumApp *>(be_app)->fMuseumIcons->fAudioIcon32;	
	info.SetIcon( largeIcon, B_LARGE_ICON);
		
	// Clean up
	fCaptureSettings.audioFile->Unset();
	delete fCaptureSettings.audioFile;
	fCaptureSettings.audioFile = NULL;
		
	// Open file in new AudioEditor
	//float maxWidthHeight = kAudioEditorHeight+kAudioToolbarHeight+kTimelineHeight;
	//BRect bounds(50, 50, 300, 50+maxWidthHeight);
	//TAudioEditor *theEditor = new TAudioEditor(bounds, &fFileRef, NULL);
	
	//if (theEditor)
	//	theEditor->Show();
	
	// Clear our copy buffer
	fCaptureSettings.audioBuf->SetSize(0);
	
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
	
	BSound *sound = new BSound(&fFileRef);
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
	err = BMediaRoster::Roster()->StopNode(fTimesource->Node(), 0, true);
	err = BMediaRoster::Roster()->StopNode(fOutput.node, 0, true);
	err = BMediaRoster::Roster()->StopNode(fInput.node, 0, true);
	
	if (err >= B_OK || err == B_TIMED_OUT)
	{
		printf("StopRecordNode::Disconnect-\n");
		err = BMediaRoster::Roster()->Disconnect( fOutput.node.node, fOutput.source, 
										  		  fInput.node.node, fInput.destination);
	}
												  
	if (err >= B_OK || err == B_TIMED_OUT) 
		err = BMediaRoster::Roster()->StopNode(fInput.node, 0, true);
			
	//	All done
	delete fRecordNode;
	fRecordNode = NULL;
	
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
	if (fCaptureSettings.audioFile)
	{
		// Clean up
		fCaptureSettings.audioFile->Unset();
		delete fCaptureSettings.audioFile;
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
			fCaptureSettings.audioFile = new BFile();
			if ( saveDir.CreateFile(fileName, fCaptureSettings.audioFile, false) == B_OK)
			{			
				if (fCaptureSettings.audioFile->InitCheck() != B_OK)
				{
					DebugAlert("Error creating file.");
					return;
				}	
				
				// Stash files entry ref
				BEntry theEntry(&saveDir, fileName);
				theEntry.GetRef(&fFileRef);
							
				// Update indicators
				fFileSize->SetText("0k");
				fSpaceRemaining->SetText("0k");	
				
				// Update status bar
				BMessage *textMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
				textMessage->AddString("StatusText", "Ready to Capture...");
				fStatusView->MessageReceived(textMessage);
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
	if (fPanel)
		fPanel->Show();
	// Create new panel
	else
	{
		// 	Create messenger to send panel messages to our channel.  We cannot send it to 
		//  ourself as we are not part of the view heirarchy.
	 	//BMessenger *messenger = new BMessenger( fChannel,  ((MuseumApp *)be_app)->GetCueSheet());
	 	BMessenger *messenger = new BMessenger(this);
		
		// Create message containing pointer to ourself
		BMessage *message = new BMessage(AUDCAP_PANEL_MSG);
		 	
	 	// Construct a file panel and set it to modal
	 	fPanel = new BFilePanel( B_SAVE_PANEL, messenger, NULL, B_FILE_NODE, false, message, NULL, true, true );
	 
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
}


//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TAudioCaptureWindow::HidePanel()
{	
	if(fPanel)
	{
		delete fPanel;
		fPanel = NULL;
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
	fCaptureSettings.audioFile = new BFile();
	
	//if ( fileDir.CreateFile("tempCapFile.raw", fCaptureSettings.audioFile, false) == B_OK)
	// Create temp file named the current system time
	char fileName[256];
	uint32 time = real_time_clock ();
	sprintf(fileName, "%ldaudio.raw", time);
	
	if ( fileDir.CreateFile(fileName, fCaptureSettings.audioFile, false) == B_OK)
	{			
		if (fCaptureSettings.audioFile->InitCheck() != B_OK)
			DebugAlert("Error creating file.");
		
		// Stash files entry ref
		BEntry theEntry(&fileDir, fileName);
		theEntry.GetRef(&fFileRef);
	}	         
}
