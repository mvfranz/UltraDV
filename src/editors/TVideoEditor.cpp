//---------------------------------------------------------------------
//
//	File:	TVideoEditor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.31.98
//
//	Desc:	Video editing window
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>
#include <TimeCode.h>

#include "MuseumApp.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppAlerts.h"
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapButton.h"
#include "TVideoEditorShuttle.h"
#include "TVideoEditorTransport.h"
#include "TVideoEditorMenus.h"
#include "TCueView.h"
#include "TMovieCue.h"
#include "TMuseumIcons.h"
#include "TBrowserWindow.h"

#include "TVideoCaptureWindow.h"
#include "TVideoEditorView.h"
#include "TVideoEditorControlsView.h"
#include "TVideoEditorText.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TVideoEditor.h"

//	Globals
int16 video_buffer[8192];

// Local prototypes
bool video_dac_writer(void* arg, char* buf, size_t count, void *header );

// Constants
//const int32 kSamplesPerFrame = 1471;
//const int32 kVideoHeaderSize = sizeof(VideoChunk) + sizeof(ChunkHeader);

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditor::TVideoEditor(entry_ref *theRef, TMovieCue *theCue, BMessage *message) : BWindow(message)
{				
	// Save file ref
	m_FileRef = *theRef;
	
	// Set up member varibles
	m_MovieCue 		= theCue;
	m_FileSavePanel = NULL;
		
	// 	Set up indicator flag specifying whether or not this file is a new capture
	//	or a previuosly saved file.  If a file originates from the /boot/var/tmp
	//	directory, we can assume it is a new capture	
	
	// Get path to tmp
	BPath path;
	find_directory(B_COMMON_TEMP_DIRECTORY, &path);
	
	// Get node info
	node_ref nref; 
   
	BEntry entry(path.Path()); 
	entry.GetNodeRef(&nref);
	         
	if ( m_FileRef.directory == nref.node)
		m_NewFile = true;
	else
		m_NewFile = false;
		
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoEditor::~TVideoEditor()
{
	
	// Exit from all threads
	status_t status;
	m_IsPlaying = false;
	m_IsLooping = false;
	
	// Clean up
	if (m_DisplayBitmap)
		delete m_DisplayBitmap;
		
	// Inform cue that we have been closed
	if (m_MovieCue)
		m_MovieCue->SetEditorOpen(false);
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoEditor::Init()
{		
	
	// Init member variables
	m_IsPlaying = false;
	m_IsLooping = false;
	
	//
	// Locate Views
	//
	
	BMessage	*tmpMessage;
	bool		retVal = false;
	
	// Create menubar
	BRect mbarRect 	= Bounds();
	mbarRect.bottom = mbarRect.top+kMenuHeight;
	BMenuBar *mbar 	= new BMenuBar(mbarRect, "mbar");
	m_EditorMenu 	= new TVideoEditorMenus(mbar, this);	
	AddChild(mbar);
	
	// Backgrounds
	m_BGView = (BView *)FindView("VideoEditorBGView");
	m_ControlsBGView = (BView *)FindView("VideoEditorControlsBGView");

	// VideoEditorView	
	BView *editView = (BView *)FindView("VideoEditorView");
	tmpMessage = new BMessage();
	editView->Archive(tmpMessage, false);
	m_EditorView = new TVideoEditorView(this, tmpMessage);
	delete tmpMessage;
	m_BGView->RemoveChild(editView);
	delete editView;
	m_BGView->AddChild(m_EditorView);
	//m_EditorView->Show();
	
	// ControlView
	BView *tmpControlsView = FindView("VideoEditorControlsView");
	tmpMessage = new BMessage();
	tmpControlsView->Archive(tmpMessage, false);	
	m_ControlsView = new TVideoEditorControlsView(tmpMessage);
	delete tmpMessage;
	m_ControlsBGView->AddChild(m_ControlsView);
	m_ControlsView->Show();
	
	// TextViews
	BView *inTextView = FindView("InTextView");
	m_InText = new TVideoEditorText(NULL, (int32)NULL, inTextView->Frame(), "InTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT); 
	retVal = tmpControlsView->RemoveChild(inTextView);
	delete inTextView;
	m_ControlsView->AddChild(m_InText);
	m_InText->Show();
	
	BView *outTextView 	= FindView("OutTextView");
	m_OutText 	= new TVideoEditorText(NULL, (int32)NULL, outTextView->Frame(), "OutTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT); 
	tmpControlsView->RemoveChild(outTextView);
	delete outTextView;
	m_ControlsView->AddChild(m_OutText);
	m_OutText->Show();
	
	BView *durTxtView = FindView("DurationTextView");
	m_DurationText 	= new TVideoEditorText(NULL, (int32)NULL, durTxtView->Frame(), "DurationTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT); 
	tmpControlsView->RemoveChild(durTxtView);
	delete durTxtView;
	m_ControlsView->AddChild(m_DurationText);
	m_DurationText->Show();
	
	BView *timeTextView = FindView("TimeTextView");
	m_TimeText 	= new TVideoEditorText( NULL, (int32)NULL, timeTextView->Frame(), "TimeTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT); 
	tmpControlsView->RemoveChild(timeTextView);
	delete timeTextView;
	m_ControlsView->AddChild(m_TimeText);
	m_TimeText->Show();
	
	// Buttons
	SetupControlButtons();
	
	// Create shuttle
	BView *shuttleView = FindView("ShuttleView");
	tmpMessage = new BMessage();
	shuttleView->Archive(tmpMessage, false);	
	m_Shuttle = new TVideoEditorShuttle(this, tmpMessage);
	delete tmpMessage;
	m_ControlsView->AddChild(m_Shuttle);
	m_Shuttle->Show();
	
	// Create transport
	BView *transportView = FindView("TransportView");
	tmpMessage = new BMessage();
	transportView->Archive(tmpMessage, false);	
	m_Transport = new TVideoEditorTransport(this, tmpMessage);
	delete tmpMessage;
	m_ControlsView->AddChild(m_Transport);
	m_Transport->Show();

	// Free temp views	
	m_ControlsBGView->RemoveChild(tmpControlsView);
	delete tmpControlsView;

	//	Init tracking variables
	m_CurrentVideoFrame = 0;

	//	Get pointer to cue's RIFFReader
	m_Reader = m_MovieCue->RIFFReader();
	
	//	Get pointer to codecs
	m_AudioCodec = m_MovieCue->AudioCodec();
	m_VideoCodec = m_MovieCue->VideoCodec();

	//	Get AVIHeader
	AVIHeader *header = m_Reader->GetAVIHeader();

	//	Create offscreen
	BRect movieRect( 0, 0, header->Width-1, header->Height-1); 
	m_DisplayBitmap = new BBitmap(movieRect, B_RGB32);
		
	//	Load first frame into display	
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
	if (!theFrame)
	{
		printf("AVIProducer::INVALID FRAME!\n");
		return;
	}
		
	//	Decode frame
	retVal = m_VideoCodec->DecodeFrame(theFrame, m_DisplayBitmap->Bits(), B_RGB32);
	if (!retVal)
	{
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return;
	}
	
	// Load in file info
	InitMovieAttributes();
	
	// Setup time indicators
	SetupTextItems();
	
	//	Show the video display
	m_EditorView->Show();
	m_EditorView->Invalidate();
	
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVideoEditor::MessageReceived(BMessage* message)
{	
	
	switch (message->what)
	{
			
		// These messages come from the BFilePanel when saving an audio file
		case B_SAVE_REQUESTED:
			Save(message);
			break;

		case B_CANCEL:
			break;
			
		// Control Messages
		case VE_IN_MSG:
			SetInPoint();
			break;
			
		case VE_OUT_MSG:
			SetOutPoint();
			break;
			
		case VE_VOLUME_MSG:
			break;
			
		case VE_PLAY_MSG:
			Play();
			break;
			
		case VE_LOOP_MSG:
			SetLooping();
			break;
			
		case VE_STOP_MSG:
			Stop();
			break;
			
		case VE_FF_MSG:
			StepForward();
			break;
			
		case VE_FB_MSG:
			StepBack();
			break;
			
		case VE_START_MSG:
			SeekStart();
			break;
			
		case VE_END_MSG:
			SeekEnd();
			break;
			
		case VE_MARK_MSG:
			break;
			
		case VE_JUMP_MSG:
			break;
		
		case VE_DETACH_MSG:
			break;
		
		// 
		// Menu Commands Handled Below
		//
			
		// Handle file menu
		case FILE_NEW_MSG:			
		case FILE_OPEN_MSG:
		case FILE_CLOSE_MSG	:
		case FILE_SAVE_MSG:
		case FILE_SAVEAS_MSG:
		case FILE_REVERT_MSG:
			HandleFileMenu(message);
			break;
			
		// Handle edit menu	
		case EDIT_UNDO_MSG:			
		case EDIT_CUT_MSG:				
		case EDIT_COPY_MSG:				
		case EDIT_PASTE_MSG:				
		case EDIT_CLEAR_MSG	:			
		case EDIT_SELECTALL_MSG:													
			HandleEditMenu(message);
			break;					
			
		default:
			BWindow::MessageReceived(message);						
			break;
	}
}	


#pragma mark -
#pragma mark === Playback Routines ===

//---------------------------------------------------------------------
//	Play
//---------------------------------------------------------------------
//
//	Play video data from disk
//

void TVideoEditor::Play()
{		
	if (m_IsPlaying)
		return;
		
	// Set playback flag to true
	m_IsPlaying = true;
		
}	


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop playback
//

void TVideoEditor::Stop()
{		
	status_t status;
	
	if (m_IsPlaying)
	{
		// Set playback flag to true
		m_IsPlaying = false;				
	}
}	


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Pause playback
//

void TVideoEditor::Pause()
{		
}	

//---------------------------------------------------------------------
//	StepBack
//---------------------------------------------------------------------
//
//	Step back one frame
//

void TVideoEditor::StepBack()
{		
	Stop();
		
	// Protect against underflow
	if (m_CurrentVideoFrame == 0)
		return;
				
	// Decrement frame and redraw
	m_CurrentVideoFrame--;
		
	// Load frame
	if ( LoadAndDecode(m_CurrentVideoFrame, m_DisplayBitmap) == true)
	{	
		// Draw frame
		Lock();
		m_EditorView->DisplayFrame(m_DisplayBitmap);
		m_EditorView->Sync();
		Unlock();
	
		// Update time	
		//m_Time->SetText(  );
	}
}	


//---------------------------------------------------------------------
//	StepForward
//---------------------------------------------------------------------
//
//	Step forward one frame
//

void TVideoEditor::StepForward()
{		
	Stop();
		
	// Increment frame and redraw
	m_CurrentVideoFrame++;
	
	// Protect against overflow
	if ( m_CurrentVideoFrame >= m_Reader->VideoFrameCount())
	{
		m_CurrentVideoFrame--;
		return;
	}

	// Load frame
	if ( LoadAndDecode(m_CurrentVideoFrame, m_DisplayBitmap) == true)
	{	
		// Draw frame
		Lock();
		m_EditorView->DisplayFrame(m_DisplayBitmap);
		m_EditorView->Sync();
		Unlock();
	
		// Update time	
		//m_Time->SetText(  );
	}

}	

//---------------------------------------------------------------------
//	SeekStart
//---------------------------------------------------------------------
//
//	Seek to start of movie
//

void TVideoEditor::SeekStart()
{		
	Stop();
	
	// Set currentFrame to start
	m_CurrentVideoFrame = 0;
	m_CurrentTime 		= 0;
	
	// Load frame
	if ( LoadAndDecode(m_CurrentVideoFrame, m_DisplayBitmap) == true)
	{	
		// Draw frame
		Lock();
		m_EditorView->DisplayFrame(m_DisplayBitmap);
		m_EditorView->Sync();
		Unlock();
	
		// Update time text
		//char tmpText[13];
		//TimeToString(m_CurrentTime, m_VideoChunk.frame_rate, tmpText, false);
		//m_TimeText->SetText(tmpText);
	}	
}	


//---------------------------------------------------------------------
//	SeekEnd
//---------------------------------------------------------------------
//
//	Seek to end of movie
//

void TVideoEditor::SeekEnd()
{		
	Stop();
	
	//	Go to last frame
	m_CurrentVideoFrame = m_Reader->VideoFrameCount() - 1;
	
	// Load frame
	if ( LoadAndDecode(m_CurrentVideoFrame, m_DisplayBitmap) == true)
	{	
		// Draw frame
		Lock();
		m_EditorView->DisplayFrame(m_DisplayBitmap);
		m_EditorView->Sync();
		Unlock();
	
		// Update time text
		//char tmpText[13];
		//TimeToString(m_CurrentTime, m_VideoChunk.frame_rate, tmpText, false);
		//m_TimeText->SetText(tmpText);
	}	
}	

//---------------------------------------------------------------------
//	SetLooping
//---------------------------------------------------------------------
//
//	Toggle looping mode
//

void TVideoEditor::SetLooping()
{		
	if (m_IsLooping)
		m_IsLooping = false;
	else
		m_IsLooping = true;
}


//---------------------------------------------------------------------
//	SetInPoint
//---------------------------------------------------------------------
//
//	Set movie in point
//

void TVideoEditor::SetInPoint()
{		
}

//---------------------------------------------------------------------
//	SetInPoint
//---------------------------------------------------------------------
//
//	Set movie in point
//

void TVideoEditor::SetOutPoint()
{		
}


#pragma mark -
#pragma mark === Utility Routines ===

//-------------------------------------------------------------------
//	SetupTextItems	
//-------------------------------------------------------------------
//
//	Initilaize text displays
//

void TVideoEditor::SetupTextItems()
{
	char tmpText[13];
	
	BFont smallFont(be_plain_font);
	smallFont.SetSize(10);
	
	//	Get AVIHeader
	AVIHeader *header = m_Reader->GetAVIHeader();
	const uint32 mSecsPerFrame = header->TimeBetweenFrames / 1000;
	const uint32 duration 	   = header->TotalNumberOfFrames * mSecsPerFrame;
	
	// In Time
	m_InText->SetFontAndColor(&smallFont, 0, kGreen);
	m_InText->SetText("00:00:00:00");
		
	// Out Time
	m_OutText->SetFontAndColor(&smallFont, 0, kGreen);
	TimeToString(duration, header->DataRate, tmpText, false);
	m_OutText->SetText(tmpText);
	
	// Duration
	m_DurationText->SetFontAndColor(&smallFont, 0, kGreen);
	TimeToString(duration, header->DataRate, tmpText, false);
	m_DurationText->SetText(tmpText);
	
	// TimeText
	BFont timeFont(be_bold_font);
	timeFont.SetSize(20);
	m_TimeText->SetFontAndColor(&timeFont, 0, kGreen);
	TimeToString(m_CurrentTime, header->DataRate, tmpText, false);
	m_TimeText->SetText("00:00:00:00");
}

//-------------------------------------------------------------------
//	SetupTextItems	
//-------------------------------------------------------------------
//
//	Initialize control buttons displays
//

void TVideoEditor::SetupControlButtons()
{
	BBitmap *offBitmap, *onBitmap;
	
	BView *tmpControlsView = FindView("VideoEditorControlsView");
		
	// In button
	offBitmap = GetIcon16FromResource("VEInUp");
	onBitmap  = GetIcon16FromResource("VEInDown");
	BView *inButton = FindView("InButtonView");
	m_InButton 	= new TBitmapButton(inButton->Frame(), "InButton", offBitmap, onBitmap, this, new BMessage(VE_IN_MSG));
	tmpControlsView->RemoveChild(inButton);
	delete inButton;
	m_ControlsView->AddChild(m_InButton);
	
	// Out button
	offBitmap = GetIcon16FromResource("VEOutUp");
	onBitmap = GetIcon16FromResource("VEOutDown");
	BView *outButton = FindView("OutButtonView");
	m_OutButton 	= new TBitmapButton(outButton->Frame(), "OutButton", offBitmap, onBitmap, this, new BMessage(VE_OUT_MSG));
	tmpControlsView->RemoveChild(outButton);
	delete outButton;
	m_ControlsView->AddChild(m_OutButton);
	m_OutButton->Show();

	// Volume button
	offBitmap = GetIcon16FromResource("VEVolumeUp");
	onBitmap = GetIcon16FromResource("VEVolumeDown");
	BView *volButton = FindView("VolumeButtonView");
	m_VolumeButton = new TBitmapButton(volButton->Frame(), "VolumeButton", offBitmap, onBitmap, this, new BMessage(VE_VOLUME_MSG));
	tmpControlsView->RemoveChild(volButton);
	delete volButton;
	m_ControlsView->AddChild(m_VolumeButton);
	m_VolumeButton->Show();	
	
	// Start button
	offBitmap = GetIcon16FromResource("VEStartUp");
	onBitmap = GetIcon16FromResource("VEStartDown");
	BView *startButton = FindView("StartButtonView");
	m_StartButton = new TBitmapButton(startButton->Frame(), "StartButton", offBitmap, onBitmap, this, new BMessage(VE_START_MSG));
	tmpControlsView->RemoveChild(startButton);
	delete startButton;
	m_ControlsView->AddChild(m_StartButton);
	m_StartButton->Show();	
	
	// End button
	offBitmap = GetIcon16FromResource("VEEndUp");
	onBitmap = GetIcon16FromResource("VEEndDown");
	BView *endButton = FindView("EndButtonView");
	m_EndButton = new TBitmapButton(endButton->Frame(), "EndButton", offBitmap, onBitmap, this, new BMessage(VE_END_MSG));
	tmpControlsView->RemoveChild(endButton);
	delete endButton;
	m_ControlsView->AddChild(m_EndButton);
	m_EndButton->Show();
	
	// FrameBack button
	offBitmap = GetIcon16FromResource("VEStepBackUp");
	onBitmap  = GetIcon16FromResource("VEStepBackDown");
	BView *backButton = FindView("FrameBackButtonView");
	m_FrameBackButton = new TBitmapButton(backButton->Frame(), "FrameBack", offBitmap, onBitmap, this, new BMessage(VE_FB_MSG));
	tmpControlsView->RemoveChild(backButton);
	delete backButton;
	m_ControlsView->AddChild(m_FrameBackButton);
	m_FrameBackButton->Show();		

	// FrameForward button
	offBitmap = GetIcon16FromResource("VEStepForwardUp");
	onBitmap  = GetIcon16FromResource("VEStepForwardDown");
	BView *forwardButton = FindView("FrameForwardButtonView");
	m_FrameForwardButton = new TBitmapButton(forwardButton->Frame(), "FrameForward", offBitmap, onBitmap, this, new BMessage(VE_FF_MSG));
	tmpControlsView->RemoveChild(forwardButton);
	delete forwardButton;
	m_ControlsView->AddChild(m_FrameForwardButton);
	m_FrameForwardButton->Show();		
	
	// Play button
	offBitmap = GetIcon16FromResource("VEPlayUp");
	onBitmap = GetIcon16FromResource("VEPlayDown");
	BView *playButton = FindView("PlayButtonView");
	m_PlayButton = new TBitmapButton(playButton->Frame(), "PlayButton", offBitmap, onBitmap, this, new BMessage(VE_PLAY_MSG));
	tmpControlsView->RemoveChild(playButton);
	delete playButton;
	m_ControlsView->AddChild(m_PlayButton);
	m_PlayButton->Show();		

	// Loop button
	offBitmap = GetIcon16FromResource("VELoopUp");
	onBitmap = GetIcon16FromResource("VELoopDown");
	BView *loopButton = FindView("LoopButtonView");
	m_LoopButton = new TBitmapButton(loopButton->Frame(), "LoopButton", offBitmap, onBitmap, this, new BMessage(VE_LOOP_MSG));
	tmpControlsView->RemoveChild(loopButton);
	delete loopButton;
	m_ControlsView->AddChild(m_LoopButton);
	m_LoopButton->Show();		

	// Mark button
	offBitmap = GetIcon16FromResource("VEMarkUp");
	onBitmap = GetIcon16FromResource("VEMarkDown");
	BView *markButton = FindView("MarkButtonView");
	m_MarkButton = new TBitmapButton(markButton->Frame(), "MarkButton", offBitmap, onBitmap, this, new BMessage(VE_MARK_MSG));
	tmpControlsView->RemoveChild(markButton);
	delete markButton;
	m_ControlsView->AddChild(m_MarkButton);
	m_MarkButton->Show();		

	// Goto button
	offBitmap = GetIcon16FromResource("VEJumpUp");
	onBitmap = GetIcon16FromResource("VEJumpDown");
	BView *gotoButton = FindView("GotoButtonView");
	m_GotoButton = new TBitmapButton(gotoButton->Frame(), "GotoButton", offBitmap, onBitmap, this, new BMessage(VE_JUMP_MSG));
	tmpControlsView->RemoveChild(gotoButton);
	delete gotoButton;
	m_ControlsView->AddChild(m_GotoButton);
	m_GotoButton->Show();	
	
	// Detach button
	//offBitmap = GetIcon16FromResource("VEDetachUp");
	//onBitmap = GetIcon16FromResource("VEDetachDown");
	offBitmap = GetIcon16FromResource("VEJumpUp");
	onBitmap = GetIcon16FromResource("VEJumpDown");
	BView *detachButton = FindView("DetachButtonView");
	m_DetachButton = new TBitmapButton(detachButton->Frame(), "DetachButton", offBitmap, onBitmap, this, new BMessage(VE_DETACH_MSG));
	tmpControlsView->RemoveChild(detachButton);
	delete detachButton;
	m_ControlsView->AddChild(m_DetachButton);
	m_DetachButton->Show();	
}

//---------------------------------------------------------------------
//	InitMovieAttributes
//---------------------------------------------------------------------
//
//	Load movie header
//

void TVideoEditor::InitMovieAttributes()
{

}


#pragma mark -
#pragma mark === Completion Routines === 

//-------------------------------------------------------------------
//	QuitRequested	
//-------------------------------------------------------------------
//
//	Check and see if file needs to be saved before we quit
//

bool TVideoEditor::QuitRequested()
{
	bool retVal = true;
	
	if ( m_NewFile == true)
	{
		// Ask user to save file
		int32 userVal = SaveAlert();
		
		// Check user response
		switch( userVal)
		{
			// User does not want to save
			case 0:
				retVal = true;
				break;
			
			// User decided not to close
			case 1:
				retVal = false;
				break;
			
			// User wants to save
			case 2:
				{
					ShowFileSavePanel();
					m_IsClosing = true;
					retVal = false;
				}
				break;
				
			default:
				retVal = true;				
		}
	}
	
	// Delete temp file if necessary
	if (retVal && m_NewFile)
	{
		// Delete temp file
		BEntry entry(&m_FileRef);
		
		if ( entry.InitCheck() == B_OK)
			entry.Remove();		
	}

	return retVal;		
}


#pragma mark -
#pragma mark === Saving Routines === 


//---------------------------------------------------------------------
//	ShowFileSavePanel
//---------------------------------------------------------------------
//
//	Show file save panel
//

void TVideoEditor::ShowFileSavePanel()
{ 		
	// 	Create messenger to send panel messages to our channel.  We cannot send it to 
	//  ourself as we are not part of the view heirarchy.
 	BMessenger messenger( this,  NULL);
 		 			
	// 	If the panel has already been constructed, show the panel
	// 	Otherwise, create the panel.  We do need to reset it's messenger to 
	//	point at the correct window
	if (m_FileSavePanel)
	{
		m_FileSavePanel->SetTarget(messenger);
		m_FileSavePanel->Show();
	}
	else
	{
		// Construct a file panel and set it to modal
	 	m_FileSavePanel = new BFilePanel( B_SAVE_PANEL, &messenger, NULL, B_FILE_NODE, false, NULL, NULL, true, true );
	 
	 	// Set it to application's home directory
	 	app_info appInfo;
	 	be_app->GetAppInfo(&appInfo); 	
	 	BEntry entry(&appInfo.ref);
	 	BDirectory parentDir;
	 	entry.GetParent(&parentDir);
	 	m_FileSavePanel->SetPanelDirectory(&parentDir);
	 		
		// Center Panel
		CenterWindow(m_FileSavePanel->Window());
	}
			
	m_FileSavePanel->Show();
}


//---------------------------------------------------------------------
//	SaveAlert
//---------------------------------------------------------------------
//
//	Inform user that the document is dirty and should be saved
//

int32 TVideoEditor::SaveAlert()
{
	int32 result;
	
	BAlert *theAlert = new BAlert( "Museum", "This document has been changed since the last save.  Save before closing?", 
								   "Don't Save", "Cancel", "Save", B_WIDTH_FROM_WIDEST, B_WARNING_ALERT);
											 
	theAlert->SetShortcut(0, B_ESCAPE); 	 							
	CenterWindow(theAlert);		
	result = theAlert->Go();
	
	return (result);
}


//---------------------------------------------------------------------
//	Save
//---------------------------------------------------------------------
//
//

void TVideoEditor::Save(BMessage *message)
{
	status_t	myErr;
	entry_ref 	theRef;
	char		*theString = NULL;
					
	// Get directory to save data to
	message->FindRef("directory", &theRef);
	
	// Get name of file to be saved as
	message->FindString("name", (const char**)&theString);
	
	// Create a BDirectory object
	BDirectory saveDir(&theRef);
													
	// Save out data
	if (m_NewFile == false)
	{
		// Create the file.
		BFile *saveFile = new BFile();
		
		myErr = saveDir.CreateFile(theString, saveFile, false);
		if (myErr != B_OK)
			return;

		// Set file type
		BNodeInfo info(saveFile);
		if ( info.InitCheck() )
			return;
		
		info.SetType("video/raw");
		
		// Set icons
		BBitmap *smallIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_MovieIcon16;
		info.SetIcon( smallIcon, B_MINI_ICON);	
		
		BBitmap *largeIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_MovieIcon32;	
		info.SetIcon( largeIcon, B_LARGE_ICON);
		
		// Clean up 
		saveFile->Unset();
		delete saveFile;
	}
	// User is saving a fresh capture
	else
	{	
		// Get entry of source data
		BEntry entry(&m_FileRef);
		
		// Move data from temp to new location
		BDirectory moveDir(&theRef);
		entry.MoveTo(&moveDir);	 
		 
		// Rename the file to the one user specified
		entry.Rename(theString); 
		
		// Update newFile flag
		m_NewFile = false;
		
		// Get updated entry_ref
		entry_ref newRef;
		entry.GetRef(&newRef);
		
		// Add file to Browser
		BMessage theMessage(ADD_REF_MSG);
		theMessage.AddRef("FileRef", &newRef);
		TBrowserWindow *theBrowser = static_cast<MuseumApp *>(be_app)->GetBrowser();
		if (theBrowser)
			theBrowser->PostMessage(&theMessage, NULL);	
			
	}
      	
	// Set window title to new filename
	SetTitle(theString);
	
	if (m_IsClosing)
		Quit();
}


#pragma mark -
#pragma mark === Menu Handling ===

//-------------------------------------------------------------------
//	HandleFileMenu	
//-------------------------------------------------------------------
//
//	Handle file menu
//

void TVideoEditor::HandleFileMenu(BMessage *message)
{
	switch(message->what)
	{	
		case FILE_NEW_MSG:				
			break;
					
		case FILE_OPEN_MSG:
			break;
			
		case FILE_CLOSE_MSG:
			Quit();
			break;
			
		case FILE_SAVE_MSG:
			ShowFileSavePanel();
			break;
			
		case FILE_SAVEAS_MSG:
			ShowFileSavePanel();
			break;
			
		case FILE_REVERT_MSG:
			break;
						
		default:
			break;
	}
}


//-------------------------------------------------------------------
//	HandleEditMenu	
//-------------------------------------------------------------------
//
//	Handle edit menu
//

void TVideoEditor::HandleEditMenu(BMessage *message)
{
	switch(message->what)
	{
		case EDIT_UNDO_MSG:	
			break;
					
		case EDIT_CUT_MSG:	
			break;
			
		case EDIT_COPY_MSG:				
			break;
			
		case EDIT_PASTE_MSG:
			break;
							
		case EDIT_CLEAR_MSG	:	
			break;
					
		case EDIT_SELECTALL_MSG:
			break;
					
		default:
			break;	
	}
}


#pragma mark -
#pragma mark === Utility functions ===

//-------------------------------------------------------------------
//	LoadAndDecode	
//-------------------------------------------------------------------
//
//	Load AVIVideoFrame and decode it
//

bool TVideoEditor::LoadAndDecode(uint32 frameNum, BBitmap *bitmap)
{
	bool retVal = false;
	
	//	Load first frame into display	
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(frameNum);
	if (!theFrame)
	{
		printf("AVIProducer::INVALID FRAME! %d\n", frameNum);
		return retVal;
	}
		
	//	Decode frame
	retVal = m_VideoCodec->DecodeFrame(theFrame, m_DisplayBitmap->Bits(), B_RGB32);
	if (!retVal)
	{
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return retVal;
	}
	
	return retVal;
}

	
