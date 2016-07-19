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
//const kTransportHeight        = kTransportButtonHeight * 2;
//const kTransportWidth         = (kTransportButtonWidth * 5) + 2;

const int32 kTransportWidth             = 182;
const int32 kTransportHeight    = 67;

const int32 kTimeWidth                  = 196;
const int32 kTimeHeight                 = 100;


//-------------------------------------------------------------------
//
//	Function:	main
//
//	Desc:
//
//-------------------------------------------------------------------
//
//

int main()
{
	BApplication *theApp = new MuseumApp();
	theApp->Run();
	delete(theApp);

	return 0;
}

//-------------------------------------------------------------------
//	MuseumApp
//-------------------------------------------------------------------
//
//	Default Constructor
//

MuseumApp::MuseumApp() : BApplication("application/x-mediapede-Museum")
{
	// Leak checking
// ABH
//#ifdef DEBUG
//SetNewLeakChecking(true);
//SetMallocLeakChecking(true);
//#endif
	WATCH("In constructor\n");

	// Init palettes to NULL
	fCueSheetWindow        = NULL;
	fActionCuePalette      = NULL;
	fMediaCuePalette       = NULL;
	fTimePalette           = NULL;
	fTransportPalette      = NULL;
	fCueSheetList          = NULL;
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
	if (fFileOpenPanel) delete fFileOpenPanel;

	if (fFileSavePanel) delete fFileSavePanel;

	if (fClipboard) delete fClipboard;

	// Quit all palettes
	//if (fActionCuePalette)
	//{
	//	fActionCuePalette->Lock();
	//	fActionCuePalette->Quit();
	//}

	if (fMediaCuePalette) {
		fMediaCuePalette->Lock();
		fMediaCuePalette->Quit();
	}

	if (fBrowserWindow) {
		fBrowserWindow->Lock();
		fBrowserWindow->Quit();
	}

	if (fTimePalette) {
		fTimePalette->Lock();
		fTimePalette->Quit();
	}

	if (fTransportPalette) {
		fTransportPalette->Lock();
		fTransportPalette->Quit();
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
	fIsCueDrag     = false;
	fFileOpenPanel = NULL;
	fFileSavePanel = NULL;

	WATCH("In ReadyToRun\n");

	// Display the About box
	TAboutBox *aboutWindow = new TAboutBox();
	CenterWindow(aboutWindow);
	aboutWindow->Show();

	//	Connect to media_server
	fMediaRoster = BMediaRoster::Roster();

	if (!fMediaRoster) {
		//	The Media Server appears to be dead -- handle that here
		ErrorAlert((char *)"Unable to connect to media_server.");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	// Create application clipboard
	fClipboard = new TMuseumClipboard();

	// Init Translation Kit services
	fTranslationRoster = new BTranslatorRoster();

	// Load our custom translators
	//roster->AddTranslators("/boot/home/config/add-ons/viewer/");

	// Load default translators
	fTranslationRoster->AddTranslators(NULL);

	// Load cursors
	LoadCursors();

	// Load app icons
	fMuseumIcons = new TMuseumIcons();

	// Load preference
	fPrefs = new TMuseumPrefs();

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
	fMediaCuePalette = new TMediaCuePalette();
	WATCH("In MuseumApp::Init\n");

	// Create transport
	BRect tranRect = theScreen.Frame();
	tranRect.bottom -= 10;
	tranRect.left += 10;
	tranRect.top = tranRect.bottom - kTransportHeight;
	tranRect.right = tranRect.left + kTransportWidth;
	fTransportPalette = new TTransportPalette(tranRect);


	// Create time palette
	BRect timeRect = theScreen.Frame();
	timeRect.bottom -= 10;
	timeRect.right -= 10;
	timeRect.top = timeRect.bottom - kTimeHeight;
	timeRect.left = timeRect.right - kTimeWidth;
	fTimePalette = new TTimePalette(timeRect);


	// Create Browser Window
	BRect browserRect       = theScreen.Frame();
	browserRect.top         += 50;
	browserRect.bottom      = browserRect.top + 300;
	browserRect.right       = browserRect.left + 200;
	fBrowserWindow         = new TBrowserWindow(browserRect);


	//	Create Stage Tools
	fStageTools    = new TStageTools();

	// Create cue sheet list and add current cue sheet to it
	fCueSheetList = new BList();


	// Create the Cue Sheet
	bounds.Set(bounds.left + 125, bounds.top + 50, bounds.right, bounds.bottom - 200);
	fCueSheetWindow = new TCueSheetWindow(bounds, fCueSheetList->CountItems() + 1);


	// Add Cue Sheet to Windows menu
	TMuseumMenus *theMenu = fCueSheetWindow->GetMenu();
	theMenu->AddToWindowsMenu(fCueSheetWindow->Title(), fCueSheetWindow->GetID());


	// Arrange the windows into their proper locations
	ArrangeWindows();

	// Show them all now...
	fMediaCuePalette->Show();
	fTimePalette->Show();
	fTransportPalette->Show();
	fBrowserWindow->Show();
	fStageTools->Show();
	fCueSheetWindow->GetStage()->Show();
	fCueSheetWindow->Show();

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
	fBrowserWindow->MoveTo(bounds.left, bounds.top + 40);

	// The cue sheet goes at the bottom left and stretches to the far right
	fCueSheetWindow->ResizeTo(bounds.Width() - 20, bounds.Height() - fBrowserWindow->Frame().Height() - 50);
	fCueSheetWindow->MoveTo(bounds.left + 10, fBrowserWindow->Frame().bottom);

	// Locator goes at the top right
	fTimePalette->MoveTo(bounds.right - fTimePalette->Bounds().Width(), bounds.top + 40);

	// Transport goes at the right below Locator
	fTransportPalette->MoveTo(fTimePalette->Frame().left, fTimePalette->Frame().bottom + 25);

	// Stage is to the left of the Locator
	TStageWindow *theStage = fCueSheetWindow->GetStage();
	theStage->MoveTo( (fTimePalette->Frame().left - theStage->Bounds().Width()) - 8, bounds.top + 40);

	// Stage Tools go to the left of the stage
	fStageTools->MoveTo((theStage->Frame().left - fStageTools->Bounds().Width()) - 8, bounds.top + 40);

	// Cues go to the left of the Stage Tools
	fMediaCuePalette->MoveTo((fStageTools->Frame().left - fMediaCuePalette->Bounds().Width()) - 8, bounds.top + 40);

	// Now resize browser to move flush against the cues left side
	fBrowserWindow->ResizeTo(fMediaCuePalette->Frame().left - 8,  fBrowserWindow->Frame().Height() - 25);
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

	// If fCueSheetList is NULL, exit
	if (!fCueSheetList) return true;

	// Close all windows in fCueSheetList.
	while (fCueSheetList->CountItems() > 0) {
		TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(0);

		// See if we need to save the cue sheet
		if (theWindow) {
			// Set this window as the current window
			if (theWindow != fCueSheetWindow) fCueSheetWindow = theWindow;

			// Bring it to the fornt
			fCueSheetWindow->Show();

			// Ask it to quit using our rotuine
			bool retVal = fCueSheetWindow->MyQuitRequested();

			// User is quiting
			if (retVal) {
				// Hide stage
				if (fCueSheetWindow->GetStage()) fCueSheetWindow->GetStage()->Hide();

				// Hide window
				fCueSheetWindow->Lock();
				fCueSheetWindow->Hide();

				// Destroy cue sheet window
				fCueSheetWindow->Quit();
				fCueSheetWindow = NULL;
			}
			// User has changed mind about quiting
			else return false;
		}
	}

	// Hide all windows before quiting
	//if (fActionCuePalette)
	//	fActionCuePalette->Hide();

	if (fMediaCuePalette) fMediaCuePalette->Hide();

	if (fTimePalette) fTimePalette->Hide();

	if (fTransportPalette) fTransportPalette->Hide();

	// Free cue sheet list
	delete fCueSheetList;

	// Close Translation Kit
	delete fTranslationRoster;

	// Free icons
	delete fMuseumIcons;

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

void MuseumApp::MessageReceived(BMessage *message)
{
	WATCH("In MuseumApp::MessageReceived\n");

	switch (message->what) {
	// Dialogs
	case NEW_PROJECT_MSG: {
		WATCH("NEW_PROJECT_MSG\n");
		// Retrieve setting from message
		BMessage archiveMessage;

		if (!message->FindMessage("Preset", &archiveMessage)) {
			BArchivable *unarchived = instantiate_object(&archiveMessage);

			if (unarchived) {
				//TPreset *preset = cast_as(unarchived, TPreset);
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
		fActionCuePalette = NULL;
		break;

	case MEDIA_PALETTE_CLOSE_MSG:
		WATCH("MEDIA_PALETTE_CLOSE_MSG\n");
		fMediaCuePalette = NULL;
		break;

	case TIME_PALETTE_CLOSE_MSG:
		WATCH("TIME_PALETTE_CLOSE_MSG\n");
		fTimePalette = NULL;
		break;

	case TRANSPORT_PALETTE_CLOSE_MSG:
		fTransportPalette = NULL;
		break;

	// Bring all windows to the front
	case ALL_FRONT_MSG:
		WATCH("ALL_FRONT_MSG\n");
		fCueSheetWindow->Activate(true);
		//fActionCuePalette->Activate(true);
		fMediaCuePalette->Activate(true);
		fTimePalette->Activate(true);
		fTransportPalette->Activate(true);
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
	case FILE_CLOSE_MSG:
	case FILE_SAVE_MSG:
	case FILE_SAVEAS_MSG:
	case FILE_REVERT_MSG:
	case FILE_PAGESETUP_MSG:
	case FILE_PRINT_MSG:
	case FILE_QUIT_MSG:
		WATCH("Some FILE MSG\n");
		HandleFileMenu(message);
		break;

	// Handle edit menu
	case EDIT_UNDO_MSG:
	case EDIT_CUT_MSG:
	case EDIT_COPY_MSG:
	case EDIT_PASTE_MSG:
	case EDIT_CLEAR_MSG:
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
	case WINDOWS_MEMORY_MSG:
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

	if (val != B_OK) {
		printf("MuseumApp::RefsReceived GetRef failed!\n");
	}

	//      Check and see if file is already open.  If so, bring it to the front and exit
	//	without loading the file
	for (int32 index = 0; index < fCueSheetList->CountItems(); index++) {
		TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(index);

		if (theWindow) {
			if (strcmp(theWindow->Title(), fileRef.name) == 0) {
				theWindow->Show();
				return;
			}
		}
	}

	// Create file from ref
	BFile *openFile = new BFile(&fileRef, B_READ_ONLY);

	if (!openFile) {
		ErrorAlert((char *)"Error Opening File.");
		return;
	}

	/*
	 * // Is this a valid file?  Read in the header and check...
	 * openFile->Seek(0, SEEK_SET);
	 * CueSheetHeaderChunk	fileHeader;
	 * openFile->Read(&fileHeader, sizeof(CueSheetHeaderChunk));
	 *
	 * // Determine file type byte order
	 * if ( fileHeader.header.chunkID == kCueSheetDocChunkMotorolaID ){
	 *      if (B_HOST_IS_LENDIAN)
	 *              swapBytes = true;
	 * }
	 * else if ( fileHeader.header.chunkID == kCueSheetDocChunkIntelID){
	 *      if (B_HOST_IS_BENDIAN)
	 *              swapBytes = true;
	 * }
	 * else {
	 *      //ErrorAlert(kFILE_ERROR, kBAD_FILE_FORMAT);
	 *      ErrorAlert("Bogus File");
	 *      delete openFile;
	 *      return;
	 * }
	 *
	 * // Swap the header
	 * if(swapBytes)
	 *      SwapCueSheetHeaderChunk(&fileHeader);
	 *
	 * // Check for valid version
	 * if (fileHeader.version > kCurrentFileVersion) {
	 *      //Failure(kFILE_ERROR, kCANT_OPEN_FILE);
	 *      ErrorAlert("Bogus File");
	 *      delete openFile;
	 *      return;
	 * }
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

		if (unarchived) {
			// Add cue to our list of unarchived cues
			cueSheet = cast_as(unarchived, TCueSheetWindow);

			if (!cueSheet) {
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
	for (int32 index = 0; index < fCueSheetList->CountItems(); index++) {
		TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(index);

		if (theWindow) {
			TMuseumMenus *theMenu = theWindow->GetMenu();

			if (theMenu) theMenu->AddToWindowsMenu(cueSheet->Title(), cueSheet->GetID());
		} else {
			printf("MuseumApp:RefsReceived bad window!\n");
		}
	}

	// Hide current CueSheet
	fCueSheetWindow->Hide();

	// Set it as the current window
	fCueSheetWindow = cueSheet;

	// Show stage and cue sheet
	fCueSheetWindow->Show();
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("InsertChannelView") != NULL) theWindow->Activate(true);
	} else {
		// Create the channel name dialog
		BRect bounds(200, 100, 450, 185);
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("DeleteChannelView") != NULL) theWindow->Activate(true);
	} else {
		// Create the channel name dialog
		BRect bounds(200, 100, 450, 185);
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("StageSetupView") != NULL) theWindow->Activate(true);
	} else {
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("StageSetupWindow");
		TStageSetup *theDialog = new TStageSetup(fCueSheetWindow->GetStage(), theMessage);
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("VidCapView") != NULL) theWindow->Activate(true);
	} else {
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("AudioCaptureView") != NULL) theWindow->Activate(true);
	} else {
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("VideoSettingsTabView") != NULL) theWindow->Activate(true);
	} else {
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("AudioSettingsTabView") != NULL) theWindow->Activate(true);
	} else {
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
		//      Double check and find it's main view. We do this to avoid conflicts
		//      with a user window of the same name
		if (theWindow->FindView("RecordPrefsView") != NULL) theWindow->Activate(true);
	} else {
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
	switch (message->what) {
	// Create new CueSheet
	case FILE_NEW_MSG: {
		// Display New File Progress Bar

		// Get screen dimensions
		BScreen theScreen(B_MAIN_SCREEN_ID);
		BRect bounds = theScreen.Frame();

		// Create the Cue Sheet
		bounds.Set(bounds.left + 125, bounds.top + 50, bounds.right, bounds.bottom - 200);
		TCueSheetWindow *cueSheet = new TCueSheetWindow(bounds, fCueSheetList->CountItems() + 1);

		// Add it to all documents Windows menu
		for (int32 index = 0; index < fCueSheetList->CountItems(); index++) {
			TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(index);

			if (theWindow) {
				TMuseumMenus *theMenu = theWindow->GetMenu();

				if (theMenu) theMenu->AddToWindowsMenu(cueSheet->Title(), cueSheet->GetID());
			}
		}

		// Hide current CueSheet
		fCueSheetWindow->Hide();

		// Set it as the current window
		fCueSheetWindow = cueSheet;

		// Hide New File Progress Bar

		// Show stage and cue sheet
		fCueSheetWindow->Show();
	}
	break;

	case FILE_OPEN_MSG:
		ShowFileOpenPanel();
		break;

	// Close active CueSheetWindow
	case FILE_CLOSE_MSG:
		fCueSheetWindow->PostMessage(B_QUIT_REQUESTED);
		break;

	//      Only show panel is we have not done a previous save.  We can check this
	//	by see if the Cue Sheets fFile has been initialized
	case FILE_SAVE_MSG:
		//if ( fCueSheetWindow->GetFile() )
		//	fCueSheetWindow->GetCueSheetView()->Save( fCueSheetWindow->GetFile(), fCueSheetWindow->Title());
		//else
		ShowFileSavePanel();
		break;

	case FILE_SAVEAS_MSG:
		ShowFileSavePanel();
		break;

	case FILE_REVERT_MSG:
		break;

	case FILE_PAGESETUP_MSG:
	case FILE_PRINT_MSG:
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
	switch (message->what) {
	case EDIT_UNDO_MSG:
		fCueSheetWindow->Undo();
		break;

	case EDIT_CUT_MSG:
		fCueSheetWindow->Cut();
		break;

	case EDIT_COPY_MSG:
		fCueSheetWindow->Copy();
		break;

	case EDIT_PASTE_MSG:
		fCueSheetWindow->Paste();
		break;

	case EDIT_CLEAR_MSG:
		fCueSheetWindow->Clear();
		break;

	case EDIT_SELECTALL_MSG:
		fCueSheetWindow->SelectAll();
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
	switch (message->what) {
	case CUE_SETUP_MSG:
	case CUE_TIMEBASE_MSG:
	case CUE_ZOOMIN_MSG:
	case CUE_ZOOMOUT_MSG:
	case CUE_SHOWDURATIONS_MSG:
	case CUE_SHOWSTOPTIMES_MSG:
		fCueSheetWindow->GetCueSheetView()->MessageReceived(message);
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
		fCueSheetWindow->GetCueSheetView()->AlignCueStartTimes();
		break;

	case CUE_ALIGNEND_MSG:
		fCueSheetWindow->GetCueSheetView()->AlignCueEndTimes();
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
	switch (message->what) {
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
		fCueSheetWindow->PostMessage(message, fCueSheetWindow->GetCueSheetView());
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
	switch (message->what) {
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
	switch (message->what) {
	case STAGE_SETUP_MSG:
		StageSetupDialog();
		break;

	case STAGE_PALETTE_MSG:
		break;

	// Bring stage to the front over the cue sheet
	case STAGE_FRONT_MSG:

		if (fCueSheetWindow) fCueSheetWindow->GetStage()->Activate(true);

		break;

	case STAGE_LUPDATE_MSG:
		fCueSheetWindow->GetCueSheetView()->SetLiveUpdate(!fCueSheetWindow->GetCueSheetView()->GetLiveUpdate() );
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

	switch (message->what) {
	case WINDOWS_STAGE_MSG:
		break;

	case WINDOWS_BROWSER_MSG:

		if (fBrowserWindow) {
			fBrowserWindow->Lock();

			if (fBrowserWindow->IsHidden()) fBrowserWindow->Show();
			else fBrowserWindow->Hide();

			fBrowserWindow->Unlock();
		}

		break;

	case WINDOWS_TRANSPORT_MSG: {
		if (fTransportPalette) {
			fTransportPalette->Lock();

			if (fTransportPalette->IsHidden()) fTransportPalette->Show();
			else fTransportPalette->Hide();

			fTransportPalette->Unlock();
		} else {
			BRect tranRect = theScreen.Frame();
			tranRect.bottom -= 10;
			tranRect.left += 10;
			tranRect.top = tranRect.bottom - kTransportHeight;
			tranRect.right = tranRect.left + kTransportWidth;
			fTransportPalette = new TTransportPalette(tranRect);
		}
	}
	break;

	case WINDOWS_COUNTER_MSG: {
		if (fTimePalette) {
			fTimePalette->Lock();

			if (fTimePalette->IsHidden()) fTimePalette->Show();
			else fTimePalette->Hide();

			fTimePalette->Unlock();
		} else {
			BRect timeRect = theScreen.Frame();
			timeRect.bottom -= 10;
			timeRect.right -= 10;
			timeRect.top = timeRect.bottom - kTimeHeight;
			timeRect.left = timeRect.right - kTimeWidth;
			fTimePalette = new TTimePalette(timeRect);
		}
	}
	break;

	case WINDOWS_MEDIA_MSG: {
		if (fMediaCuePalette) {
			fMediaCuePalette->Lock();

			if (fMediaCuePalette->IsHidden()) fMediaCuePalette->Show();
			else fMediaCuePalette->Hide();

			fMediaCuePalette->Unlock();
		} else fMediaCuePalette = new TMediaCuePalette();
	}
	break;

	case WINDOWS_CONTROL_MSG: {
		//if (fActionCuePalette)
		//{
		//	if (fActionCuePalette->IsHidden())
		//		fActionCuePalette->Show();
		//	else
		//		fActionCuePalette->Hide();
		//}
		//else
		//	fActionCuePalette = new TActionCuePalette();
	}
	break;

	case WINDOWS_MEMORY_MSG:
		break;

	case WINDOWS_STAGE_TOOLS_MSG: {
		if (fStageTools) {
			fStageTools->Lock();

			if (fStageTools->IsHidden()) fStageTools->Show();
			else fStageTools->Hide();

			fStageTools->Unlock();
		} else fStageTools = new TStageTools();
	}
	break;


	// User is bringing a cue sheet to the front
	case WINDOWS_DOCUMENT_MSG: {
		// Extract cue sheet id from message
		int32 theID;
		status_t retVal = message->FindInt32("CueSheetID", &theID);

		if (retVal == B_NO_ERROR) {
			// Bring window to front
			for (int32 index = 0; index < fCueSheetList->CountItems(); index++) {
				TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(index);

				if (theWindow) {
					if (theWindow->GetID() == theID) {
						// Do nothing if this window is already fCueSheetWindow
						if (theWindow == fCueSheetWindow) return;

						// Hide current window
						fCueSheetWindow->Hide();

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
	for (int32 index = 0; index < fCueSheetList->CountItems(); index++) {
		TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(index);

		if (theWindow) {
			// Get windows menu
			TMuseumMenus *theMenu = theWindow->GetMenu();

			if (theMenu) {
				// First, strip out all documents from menu
				theMenu->RemoveDocumentsInWindowsMenu();

				// Now, rebuild with open documents
				theMenu->AddToWindowsMenu(theWindow->Title(), theWindow->GetID());
			}

			// Update CueSheet ID
			theWindow->SetID(index + 1);
		}
	}

	// Now we need to set the first window in the list to the front
	TCueSheetWindow *theWindow = (TCueSheetWindow *)fCueSheetList->ItemAt(0);
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
	if (fFileOpenPanel) fFileOpenPanel->Show();
	else {
		// Create a RefFilter for a "video" type
		refFilter = new TRefFilter(kCueSheetFilter);

		// Construct a file panel and set it to modal
		fFileOpenPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false, NULL, refFilter, true, true);

		// Set it to application's home directory
		app_info appInfo;
		be_app->GetAppInfo(&appInfo);
		BEntry entry(&appInfo.ref);
		BDirectory parentDir;
		entry.GetParent(&parentDir);
		fFileOpenPanel->SetPanelDirectory(&parentDir);

		// Center Panel
		CenterWindow(fFileOpenPanel->Window());
	}

	fFileOpenPanel->Show();
}

//---------------------------------------------------------------------
//	ShowFileSavePanel
//---------------------------------------------------------------------
//
//	Show file save panel
//

void MuseumApp::ShowFileSavePanel()
{
	//      Create messenger to send panel messages to CueSheetWindow with focus
	BMessenger messenger(fCueSheetWindow,  NULL);

	//      If the panel has already been constructed, show the panel
	//      Otherwise, create the panel.  We do need to reset it's messenger to
	//	point at the correct window
	if (fFileSavePanel) {
		fFileSavePanel->SetTarget(messenger);
		fFileSavePanel->Show();
	} else {
		// Construct a file panel and set it to modal
		fFileSavePanel = new BFilePanel(B_SAVE_PANEL, &messenger, NULL, B_FILE_NODE, false, NULL, NULL, true, true);

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

	status_t myErr;
	BList *cueList = new BList();

	if (cueDirectory.InitCheck() == B_OK) {
		int32 entries = cueDirectory.CountEntries();

		for (int32 index = 0; index < entries; index++) {
			BEntry *theEntry = new BEntry();
			myErr = cueDirectory.GetNextEntry(theEntry, true);

			// Add it to our list if we are successful
			if (myErr != B_ENTRY_NOT_FOUND) {
				cueList->AddItem(theEntry);
			} else delete theEntry;
		}
	}

	// Validate that the items in the list are cue add-ons
	BEntry *addOnImage = (BEntry *)cueList->ItemAt(0);

	if (addOnImage) {
		BPath thePath;
		addOnImage->GetPath(&thePath);
		image_id id = load_add_on(thePath.Path());

		if (id != B_ERROR) {
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

	retVal = GetCursorFromResource("HandOpen", &fHandOpenCursor);
	retVal = GetCursorFromResource("HandClosed", &fHandClosedCursor);
	retVal = GetCursorFromResource("Crop", &fCropCursor);
	retVal = GetCursorFromResource("Eyedropper", &fEyedropperCursor);
	retVal = GetCursorFromResource("Move", &fMoveCursor);
	retVal = GetCursorFromResource("Pencil", &fPencilCursor);
	retVal = GetCursorFromResource("ResizeDiagLeft", &fResizeDiagLeftCursor);
	retVal = GetCursorFromResource("ResizeDiagRight", &fResizeDiagRightCursor);
	retVal = GetCursorFromResource("ResizeHorz", &fResizeHorzCursor);
	retVal = GetCursorFromResource("ResizeVert", &fResizeVertCursor);
	retVal = GetCursorFromResource("SizeHorz", &fSizeHorzCursor);
	retVal = GetCursorFromResource("SizeVert", &fSizeVertCursor);
	retVal = GetCursorFromResource("ZoomIn", &fZoomInCursor);
	retVal = GetCursorFromResource("ZoomOut", &fZoomOutCursor);
	retVal = GetCursorFromResource("Rotate", &fRotateCursor);
	retVal = GetCursorFromResource("SizeLeftRight", &fSizeLeftRightCursor);
	retVal = GetCursorFromResource("SizeTopBottom", &fSizeTopBottomCursor);
	retVal = GetCursorFromResource("SizeTopLeftBottom", &fSizeTopLeftBottomCursor);
	retVal = GetCursorFromResource("SizeTopRightBottom", &fSizeTopRightBottomCursor);
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
