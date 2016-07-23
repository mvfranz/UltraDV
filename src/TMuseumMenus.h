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
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TMUSEUMMENUS_H__
#define __TMUSEUMMENUS_H__

#include "MenuConstants.h"
#include "MenuMessages.h"

class TMuseumMenus
{
	public:
		// Member Functions
		TMuseumMenus(BMenuBar* mbar);
		TMuseumMenus();
		~TMuseumMenus();
				
		bool 	AddToWindowsMenu(const char *documentName, int32 documentID);
		bool 	RemoveDocumentsInWindowsMenu();
		
		void 	LocateMenus();
							
		//	Accessor Functions
		void	SetMenuBar(BMenuBar *menuBar);
		
		inline BMenu *GetFileMenu(){ return fFileMenu; }
		inline BMenu *GetEditMenu(){ return fEditMenu; }
		inline BMenu *GetCueSheetMenu(){ return fCueSheetMenu; }
		inline BMenu *GetCueMenu(){ return fCueMenu; }		
		inline BMenu *GetStageMenu(){ return fStageMenu; }
		inline BMenu *GetCaptureMenu(){ return fCaptureMenu; }
		inline BMenu *GetExportMenu(){ return fExportMenu; }
		inline BMenu *GetWindowsMenu(){ return fWindowsMenu; }
	
	private:				
		// Member variables
		BMenuBar 	*fMenuBar;
		BMenu		*fFileMenu;
		BMenu		*fEditMenu;
		BMenu  		*fCueSheetMenu;
		BMenu		*fCueMenu;
		BMenu		*fStageMenu;
		BMenu		*fCaptureMenu;
		BMenu		*fExportMenu;
		BMenu		*fWindowsMenu;
		
		bool	AddFileMenu();
		bool 	AddEditMenu();
		bool 	AddCueSheetMenu();
		bool 	AddCueMenu();		
		bool 	AddStageMenu();
		bool 	AddCaptureMenu();
		bool	AddExportMenu();
		bool 	AddWindowsMenu();		
};


// Windows menu IDs
typedef enum 
{
	kWindowsStageItem = 1,
	kWindowsBrowserItem,
	kWindowsTransportItem,
	kWindowsLocatorItem,
	kWindowsMediaItem,
	kWindowsMemoryItem,
	kWindowsStageToolsItem,
	kWindowsSep01
} kWindowMenuIDs;

#endif
