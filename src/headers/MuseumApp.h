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
		inline TCueSheetWindow		*GetCueSheet(){ return m_CueSheetWindow; }
		inline void					SetCueSheet( TCueSheetWindow *theCueSheet){ m_CueSheetWindow = theCueSheet; }
		inline BList				*GetCueSheetList(){ return m_CueSheetList; }
		inline TActionCuePalette	*GetActionPalette(){ return m_ActionCuePalette; }
		inline TMediaCuePalette		*GetMediaPalette(){ return m_MediaCuePalette; }
		inline TTimePalette 		*GetTimePalette(){ return m_TimePalette; }
		inline TTransportPalette 	*GetTransport(){ return m_TransportPalette; }
		inline TBrowserWindow 		*GetBrowser(){ return m_BrowserWindow; }
		inline TStageTools			*GetStageTools(){ return m_StageTools; }
				
		BMediaRoster 				*GetMediaRoster(){ return m_MediaRoster; }
		BTranslatorRoster 			*GetTranslator(){ return m_TranslationRoster; }
								
		// Member variables
		//
		
		BMediaRoster 		*m_MediaRoster;		//	Conect to media_server node roster
		
		TMuseumClipboard	*m_Clipboard;		// Interface to be_clipboard
		
		bool				m_IsCueDrag;		// Set to true when cue is being dragged
		int32				m_CueDragDelta;		// Set to delta when cue is being dragged
		
		TMuseumIcons		*m_MuseumIcons;		// Global object storing common app bitmaps
		
		// Cursors
		Cursor 	m_HandOpenCursor;
		Cursor 	m_HandClosedCursor;
		Cursor 	m_CropCursor;
		Cursor 	m_EyedropperCursor;
		Cursor 	m_MoveCursor;
		Cursor 	m_PencilCursor;
		Cursor 	m_ResizeDiagLeftCursor;
		Cursor 	m_ResizeDiagRightCursor;
		Cursor 	m_ResizeHorzCursor;		
		Cursor 	m_ResizeVertCursor;
		Cursor 	m_SizeHorzCursor;
		Cursor 	m_SizeVertCursor;
		Cursor 	m_ZoomInCursor;
		Cursor 	m_ZoomOutCursor;		
		Cursor 	m_RotateCursor;
		Cursor 	m_SizeLeftRightCursor;
		Cursor 	m_SizeTopBottomCursor;
		Cursor 	m_SizeTopLeftBottomCursor;
		Cursor 	m_SizeTopRightBottomCursor;
		
	private:		
		TMuseumPrefs		*m_Prefs;
		TCueSheetWindow		*m_CueSheetWindow;
		TActionCuePalette	*m_ActionCuePalette;		
		TMediaCuePalette	*m_MediaCuePalette;		
		TTimePalette		*m_TimePalette;		
		TTransportPalette	*m_TransportPalette;
		TBrowserWindow		*m_BrowserWindow;
		TStageTools			*m_StageTools;		
		
		BList				*m_CueSheetList;
		
		BTranslatorRoster 	*m_TranslationRoster;
		
		// Initialization Functions
		void		Init();
		
		void		ArrangeWindows();
		
		// File Panels
		BFilePanel	*m_FileOpenPanel;
		BFilePanel	*m_FileSavePanel;
				
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
