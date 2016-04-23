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
		
		inline BMenu *GetFileMenu(){ return m_FileMenu; }
		inline BMenu *GetEditMenu(){ return m_EditMenu; }
		inline BMenu *GetCueSheetMenu(){ return m_CueSheetMenu; }
		inline BMenu *GetCueMenu(){ return m_CueMenu; }		
		inline BMenu *GetStageMenu(){ return m_StageMenu; }
		inline BMenu *GetCaptureMenu(){ return m_CaptureMenu; }
		inline BMenu *GetExportMenu(){ return m_ExportMenu; }
		inline BMenu *GetWindowsMenu(){ return m_WindowsMenu; }
	
	private:				
		// Member variables
		BMenuBar 	*m_MenuBar;
		BMenu		*m_FileMenu;
		BMenu		*m_EditMenu;
		BMenu  		*m_CueSheetMenu;
		BMenu		*m_CueMenu;
		BMenu		*m_StageMenu;
		BMenu		*m_CaptureMenu;
		BMenu		*m_ExportMenu;
		BMenu		*m_WindowsMenu;
		
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
