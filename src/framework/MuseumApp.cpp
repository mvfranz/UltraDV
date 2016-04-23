//---------------------------------------------------------------------
//
//	File:	MuseumApp.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Main application initialization routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>
#include <MediaRoster.h>

#include "TMuseumIcons.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TAboutBox.h"

#include "AppAlerts.h"
#include "AppConstants.h"

#include "AppMessages.h"

#include "TMuseumClipboard.h"
#include "TCursor.h"
#include "TMuseumPrefs.h"
#include "TRefFilter.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"

// Palettes
#include "TMediaCuePalette.h"
#include "TTimePalette.h"
#include "TTransportPalette.h"
#include "TBrowserWindow.h"
#include "TStageTools.h"

#include "TPlaybackEngine.h"
#include "TTimelineView.h"
#include "TPreset.h"

#include "TMuseumMenus.h"
#include "MenuMessages.h"


// Dialogs
#include "TAddChannel.h"
#include "TDeleteChannel.h"
#include "TStageSetup.h"
#include "TNewProject.h"
#include "TAudioSettingsDialog.h"
#include "TVideoSettingsDialog.h"
#include "TAudioCaptureWindow.h"
#include "TVideoCaptureWindow.h"
#include "TRecordPrefsDialog.h"
#include "TVideoEditor.h"

#include "Watch.h"

// ABH assert.h
#include <assert.h>
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(x) assert(x)
 

#include "MuseumApp.h"

// Constants
//const kTransportHeight 	= kTransportButtonHeight * 2;
//const kTransportWidth 	= (kTransportButtonWidth * 5) + 2;

const int32 kTransportWidth		= 182;
const int32 kTransportHeight	= 67;

const int32 kTimeWidth 			= 196;
const int32 kTimeHeight 		= 100;


//-------------------------------------------------------------------
//
//	Function:	main
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

main()
{	
	
	BApplication *theApp = new MuseumApp();
	theApp->Run();	
	delete(theApp);
	
	return(0);
}


//-------------------------------------------------------------------
//	MuseumApp	
//-------------------------------------------------------------------
//
//	Default Constructor
//

MuseumApp::MuseumApp(): BApplication("application/x-mediapede-Museum")
{	
	// Leak checking
// ABH
//#ifdef DEBUG
//SetNewLeakChecking(true); 
//SetMallocLeakChecking(true); 
//#endif
WATCH("In constructor\n");
	
	// Init palettes to NULL
	m_CueSheetWindow 	= NULL;
	m_ActionCuePalette 	= NULL;		
	m_MediaCuePalette 	= NULL;		
	m_TimePalette		= NULL;		
	m_TransportPalette 	= NULL;
	m_CueSheetList		= NULL;
	
}


//-------------------------------------------------------------------
//	MuseumApp	
//-------------------------------------------------------------------
//
//	Default Destructor
//

MuseumApp::~MuseumApp()
{
	// Clean up
	if (m_FileOpenPanel)
		delete m_FileOpenPanel;
	
	if (m_FileSavePanel)
		delete m_FileSavePanel;

	if (m_Clipboard)
		delete m_Clipboard;

	// Quit all palettes
	//if (m_ActionCuePalette)
	//{
	//	m_ActionCuePalette->Lock();
	//	m_ActionCuePalette->Quit();
	//}
	
	if (m_MediaCuePalette)
	{
		m_MediaCuePalette->Lock();
		m_MediaCuePalette->Quit();		
	}
	
	if (m_BrowserWindow)
	{
		m_BrowserWindow->Lock();
		m_BrowserWindow->Quit();
	}
	
	if (m_TimePalette)
	{
		m_TimePalette->Lock();
		m_TimePalette->Quit();		
	}
	
	if (m_TransportPalette)
	{
		m_TransportPalette->Lock();
		m_TransportPalette->Quit();
	}
	
	FreeCursors();
}


//-------------------------------------------------------------------
//
//	Function:	ReadyToRun
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

void MuseumApp::ReadyToRun()
{
	
	// Init member variables	
	m_IsCueDrag 	= false;
	m_FileOpenPanel = NULL;
	m_FileSavePanel = NULL;

WATCH("In ReadyToRun\n");
	
	// Display the About box
	TAboutBox *aboutWindow = new TAboutBox();
	CenterWindow(aboutWindow);
	aboutWindow->Show();
	
	//	Connect to media_server
	m_MediaRoster = BMediaRoster::Roster();
		 
	if (!m_MediaRoster) 
	{ 
		//	The Media Server appears to be dead -- handle that here
		ErrorAlert((char *)"Unable to connect to media_server.");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}
		
	// Create application clipboard
	m_Clipboard = new TMuseumClipboard();
	
	// Init Translation Kit services
	m_TranslationRoster = new BTranslatorRoster();

	// Load our custom translators
   	//roster->AddTranslators("/boot/home/config/add-ons/viewer/"); 
   	
   	// Load default translators
	m_TranslationRoster->AddTranslators(NULL);
				
	// Load cursors
	LoadCursors();
	
	// Load app icons
	m_MuseumIcons = new TMuseumIcons();
		
	// Load preference
	m_Prefs = new TMuseumPrefs();
						
	// Load cue add-ons and create list
	LoadCueAddOns();
		
	// Present user with new project setting
	BMessage *theMessage = GetWindowFromResource("NewProjectWindow");
	TNewProject *theDialog = new TNewProject(this, theMessage);
	ASSERT(theDialog);
	CenterWindow(theDialog);

	// Show the dialog
	theDialog->Show();		
WATCH("End of ReadyToRun\n");
}	


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//	Complete application initialization
//

void MuseumApp::Init()
{			
	//
	// Create cue palettes
	//
	
	// Get screen dimensions
	BScreen theScreen(B_MAIN_SCREEN_ID);
	BRect bounds = theScreen.Frame();	
	
	
	// Lazlo does not have a control palette
	m_MediaCuePalette = new TMediaCuePalette();
WATCH("In MuseumApp::Init\n");		
	
	// Create transport
	BRect tranRect = theScreen.Frame(); 
	tranRect.bottom -= 10;
	tranRect.left += 10;
	tranRect.top = tranRect.bottom - kTransportHeight;
	tranRect.right = tranRect.left + kTransportWidth;
	m_TransportPalette = new TTransportPalette(tranRect);
		
	
	// Create time palette
	BRect timeRect = theScreen.Frame(); 
	timeRect.bottom -= 10;
	timeRect.right -= 10;
	timeRect.top = timeRect.bottom - kTimeHeight;
	timeRect.left = timeRect.right - kTimeWidth;
	m_TimePalette = new TTimePalette(timeRect);
		
	
	// Create Browser Window
	BRect browserRect 	= theScreen.Frame(); 
	browserRect.top 	+= 50;
	browserRect.bottom 	= browserRect.top + 300;
	browserRect.right 	= browserRect.left + 200;
	m_BrowserWindow 	= new TBrowserWindow(browserRect);
	
	
	//	Create Stage Tools
	m_StageTools 	= new TStageTools();

	// Create cue sheet list and add current cue sheet to it
	m_CueSheetList = new BList();
		
	
	// Create the Cue Sheet
	bounds.Set( bounds.left+125, bounds.top+50, bounds.right, bounds.bottom-200);
	m_CueSheetWindow = new TCueSheetWindow( bounds, m_CueSheetList->CountItems()+1 );
				
	
	// Add Cue Sheet to Windows menu
	TMuseumMenus *theMenu = m_CueSheetWindow->GetMenu();
	theMenu->AddToWindowsMenu( m_CueSheetWindow->Title(), m_CueSheetWindow->GetID());
	
	
	// Arrange the windows into their proper locations
	ArrangeWindows();
		
	// Show them all now...
	m_MediaCuePalette->Show();		
	m_TimePalette->Show();		
	m_TransportPalette->Show();
	m_BrowserWindow->Show();
	m_StageTools->Show();
	m_CueSheetWindow->GetStage()->Show();
	m_CueSheetWindow->Show();

WATCH("MuseumApp::Init 7 windows should display\n");
	
	BMessage *message = new BMessage(ALL_FRONT_MSG);
	PostMessage(message, be_app);
	delete message;
	
}


//-------------------------------------------------------------------
//	ArrangeWindows
//-------------------------------------------------------------------
//
//	

void MuseumApp::ArrangeWindows()
{
	// Get screen dimensions
	BScreen theScreen(B_MAIN_SCREEN_ID);
	BRect bounds = theScreen.Frame();	
	
	// The browser goes at the far left
	m_BrowserWindow->MoveTo(bounds.left, bounds.top+40);
	
	// The cue sheet goes at the bottom left and stretches to the far right
	m_CueSheetWindow->ResizeTo(bounds.Width() - 20, bounds.Height()-m_BrowserWindow->Frame().Height() - 50);
	m_CueSheetWindow->MoveTo(bounds.left+10, m_BrowserWindow->Frame().bottom);
	
	// Locator goes at the top right
	m_TimePalette->MoveTo( bounds.right - m_TimePalette->Bounds().Width(), bounds.top+40);
	
	// Transport goes at the right below Locator
	m_TransportPalette->MoveTo(m_TimePalette->Frame().left, m_TimePalette->Frame().bottom+25);
		
	// Stage is to the left of the Locator
	TStageWindow *theStage = m_CueSheetWindow->GetStage();
	theStage->MoveTo( (m_TimePalette->Frame().left - theStage->Bounds().Width())-8, bounds.top+40);

	// Stage Tools go to the left of the stage	
	m_StageTools->MoveTo((theStage->Frame().left - m_StageTools->Bounds().Width())-8, bounds.top+40);

	// Cues go to the left of the Stage Tools	
	m_MediaCuePalette->MoveTo((m_StageTools->Frame().left - m_MediaCuePalette->Bounds().Width())-8, bounds.top+40);
	
	// Now resize browser to move flush against the cues left side
	m_BrowserWindow->ResizeTo(m_MediaCuePalette->Frame().left-8,  m_BrowserWindow->Frame().Height() - 25);

}

//-------------------------------------------------------------------
//
//	Function:	AboutRequested
//
//	Desc:		Display about box in response to message
//
//-------------------------------------------------------------------
//
//

void MuseumApp::AboutRequested()
{
	
	// Display the about dialog
	TAboutBox *aboutWindow = new TAboutBox();
	
	// Show the window
	CenterWindow(aboutWindow);
	aboutWindow->Show();
}


//-------------------------------------------------------------------
//
//	Function:	QuitRequested
//
//	Desc:		Check program state before quiting
//
//-------------------------------------------------------------------
//
//

bool MuseumApp::QuitRequested()
{			
	// Stop playback
	       
	// If m_CueSheetList is NULL, exit
	if (!m_CueSheetList)
		return true;
		
	// Close all windows in m_CueSheetList.
	while (m_CueSheetList->CountItems() > 0)
	{
		TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(0);
		
		// See if we need to save the cue sheet
		if (theWindow)
		{			
			// Set this window as the current window
			if (theWindow != m_CueSheetWindow)
				m_CueSheetWindow = theWindow;
				
			// Bring it to the fornt
			m_CueSheetWindow->Show();										
			
			// Ask it to quit using our rotuine
			bool retVal = m_CueSheetWindow->MyQuitRequested();
			
			// User is quiting
			if (retVal)
			{
				// Hide stage
				if (m_CueSheetWindow->GetStage())
					m_CueSheetWindow->GetStage()->Hide();
					
				// Hide window
				m_CueSheetWindow->Lock();
				m_CueSheetWindow->Hide();
					
				// Destroy cue sheet window				
				m_CueSheetWindow->Quit();
				m_CueSheetWindow = NULL;
			}
			// User has changed mind about quiting
			else
				return false;
		}
	}
	
	// Hide all windows before quiting
	//if (m_ActionCuePalette)
	//	m_ActionCuePalette->Hide();		
		
	if (m_MediaCuePalette)	
		m_MediaCuePalette->Hide();		
		
	if (m_TimePalette)
		m_TimePalette->Hide();		
		
	if (m_TransportPalette)
		m_TransportPalette->Hide();
		
	// Free cue sheet list 
	delete m_CueSheetList;
		
	// Close Translation Kit
	delete m_TranslationRoster;
			
	// Free icons
	delete m_MuseumIcons;
	
	return true;	
}

#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//
//	Function:	MessageReceived
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

void MuseumApp::MessageReceived(BMessage* message)
{	
WATCH("In MuseumApp::MessageReceived\n");
	
	switch(message->what)
	{											
		// Dialogs
		case NEW_PROJECT_MSG: {
WATCH("NEW_PROJECT_MSG\n");
				// Retrieve setting from message
				BMessage archiveMessage;
				if ( !message->FindMessage("Preset", &archiveMessage)){
				
					BArchivable *unarchived = instantiate_object(&archiveMessage); 
			 		if ( unarchived ){	 						
						;//TPreset *preset = cast_as(unarchived, TPreset); 
					} else {
						printf("MA:: instantiate_object failed!\n");
						ASSERT(unarchived);
					}
										
					// Complete initialization
					Init();				
				} else {
					WATCH("MuseumApp::MessageReceived: NEW_PROJECT_MSG - could not unarchive!\n");
					TRESPASS();	
				}							
			}		
			break;
			
		//	Handle 
		case ACTION_PALETTE_CLOSE_MSG:
WATCH("ACTION_PALETTE_CLOSE_MSG\n");
			m_ActionCuePalette = NULL;
			break;
			
		case MEDIA_PALETTE_CLOSE_MSG:
WATCH("MEDIA_PALETTE_CLOSE_MSG\n");
			m_MediaCuePalette = NULL;
			break;

		case TIME_PALETTE_CLOSE_MSG:
WATCH("TIME_PALETTE_CLOSE_MSG\n");
			m_TimePalette = NULL;
			break;

		case TRANSPORT_PALETTE_CLOSE_MSG:
			m_TransportPalette = NULL;
			break;		
		
		// Bring all windows to the front
		case ALL_FRONT_MSG:
WATCH("ALL_FRONT_MSG\n");
			m_CueSheetWindow->Activate(true);
			//m_ActionCuePalette->Activate(true);		
			m_MediaCuePalette->Activate(true);		
			m_TimePalette->Activate(true);		
			m_TransportPalette->Activate(true);
			break;	
			

		// Update the documents in the Windows menu
		case FIX_WINDOWS_MENU_MSG:
			UpdateWindowsMenuDocs();
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
		case FILE_PAGESETUP_MSG	:
		case FILE_PRINT_MSG	:						
		case FILE_QUIT_MSG:
WATCH("Some FILE MSG\n");
			HandleFileMenu(message);
			break;
			
		// Handle edit menu	
		case EDIT_UNDO_MSG:			
		case EDIT_CUT_MSG:				
		case EDIT_COPY_MSG:				
		case EDIT_PASTE_MSG:				
		case EDIT_CLEAR_MSG	:			
		case EDIT_SELECTALL_MSG:													
		case EDIT_INSERTTIME_MSG:
		case EDIT_ALIGNSTART_MSG:
		case EDIT_ALIGNEND_MSG:
WATCH("Some EDIT MSG\n");
			HandleEditMenu(message);
			break;					
																		
		// Cue Sheet Menu
		case CUE_SETUP_MSG:
		case CUE_TIMEBASE_MSG:
		case CUE_ZOOMIN_MSG:
		case CUE_ZOOMOUT_MSG:
		case CUE_SHOWDURATIONS_MSG:
		case CUE_SHOWSTOPTIMES_MSG:
		case CUE_INSERTCHANNELS_MSG:
		case CUE_DELETECHANNELS_MSG:			
		case CUE_INSERTTIME_MSG:
		case CUE_ALIGNSTART_MSG:
		case CUE_ALIGNEND_MSG:
		case CUE_COLLAPSEALL_MSG:
		case CUE_JUMPTO_MSG:
WATCH("Some CUE MSG\n");
			HandleCueSheetMenu(message);
			break;
			
		// Cue Menu
		case CUESHEET_GETINFO_MSG:
		case CUESHEET_DEFSETUP_MSG:
		case CUESHEET_SHOWCONTENTS_MSG:
		case CUESHEET_SHOWALLCONTENTS_MSG:
		case CUESHEET_HIDEALLCONTENTS_MSG:
		case CUESHEET_SETUP_MSG:
		case CUESHEET_EDIT_MSG:
		case CUESHEET_KEYFRAME_MSG:
WATCH("Some CUESHEET MSG\n");
			HandleCueMenu(message);
			break;

		// Capture Menu
		case CAPTURE_VIDCAP_MSG:
		case CAPTURE_VIDSETTNGS_MSG:
		case CAPTURE_AUDCAP_MSG:		
		case CAPTURE_AUDSETTNGS_MSG:
		case CAPTURE_PREFS_MSG:
		case CAPTURE_LEVELS_MSG:
WATCH("Some CAPTURE MSG\n");
			HandleCaptureMenu(message);
			break;
			
		// Stage Menu
		case STAGE_SETUP_MSG:
		case STAGE_PALETTE_MSG:
		case STAGE_FRONT_MSG:
		case STAGE_LUPDATE_MSG:
		case STAGE_GRID_MSG:
		case STAGE_GRIDSETUP_MSG:
		case STAGE_PICTURE_MSG:
WATCH("Some STAGE MSG\n");
			HandleStageMenu(message);
			break;
			
		// Windows Menu
		case WINDOWS_STAGE_MSG:
		case WINDOWS_BROWSER_MSG:
		case WINDOWS_TRANSPORT_MSG:
		case WINDOWS_COUNTER_MSG:
		case WINDOWS_MEDIA_MSG:
		case WINDOWS_CONTROL_MSG:
		case WINDOWS_MEMORY_MSG	:
		case WINDOWS_STAGE_TOOLS_MSG:
		case WINDOWS_DOCUMENT_MSG:
WATCH("Some WINDOWS control MSG\n");
			HandleWindowsMenu(message);
			break;
		
		default:
			BApplication::MessageReceived(message);						
			break;

	}
}



//-------------------------------------------------------------------
//	RefsReceived	
//-------------------------------------------------------------------
//
//	Called by either File Open or a File Drag message
//

void MuseumApp::RefsReceived(BMessage *message)
{	
	//	gzr: to do...
	//	Compartmentalize this so we can open multiple file types
	//
		
	//  All of this below is for opening cue sheets...
	
	// Extract file ref from message
	
	// ABH
	printf("MA:RR\n");
	status_t val;
	entry_ref fileRef;
	message->FindRef("refs", 0, &fileRef);

	// Resolve possible symlink...
	BEntry entry(&fileRef, true);
	val = entry.GetRef(&fileRef);
	if (val != B_OK){
		printf("MuseumApp::RefsReceived GetRef failed!\n");
	}
	

	// 	Check and see if file is already open.  If so, bring it to the front and exit
	//	without loading the file
	for (int32 index = 0; index < m_CueSheetList->CountItems(); index++)
	{	
		TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(index);
		
		if (theWindow){
			if ( strcmp( theWindow->Title(), fileRef.name) == 0 )
			{
				theWindow->Show();
				return;
			}
		}		
	}
	
	// Create file from ref
	BFile *openFile = new BFile(&fileRef, B_READ_ONLY);
	
	if (!openFile){
		ErrorAlert((char *)"Error Opening File.");
		return;
	}
	
	/*
	// Is this a valid file?  Read in the header and check...
	openFile->Seek(0, SEEK_SET);
	CueSheetHeaderChunk	fileHeader;
	openFile->Read(&fileHeader, sizeof(CueSheetHeaderChunk));
	
	// Determine file type byte order
	if ( fileHeader.header.chunkID == kCueSheetDocChunkMotorolaID ){
		if (B_HOST_IS_LENDIAN)
			swapBytes = true;
	}
	else if ( fileHeader.header.chunkID == kCueSheetDocChunkIntelID){
		if (B_HOST_IS_BENDIAN)
			swapBytes = true;
	}
	else {
		//ErrorAlert(kFILE_ERROR, kBAD_FILE_FORMAT);
		ErrorAlert("Bogus File");
		delete openFile;
		return;
	}
					
	// Swap the header
	if(swapBytes)
		SwapCueSheetHeaderChunk(&fileHeader);
		
	// Check for valid version
	if (fileHeader.version > kCurrentFileVersion) {
		//Failure(kFILE_ERROR, kCANT_OPEN_FILE);
		ErrorAlert("Bogus File");
		delete openFile;
		return;
	}
	*/		
	// Show progress dialog
	
	TCueSheetWindow *cueSheet = NULL;
	
	printf("MA:RR GetSize\n");
	// Load in archived BMessage
	off_t fileSize;
	openFile->GetSize(&fileSize);
	void *data = malloc(fileSize);				
	openFile->Read(data, fileSize);
	
	// Unflatten the archive
	BMessage *presetMessage = new BMessage();
	presetMessage->Unflatten((const char *)data);
	
	if (presetMessage) {
	 	BArchivable *unarchived = instantiate_object(presetMessage);
	 	if ( unarchived ) {
			// Add cue to our list of unarchived cues
			cueSheet = cast_as(unarchived, TCueSheetWindow); 
			if (!cueSheet){
				printf("MA::RR cast_as failed!\n");
				ASSERT(cueSheet);

			}
		} else {
			printf("MS::RR !unarchived\n");
		}				
	} else {
		printf("MA::RR presetMessage == null\n");
	}
	ASSERT(cueSheet);
	
	//	Clean up
	free(data);	
	delete presetMessage;

// ABH debug
	printf("MA:RR add to all doc window\n");
	
	// Add it to all documents Windows menu
	for (int32 index = 0; index < m_CueSheetList->CountItems(); index++) {	
		TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(index);
		
		if (theWindow) {
			TMuseumMenus *theMenu = theWindow->GetMenu();
			
			if (theMenu)
				theMenu->AddToWindowsMenu( cueSheet->Title(), cueSheet->GetID());
		} else {
			printf("MuseumApp:RefsReceived bad window!\n");
		}
		
	}
		
	// Hide current CueSheet
	m_CueSheetWindow->Hide();
	
	// Set it as the current window
	m_CueSheetWindow = cueSheet;
	
	// Show stage and cue sheet
	m_CueSheetWindow->Show();

}
		
#pragma mark -
#pragma mark === Dialog Routines ===

//-------------------------------------------------------------------
//	AddChannelDialog	
//-------------------------------------------------------------------
//
//	Display add channel dialog
//

void MuseumApp::AddChannelDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Insert Channel");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("InsertChannelView") != NULL)
			theWindow->Activate(true);	

	}
	else {
		// Create the channel name dialog
		BRect bounds( 200, 100, 450, 185);
		TAddChannel *theDialog = new TAddChannel(bounds, GetCueSheet()->GetCueSheetView());
		ASSERT(theDialog);

		// Center it
		CenterWindow(theDialog);
		
		// Show the dialog
		theDialog->Show();				
	}
}


//-------------------------------------------------------------------
//	DeleteChannelDialog	
//-------------------------------------------------------------------
//
//	Display delete channel dialog
//

void MuseumApp::DeleteChannelDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Delete Channel");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("DeleteChannelView") != NULL)
			theWindow->Activate(true);	
	}
	else {
		// Create the channel name dialog
		BRect bounds( 200, 100, 450, 185);
		TDeleteChannel *theDialog = new TDeleteChannel(bounds, GetCueSheet()->GetCueSheetView());
		ASSERT(theDialog);

		// Center it
		CenterWindow(theDialog);
		
		// Show the dialog
		theDialog->Show();				
	}
}


//-------------------------------------------------------------------
//	StageSetupDialog	
//-------------------------------------------------------------------
//
//	Display stage setup dialog
//

void MuseumApp::StageSetupDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Stage Setup");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("StageSetupView") != NULL)
			theWindow->Activate(true);	
	}
	else {
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("StageSetupWindow");
		TStageSetup *theDialog = new TStageSetup( m_CueSheetWindow->GetStage(), theMessage);
		ASSERT(theDialog);
		
		// Center it
		CenterWindow(theDialog);
		
		// Show the dialog
		theDialog->Show();	
	}
}

//-------------------------------------------------------------------
//	VideoCaptureDialog	
//-------------------------------------------------------------------
//
//	Display video capture dialog
//

void MuseumApp::VideoCaptureDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Capture Video");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("VidCapView") != NULL)
			theWindow->Activate(true);	
	}
	else {
		// Create the window
		TVideoCaptureWindow *theWindow = new TVideoCaptureWindow();
		ASSERT(theWindow);
		
		// Center it
		CenterWindow(theWindow);
		
		// Show the dialog
		theWindow->Show();	
	}
}


//-------------------------------------------------------------------
//	AudioCaptureDialog	
//-------------------------------------------------------------------
//
//	Display audio capture dialog
//

void MuseumApp::AudioCaptureDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Capture Audio");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("AudioCaptureView") != NULL)
			theWindow->Activate(true);
	}
	else {
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("AudioCaptureWindow");
		TAudioCaptureWindow *theWindow = new TAudioCaptureWindow(theMessage);
		ASSERT(theWindow);
		
		// Center it
		CenterWindow(theWindow);
		
		// Show the dialog
		theWindow->Show();			
	}
}


//-------------------------------------------------------------------
//	VideoSettingsDialog	
//-------------------------------------------------------------------
//
//	Display video settings dialog
//

void MuseumApp::VideoSettingsDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Video Settings");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("VideoSettingsTabView") != NULL)
			theWindow->Activate(true);	
	}
	else {		
		// Create the dialog
		BRect dialogRect(0, 0, 385, 250);		 
		TVideoSettingsDialog *theDialog = new TVideoSettingsDialog(dialogRect);
		ASSERT(theDialog);
		
		// Center it
		CenterWindow(theDialog);
		
		// Show the dialog
		theDialog->Show();	
	}
}


//-------------------------------------------------------------------
//	AudioSettingsDialog	
//-------------------------------------------------------------------
//
//	Display audio settings dialog
//

void MuseumApp::AudioSettingsDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Audio Settings");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("AudioSettingsTabView") != NULL)
			theWindow->Activate(true);	
	}
	else {		
		// Create the dialog
		BRect dialogRect(0, 0, 320, 250);		 
		TAudioSettingsDialog *theDialog = new TAudioSettingsDialog(dialogRect);
		ASSERT(theDialog);
		
		// Center it
		CenterWindow(theDialog);
		
		// Show the dialog
		theDialog->Show();	
	}
}


//-------------------------------------------------------------------
//	RecordPrefsDialog	
//-------------------------------------------------------------------
//
//	Display Record Prefernces dialog
//

void MuseumApp::RecordPrefsDialog()
{
	// See if the dialog is already open.  If so, bring it to front
	BWindow *theWindow = FindWindow("Record Preferences");
	
	if (theWindow) {
		// 	Double check and find it's main view. We do this to avoid conflicts
		// 	with a user window of the same name
		if ( theWindow->FindView("RecordPrefsView") != NULL)
			theWindow->Activate(true);
	}
	else {
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("RecordPrefsWindow");
		TRecordPrefsDialog *theWindow = new TRecordPrefsDialog(theMessage);
		ASSERT(theWindow);
		
		// Center it
		CenterWindow(theWindow);
		
		// Show the dialog
		theWindow->Show();			
	}
}


#pragma mark -
#pragma mark === Menu Handling ===

//-------------------------------------------------------------------
//	HandleFileMenu	
//-------------------------------------------------------------------
//
//	Handle file menu
//

void MuseumApp::HandleFileMenu(BMessage *message)
{
	switch(message->what)
	{	
		// Create new CueSheet
		case FILE_NEW_MSG:{
				// Display New File Progress Bar
				
				// Get screen dimensions
				BScreen theScreen(B_MAIN_SCREEN_ID);
				BRect bounds = theScreen.Frame();	
	
				// Create the Cue Sheet
				bounds.Set( bounds.left+125, bounds.top+50, bounds.right, bounds.bottom-200);
				TCueSheetWindow *cueSheet = new TCueSheetWindow( bounds, m_CueSheetList->CountItems()+1 );
				
				// Add it to all documents Windows menu
				for (int32 index = 0; index < m_CueSheetList->CountItems(); index++) {	
					TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(index);
					
					if (theWindow) {
						TMuseumMenus *theMenu = theWindow->GetMenu();
						
						if (theMenu)
							theMenu->AddToWindowsMenu( cueSheet->Title(), cueSheet->GetID());
					}					
				}
					
				// Hide current CueSheet
				m_CueSheetWindow->Hide();
				
				// Set it as the current window
				m_CueSheetWindow = cueSheet;
				
				// Hide New File Progress Bar
				
				// Show stage and cue sheet
				m_CueSheetWindow->Show();					
			}
			break;
					
		case FILE_OPEN_MSG:
			ShowFileOpenPanel();			
			break;
			
		// Close active CueSheetWindow
		case FILE_CLOSE_MSG:
			m_CueSheetWindow->PostMessage(B_QUIT_REQUESTED);
			break;
			
		// 	Only show panel is we have not done a previous save.  We can check this 
		//	by see if the Cue Sheets m_File has been initialized
		case FILE_SAVE_MSG:
			//if ( m_CueSheetWindow->GetFile() )
			//	m_CueSheetWindow->GetCueSheetView()->Save( m_CueSheetWindow->GetFile(), m_CueSheetWindow->Title());
			//else
				ShowFileSavePanel();				
			break;
			
		case FILE_SAVEAS_MSG:
			ShowFileSavePanel();
			break;
			
		case FILE_REVERT_MSG:
			break;
			
		case FILE_PAGESETUP_MSG	:
		case FILE_PRINT_MSG	:						
			break;
			
		case FILE_QUIT_MSG:
			be_app->PostMessage(B_QUIT_REQUESTED);
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

void MuseumApp::HandleEditMenu(BMessage *message)
{
	switch(message->what)
	{
		case EDIT_UNDO_MSG:	
			m_CueSheetWindow->Undo();
			break;
					
		case EDIT_CUT_MSG:	
			m_CueSheetWindow->Cut();			
			break;
			
		case EDIT_COPY_MSG:				
			m_CueSheetWindow->Copy();
			break;
			
		case EDIT_PASTE_MSG:
			m_CueSheetWindow->Paste();
			break;
							
		case EDIT_CLEAR_MSG	:	
			m_CueSheetWindow->Clear();
			break;
					
		case EDIT_SELECTALL_MSG:
			m_CueSheetWindow->SelectAll();
			break;
					
		default:
			break;	
	}
}


//-------------------------------------------------------------------
//	HandleCueSheetMenu	
//-------------------------------------------------------------------
//
//	Handle cue sheet menu
//

void MuseumApp::HandleCueSheetMenu(BMessage *message)
{
	switch(message->what)
	{
		case CUE_SETUP_MSG:
		case CUE_TIMEBASE_MSG:
		case CUE_ZOOMIN_MSG:
		case CUE_ZOOMOUT_MSG:
		case CUE_SHOWDURATIONS_MSG:
		case CUE_SHOWSTOPTIMES_MSG:
			m_CueSheetWindow->GetCueSheetView()->MessageReceived(message);
			break;
			
		case CUE_INSERTCHANNELS_MSG:
			AddChannelDialog();
			break;			
		
		case CUE_DELETECHANNELS_MSG:			
			DeleteChannelDialog();
			break;			
		
		case CUE_INSERTTIME_MSG:
			break;
			
		case CUE_ALIGNSTART_MSG:
			m_CueSheetWindow->GetCueSheetView()->AlignCueStartTimes();
			break;
			
		case CUE_ALIGNEND_MSG:
			m_CueSheetWindow->GetCueSheetView()->AlignCueEndTimes();
			break;
			
		case CUE_COLLAPSEALL_MSG:
		case CUE_JUMPTO_MSG:
			break;

		default:
			break;	
	}
}


//-------------------------------------------------------------------
//	HandleCueMenu	
//-------------------------------------------------------------------
//
//	Handle cue menu
//

void MuseumApp::HandleCueMenu(BMessage *message)
{
	switch(message->what)
	{
		case CUESHEET_GETINFO_MSG:
			break;
			
		case CUESHEET_DEFSETUP_MSG:
			break;
			
		case CUESHEET_SHOWCONTENTS_MSG:
			break;
		
		case CUESHEET_SHOWALLCONTENTS_MSG:
			break;
			
		case CUESHEET_HIDEALLCONTENTS_MSG:
			break;
			
		case CUESHEET_SETUP_MSG:
			break;
			
		case CUESHEET_EDIT_MSG:
			break;
		
		//	Add a keyframe.  Pass down to cue sheet
		case CUESHEET_KEYFRAME_MSG:
			m_CueSheetWindow->PostMessage(message, m_CueSheetWindow->GetCueSheetView());
			break;
				
		default:
			break;	
	}
}


//-------------------------------------------------------------------
//	HandleCaptureMenu	
//-------------------------------------------------------------------
//
//	Handle capture menu
//

void MuseumApp::HandleCaptureMenu(BMessage *message)
{
	switch(message->what)
	{
		case CAPTURE_VIDCAP_MSG:	
			VideoCaptureDialog();
			break;
		
		case CAPTURE_VIDSETTNGS_MSG:
			VideoSettingsDialog();
			break;
	
		case CAPTURE_AUDCAP_MSG:
			AudioCaptureDialog();
			break;
			
		case CAPTURE_AUDSETTNGS_MSG:
			AudioSettingsDialog();
			break;
						
		case CAPTURE_PREFS_MSG:
			RecordPrefsDialog();
			break;
			
		case CAPTURE_LEVELS_MSG:
			break;
			
		default:
			break;	
	}
}

		
//-------------------------------------------------------------------
//	HandleStageMenu	
//-------------------------------------------------------------------
//
//	Handle stage menu
//

void MuseumApp::HandleStageMenu(BMessage *message)
{
	switch(message->what)
	{
		case STAGE_SETUP_MSG:
			StageSetupDialog();
			break;
			
		case STAGE_PALETTE_MSG:
			break;
			
		// Bring stage to the front over the cue sheet
		case STAGE_FRONT_MSG:
			if (m_CueSheetWindow)
				m_CueSheetWindow->GetStage()->Activate(true);	
			break;
		
		case STAGE_LUPDATE_MSG:
			m_CueSheetWindow->GetCueSheetView()->SetLiveUpdate( !m_CueSheetWindow->GetCueSheetView()->GetLiveUpdate() );
			break;
						
		case STAGE_GRID_MSG:
		case STAGE_GRIDSETUP_MSG:
		case STAGE_PICTURE_MSG:
			break;
		
		default:
			break;	
	}
}


		
		
//-------------------------------------------------------------------
//	HandleWindowsMenu	
//-------------------------------------------------------------------
//
//	Handle windows menu
//

void MuseumApp::HandleWindowsMenu(BMessage *message)
{
	// Get screen for screen area rect location
	BScreen theScreen(B_MAIN_SCREEN_ID);					
						
	switch(message->what)
	{
		case WINDOWS_STAGE_MSG:
			break;
			
		case WINDOWS_BROWSER_MSG:
			if (m_BrowserWindow)
			{
				m_BrowserWindow->Lock();
				if (m_BrowserWindow->IsHidden())
					m_BrowserWindow->Show();
				else
					m_BrowserWindow->Hide();
				m_BrowserWindow->Unlock();			
			}
			break;
					
		case WINDOWS_TRANSPORT_MSG:
			{
				if (m_TransportPalette)
				{
					m_TransportPalette->Lock();
					if (m_TransportPalette->IsHidden())
						m_TransportPalette->Show();
					else
						m_TransportPalette->Hide();
					m_TransportPalette->Unlock();
				}
				else
				{
					BRect tranRect = theScreen.Frame(); 
					tranRect.bottom -= 10;
					tranRect.left += 10;
					tranRect.top = tranRect.bottom - kTransportHeight;
					tranRect.right = tranRect.left + kTransportWidth;
					m_TransportPalette = new TTransportPalette(tranRect);
				}
			}
			break;
			
		case WINDOWS_COUNTER_MSG:
			{
				if (m_TimePalette)
				{
					m_TimePalette->Lock();
					if (m_TimePalette->IsHidden())
						m_TimePalette->Show();
					else
						m_TimePalette->Hide();
					m_TimePalette->Unlock();
				}
				else
				{
					BRect timeRect = theScreen.Frame(); 
					timeRect.bottom -= 10;
					timeRect.right -= 10;
					timeRect.top = timeRect.bottom - kTimeHeight;
					timeRect.left = timeRect.right - kTimeWidth;
					m_TimePalette = new TTimePalette(timeRect);
				}
			}
			break;
			
		case WINDOWS_MEDIA_MSG:
			{
				if (m_MediaCuePalette)
				{
					m_MediaCuePalette->Lock();
					if (m_MediaCuePalette->IsHidden())
						m_MediaCuePalette->Show();
					else
						m_MediaCuePalette->Hide();
					m_MediaCuePalette->Unlock();
				}
				else
					m_MediaCuePalette = new TMediaCuePalette();
			}
			break;
			
		case WINDOWS_CONTROL_MSG:
			{
				//if (m_ActionCuePalette)
				//{
				//	if (m_ActionCuePalette->IsHidden())
				//		m_ActionCuePalette->Show();
				//	else
				//		m_ActionCuePalette->Hide();
				//}
				//else
				//	m_ActionCuePalette = new TActionCuePalette();
			}
			break;
			
		case WINDOWS_MEMORY_MSG	:
			break;
			
		case WINDOWS_STAGE_TOOLS_MSG:
			{
				if (m_StageTools)
				{
					m_StageTools->Lock();
					if (m_StageTools->IsHidden())
						m_StageTools->Show();
					else
						m_StageTools->Hide();
					m_StageTools->Unlock();
				}
				else
					m_StageTools = new TStageTools();
			}
			break;
			
			
		// User is bringing a cue sheet to the front
		case WINDOWS_DOCUMENT_MSG:
			{			
				// Extract cue sheet id from message
				int32 theID;
				status_t retVal = message->FindInt32("CueSheetID", &theID);
				
				if (retVal == B_NO_ERROR)
				{
					// Bring window to front
					for(int32 index = 0; index < m_CueSheetList->CountItems(); index++)
					{
						TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(index);
						
						if (theWindow)
						{
							if (theWindow->GetID() == theID)
							{
								// Do nothing if this window is already m_CueSheetWindow
								if (theWindow == m_CueSheetWindow)
									return;
									
								// Hide current window								
								m_CueSheetWindow->Hide();
																
								// Show the new window
								theWindow->Show();
																								
								return;
							}
						}					
					}
				}
			}
			break;
		
		default:
			break;	
	}
}


//---------------------------------------------------------------------
//	UpdateWindowsMenuDocs
//---------------------------------------------------------------------
//
//	Rebuild Windows menu of all cue sheets to reflect all open documents
//

void MuseumApp::UpdateWindowsMenuDocs()
{		
	// Go through all open cue sheets
	for(int32 index = 0; index < m_CueSheetList->CountItems(); index++)
	{
		TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(index);
		
		if (theWindow)
		{
			// Get windows menu
			TMuseumMenus *theMenu = theWindow->GetMenu();
			if(theMenu)
			{
				// First, strip out all documents from menu
				theMenu->RemoveDocumentsInWindowsMenu();
				
				// Now, rebuild with open documents
				theMenu->AddToWindowsMenu(theWindow->Title(), theWindow->GetID());
			}
			
			// Update CueSheet ID
			theWindow->SetID(index+1);
		}					
	}
	
	// Now we need to set the first window in the list to the front
	TCueSheetWindow *theWindow = (TCueSheetWindow *)m_CueSheetList->ItemAt(0);
	theWindow->Show();
}

#pragma mark -
#pragma mark === File IO Routines ===

//---------------------------------------------------------------------
//	ShowFileOpenPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void MuseumApp::ShowFileOpenPanel()
{ 		
	TRefFilter *refFilter;
	
	// If the panel has already been constructed, show the panel
	// Otherwise, create the panel	
	if (m_FileOpenPanel)
		m_FileOpenPanel->Show();
	else
	{
		// Create a RefFilter for a "video" type
		refFilter = new TRefFilter(kCueSheetFilter);

		// Construct a file panel and set it to modal
	 	m_FileOpenPanel = new BFilePanel( B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false, NULL, refFilter, true, true );
	 
	 	// Set it to application's home directory
	 	app_info appInfo;
	 	be_app->GetAppInfo(&appInfo); 	
	 	BEntry entry(&appInfo.ref);
	 	BDirectory parentDir;
	 	entry.GetParent(&parentDir);
	 	m_FileOpenPanel->SetPanelDirectory(&parentDir);
	 		
		// Center Panel
		CenterWindow(m_FileOpenPanel->Window());
	}
	
	m_FileOpenPanel->Show();	
}


//---------------------------------------------------------------------
//	ShowFileSavePanel
//---------------------------------------------------------------------
//
//	Show file save panel
//

void MuseumApp::ShowFileSavePanel()
{ 		
	// 	Create messenger to send panel messages to CueSheetWindow with focus
 	BMessenger messenger( m_CueSheetWindow,  NULL);
 		 			
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


#pragma mark -
#pragma mark === Add-on Routines ===

//---------------------------------------------------------------------
//	LoadCueAddOns
//---------------------------------------------------------------------
//
//	Load cue add-ons in the /add-ons/cues directory
//

void MuseumApp::LoadCueAddOns()
{
	// Get listing of add-ons in directory
	BDirectory cueDirectory("add-ons/cues/");
	
	status_t 	myErr;
	BList		*cueList = new BList();
   
	if (cueDirectory.InitCheck() == B_OK) 
	{ 
		int32 entries = cueDirectory.CountEntries();
		
		for( int32 index = 0; index < entries; index++)
		{
			BEntry *theEntry = new BEntry();
			myErr = cueDirectory.GetNextEntry(theEntry, true);
			
			// Add it to our list if we are successful
			if (myErr != B_ENTRY_NOT_FOUND)
			{
				cueList->AddItem(theEntry);
			}
			else
				delete theEntry;
		}
	}	
	
	// Validate that the items in the list are cue add-ons
	BEntry *addOnImage = (BEntry *)cueList->ItemAt(0);
	
	if (addOnImage)
	{
		BPath thePath;
		addOnImage->GetPath(&thePath);
		image_id id = load_add_on(thePath.Path());
		
		if (id != B_ERROR)
		{
			//	Create the constructor string based on the add-on name
			
			//image_info info;
			//get_image_info(id, &info);
			
			//  Load contructor from image					
			//TCueView (*func_constuct)(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime, char *name);
			//get_image_symbol(id,  , B_SYMBOL_TYPE_TEXT, &func_constuct);
			
			//myErr = get_image_symbol(id, "__ct__11TPictureCueFsP11TCueChannel5BRect6BPointl", B_SYMBOL_TYPE_TEXT, &func_constuct);			
		}
	}
		
	// Clean up
	delete cueList;
}


#pragma mark -
#pragma mark === Cursor Routines ===

//---------------------------------------------------------------------
//	LoadCursors
//---------------------------------------------------------------------
//
//	Load application cursors
//

void MuseumApp::LoadCursors()
{
	bool retVal;
	
	retVal = GetCursorFromResource("HandOpen", &m_HandOpenCursor);
	retVal = GetCursorFromResource("HandClosed", &m_HandClosedCursor);
	retVal = GetCursorFromResource("Crop", &m_CropCursor);
	retVal = GetCursorFromResource("Eyedropper", &m_EyedropperCursor);
	retVal = GetCursorFromResource("Move", &m_MoveCursor);
	retVal = GetCursorFromResource("Pencil", &m_PencilCursor);
	retVal = GetCursorFromResource("ResizeDiagLeft", &m_ResizeDiagLeftCursor);
	retVal = GetCursorFromResource("ResizeDiagRight", &m_ResizeDiagRightCursor);
	retVal = GetCursorFromResource("ResizeHorz", &m_ResizeHorzCursor);
	retVal = GetCursorFromResource("ResizeVert", &m_ResizeVertCursor);
	retVal = GetCursorFromResource("SizeHorz", &m_SizeHorzCursor);
	retVal = GetCursorFromResource("SizeVert", &m_SizeVertCursor);
	retVal = GetCursorFromResource("ZoomIn", &m_ZoomInCursor);
	retVal = GetCursorFromResource("ZoomOut", &m_ZoomOutCursor);
	retVal = GetCursorFromResource("Rotate", &m_RotateCursor);
	retVal = GetCursorFromResource("SizeLeftRight", &m_SizeLeftRightCursor);
	retVal = GetCursorFromResource("SizeTopBottom", &m_SizeTopBottomCursor);
	retVal = GetCursorFromResource("SizeTopLeftBottom", &m_SizeTopLeftBottomCursor);
	retVal = GetCursorFromResource("SizeTopRightBottom", &m_SizeTopRightBottomCursor);	
}


//---------------------------------------------------------------------
//	FreeCursors
//---------------------------------------------------------------------
//
//	Free application cursors
//

void MuseumApp::FreeCursors()
{

}

