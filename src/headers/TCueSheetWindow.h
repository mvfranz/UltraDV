//---------------------------------------------------------------------
//
//	File:	TCueSheetWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	CueSheet window creation and handling routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUESHEETWINDOW_H__
#define __TCUESHEETWINDOW_H__

// Includes
#include "TCueSheetPrefs.h"
#include "TAudioEngine.h"

// Class definitions
class TCueSheetWindow: public BWindow
{		
	friend class TCueSheetView;
	
	public:								
		TCueSheetWindow(BRect bounds, int32 id);
		TCueSheetWindow(BMessage *data);
		~TCueSheetWindow();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void	MessageReceived(BMessage* message);			
		void 	FrameResized( float newWidth, float newHeight);		
		void 	MenusBeginning();		
		void 	AdjustScrollBars();
		void	Hide();
		void	Show();
		void 	WindowActivated( bool active);
		bool 	QuitRequested();
		bool 	MyQuitRequested();
		
		// Clipboard handling
		void 	Undo();
		void 	Cut();
		void 	Copy();
		void 	Paste();
		void 	Clear();
		void 	SelectAll();
		
		// Utility Functions
		void	SetPanelOpen(bool theVal);
		
		// Accessor Functions
		inline TCueSheetPrefs		*GetCueSheetPrefs(){ return &m_CueSheetPrefs; }
		inline TPlaybackEngine		*GetPlaybackEngine(){ return m_PlaybackEngine; }
		inline TAudioEngine			*GetAudioEngine(){ return m_AudioEngine; }
		inline TStageWindow			*GetStage(){ return m_StageWindow; }
		inline THeaderContainerView *GetHeaderContainer() { return m_HeaderContainer; }
		inline TCueSheetView 		*GetCueSheetView() { return m_CueSheetView; }
		inline TExportTimeView 		*GetExportTimeView() { return m_ExportTimeView; }
		inline TExportZone 			*GetExportZone() { return m_ExportZone; }
		inline TTimelineView 		*GetTimeline() { return m_Timeline; }
		inline TCueSheetScrollBarH 	*GetScrollH() { return m_HScroll; }
		inline TCueSheetScrollBarV 	*GetScrollV() { return m_VScroll; }
		inline	bool				IsPanelOpen(){ return m_PanelOpen; }
		inline	int32				GetID(){ return m_ID; }
		inline	void				SetID(int32 theID){  m_ID = theID; }
		inline	TMuseumMenus		*GetMenu(){ return m_AppMenu; }
		inline	BFile				*GetFile(){ return m_File; } 
		
		// Member Variables
							
	private:				
		// Member variables
		TUndoEngine				*m_UndoEngine;
		TPlaybackEngine			*m_PlaybackEngine;
		TAudioEngine			*m_AudioEngine;
		TCueSheetPrefs	 		m_CueSheetPrefs;					
		TStageWindow			*m_StageWindow;
		THeaderContainerView 	*m_HeaderContainer;
		TToolbar			 	*m_Toolbar;		
		TCueSheetTimeView	 	*m_TimeZone;
		TExportTimeView			*m_ExportTimeView;
		TExportZone				*m_ExportZone;
		TTimelineView		 	*m_Timeline;
		TCueSheetView 		 	*m_CueSheetView;
		TCueSheetScrollBarH	 	*m_HScroll;
		TCueSheetScrollBarV	 	*m_VScroll;
		TTimeScalerView		 	*m_TimeScaler;
		TMuseumMenus		 	*m_AppMenu;
		
		int32					m_ID;		
		BFile					*m_File;
		bool					m_PanelOpen;
		
		// Member functions
		void	Init();	
				
		void	AdjustFileMenu();
		void	AdjustEditMenu();
		void	AdjustCueSheetMenu();
		void	AdjustCueMenu();
		void	AdjustStageMenu();
		void 	AdjustWindowsMenu()	;
		
		//	File IO
		void 	Save(BMessage *message);
		void 	WriteHeader(BFile *theFile);
		
};

#endif
