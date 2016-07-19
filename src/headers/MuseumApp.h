//-------------------------------------------------------------------
//
//	File:	TMuseumApp.h
//
//	Author:	Gene Z. Ragan
//
//	Data:	01.29.98
//
//-------------------------------------------------------------------

#if  !defined(__TMUSEUMAPP_H__)
#define __TMUSEUMAPP_H__

#include "BuildApp.h"

#include "TCursor.h"

// Class declaratons
class MuseumApp : public BApplication
{
	public:
	
		MuseumApp();
		~MuseumApp();
		virtual void	ReadyToRun();
		virtual void	MessageReceived(BMessage* message);
		void			AboutRequested();
		bool			QuitRequested();
		void			RefsReceived(BMessage *a_message);
		void ShowFileOpenPanel();	// ABH  make it public
		
		// Accessor methods		
		inline TCueSheetWindow		*GetCueSheet(){ return fCueSheetWindow; }
		inline void					SetCueSheet( TCueSheetWindow *theCueSheet){ fCueSheetWindow = theCueSheet; }
		inline BList				*GetCueSheetList(){ return fCueSheetList; }
		inline TActionCuePalette	*GetActionPalette(){ return fActionCuePalette; }
		inline TMediaCuePalette		*GetMediaPalette(){ return fMediaCuePalette; }
		inline TTimePalette 		*GetTimePalette(){ return fTimePalette; }
		inline TTransportPalette 	*GetTransport(){ return fTransportPalette; }
		inline TBrowserWindow 		*GetBrowser(){ return fBrowserWindow; }
		inline TStageTools			*GetStageTools(){ return fStageTools; }
				
		BMediaRoster 				*GetMediaRoster(){ return fMediaRoster; }
		BTranslatorRoster 			*GetTranslator(){ return fTranslationRoster; }
								
		// Member variables
		//
		
		BMediaRoster 		*fMediaRoster;		//	Conect to media_server node roster
		
		TMuseumClipboard	*fClipboard;		// Interface to be_clipboard
		
		bool				fIsCueDrag;		// Set to true when cue is being dragged
		int32				fCueDragDelta;		// Set to delta when cue is being dragged
		
		TMuseumIcons		*fMuseumIcons;		// Global object storing common app bitmaps
		
		// Cursors
		Cursor 	fHandOpenCursor;
		Cursor 	fHandClosedCursor;
		Cursor 	fCropCursor;
		Cursor 	fEyedropperCursor;
		Cursor 	fMoveCursor;
		Cursor 	fPencilCursor;
		Cursor 	fResizeDiagLeftCursor;
		Cursor 	fResizeDiagRightCursor;
		Cursor 	fResizeHorzCursor;		
		Cursor 	fResizeVertCursor;
		Cursor 	fSizeHorzCursor;
		Cursor 	fSizeVertCursor;
		Cursor 	fZoomInCursor;
		Cursor 	fZoomOutCursor;		
		Cursor 	fRotateCursor;
		Cursor 	fSizeLeftRightCursor;
		Cursor 	fSizeTopBottomCursor;
		Cursor 	fSizeTopLeftBottomCursor;
		Cursor 	fSizeTopRightBottomCursor;
		
	private:		
		TMuseumPrefs		*fPrefs;
		TCueSheetWindow		*fCueSheetWindow;
		TActionCuePalette	*fActionCuePalette;		
		TMediaCuePalette	*fMediaCuePalette;		
		TTimePalette		*fTimePalette;		
		TTransportPalette	*fTransportPalette;
		TBrowserWindow		*fBrowserWindow;
		TStageTools			*fStageTools;		
		
		BList				*fCueSheetList;
		
		BTranslatorRoster 	*fTranslationRoster;
		
		// Initialization Functions
		void		Init();
		
		void		ArrangeWindows();
		
		// File Panels
		BFilePanel	*fFileOpenPanel;
		BFilePanel	*fFileSavePanel;
				
		// Dialog Routines
		void	AddChannelDialog();
		void	DeleteChannelDialog();
		void 	StageSetupDialog();
		void 	VideoCaptureDialog();
		void 	VideoSettingsDialog();
		void 	AudioCaptureDialog();
		void 	AudioSettingsDialog();				
		void	RecordPrefsDialog();
		
		// Menu Handling
		void	HandleFileMenu(BMessage *message);
		void	HandleEditMenu(BMessage *message);
		void	HandleCueSheetMenu(BMessage *message);
		void	HandleCueMenu(BMessage *message);
		void	HandleCaptureMenu(BMessage *message);
		void 	HandleStageMenu(BMessage *message);
		void	HandleWindowsMenu(BMessage *message);
		
		void 	UpdateWindowsMenuDocs();
		
		// File IO
//		void 	ShowFileOpenPanel();
		void 	ShowFileSavePanel();
		
		// Add-ons
		void 	LoadCueAddOns();
		
		//	Cursor
		void 	LoadCursors();
		void	FreeCursors();
		
		
};


#endif
