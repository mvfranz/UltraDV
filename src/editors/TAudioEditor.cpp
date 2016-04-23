//---------------------------------------------------------------------
//
//	File:	TAudioEditor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.18.98
//
//	Desc:	Audio Cue editor.  Opens cue into a window that displays it's 
//			waveform and allows the user to perform some actions.
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
#include "MuseumApp.h"
#include "AppUtils.h"
#include "TMuseumIcons.h"

#include "TAudioEditor.h"
#include "TAudioEditorView.h"
#include "TAudioEditorToolbar.h"
#include "TAudioEngine.h"
#include "TAudioEditorMenus.h"
#include "TBrowserWindow.h"

#include "TCueView.h"
#include "TAudioCue.h"
#include "TAudioScalerView.h"
#include "TAudioTimelineView.h"

// Constants
const short kAudioScalerWidth = 170;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Contruct with an entry_ref and a cue.  If the cue is null, we are
//	opening a file directly after an audio capture and the file has
//	not been attached to a cue.
//

TAudioEditor::TAudioEditor(BRect bounds, entry_ref *ref, TAudioCue *theCue): BWindow(bounds, "Untitled", B_DOCUMENT_WINDOW, 0,0)
{
	// Save file ref
	m_FileRef = *ref;
	
	// Save cue
	m_AudioCue = theCue;
	
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
	         
	if ( ref->directory == nref.node)
		m_NewFile = true;
	else
		m_NewFile = false;
	
	// Default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioEditor::~TAudioEditor()
{
	if (m_IsPlaying)
	{
		//m_Engine->Stop();
	}
		
	// Inform cue that we have been closed
	if (m_AudioCue)
		m_AudioCue->SetEditorOpen(false);	
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//


void TAudioEditor::Init()
{
	//	Create BSound
	m_Sound = new BSound(&m_FileRef);
	
	//	Check for error
				
	// Set up state variables
	m_IsClosing		= false;
	m_SelectStart 	= 0;
	m_SelectEnd 	= 0;
	m_CaretTime 	= 0;
	m_IsPlaying 	= false;
					
	// Create menubar
	BRect mbarRect 	= Bounds();
	mbarRect.bottom = mbarRect.top+kMenuHeight;
	BMenuBar *mbar 	= new BMenuBar(mbarRect, "mbar");
	m_EditorMenu 	= new TAudioEditorMenus(mbar, this);	
	AddChild(mbar);

	// Create Toolbar
	BRect bounds = Bounds();
	BRect toolBounds(bounds.left, bounds.top+kMenuHeight , bounds.right - (B_V_SCROLL_BAR_WIDTH+1), bounds.top+kMenuHeight+kAudioToolbarHeight);
	m_Toolbar = new TAudioEditorToolbar(this, toolBounds);
	AddChild(m_Toolbar);
	m_Toolbar->Show();
	
	// Create Background View.  It hold all the other views...
	BRect bgRect   	= Bounds();
	bgRect.top		= m_Toolbar->Frame().bottom+1;
	bgRect.right   	-= (B_V_SCROLL_BAR_WIDTH+1);
	bgRect.bottom  	-= (B_H_SCROLL_BAR_HEIGHT+1);
	m_Background = new BView(bgRect, "Container", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(m_Background);
	m_Background->SetViewColor(kWhite);
	m_Background->Show();	
	
	// 	Create Timeline.  It is always located below the toolbar.
	//	This is where the time is indicated and the playback head resides
	BRect timeRect 	= m_Background->Bounds();
	timeRect.right	= 500;
	timeRect.bottom = timeRect.top+kTimelineHeight;
	m_Timeline = new TAudioTimelineView(timeRect, this);
	m_Background->AddChild(m_Timeline);
	m_Timeline->Show();
		
	// Init sound file attributes
	InitSoundfileAttributes();
	
	// Rewind sound file
	// m_Engine->m_SoundFile->SeekToFrame(0);
		
	// Create editor view.  It should be the width of the sound in pixels	
	float right = m_NumSamples / m_SamplesPerPixel;
	BRect audioBounds( bounds.left, m_Timeline->Frame().bottom+1, right, bounds.bottom-(B_H_SCROLL_BAR_HEIGHT+1) );
	m_EditorView = new TAudioEditorView(this, audioBounds);
	m_Background->AddChild(m_EditorView);
	m_EditorView->Show();
	
	//
	// Create scroll bars
	//
	
	// Horizontal	
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.left + kAudioScalerWidth, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT, scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.bottom);
	m_HScroll = new BScrollBar(scrollRect, "HAudioScroll", m_Background, 0, right, B_HORIZONTAL);
	AddChild(m_HScroll);	
		
	// Vertical
	scrollRect = Bounds();
	scrollRect.Set(scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.top+kAudioToolbarHeight+kMenuHeight+1, scrollRect.right, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT);
	m_VScroll = new BScrollBar(scrollRect, "VAudioScroll", m_EditorView, 0, right, B_VERTICAL);
	AddChild(m_VScroll);	
	
	// Create TimeScaler
	BRect scalerRect(Bounds().left, Bounds().bottom - B_H_SCROLL_BAR_HEIGHT, kAudioScalerWidth, Bounds().bottom);
	m_AudioScaler = new TAudioScalerView(scalerRect, m_EditorView);
	AddChild(m_AudioScaler);
	
	// Update view size based on sound file attributes
	m_EditorView->SetAudioViewBounds();	
	
	// Resize toolbar based on editor view settings
	m_Toolbar->ResizeTo(Bounds().right, m_Toolbar->Frame().Height() );	
	
	// Set minimum window size and then open it to max	
	float maxWidthHeight = kAudioEditorHeight+kAudioToolbarHeight+kTimelineHeight;		
	SetSizeLimits(kAudioScalerWidth+20, 600, maxWidthHeight, maxWidthHeight);
	
	//Zoom();
}


#pragma mark -
#pragma mark === Message Handling === 

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioEditor::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{
		case AUDIO_PLAY_BUTTON_MSG:		
			//m_SoundID = m_Engine->StartPlaying(m_Sound);
			break;
			
		case AUDIO_STOP_BUTTON_MSG:
			//m_Engine->Stop();
			break;			
		
		// These messages come from the BFilePanel when saving an audio file
		case B_SAVE_REQUESTED:
			Save(message);
			break;

		case B_CANCEL:
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
#pragma mark === Soundfile Handling === 

//---------------------------------------------------------------------
//	InitSoundfileAttributes
//---------------------------------------------------------------------
//
//

void TAudioEditor::InitSoundfileAttributes()
{
	// Get total channels
	//m_NumChannels = m_Engine->m_SoundFile->CountChannels();	
					
	// Determine number of samples
	//m_NumSamples = ( m_Engine->m_SoundFile->CountFrames() * m_Engine->m_SoundFile->FrameSize() ) / m_NumChannels;
	
	// Determine samples per pixel
	//m_SamplesPerPixel = m_NumSamples / Bounds().Width() + 1;	
	
	// Get sample size
	//m_SampleSize = m_Engine->m_SoundFile->SampleSize();	
}



#pragma mark -
#pragma mark === Menu Handling ===

//-------------------------------------------------------------------
//	HandleFileMenu	
//-------------------------------------------------------------------
//
//	Handle file menu
//

void TAudioEditor::HandleFileMenu(BMessage *message)
{
	switch(message->what)
	{	
		case FILE_NEW_MSG:				
			break;
					
		case FILE_OPEN_MSG:
			break;
			
		case FILE_CLOSE_MSG:
			Lock();
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

void TAudioEditor::HandleEditMenu(BMessage *message)
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
#pragma mark === Completion Routines === 

//-------------------------------------------------------------------
//	QuitRequested	
//-------------------------------------------------------------------
//
//	Check and see if file needs to be saved before we quit
//

bool TAudioEditor::QuitRequested()
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

void TAudioEditor::ShowFileSavePanel()
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

int32 TAudioEditor::SaveAlert()
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

void TAudioEditor::Save(BMessage *message)
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

		//BFile soundFile(m_FileRef);
		//soundFile->SeekSet(0, 
		//saveFile->Write(  );
		
		// Set file type
		BNodeInfo info(saveFile);
		if ( info.InitCheck() )
			return;
		
		info.SetType("audio/raw");
		
		// Set icons
		BBitmap *smallIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_AudioIcon16;
		info.SetIcon( smallIcon, B_MINI_ICON);	
		
		BBitmap *largeIcon	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_AudioIcon32;	
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
	{
		Lock();
		Quit();
	}
}
