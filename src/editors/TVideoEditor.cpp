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

#include "TVideoEditor.h"

//	Globals
int16 video_buffer[8192];

// Local prototypes
bool video_dac_writer(void* arg, char* buf, size_t count, void* header );

// Constants
//const int32 kSamplesPerFrame = 1471;
//const int32 kVideoHeaderSize = sizeof(VideoChunk) + sizeof(ChunkHeader);

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditor::TVideoEditor(entry_ref* theRef, TMovieCue* theCue, BMessage* message) : BWindow(message)
{
	// Save file ref
	fFileRef = *theRef;

	// Set up member varibles
	fMovieCue               = theCue;
	fFileSavePanel = NULL;

	//      Set up indicator flag specifying whether or not this file is a new capture
	//	or a previuosly saved file.  If a file originates from the /boot/var/tmp
	//	directory, we can assume it is a new capture

	// Get path to tmp
	BPath path;
	find_directory(B_SYSTEM_TEMP_DIRECTORY, &path);

	// Get node info
	node_ref nref;

	BEntry entry(path.Path());
	entry.GetNodeRef(&nref);

	if ( fFileRef.directory == nref.node)
		fNewFile = true;
	else
		fNewFile = false;

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
	fIsPlaying = false;
	fIsLooping = false;

	// Clean up
	if (fDisplayBitmap)
		delete fDisplayBitmap;

	// Inform cue that we have been closed
	if (fMovieCue)
		fMovieCue->SetEditorOpen(false);
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoEditor::Init()
{

	// Init member variables
	fIsPlaying = false;
	fIsLooping = false;

	//
	// Locate Views
	//

	BMessage* tmpMessage;
	bool retVal = false;

	// Create menubar
	BRect mbarRect  = Bounds();
	mbarRect.bottom = mbarRect.top+kMenuHeight;
	BMenuBar* mbar  = new BMenuBar(mbarRect, "mbar");
	fEditorMenu     = new TVideoEditorMenus(mbar, this);
	AddChild(mbar);

	// Backgrounds
	fBGView = (BView*)FindView("VideoEditorBGView");
	fControlsBGView = (BView*)FindView("VideoEditorControlsBGView");

	// VideoEditorView
	BView* editView = (BView*)FindView("VideoEditorView");
	tmpMessage = new BMessage();
	editView->Archive(tmpMessage, false);
	fEditorView = new TVideoEditorView(this, tmpMessage);
	delete tmpMessage;
	fBGView->RemoveChild(editView);
	delete editView;
	fBGView->AddChild(fEditorView);
	//fEditorView->Show();

	// ControlView
	BView* tmpControlsView = FindView("VideoEditorControlsView");
	tmpMessage = new BMessage();
	tmpControlsView->Archive(tmpMessage, false);
	fControlsView = new TVideoEditorControlsView(tmpMessage);
	delete tmpMessage;
	fControlsBGView->AddChild(fControlsView);
	fControlsView->Show();

	// TextViews
	BView* inTextView = FindView("InTextView");
	fInText = new TVideoEditorText(NULL, (int32)NULL, inTextView->Frame(), "InTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT);
	retVal = tmpControlsView->RemoveChild(inTextView);
	delete inTextView;
	fControlsView->AddChild(fInText);
	fInText->Show();

	BView* outTextView      = FindView("OutTextView");
	fOutText        = new TVideoEditorText(NULL, (int32)NULL, outTextView->Frame(), "OutTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT);
	tmpControlsView->RemoveChild(outTextView);
	delete outTextView;
	fControlsView->AddChild(fOutText);
	fOutText->Show();

	BView* durTxtView = FindView("DurationTextView");
	fDurationText   = new TVideoEditorText(NULL, (int32)NULL, durTxtView->Frame(), "DurationTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT);
	tmpControlsView->RemoveChild(durTxtView);
	delete durTxtView;
	fControlsView->AddChild(fDurationText);
	fDurationText->Show();

	BView* timeTextView = FindView("TimeTextView");
	fTimeText       = new TVideoEditorText( NULL, (int32)NULL, timeTextView->Frame(), "TimeTextView", B_FOLLOW_TOP|B_FOLLOW_LEFT);
	tmpControlsView->RemoveChild(timeTextView);
	delete timeTextView;
	fControlsView->AddChild(fTimeText);
	fTimeText->Show();

	// Buttons
	SetupControlButtons();

	// Create shuttle
	BView* shuttleView = FindView("ShuttleView");
	tmpMessage = new BMessage();
	shuttleView->Archive(tmpMessage, false);
	fShuttle = new TVideoEditorShuttle(this, tmpMessage);
	delete tmpMessage;
	fControlsView->AddChild(fShuttle);
	fShuttle->Show();

	// Create transport
	BView* transportView = FindView("TransportView");
	tmpMessage = new BMessage();
	transportView->Archive(tmpMessage, false);
	fTransport = new TVideoEditorTransport(this, tmpMessage);
	delete tmpMessage;
	fControlsView->AddChild(fTransport);
	fTransport->Show();

	// Free temp views
	fControlsBGView->RemoveChild(tmpControlsView);
	delete tmpControlsView;

	//	Init tracking variables
	fCurrentVideoFrame = 0;

	/*
	//	Get pointer to cue's RIFFReader
	fReader = fMovieCue->RIFFReader();

	//	Get pointer to codecs
	fAudioCodec = fMovieCue->AudioCodec();
	fVideoCodec = fMovieCue->VideoCodec();

	//	Get AVIHeader
	AVIHeader* header = fReader->GetAVIHeader();

	//	Create offscreen
	BRect movieRect( 0, 0, header->Width-1, header->Height-1);
	fDisplayBitmap = new BBitmap(movieRect, B_RGB32);

	//	Load first frame into display
	AVIVideoFrame* theFrame = (AVIVideoFrame*)fReader->GetVideoFrameList()->ItemAt(fCurrentVideoFrame);
	if (!theFrame) {
		printf("AVIProducer::INVALID FRAME!\n");
		return;
	}

	//	Decode frame
	retVal = fVideoCodec->DecodeFrame(theFrame, fDisplayBitmap->Bits(), B_RGB32);
	if (!retVal) {
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return;
	}*/

	// Load in file info
	InitMovieAttributes();

	// Setup time indicators
	SetupTextItems();

	//	Show the video display
	fEditorView->Show();
	fEditorView->Invalidate();

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
	case FILE_CLOSE_MSG:
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
	case EDIT_CLEAR_MSG:
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
	if (fIsPlaying)
		return;

	// Set playback flag to true
	fIsPlaying = true;

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

	if (fIsPlaying) {
		// Set playback flag to true
		fIsPlaying = false;
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
	if (fCurrentVideoFrame == 0)
		return;

	// Decrement frame and redraw
	fCurrentVideoFrame--;

	// Load frame
	if ( LoadAndDecode(fCurrentVideoFrame, fDisplayBitmap) == true) {
		// Draw frame
		Lock();
		fEditorView->DisplayFrame(fDisplayBitmap);
		fEditorView->Sync();
		Unlock();

		// Update time
		//fTime->SetText(  );
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
	/*fCurrentVideoFrame++;

	// Protect against overflow
	if ( fCurrentVideoFrame >= fReader->VideoFrameCount()) {
		fCurrentVideoFrame--;
		return;
	}

	// Load frame
	if ( LoadAndDecode(fCurrentVideoFrame, fDisplayBitmap) == true) {
		// Draw frame
		Lock();
		fEditorView->DisplayFrame(fDisplayBitmap);
		fEditorView->Sync();
		Unlock();

		// Update time
		//fTime->SetText(  );
	}*/

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
	fCurrentVideoFrame = 0;
	fCurrentTime            = 0;

	// Load frame
	if ( LoadAndDecode(fCurrentVideoFrame, fDisplayBitmap) == true) {
		// Draw frame
		Lock();
		fEditorView->DisplayFrame(fDisplayBitmap);
		fEditorView->Sync();
		Unlock();

		// Update time text
		//char tmpText[13];
		//TimeToString(fCurrentTime, fVideoChunk.frame_rate, tmpText, false);
		//fTimeText->SetText(tmpText);
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

	/*//	Go to last frame
	fCurrentVideoFrame = fReader->VideoFrameCount() - 1;

	// Load frame
	if ( LoadAndDecode(fCurrentVideoFrame, fDisplayBitmap) == true) {
		// Draw frame
		Lock();
		fEditorView->DisplayFrame(fDisplayBitmap);
		fEditorView->Sync();
		Unlock();

		// Update time text
		//char tmpText[13];
		//TimeToString(fCurrentTime, fVideoChunk.frame_rate, tmpText, false);
		//fTimeText->SetText(tmpText);
	}*/
}

//---------------------------------------------------------------------
//	SetLooping
//---------------------------------------------------------------------
//
//	Toggle looping mode
//

void TVideoEditor::SetLooping()
{
	if (fIsLooping)
		fIsLooping = false;
	else
		fIsLooping = true;
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
	/*AVIHeader* header = fReader->GetAVIHeader();
	const uint32 mSecsPerFrame = header->TimeBetweenFrames / 1000;
	const uint32 duration      = header->TotalNumberOfFrames * mSecsPerFrame;

	// In Time
	fInText->SetFontAndColor(&smallFont, 0, kGreen);
	fInText->SetText("00:00:00:00");

	// Out Time
	fOutText->SetFontAndColor(&smallFont, 0, kGreen);
	TimeToString(duration, header->DataRate, tmpText, false);
	fOutText->SetText(tmpText);

	// Duration
	fDurationText->SetFontAndColor(&smallFont, 0, kGreen);
	TimeToString(duration, header->DataRate, tmpText, false);
	fDurationText->SetText(tmpText);

	// TimeText
	BFont timeFont(be_bold_font);
	timeFont.SetSize(20);
	fTimeText->SetFontAndColor(&timeFont, 0, kGreen);
	TimeToString(fCurrentTime, header->DataRate, tmpText, false);
	fTimeText->SetText("00:00:00:00");*/
}

//-------------------------------------------------------------------
//	SetupTextItems
//-------------------------------------------------------------------
//
//	Initialize control buttons displays
//

void TVideoEditor::SetupControlButtons()
{
	BBitmap* offBitmap, * onBitmap;

	BView* tmpControlsView = FindView("VideoEditorControlsView");

	// In button
	offBitmap = GetIcon16FromResource("VEInUp");
	onBitmap  = GetIcon16FromResource("VEInDown");
	BView* inButton = FindView("InButtonView");
	fInButton       = new TBitmapButton(inButton->Frame(), "InButton", offBitmap, onBitmap, this, new BMessage(VE_IN_MSG));
	tmpControlsView->RemoveChild(inButton);
	delete inButton;
	fControlsView->AddChild(fInButton);

	// Out button
	offBitmap = GetIcon16FromResource("VEOutUp");
	onBitmap = GetIcon16FromResource("VEOutDown");
	BView* outButton = FindView("OutButtonView");
	fOutButton      = new TBitmapButton(outButton->Frame(), "OutButton", offBitmap, onBitmap, this, new BMessage(VE_OUT_MSG));
	tmpControlsView->RemoveChild(outButton);
	delete outButton;
	fControlsView->AddChild(fOutButton);
	fOutButton->Show();

	// Volume button
	offBitmap = GetIcon16FromResource("VEVolumeUp");
	onBitmap = GetIcon16FromResource("VEVolumeDown");
	BView* volButton = FindView("VolumeButtonView");
	fVolumeButton = new TBitmapButton(volButton->Frame(), "VolumeButton", offBitmap, onBitmap, this, new BMessage(VE_VOLUME_MSG));
	tmpControlsView->RemoveChild(volButton);
	delete volButton;
	fControlsView->AddChild(fVolumeButton);
	fVolumeButton->Show();

	// Start button
	offBitmap = GetIcon16FromResource("VEStartUp");
	onBitmap = GetIcon16FromResource("VEStartDown");
	BView* startButton = FindView("StartButtonView");
	fStartButton = new TBitmapButton(startButton->Frame(), "StartButton", offBitmap, onBitmap, this, new BMessage(VE_START_MSG));
	tmpControlsView->RemoveChild(startButton);
	delete startButton;
	fControlsView->AddChild(fStartButton);
	fStartButton->Show();

	// End button
	offBitmap = GetIcon16FromResource("VEEndUp");
	onBitmap = GetIcon16FromResource("VEEndDown");
	BView* endButton = FindView("EndButtonView");
	fEndButton = new TBitmapButton(endButton->Frame(), "EndButton", offBitmap, onBitmap, this, new BMessage(VE_END_MSG));
	tmpControlsView->RemoveChild(endButton);
	delete endButton;
	fControlsView->AddChild(fEndButton);
	fEndButton->Show();

	// FrameBack button
	offBitmap = GetIcon16FromResource("VEStepBackUp");
	onBitmap  = GetIcon16FromResource("VEStepBackDown");
	BView* backButton = FindView("FrameBackButtonView");
	fFrameBackButton = new TBitmapButton(backButton->Frame(), "FrameBack", offBitmap, onBitmap, this, new BMessage(VE_FB_MSG));
	tmpControlsView->RemoveChild(backButton);
	delete backButton;
	fControlsView->AddChild(fFrameBackButton);
	fFrameBackButton->Show();

	// FrameForward button
	offBitmap = GetIcon16FromResource("VEStepForwardUp");
	onBitmap  = GetIcon16FromResource("VEStepForwardDown");
	BView* forwardButton = FindView("FrameForwardButtonView");
	fFrameForwardButton = new TBitmapButton(forwardButton->Frame(), "FrameForward", offBitmap, onBitmap, this, new BMessage(VE_FF_MSG));
	tmpControlsView->RemoveChild(forwardButton);
	delete forwardButton;
	fControlsView->AddChild(fFrameForwardButton);
	fFrameForwardButton->Show();

	// Play button
	offBitmap = GetIcon16FromResource("VEPlayUp");
	onBitmap = GetIcon16FromResource("VEPlayDown");
	BView* playButton = FindView("PlayButtonView");
	fPlayButton = new TBitmapButton(playButton->Frame(), "PlayButton", offBitmap, onBitmap, this, new BMessage(VE_PLAY_MSG));
	tmpControlsView->RemoveChild(playButton);
	delete playButton;
	fControlsView->AddChild(fPlayButton);
	fPlayButton->Show();

	// Loop button
	offBitmap = GetIcon16FromResource("VELoopUp");
	onBitmap = GetIcon16FromResource("VELoopDown");
	BView* loopButton = FindView("LoopButtonView");
	fLoopButton = new TBitmapButton(loopButton->Frame(), "LoopButton", offBitmap, onBitmap, this, new BMessage(VE_LOOP_MSG));
	tmpControlsView->RemoveChild(loopButton);
	delete loopButton;
	fControlsView->AddChild(fLoopButton);
	fLoopButton->Show();

	// Mark button
	offBitmap = GetIcon16FromResource("VEMarkUp");
	onBitmap = GetIcon16FromResource("VEMarkDown");
	BView* markButton = FindView("MarkButtonView");
	fMarkButton = new TBitmapButton(markButton->Frame(), "MarkButton", offBitmap, onBitmap, this, new BMessage(VE_MARK_MSG));
	tmpControlsView->RemoveChild(markButton);
	delete markButton;
	fControlsView->AddChild(fMarkButton);
	fMarkButton->Show();

	// Goto button
	offBitmap = GetIcon16FromResource("VEJumpUp");
	onBitmap = GetIcon16FromResource("VEJumpDown");
	BView* gotoButton = FindView("GotoButtonView");
	fGotoButton = new TBitmapButton(gotoButton->Frame(), "GotoButton", offBitmap, onBitmap, this, new BMessage(VE_JUMP_MSG));
	tmpControlsView->RemoveChild(gotoButton);
	delete gotoButton;
	fControlsView->AddChild(fGotoButton);
	fGotoButton->Show();

	// Detach button
	//offBitmap = GetIcon16FromResource("VEDetachUp");
	//onBitmap = GetIcon16FromResource("VEDetachDown");
	offBitmap = GetIcon16FromResource("VEJumpUp");
	onBitmap = GetIcon16FromResource("VEJumpDown");
	BView* detachButton = FindView("DetachButtonView");
	fDetachButton = new TBitmapButton(detachButton->Frame(), "DetachButton", offBitmap, onBitmap, this, new BMessage(VE_DETACH_MSG));
	tmpControlsView->RemoveChild(detachButton);
	delete detachButton;
	fControlsView->AddChild(fDetachButton);
	fDetachButton->Show();
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

	if ( fNewFile == true) {
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
			fIsClosing = true;
			retVal = false;
		}
		break;

		default:
			retVal = true;
		}
	}

	// Delete temp file if necessary
	if (retVal && fNewFile) {
		// Delete temp file
		BEntry entry(&fFileRef);

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
	//      Create messenger to send panel messages to our channel.  We cannot send it to
	//  ourself as we are not part of the view heirarchy.
	BMessenger messenger( this,  NULL);

	//      If the panel has already been constructed, show the panel
	//      Otherwise, create the panel.  We do need to reset it's messenger to
	//	point at the correct window
	if (fFileSavePanel) {
		fFileSavePanel->SetTarget(messenger);
		fFileSavePanel->Show();
	} else {
		// Construct a file panel and set it to modal
		fFileSavePanel = new BFilePanel( B_SAVE_PANEL, &messenger, NULL, B_FILE_NODE, false, NULL, NULL, true, true );

		// Set it to application's home directory
		app_info appInfo;
		be_app->GetAppInfo(&appInfo);
		BEntry entry(&appInfo.ref);
		BDirectory parentDir;
		entry.GetParent(&parentDir);
		fFileSavePanel->SetPanelDirectory(&parentDir);

		// Center Panel
		CenterWindow(fFileSavePanel->Window());
	}

	fFileSavePanel->Show();
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

	BAlert* theAlert = new BAlert( "Museum", "This document has been changed since the last save.  Save before closing?",
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

void TVideoEditor::Save(BMessage* message)
{
	status_t myErr;
	entry_ref theRef;
	char* theString = NULL;

	// Get directory to save data to
	message->FindRef("directory", &theRef);

	// Get name of file to be saved as
	message->FindString("name", (const char**)&theString);

	// Create a BDirectory object
	BDirectory saveDir(&theRef);

	// Save out data
	if (fNewFile == false) {
		// Create the file.
		BFile* saveFile = new BFile();

		myErr = saveDir.CreateFile(theString, saveFile, false);
		if (myErr != B_OK)
			return;

		// Set file type
		BNodeInfo info(saveFile);
		if ( info.InitCheck() )
			return;

		info.SetType("video/raw");

		// Set icons
		BBitmap* smallIcon      = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fMovieIcon16;
		info.SetIcon( smallIcon, B_MINI_ICON);

		BBitmap* largeIcon      = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fMovieIcon32;
		info.SetIcon( largeIcon, B_LARGE_ICON);

		// Clean up
		saveFile->Unset();
		delete saveFile;
	}
	// User is saving a fresh capture
	else{
		// Get entry of source data
		BEntry entry(&fFileRef);

		// Move data from temp to new location
		BDirectory moveDir(&theRef);
		entry.MoveTo(&moveDir);

		// Rename the file to the one user specified
		entry.Rename(theString);

		// Update newFile flag
		fNewFile = false;

		// Get updated entry_ref
		entry_ref newRef;
		entry.GetRef(&newRef);

		// Add file to Browser
		BMessage theMessage(ADD_REF_MSG);
		theMessage.AddRef("FileRef", &newRef);
		TBrowserWindow* theBrowser = static_cast<MuseumApp*>(be_app)->GetBrowser();
		if (theBrowser)
			theBrowser->PostMessage(&theMessage, NULL);

	}

	// Set window title to new filename
	SetTitle(theString);

	if (fIsClosing)
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

void TVideoEditor::HandleFileMenu(BMessage* message)
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

void TVideoEditor::HandleEditMenu(BMessage* message)
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

	case EDIT_CLEAR_MSG:
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

bool TVideoEditor::LoadAndDecode(uint32 frameNum, BBitmap* bitmap)
{
	bool retVal = false;

	//	Load first frame into display
	/*AVIVideoFrame* theFrame = (AVIVideoFrame*)fReader->GetVideoFrameList()->ItemAt(frameNum);
	if (!theFrame) {
		printf("AVIProducer::INVALID FRAME! %d\n", frameNum);
		return retVal;
	}

	//	Decode frame
	retVal = fVideoCodec->DecodeFrame(theFrame, fDisplayBitmap->Bits(), B_RGB32);
	if (!retVal) {
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return retVal;
	}*/

	return retVal;
}


