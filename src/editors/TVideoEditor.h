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
		inline TVideoEditorView *GetVideoEditorView(){ return fEditorView; }
		inline bool				IsPlaying(){ return fIsPlaying; }
		inline BBitmap			*GetDisplayBitmap(){ return fDisplayBitmap; }
		
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
		TVideoEditorMenus           *fEditorMenu;
		BView						*fBGView;
		BView						*fControlsBGView;
		TVideoEditorView			*fEditorView;
		TVideoEditorControlsView	*fControlsView;
		TVideoEditorText			*fInText;
		TVideoEditorText			*fOutText;
		TVideoEditorText			*fDurationText;
		TVideoEditorText			*fTimeText;
		TVideoEditorTransport		*fTransport;
		TVideoEditorShuttle			*fShuttle;
		
		TBitmapButton				*fInButton;
		TBitmapButton				*fOutButton;
		TBitmapButton				*fStartButton;
		TBitmapButton				*fVolumeButton;
		TBitmapButton				*fEndButton;
		TBitmapButton				*fFrameForwardButton;
		TBitmapButton				*fFrameBackButton;
		TBitmapButton				*fPlayButton;
		TBitmapButton				*fLoopButton;
		TBitmapButton				*fMarkButton;
		TBitmapButton				*fGotoButton;
		TBitmapButton				*fDetachButton;
			
		bool			fIsPlaying;	
		bool			fIsLooping;
		entry_ref		fFileRef;
		
		TMovieCue		*fMovieCue;		
		BBitmap 		*fDisplayBitmap;
		BFilePanel		*fFileSavePanel;
		bool			fNewFile;
		bool			fIsClosing;
				
		TRIFFReader		*fReader;
		TAudioCodec 	*fAudioCodec;
		TVideoCodec 	*fVideoCodec;

		uint32			fCurrentVideoFrame;
		uint32			fCurrentTime;
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

