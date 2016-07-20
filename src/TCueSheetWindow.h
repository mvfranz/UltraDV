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
		inline TCueSheetPrefs		*GetCueSheetPrefs(){ return &fCueSheetPrefs; }
		inline TPlaybackEngine		*GetPlaybackEngine(){ return fPlaybackEngine; }
		inline TAudioEngine			*GetAudioEngine(){ return fAudioEngine; }
		inline TStageWindow			*GetStage(){ return fStageWindow; }
		inline THeaderContainerView *GetHeaderContainer() { return fHeaderContainer; }
		inline TCueSheetView 		*GetCueSheetView() { return fCueSheetView; }
		inline TExportTimeView 		*GetExportTimeView() { return fExportTimeView; }
		inline TExportZone 			*GetExportZone() { return fExportZone; }
		inline TTimelineView 		*GetTimeline() { return fTimeline; }
		inline TCueSheetScrollBarH 	*GetScrollH() { return fHScroll; }
		inline TCueSheetScrollBarV 	*GetScrollV() { return fVScroll; }
		inline	bool				IsPanelOpen(){ return fPanelOpen; }
		inline	int32				GetID(){ return fID; }
		inline	void				SetID(int32 theID){  fID = theID; }
		inline	TMuseumMenus		*GetMenu(){ return fAppMenu; }
		inline	BFile				*GetFile(){ return fFile; } 
		
		// Member Variables
							
	private:				
		// Member variables
		TUndoEngine				*fUndoEngine;
		TPlaybackEngine			*fPlaybackEngine;
		TAudioEngine			*fAudioEngine;
		TCueSheetPrefs	 		fCueSheetPrefs;					
		TStageWindow			*fStageWindow;
		THeaderContainerView 	*fHeaderContainer;
		TToolbar			 	*fToolbar;		
		TCueSheetTimeView	 	*fTimeZone;
		TExportTimeView			*fExportTimeView;
		TExportZone				*fExportZone;
		TTimelineView		 	*fTimeline;
		TCueSheetView 		 	*fCueSheetView;
		TCueSheetScrollBarH	 	*fHScroll;
		TCueSheetScrollBarV	 	*fVScroll;
		TTimeScalerView		 	*fTimeScaler;
		TMuseumMenus		 	*fAppMenu;
		
		int32					fID;		
		BFile					*fFile;
		bool					fPanelOpen;
		
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
