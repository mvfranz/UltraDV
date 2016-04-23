//---------------------------------------------------------------------
//
//	File:	TMuseumMenus.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	Menu creation and handling routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <stdio.h>

#include <stdio.h>

#include "MuseumApp.h"
#include "ResourceManager.h"
#include "TCueSheetWindow.h"

#include "TMuseumMenus.h"

// Constants

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//


TMuseumMenus::TMuseumMenus(BMenuBar* mbar)
{
		
	// Save the BMenuBar into our member variable
	m_MenuBar = mbar;
	
	//
	// Add our individual menu items
	//
		
	//	File menu
	AddFileMenu();
		
	// 	Edit menu
	AddEditMenu();
		
	// 	Cue Sheet menu
	AddCueSheetMenu();
	
	// 	Cues menu
	AddCueMenu();
	
	// 	Stage menu
	AddStageMenu();
	
	// 	Capture Menu
	AddCaptureMenu();
	
	//	Export Menu
	AddExportMenu();
	
	//	Windows menu
	AddWindowsMenu();

}


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TMuseumMenus::TMuseumMenus()
{
	m_MenuBar = NULL;
}

//-------------------------------------------------------------------
//
//	Function:	Default Destructor
//
//	Desc:
//	
//-------------------------------------------------------------------
//
//

TMuseumMenus::~TMuseumMenus()
{
}

#pragma mark -
#pragma mark === Menu Creation Routines ===

//-------------------------------------------------------------------
//	AddFileMenu
//-------------------------------------------------------------------
//
//	Add file menu to the BMenuBar
//

bool TMuseumMenus::AddFileMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
		
	//	Get menu string
	retVal = GetString("FileMenuStrings", kFileMenuItem, menuStr);	
	
	//	Create menu
	m_FileMenu = new BMenu((char *)menuStr);
	m_MenuBar->AddItem(m_FileMenu);
	
	// New...
	retVal = GetString("FileMenuStrings", kFileNewItem, menuStr);
	retVal = GetString("FileMenuStrings", kFileNewAccel, &accelStr);
	BMenuItem* newItem = new BMenuItem(menuStr, new BMessage(FILE_NEW_MSG), accelStr);
	m_FileMenu->AddItem(newItem);
	
	// Open...
	retVal = GetString("FileMenuStrings", kFileOpenItem, menuStr);
	retVal = GetString("FileMenuStrings", kFileOpenAccel, &accelStr);
	BMenuItem* openItem = new BMenuItem(menuStr, new BMessage(FILE_OPEN_MSG), accelStr);
	m_FileMenu->AddItem(openItem);
	
	// Seperator01
	m_FileMenu->AddSeparatorItem();

	// Close
	retVal = GetString("FileMenuStrings", kFileCloseItem, menuStr);
	retVal = GetString("FileMenuStrings", kFileCloseAccel, &accelStr);
	BMenuItem* closeItem = new BMenuItem(menuStr, new BMessage(FILE_CLOSE_MSG), accelStr);
	m_FileMenu->AddItem(closeItem);
	
	// Save...
	retVal = GetString("FileMenuStrings", kFileSaveItem, menuStr);
	retVal = GetString("FileMenuStrings", kFileSaveAccel, &accelStr);
	BMenuItem* saveItem = new BMenuItem(menuStr, new BMessage(FILE_SAVE_MSG), accelStr);
	m_FileMenu->AddItem(saveItem);
	
	// Save As...
	retVal = GetString("FileMenuStrings", kFileSaveAsItem, menuStr);
	BMenuItem* saveAsItem = new BMenuItem(menuStr, new BMessage(FILE_SAVEAS_MSG));
	m_FileMenu->AddItem(saveAsItem);
	
	// Revert
	retVal = GetString("FileMenuStrings", kFileRevertItem, menuStr);
	BMenuItem* revertItem = new BMenuItem(menuStr, new BMessage(FILE_REVERT_MSG));
	m_FileMenu->AddItem(revertItem);
		
	// Seperator02
	m_FileMenu->AddSeparatorItem();
			
	// Page Setup...
	retVal = GetString("FileMenuStrings", kFilePageSetupItem, menuStr);
	BMenuItem* pageItem = new BMenuItem(menuStr, new BMessage(FILE_PAGESETUP_MSG));
	m_FileMenu->AddItem(pageItem);
	
	// Print...
	retVal = GetString("FileMenuStrings", kFilePrintItem, menuStr);
	retVal = GetString("FileMenuStrings", kFilePrintAccel, &accelStr);
	BMenuItem* printItem = new BMenuItem(menuStr, new BMessage(FILE_PRINT_MSG), accelStr);
	m_FileMenu->AddItem(printItem);
	
	// Seperator03
	m_FileMenu->AddSeparatorItem();
	
	// About
	retVal = GetString("FileMenuStrings", kFileAboutItem, menuStr);
	BMenuItem* aboutItem = new BMenuItem(menuStr, new BMessage(B_ABOUT_REQUESTED));
	m_FileMenu->AddItem(aboutItem);
	
	// Seperator04
	m_FileMenu->AddSeparatorItem();
	
	// Quit
	retVal = GetString("FileMenuStrings", kFileQuitItem, menuStr);
	retVal = GetString("FileMenuStrings", kFileQuitAccel, &accelStr);
	BMenuItem* quitItem = new BMenuItem(menuStr, new BMessage(FILE_QUIT_MSG), accelStr);
	m_FileMenu->AddItem(quitItem);
	
	// All these items message to the application
	m_FileMenu->SetTargetForItems(be_app);
		
	return true;
}


//-------------------------------------------------------------------
//	AddEditMenu
//-------------------------------------------------------------------
//
//	Add edit menu to the BMenuBar
//

bool TMuseumMenus::AddEditMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
	
	//	Create menu
	retVal = GetString("EditMenuStrings", kEditMenuItem, menuStr);
	m_EditMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_EditMenu);
	
	// Undo
	retVal = GetString("EditMenuStrings", kEditUndoItem, menuStr);
	retVal = GetString("EditMenuStrings", kEditUndoAccel, &accelStr);
	BMenuItem* undoItem = new BMenuItem(menuStr, new BMessage(EDIT_UNDO_MSG), accelStr);
	undoItem->SetTarget(be_app);
	m_EditMenu->AddItem(undoItem);
	
	// Seperator01
	m_EditMenu->AddSeparatorItem();
	
	// Cut
	retVal = GetString("EditMenuStrings", kEditCutItem, menuStr);
	retVal = GetString("EditMenuStrings", kEditCutAccel, &accelStr);
	BMenuItem* cutItem = new BMenuItem(menuStr, new BMessage(EDIT_CUT_MSG), accelStr);
	cutItem->SetTarget(be_app);
	m_EditMenu->AddItem(cutItem);
	
	// Copy
	retVal = GetString("EditMenuStrings", kEditCopyItem, menuStr);
	retVal = GetString("EditMenuStrings", kEditCopyAccel, &accelStr);
	BMenuItem *copyItem = new BMenuItem(menuStr, new BMessage(EDIT_COPY_MSG), accelStr);
	copyItem->SetTarget(be_app);
	m_EditMenu->AddItem(copyItem);
	
	// Paste
	retVal = GetString("EditMenuStrings", kEditPasteItem, menuStr);
	retVal = GetString("EditMenuStrings", kEditPasteAccel, &accelStr);
	BMenuItem *pasteItem = new BMenuItem(menuStr, new BMessage(EDIT_PASTE_MSG), accelStr);
	pasteItem->SetTarget(be_app);
	m_EditMenu->AddItem(pasteItem);
	
	// Clear
	retVal = GetString("EditMenuStrings", kEditClearItem, menuStr);
	BMenuItem* clearItem = new BMenuItem(menuStr, new BMessage(EDIT_CLEAR_MSG));
	clearItem->SetTarget(be_app);
	m_EditMenu->AddItem(clearItem);
	
	// Select All
	retVal = GetString("EditMenuStrings", kEditSelectAllItem, menuStr);
	retVal = GetString("EditMenuStrings", kEditSelectAllAccel, &accelStr);	
	BMenuItem* selectAllItem = new BMenuItem(menuStr, new BMessage(EDIT_SELECTALL_MSG), accelStr);
	selectAllItem->SetTarget(be_app);
	m_EditMenu->AddItem(selectAllItem);
			
	return true;
}


//-------------------------------------------------------------------
//	AddCueSheetMenu
//-------------------------------------------------------------------
//
//	Add cue sheet menu to the BMenuBar
//

bool TMuseumMenus::AddCueSheetMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
	
	//	Create menu
	retVal = GetString("CueSheetMenuStrings", kCueSheetMenuItem, menuStr);
	m_CueSheetMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_CueSheetMenu);
	
	// Setup
	retVal = GetString("CueSheetMenuStrings", kCueSheetSetupItem, menuStr);
	BMenuItem* setupItem = new BMenuItem(menuStr, new BMessage(CUE_SETUP_MSG));
	m_CueSheetMenu->AddItem(setupItem);
	
	// Time Base
	retVal = GetString("CueSheetMenuStrings", kCueSheetTimeBaseItem, menuStr);
	BMenuItem* timeBaseItem = new BMenuItem(menuStr, new BMessage(CUE_TIMEBASE_MSG));
	m_CueSheetMenu->AddItem(timeBaseItem);
	
	// Seperator01
	m_CueSheetMenu->AddSeparatorItem();
	
	// Zoom In
	retVal = GetString("CueSheetMenuStrings", kCueSheetZoomInItem, menuStr);
	BMenuItem* zoomInItem = new BMenuItem(menuStr, new BMessage(CUE_ZOOMIN_MSG));
	m_CueSheetMenu->AddItem(zoomInItem);
	
	// Zoom Out
	retVal = GetString("CueSheetMenuStrings", kCueSheetZoomOutItem, menuStr);
	BMenuItem* zoomOutItem = new BMenuItem(menuStr, new BMessage(CUE_ZOOMOUT_MSG));
	m_CueSheetMenu->AddItem(zoomOutItem);
	
	// Seperator02
	m_CueSheetMenu->AddSeparatorItem();
	
	// Show Cue Durations
	retVal = GetString("CueSheetMenuStrings", kCueSheetShowCueDurationsItem, menuStr);
	BMenuItem* showDurationsItem = new BMenuItem(menuStr, new BMessage(CUE_SHOWDURATIONS_MSG));
	m_CueSheetMenu->AddItem(showDurationsItem);
	
	// Show Stop Times
	retVal = GetString("CueSheetMenuStrings", kCueSheetShowStopTimesItem, menuStr);
	BMenuItem* showStopItem = new BMenuItem(menuStr, new BMessage(CUE_SHOWSTOPTIMES_MSG));
	m_CueSheetMenu->AddItem(showStopItem);
	
	// Seperator02
	m_CueSheetMenu->AddSeparatorItem();
	
	// Insert Channels
	retVal = GetString("CueSheetMenuStrings", kCueSheetInsertChannelsItem, menuStr);
	BMenuItem* insertItem = new BMenuItem(menuStr, new BMessage(CUE_INSERTCHANNELS_MSG));
	m_CueSheetMenu->AddItem(insertItem);
	
	// Delete Channels
	retVal = GetString("CueSheetMenuStrings", kCueSheetDeleteChannelsItem, menuStr);
	BMenuItem* deleteItem = new BMenuItem(menuStr, new BMessage(CUE_DELETECHANNELS_MSG));
	m_CueSheetMenu->AddItem(deleteItem);
		
	// Seperator03
	m_CueSheetMenu->AddSeparatorItem();
	
	// Insert Time
	retVal = GetString("CueSheetMenuStrings", kCueSheetInsertTimeItem, menuStr);
	BMenuItem* timeItem = new BMenuItem(menuStr, new BMessage(CUE_INSERTTIME_MSG));
	m_CueSheetMenu->AddItem(timeItem);
	
	// Seperator04
	m_CueSheetMenu->AddSeparatorItem();
	
	// Align Cue Starts
	retVal = GetString("CueSheetMenuStrings", kCueSheetAlignCueStartsItem, menuStr);
	BMenuItem* startItem = new BMenuItem(menuStr, new BMessage(CUE_ALIGNSTART_MSG));
	m_CueSheetMenu->AddItem(startItem);
	
	// Align Cue Ends
	retVal = GetString("CueSheetMenuStrings", kCueSheetAlignCueEndsItem, menuStr);
	BMenuItem* endItem = new BMenuItem(menuStr, new BMessage(CUE_ALIGNEND_MSG));
	m_CueSheetMenu->AddItem(endItem);
	
	// Seperator05
	m_CueSheetMenu->AddSeparatorItem();
	
	// Collapse All
	retVal = GetString("CueSheetMenuStrings", kCueSheetCollapseAllItem, menuStr);
	BMenuItem* collapseItem = new BMenuItem(menuStr, new BMessage(CUE_COLLAPSEALL_MSG));
	m_CueSheetMenu->AddItem(collapseItem);

	// Seperator06
	m_CueSheetMenu->AddSeparatorItem();
	
	// Jump To Marker
	retVal = GetString("CueSheetMenuStrings", kCueSheetJumpToMarkerItem, menuStr);
	BMenuItem* jumpToItem = new BMenuItem(menuStr, new BMessage(CUE_JUMPTO_MSG));
	m_CueSheetMenu->AddItem(jumpToItem);
	
	// All these items message to the application
	m_CueSheetMenu->SetTargetForItems(be_app);

	return true;
	
}


//-------------------------------------------------------------------
//	AddCueMenu
//-------------------------------------------------------------------
//
//	Add cue menu to the BMenuBar
//

bool TMuseumMenus::AddCueMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
	
	//	Create menu
	retVal = GetString("CueMenuStrings", kCueMenuItem, menuStr);
	m_CueMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_CueMenu);
	
	// Get Cue Info
	retVal = GetString("CueMenuStrings", kCueGetCueInfoItem, menuStr);
	BMenuItem* infoItem = new BMenuItem(menuStr, new BMessage(CUESHEET_GETINFO_MSG));
	infoItem->SetTarget(be_app);
	m_CueMenu->AddItem(infoItem);

	// Default Cue Setup
	retVal = GetString("CueMenuStrings", kCueDefaultCueSetupItem, menuStr);
	BMenuItem* defaultItem = new BMenuItem(menuStr, new BMessage(CUESHEET_DEFSETUP_MSG));
	defaultItem->SetTarget(be_app);
	m_CueMenu->AddItem(defaultItem);

	// Seperator01
	m_CueMenu->AddSeparatorItem();
	
	// Show Cue Contents
	retVal = GetString("CueMenuStrings", kCueShowCueContentsItem, menuStr);
	BMenuItem* contentsItem = new BMenuItem(menuStr, new BMessage(CUESHEET_SHOWCONTENTS_MSG));
	contentsItem->SetTarget(be_app);
	m_CueMenu->AddItem(contentsItem);

	// Seperator02
	m_CueMenu->AddSeparatorItem();

	// Show All Cue Contents
	retVal = GetString("CueMenuStrings", kCueShowAllCueContentsItem, menuStr);
	BMenuItem* showAllItem = new BMenuItem(menuStr, new BMessage(CUESHEET_SHOWALLCONTENTS_MSG));
	showAllItem->SetTarget(be_app);
	m_CueMenu->AddItem(showAllItem);

	// Hide All Cue Contents
	retVal = GetString("CueMenuStrings", kCueHideAllCueContentsItem, menuStr);
	BMenuItem* hideAllItem = new BMenuItem(menuStr, new BMessage(CUESHEET_HIDEALLCONTENTS_MSG));
	hideAllItem->SetTarget(be_app);
	m_CueMenu->AddItem(hideAllItem);
	
	// Seperator03
	m_CueMenu->AddSeparatorItem();
	
	// Cue Setup
	retVal = GetString("CueMenuStrings", kCueCueSetupItem, menuStr);
	BMenuItem* setupItem = new BMenuItem(menuStr, new BMessage(CUESHEET_SETUP_MSG));
	setupItem->SetTarget(be_app);
	m_CueMenu->AddItem(setupItem);
	
	// Open Cue Editor
	retVal = GetString("CueMenuStrings", kCueOpenCueEditorItem, menuStr);
	BMenuItem* editItem = new BMenuItem(menuStr, new BMessage(CUESHEET_EDIT_MSG));
	editItem->SetTarget(be_app);
	m_CueMenu->AddItem(editItem);	

	// Seperator04
	m_CueMenu->AddSeparatorItem();
	
	// Add Keyframe
	retVal = GetString("CueMenuStrings", kCueAddKeyframeItem, menuStr);
	retVal = GetString("CueMenuStrings", kCueAddKeyframeAccel, &accelStr);
	BMenuItem* keyframeItem = new BMenuItem(menuStr, new BMessage(CUESHEET_KEYFRAME_MSG), accelStr);
	keyframeItem->SetTarget(be_app);
	m_CueMenu->AddItem(keyframeItem);
					
	return true;	
}


//-------------------------------------------------------------------
//	AddStageMenu
//-------------------------------------------------------------------
//
//	Add stage menu to the BMenuBar
//

bool TMuseumMenus::AddStageMenu()
{
	bool  	retVal;
	char 	accelStr[1];
	char	menuStr[256];
	
	//	Create Menu
	retVal = GetString("StageMenuStrings", kStageMenuItem, menuStr);
	m_StageMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_StageMenu);
	
	// Stage Setup
	retVal = GetString("StageMenuStrings", kStageStageSetupItem, menuStr);
	BMenuItem* stageItem = new BMenuItem(menuStr, new BMessage(STAGE_SETUP_MSG));
	stageItem->SetTarget(be_app);
	m_StageMenu->AddItem(stageItem);
	
	// Palette
	retVal = GetString("StageMenuStrings", kStagePaletteItem, menuStr);
	BMenuItem* paletteItem = new BMenuItem(menuStr, new BMessage(STAGE_PALETTE_MSG));
	paletteItem->SetTarget(be_app);
	m_StageMenu->AddItem(paletteItem);
	
	// Bring To Front
	retVal = GetString("StageMenuStrings", kStageBringToFrontItem, menuStr);
	BMenuItem* frontItem = new BMenuItem(menuStr, new BMessage(STAGE_FRONT_MSG));
	frontItem->SetTarget(be_app);
	m_StageMenu->AddItem(frontItem);

	// Seperator
	m_StageMenu->AddSeparatorItem();

	// Live Update
	retVal = GetString("StageMenuStrings", kStageLiveUpdateItem, menuStr);
	BMenuItem* updateItem = new BMenuItem(menuStr, new BMessage(STAGE_LUPDATE_MSG));
	updateItem->SetTarget(be_app);
	m_StageMenu->AddItem(updateItem);
	
	// Seperator
	m_StageMenu->AddSeparatorItem();

	// Grid Setup
	retVal = GetString("StageMenuStrings", kStageGridSetupItem, menuStr);
	BMenuItem* setupItem = new BMenuItem(menuStr, new BMessage(STAGE_GRIDSETUP_MSG));
	setupItem->SetTarget(be_app);
	m_StageMenu->AddItem(setupItem);

	// Seperator
	m_StageMenu->AddSeparatorItem();
	
	// Take Stage Picture
	retVal = GetString("StageMenuStrings", kStageTakeStagePictureItem, menuStr);
	BMenuItem* pictureItem = new BMenuItem(menuStr, new BMessage(STAGE_PICTURE_MSG));
	pictureItem->SetTarget(be_app);
	m_StageMenu->AddItem(pictureItem);
			
	return true;	
}


//-------------------------------------------------------------------
//	AddCaptureMenu
//-------------------------------------------------------------------
//
//	Add capture menu to the BMenuBar
//

bool TMuseumMenus::AddCaptureMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
	
	//	Create menu
	retVal = GetString("CaptureMenuStrings", kCaptureMenuItem, menuStr);
	m_CaptureMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_CaptureMenu);
	
	// Capture Video
	retVal = GetString("CaptureMenuStrings", kCaptureVideoItem, menuStr);
	BMenuItem* vidCapItem = new BMenuItem(menuStr, new BMessage(CAPTURE_VIDCAP_MSG));
	vidCapItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(vidCapItem);

	// Video Settings
	retVal = GetString("CaptureMenuStrings", kCaptureVideoSettingsItem, menuStr);	
	BMenuItem* vidSettingsItem = new BMenuItem(menuStr, new BMessage(CAPTURE_VIDSETTNGS_MSG));
	vidSettingsItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(vidSettingsItem);
	
	// Seperator01
	m_CaptureMenu->AddSeparatorItem();
	
	// Capture Audio
	retVal = GetString("CaptureMenuStrings", kCaptureAudioItem, menuStr);
	BMenuItem* audCapItem = new BMenuItem(menuStr, new BMessage(CAPTURE_AUDCAP_MSG));
	audCapItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(audCapItem);
	
	// Audio Settings
	retVal = GetString("CaptureMenuStrings", kCaptureAudioSettingsItem, menuStr);
	BMenuItem* audSettingsItem = new BMenuItem(menuStr, new BMessage(CAPTURE_AUDSETTNGS_MSG));
	audSettingsItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(audSettingsItem);
	
	// Seperator02
	m_CaptureMenu->AddSeparatorItem();
	
	// Capture Preferences
	retVal = GetString("CaptureMenuStrings", kCapturePreferencesItem, menuStr);
	BMenuItem* audPrefsItem = new BMenuItem(menuStr, new BMessage(CAPTURE_PREFS_MSG));
	audPrefsItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(audPrefsItem);
	
	// Seperator03
	m_CaptureMenu->AddSeparatorItem();

	// Levels
	retVal = GetString("CaptureMenuStrings", kCaptureDisplayLevelsItem, menuStr);	
	BMenuItem* levelsItem = new BMenuItem(menuStr, new BMessage(CAPTURE_LEVELS_MSG));
	levelsItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(levelsItem);
					
	return true;	
}


//-------------------------------------------------------------------
//	AddExportMenu
//-------------------------------------------------------------------
//
//	Add export menu to the BMenuBar
//

bool TMuseumMenus::AddExportMenu()
{
	bool  	retVal;
	char 	accelStr;
	char	menuStr[256];
	

	//	Create Menu
	retVal = GetString("ExportMenuStrings", kExportMenuItem, menuStr);	
	m_ExportMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_ExportMenu);
	
	/*	
	// Capture Video
	BMenuItem* vidCapItem = new BMenuItem("Capture Video...", new BMessage(CAPTURE_VIDCAP_MSG));
	vidCapItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(vidCapItem);

	// Video Settings
	BMenuItem* vidSettingsItem = new BMenuItem("Video Settings...", new BMessage(CAPTURE_VIDSETTNGS_MSG));
	vidSettingsItem->SetTarget(be_app);
	m_CaptureMenu->AddItem(vidSettingsItem);
	
	// Seperator01
	m_CaptureMenu->AddSeparatorItem();
	*/	
	
	return true;	
}	

//-------------------------------------------------------------------
//	AddWindowsMenu
//-------------------------------------------------------------------
//
//	Add windows menu to the BMenuBar
//

bool TMuseumMenus::AddWindowsMenu()
{
	bool  	retVal;	
	char 	accelStr;
	char 	idStr;
	char	menuStr[256];
	
	//	Create menu
	retVal = GetString("WindowsMenuStrings", kWindowsMenuItem, menuStr);	
	m_WindowsMenu = new BMenu(menuStr);
	m_MenuBar->AddItem(m_WindowsMenu);
	
	// Show Stage
	retVal = GetString("WindowsMenuStrings", kWindowsHideStageItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsStageItem);
	BMenuItem* stageItem = new BMenuItem(menuStr, new BMessage(WINDOWS_STAGE_MSG), idStr);
	stageItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(stageItem);
	
	// Show Browser
	retVal = GetString("WindowsMenuStrings", kWindowsHideBrowserItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsBrowserItem);
	BMenuItem* browserItem = new BMenuItem(menuStr, new BMessage(WINDOWS_BROWSER_MSG), idStr);
	browserItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(browserItem);
	
	// Show Transport
	retVal = GetString("WindowsMenuStrings", kWindowsHideTransportItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsTransportItem);
	BMenuItem* transportItem = new BMenuItem(menuStr, new BMessage(WINDOWS_TRANSPORT_MSG), idStr);
	transportItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(transportItem);
	
	// Show Locator Palette
	retVal = GetString("WindowsMenuStrings", kWindowsHideLocatorItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsLocatorItem);
	BMenuItem* counterItem = new BMenuItem(menuStr, new BMessage(WINDOWS_COUNTER_MSG), idStr);
	counterItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(counterItem);
	
	// Show Media Cue Palette
	retVal = GetString("WindowsMenuStrings", kWindowsHideMediaItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsMediaItem);
	BMenuItem* mediaItem = new BMenuItem(menuStr, new BMessage(WINDOWS_MEDIA_MSG), idStr);
	mediaItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(mediaItem);

	// Show Memory Palette
	retVal = GetString("WindowsMenuStrings", kWindowsHideMemoryItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsMemoryItem);
	BMenuItem* memoryItem = new BMenuItem(menuStr, new BMessage(WINDOWS_MEMORY_MSG), idStr);
	memoryItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(memoryItem);
	
	// Show Stage Tools Palette
	retVal = GetString("WindowsMenuStrings", kWindowsHideStageToolsItem, menuStr);	
	sprintf(&idStr, "%d", kWindowsStageToolsItem);
	BMenuItem* stageToolsItem = new BMenuItem(menuStr, new BMessage(WINDOWS_STAGE_TOOLS_MSG), idStr);
	stageToolsItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(stageToolsItem);


	// Seperator01
	m_WindowsMenu->AddSeparatorItem();
	
	// Now check and see if we need to add open documents to the menu
	BList *cueSheetList = static_cast<MuseumApp *>(be_app)->GetCueSheetList();	
	if (cueSheetList)
	{
		for (int32 index = 0; index < cueSheetList->CountItems(); index++)
		{	
			TCueSheetWindow *theWindow = (TCueSheetWindow *)cueSheetList->ItemAt(index);			
			if (theWindow)
				AddToWindowsMenu( theWindow->Title(), theWindow->GetID());					
		}
	}
				
	return true;	
}


//-------------------------------------------------------------------
//	AddToWindowsMenu
//-------------------------------------------------------------------
//
//	Add a newly opened Cue Sheet to the end of the menu
//

bool TMuseumMenus::AddToWindowsMenu(const char *documentName, int32 documentID)
{
	// Create message and add document ID to it
	BMessage *theMessage = new BMessage(WINDOWS_DOCUMENT_MSG);
	theMessage->AddInt32("CueSheetID", documentID);
	
	// Show Memory Palette
	BMenuItem* documentItem = new BMenuItem(documentName, theMessage);
	documentItem->SetTarget(be_app);
	m_WindowsMenu->AddItem(documentItem);
	
	return true;
}


//-------------------------------------------------------------------
//	RemoveDocumentsInWindowsMenu
//-------------------------------------------------------------------
//
//	
//

bool TMuseumMenus::RemoveDocumentsInWindowsMenu()
{
	// Remove all documents
	while( m_WindowsMenu->CountItems() > 7)
	{	
		 BMenuItem *theItem = m_WindowsMenu->RemoveItem( m_WindowsMenu->CountItems()-1 );
		 if (theItem)
		 	delete theItem;
	}
	
	return true;
}


#pragma mark -
#pragma mark === Utility Functions ===

//-------------------------------------------------------------------
//	LocateMenus
//-------------------------------------------------------------------
//
//	Locate child menus.  This is done after a menu has been
//	unarchived
//	

void TMuseumMenus::LocateMenus()
{
	if (m_MenuBar)
	{
		m_FileMenu 		= (BMenu *)m_MenuBar->FindView("File");
		m_EditMenu 		= (BMenu *)m_MenuBar->FindView("Edit");
		m_CueSheetMenu 	= (BMenu *)m_MenuBar->FindView("Cue Sheet");
		m_CueMenu 		= (BMenu *)m_MenuBar->FindView("Cue");
		m_CaptureMenu 	= (BMenu *)m_MenuBar->FindView("Capture");
		m_StageMenu 	= (BMenu *)m_MenuBar->FindView("Stage");
		m_WindowsMenu 	= (BMenu *)m_MenuBar->FindView("Windows");
	}	
}		


#pragma mark -
#pragma mark === Accessor Functions ===

//-------------------------------------------------------------------
//	SetMenuBar
//-------------------------------------------------------------------
//
//	

void TMuseumMenus::SetMenuBar(BMenuBar *menuBar)
{
	m_MenuBar = menuBar;
}		