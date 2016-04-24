//-------------------------------------------------------------------
//
//	File:	TNewProject.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.18.98
//
//	Desc:	Dialog for setting up new project settings
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/StringView.h>
#include <support/Debug.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <assert.h>

using namespace std;

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TNewProjectBox.h"
#include "TPreset.h"

#include "TNewProject.h"

#ifdef WATCH
#undef WATCH
#endif
#define WATCH(x) printf(x);

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TNewProject::TNewProject(MuseumApp *parent, BMessage *theMessage) : BWindow(theMessage)
{
	m_Parent = parent;
	
	m_PresetList = NULL;
	
	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TNewProject::~TNewProject()
{
	if (m_PresetList)
	{
		FreePresetList(m_PresetList);
		delete m_PresetList;
	}
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TNewProject::Init()
{			
WATCH("In TNewProject::Init\n");
	// Get dialog items	
	m_Background = (BView *)FindView("NewProjectView");
	ASSERT(m_Background);
	
	BBox *settingsBox = (BBox *)FindView("SettingsBox");
	ASSERT(settingsBox);
	
	// replace setting with our own box
	BMessage *theMessage = new BMessage();
	settingsBox->Archive(theMessage, true);
	m_SettingsBox = new TNewProjectBox(theMessage);
	m_Background->RemoveChild(settingsBox);
	m_Background->AddChild(m_SettingsBox);
	delete settingsBox;
	delete theMessage;
	
	m_PresetsBox = (BBox *)FindView("PresetsBox");
	ASSERT(m_PresetsBox);

	m_PresetsListScrollView = (BScrollView *)FindView("PresetsListScrollView");
	ASSERT(m_PresetsListScrollView);
		  
	m_PresetsListView = (BListView *)FindView("PresetsListView");
	ASSERT(m_PresetsListView);
	BMessage *selectMessage = new BMessage(LIST_SELECT_MSG);
	m_PresetsListView->SetSelectionMessage(selectMessage);
	
	BMessage *invokeMessage = new BMessage(LIST_INVOKE_MSG);
	m_PresetsListView->SetInvocationMessage(invokeMessage);
		
	//
	// Load presets list with presets in "settings/presets" directory
	//
	
	m_PresetList = LoadPresets();
		
	// Load presets into dialog
	if (m_PresetList->CountItems() > 0 ) {
		SetupPresetList(m_PresetList, m_PresetsListView);
	}
	// No presets!
	else {
		WATCH("No presets found, create default presets\n");
		// Create a set of default presets for user
		CreateDefaultPresets();
		
		// Now load them in
		if (m_PresetList)
			delete m_PresetList;
		
		m_PresetList = LoadPresets();
		if (m_PresetList) {
printf("LoadPresets returned count of %d\n",m_PresetList->CountItems());
			SetupPresetList(m_PresetList, m_PresetsListView);			
		}
		// We have a big problem if we can't create a new set...
		else {
			WATCH("Cannot create default presets!\n");
			TRESPASS();
		}
	}
			
	// Set focus to first item in list
	m_PresetsListView->Select(0);
	m_PresetsListView->MakeFocus(true);	
WATCH("Leaving TNewProject::Init\n");
}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	Function:	MessageReceived		
//-------------------------------------------------------------------
//
//

void TNewProject::MessageReceived(BMessage* message)
{
WATCH("In TNewProject::MessageReceived\n");


			char msg1[10];
			memset(msg1, '\0', 9);
			memcpy(msg1,&(message->what),4);
			printf("TNP::MR before switch: ");
			printf(" %s\n",msg1);
		

	switch(message->what)
	{	// User pressed OK button.  Inform app about project settings
		case OK_MSG:
			{					
WATCH("TWP::MR: OK button\n");
				// Retrive preset from list
				TPreset *thePreset = static_cast<TPreset *>(m_PresetList->ItemAt(GetSelectedItem()));
					
				// Archive it and add it to the message
				BMessage invokeMessage(NEW_PROJECT_MSG);
				BMessage *archiveMessage = new BMessage();
				thePreset->Archive(archiveMessage);
				invokeMessage.AddMessage("Preset", archiveMessage);
					
				// Inform application									
				m_Parent->PostMessage(&invokeMessage);				
				Lock();
				Quit();				
			}
			break;
			
		case CANCEL_MSG:
			{
WATCH("TWP:MR: CANCEL\n");
				// 	Quit application if there are no cue sheets open, otherwise
				//	just quit
				BList *theList = m_Parent->GetCueSheetList();
				if (theList)
				{				
					if (theList->CountItems() == 0)				
						m_Parent->PostMessage(B_QUIT_REQUESTED);
				}
				else
					m_Parent->PostMessage(B_QUIT_REQUESTED);
				
				Lock();
				Quit();	
			}
			break;
			
		//case OPEN_MSG:
		//	break;
			
		// List item was selected
		case LIST_SELECT_MSG:
			{
WATCH("TNP::MessageReceived: LIST_SELECT_MSG\n");
				// 	Make sure that we have an item selected.  If not,
				//	select first item
				if ( m_PresetsListView->CurrentSelection() < 0)
				{
					m_PresetsListView->Select(0);
					m_PresetsListView->ScrollToSelection();
				}
				
				// Display settings in setting area
				m_SettingsBox->Invalidate();
					
			}
			break;
	
		//  User double clicked item.  Load preset and notify app
		case LIST_INVOKE_MSG:			
			{	
			
WATCH("TNP::MessageReceived: LIST_INVOKE_MSG\n")
				// Make sure an item is selected
				if ( m_PresetsListView->CurrentSelection() > 0){
					// Get item selected from message
					int32 theItem;
					message->FindInt32("index", &theItem);
					
					// Retreive preset from list
					TPreset *thePreset = static_cast<TPreset *>(m_PresetList->ItemAt(theItem));
					
					// Archive it and add it to the message
					BMessage invokeMessage(NEW_PROJECT_MSG);
					BMessage *archiveMessage = new BMessage();
					thePreset->Archive(archiveMessage);
					invokeMessage.AddMessage("Preset", archiveMessage);
					
					// Inform application									
					m_Parent->PostMessage(&invokeMessage);				
					Lock();
					Quit();				
				} else {
					WATCH("PresetListView: no current selection!\n");
				}
			}
			break;

			
		default:
			BWindow::MessageReceived(message);						
			break;
	}
	
}


#pragma mark -
#pragma mark === Dialog Handling ===

//-------------------------------------------------------------------
//	Function:	GetDialogSettings		
//-------------------------------------------------------------------
//
//	Retrieve dialog settings and apply them to the current cue sheet
//

void TNewProject::GetDialogSettings()
{
printf("TNP:GDS not implemented!\n");
	/*
	// Set cue sheet's TimeFormat based on user changes
	BMessage *message = new BMessage(TIMEFORMAT_CHANGED_MSG);
	short format = m_TimeFormat;
	message->AddInt16("TimeFormat", format);
	TCueSheetWindow *theWindow = static_cast<MuseumApp *>(be_app)->GetCueSheet();
	TCueSheetView *theView = theWindow->GetCueSheetView();
	theWindow->PostMessage( message, theView); 
	delete message;
	*/
	// Set new start time
	
	// Now set duration.  We can use this and ignore the end time because it has
	// already been verified as good at entry time
	
}



//---------------------------------------------------------------------
//	LoadCueAddOns
//---------------------------------------------------------------------
//
//	Load cue add-ons in the /add-ons/cues directory
//

BList  *TNewProject::LoadPresets()
{
	// Create list to hold presets to be returned
	BList *presetList = new BList();
	
	// Get listing of add-ons in directory
	app_info appInfo;
	be_app->GetAppInfo(&appInfo); 	
	BEntry entry(&appInfo.ref);
	BDirectory settingsDir;
	entry.GetParent(&settingsDir);
// ABH 	if (settingsDir.SetTo(&settingsDir, kPresetsPathString) != B_OK){
	if (settingsDir.SetTo(kPresetsPathString) != B_OK){
		printf("LoadPresets: SetTo failed\n");
	} else {
		printf("LoadPresets: SetTo succeeded\n");
	}
		
	status_t 	myErr;
	BList		*refList = new BList();
   
	if (settingsDir.InitCheck() == B_OK) { // ABH this fails
		int32 entries = settingsDir.CountEntries();
		
		for( int32 index = 0; index < entries; index++)	{
			BEntry *theEntry = new BEntry();
			myErr = settingsDir.GetNextEntry(theEntry, true);
			
			// Add it to our list if we are successful
			if (myErr != B_ENTRY_NOT_FOUND)	{
				refList->AddItem(theEntry);
			}else 
				delete theEntry;
		}
	} else { //ABH
		printf("settingsDir failed in TNP::LoadPresets\n");
		int r_val = settingsDir.InitCheck();
		switch (r_val){
			case B_OK:
				printf("B_OK\n");
				break;
			case B_BAD_VALUE:
				printf("bad value\n");
				break;
			case B_ENTRY_NOT_FOUND:
				printf("entry not found\n");
				break;
			case B_NAME_TOO_LONG:
				printf("name too long\n");
				break;
			case B_LINK_LIMIT:
				printf("link limit\n");
				break;
			case B_NO_MEMORY:
				printf("no memory\n");
				break;
			case B_FILE_ERROR:
				printf("file error\n");
				break;
			default:
				printf("default\n");
				break;
		}
	}
	
	// March through list and load presets in the refList
	for (int32 index = 0; index < refList->CountItems(); index++)
	{
		// Validate that the items in the list are cue add-ons
		//BEntry *presetEntry = (BEntry *)refList->ItemAt(index);
		BEntry *presetEntry = static_cast<BEntry *>(refList->ItemAt(index));
		
		if (presetEntry){
		
			// Create file from ref
			BFile *openFile = new BFile(presetEntry, B_READ_ONLY);
			
			if (openFile->InitCheck() == B_OK){
			
				// Is this a valid file?  Read in the header and check...
				openFile->Seek(0, SEEK_SET);
				ChunkHeader	fileHeader;
				openFile->Read(&fileHeader, sizeof(ChunkHeader));
				
				if (fileHeader.chunkID == kPresetChunkID){
				
					// Load in archived BMessage
					void *data = malloc( fileHeader.chunkSize);				
					openFile->Read(data, fileHeader.chunkSize);
					
					// Unflatten the archive
					BMessage *presetMessage = new BMessage();
					presetMessage->Unflatten((const char *)data);
					
					if (presetMessage){
					
					 	BArchivable *unarchived = instantiate_object(presetMessage);
					 	if ( unarchived ){
				 		
							// Add cue to our list of unarchived cues
							TPreset *thePreset = cast_as(unarchived, TPreset); 
		  					if (thePreset) 
								presetList->AddItem(thePreset);	  					
						}				
					}
					
					//	Clean up 
					free(data);
					delete presetMessage;
				}
			} else {
				WATCH("new BFile presetEntry failed\n")
			}
		} else {
			WATCH("BEntry presetEntry == NULL\n")
		}
	}
		
	// Clean up
	delete refList;

	// Return our list of loaded presets	
	return presetList;
}


#pragma mark -
#pragma mark === Preset List Handling ===

//---------------------------------------------------------------------
//	SetupPresetList
//---------------------------------------------------------------------
//
//	Load presets into list
//	

void TNewProject::SetupPresetList(BList *presetList, BListView *presetListView )
{
	ASSERT(presetList);
	ASSERT(presetListView);
	// ABH
	if (!presetList){
		printf("TNP::SetupPresetList: presetList == NULL!\n");
	}
	if (!presetListView){
		printf("TNP::SetupPresetList: presetListView == NULL!\n");
	}
	
	for( int32 index = 0; index < presetList->CountItems(); index++ )
	{
		TPreset *thePreset = static_cast<TPreset *>(presetList->ItemAt(index));
		
		if (thePreset)
		{
			BStringItem *theItem = new BStringItem(thePreset->m_Name, 0, true);
			presetListView->AddItem(theItem);
		}
	}
}


//---------------------------------------------------------------------
//	FreePresetList
//---------------------------------------------------------------------
//
//	Free presets in list
//	

void TNewProject::FreePresetList(BList *presetList)
{
	if (presetList)
	{
		for(int32 index = 0; index < presetList->CountItems(); index++)
		{
			TPreset *preset = static_cast<TPreset *>(presetList->ItemAt(index));
			if (preset)
				delete preset;
		}
	}
}


#pragma mark -
#pragma mark === Preset Creation ===

//---------------------------------------------------------------------
//	CreateDefaultPresets
//---------------------------------------------------------------------
//
//	Create a set of default presets.  We do this in the case that the 
//	default presets cannot be located
//

void TNewProject::CreateDefaultPresets()
{
	//
	//	Create default set
	//
printf("In TNP::CreateDefaultPresets\n");

	// Online Video 01
	TPreset online01Preset("Online Video 01");
	strcpy (online01Preset.m_Description01, "Online Video 01");
	strcpy (online01Preset.m_Description02, "Description Line Two");
	strcpy (online01Preset.m_Description03, "Description Line Three");
	strcpy (online01Preset.m_Description04, "Description Line Four");
	strcpy (online01Preset.m_Description05, "Description Line Five");
	online01Preset.m_Timebase 		 = B_TIMECODE_24;
	online01Preset.m_AudioCompressor = M_A_MULAW_TYPE;
	online01Preset.m_VideoCompressor = M_V_SORENSON_TYPE;
	online01Preset.m_FrameWidth  	 = 320;
	online01Preset.m_FrameHeight 	 = 240;
	online01Preset.WriteToFile(online01Preset.m_Name);
	
	// Online Video 02
	TPreset online02Preset("Online Video 02");
	strcpy (online02Preset.m_Description01, "Online Video 02");
	strcpy (online02Preset.m_Description02, "Description Line Two");
	strcpy (online02Preset.m_Description03, "Description Line Three");
	strcpy (online02Preset.m_Description04, "Description Line Four");
	strcpy (online02Preset.m_Description05, "Description Line Five");
	online02Preset.m_Timebase 			= B_TIMECODE_24;
	online02Preset.m_AudioCompressor 	= M_A_MULAW_TYPE;
	online02Preset.m_VideoCompressor 	= M_V_SORENSON_TYPE;
	online02Preset.m_FrameWidth  		= 320;
	online02Preset.m_FrameHeight 		= 240;
	online02Preset.WriteToFile(online02Preset.m_Name);
		
	// Online Video 03
	TPreset online03Preset("Online Video 03");
	strcpy (online03Preset.m_Description01, "Online Video 03");
	strcpy (online03Preset.m_Description02, "Description Line Two");
	strcpy (online03Preset.m_Description03, "Description Line Three");
	strcpy (online03Preset.m_Description04, "Description Line Four");
	strcpy (online03Preset.m_Description05, "Description Line Five");	
	online03Preset.m_Timebase = B_TIMECODE_24;
	online03Preset.m_AudioCompressor = M_A_MULAW_TYPE;
	online03Preset.m_VideoCompressor = M_V_SORENSON_TYPE;
	online03Preset.m_FrameWidth = 320;
	online03Preset.m_FrameHeight = 240;
	online03Preset.WriteToFile(online03Preset.m_Name);
	
	// Presentation Video 01
	TPreset pv01("Presentation Video 01");
	strcpy (pv01.m_Description01, "Presentation Video 01");
	strcpy (pv01.m_Description02, "Description Line Two");
	strcpy (pv01.m_Description03, "Description Line Three");
	strcpy (pv01.m_Description04, "Description Line Four");
	strcpy (pv01.m_Description05, "Description Line Five");	
	pv01.m_Timebase = B_TIMECODE_24;
	pv01.m_AudioCompressor = M_A_MULAW_TYPE;
	pv01.m_VideoCompressor = M_V_SORENSON_TYPE;
	pv01.m_FrameWidth = 320;
	pv01.m_FrameHeight = 240;
	pv01.WriteToFile(pv01.m_Name);

	// Presentation Video 02
	TPreset pv02("Presentation Video 02");
	strcpy (pv02.m_Description01, "Presentation Video 02");
	strcpy (pv02.m_Description02, "Description Line Two");
	strcpy (pv02.m_Description03, "Description Line Three");
	strcpy (pv02.m_Description04, "Description Line Four");
	strcpy (pv02.m_Description05, "Description Line Five");	
	pv02.m_Timebase = B_TIMECODE_24;
	pv02.m_AudioCompressor = M_A_MULAW_TYPE;
	pv02.m_VideoCompressor = M_V_SORENSON_TYPE;
	pv02.m_FrameWidth = 320;
	pv02.m_FrameHeight = 240;
	pv02.WriteToFile(pv02.m_Name);

	// Presentation Video 03
	TPreset pv03("Presentation Video 03");
	strcpy (pv03.m_Description01, "Presentation Video 03");
	strcpy (pv03.m_Description02, "Description Line Two");
	strcpy (pv03.m_Description03, "Description Line Three");
	strcpy (pv03.m_Description04, "Description Line Four");
	strcpy (pv03.m_Description05, "Description Line Five");	
	pv03.m_Timebase = B_TIMECODE_24;
	pv03.m_AudioCompressor = M_A_MULAW_TYPE;
	pv03.m_VideoCompressor = M_V_SORENSON_TYPE;
	pv03.m_FrameWidth = 320;
	pv03.m_FrameHeight = 240;
	pv03.WriteToFile(pv03.m_Name);
	
	// CD-ROM Video 01
	TPreset cd01("CD-ROM Video 01");
	strcpy (cd01.m_Description01, "CD-ROM Video 01");
	strcpy (cd01.m_Description02, "Description Line Two");
	strcpy (cd01.m_Description03, "Description Line Three");
	strcpy (cd01.m_Description04, "Description Line Four");
	strcpy (cd01.m_Description05, "Description Line Five");	
	cd01.m_Timebase 		= B_TIMECODE_24;
	cd01.m_AudioCompressor 	= M_A_MULAW_TYPE;
	cd01.m_VideoCompressor 	= M_V_SORENSON_TYPE;
	cd01.m_FrameWidth 		= 320;
	cd01.m_FrameHeight 		= 240;
	cd01.WriteToFile(cd01.m_Name);

	// CD-ROM Video 02
	TPreset cd02("CD-ROM Video 02");
	strcpy (cd02.m_Description01, "CD-ROM Video 02");
	strcpy (cd02.m_Description02, "Description Line Two");
	strcpy (cd02.m_Description03, "Description Line Three");
	strcpy (cd02.m_Description04, "Description Line Four");
	strcpy (cd02.m_Description05, "Description Line Five");	
	cd02.m_Timebase = B_TIMECODE_24;
	cd02.m_AudioCompressor = M_A_MULAW_TYPE;
	cd02.m_VideoCompressor = M_V_SORENSON_TYPE;
	cd02.m_FrameWidth = 320;
	cd02.m_FrameHeight = 240;
	cd02.WriteToFile(cd02.m_Name);

}


#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	CreateDefaultPresets
//---------------------------------------------------------------------
//
//	Return the item selected in the preset list
//

int32 TNewProject::GetSelectedItem()
{
	return m_PresetsListView->CurrentSelection();
}
