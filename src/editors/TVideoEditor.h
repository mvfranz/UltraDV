//---------------------------------------------------------------------
//
//	File:	TVideoEditor.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.31.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOEDITOR_H__
#define __TVIDEOEDITOR_H__

// Includes

// Constants
const float kVidEditViewWidth 	 = 319;
const float kVidEditViewHeight	 = 239;

const BRect kVideoEditorBounds(0, 0, kVidEditViewWidth, kVidEditViewHeight);

// Forward Declarations
class TVideoEditorView;
class TVideoEditorControlsView;
class TBitmapButton;
class TVideoEditorText;
class TVideoEditorMenus;
class TVideoEditorShuttle;
class TVideoEditorTransport;
class TRIFFReader;
class TAudioCodec;
class TVideoCodec;

// Class Definition
class TVideoEditor: public BWindow
{
	public:
		// Member Functions
		TVideoEditor(entry_ref *theRef, TMovieCue *theCue, BMessage *message);
		~TVideoEditor();
		
		void	MessageReceived(BMessage* message);	
		bool 	QuitRequested();
		
		// Accessor Functions
		inline TVideoEditorView *GetVideoEditorView(){ return m_EditorView; }
		inline bool				IsPlaying(){ return m_IsPlaying; }
		inline BBitmap			*GetDisplayBitmap(){ return m_DisplayBitmap; }
		
	private:	
		// Member Functions
		void	Init();	
		
		void	Record();
		void 	Play();
		void 	Stop();
		void 	Pause();
		void	StepBack();
		void	StepForward();
		void	SeekStart();
		void	SeekEnd();
		void	SetInPoint();
		void	SetOutPoint();
		void	SetLooping();
		
		static int32 	_playback(void *arg);
		int32 			Playback();
		
		// Utility Routines
		void 			SetupTextItems();
		void 			SetupControlButtons();
		void 			InitMovieAttributes();
		
		void 			ShowFileSavePanel();
		int32 			SaveAlert();
		void 			Save(BMessage *message);
		
		// Menu Handling
		void 			HandleFileMenu(BMessage *message);
		void 			HandleEditMenu(BMessage *message);
		
		//	Utility
		bool 			LoadAndDecode(uint32 theFrame, BBitmap *bitmap);
		
		// Member Variables	
		TVideoEditorMenus           *m_EditorMenu;
		BView						*m_BGView;
		BView						*m_ControlsBGView;
		TVideoEditorView			*m_EditorView;
		TVideoEditorControlsView	*m_ControlsView;
		TVideoEditorText			*m_InText;
		TVideoEditorText			*m_OutText;
		TVideoEditorText			*m_DurationText;
		TVideoEditorText			*m_TimeText;
		TVideoEditorTransport		*m_Transport;
		TVideoEditorShuttle			*m_Shuttle;
		
		TBitmapButton				*m_InButton;
		TBitmapButton				*m_OutButton;
		TBitmapButton				*m_StartButton;
		TBitmapButton				*m_VolumeButton;
		TBitmapButton				*m_EndButton;
		TBitmapButton				*m_FrameForwardButton;
		TBitmapButton				*m_FrameBackButton;
		TBitmapButton				*m_PlayButton;
		TBitmapButton				*m_LoopButton;
		TBitmapButton				*m_MarkButton;
		TBitmapButton				*m_GotoButton;
		TBitmapButton				*m_DetachButton;
			
		bool			m_IsPlaying;	
		bool			m_IsLooping;
		entry_ref		m_FileRef;
		
		TMovieCue		*m_MovieCue;		
		BBitmap 		*m_DisplayBitmap;
		BFilePanel		*m_FileSavePanel;
		bool			m_NewFile;
		bool			m_IsClosing;
				
		TRIFFReader		*m_Reader;
		TAudioCodec 	*m_AudioCodec;
		TVideoCodec 	*m_VideoCodec;

		uint32			m_CurrentVideoFrame;
		uint32			m_CurrentTime;
};


// Editor messages
#define VE_IN_MSG		'veIN'
#define VE_OUT_MSG		'veOT'
#define VE_VOLUME_MSG	'veVL'
#define VE_PLAY_MSG		'vePL'
#define VE_LOOP_MSG		'veLP'
#define VE_STOP_MSG		'veST'
#define VE_FF_MSG		'veFF'
#define VE_FB_MSG		'veFB'
#define VE_START_MSG	'veSR'
#define VE_END_MSG		'veEN'
#define VE_MARK_MSG		'veMA'
#define VE_JUMP_MSG		'veJU'
#define VE_DETACH_MSG	'veDT'

#endif

