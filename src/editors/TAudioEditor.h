//---------------------------------------------------------------------
//
//	File:	TAudioEditor.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.18.98
//
//	Desc:	Audio Cue editor.  Opens cue into a window that displays it's 
//			waveform and allows the user to perform some actions.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TAUIOEDITORVIEW_H__
#define __TAUIOEDITORVIEW_H__

// Constants
const float kAudioToolbarHeight = 23;
const float kAudioEditorHeight 	= 300;


//	Includes
#include <SoundPlayer.h>
#include <Sound.h>

// Forward declarations
class TAudioCue;
class TAudioEditorView;
class TAudioEditorToolbar;
class TAudioTimelineView;
class TAudioEngine;
class TAudioScalerView;
class TAudioEditorMenus;

// Class definitions
class TAudioEditor: public BWindow
{
	public:
		TAudioEditor(BRect bounds, entry_ref *ref, TAudioCue *theCue);
		~TAudioEditor();
		
		void		MessageReceived(BMessage* message);	
		
		bool 		QuitRequested();
		
		// Accessor Functions
		inline 	TAudioTimelineView *GetTimeline(){ return fTimeline; }	
		inline 	TAudioEditorView *GetEditorView(){ return fEditorView; }	
		
	private:	
		friend class TAudioEditorView;
		
		// Member Functions
		void 	HandleFileMenu(BMessage *message);
		void 	HandleEditMenu(BMessage *message);
		
		// Member Variables
		TAudioEditorMenus	*fEditorMenu;
		bool				fNewFile;
		bool				fIsClosing;
							
		TAudioCue			*fAudioCue;
		BView				*fBackground;
		TAudioEditorToolbar	*fToolbar;
		TAudioTimelineView	*fTimeline;
		TAudioEditorView	*fEditorView;
		
		BScrollBar			*fVScroll;
		BScrollBar			*fHScroll;
		
		TAudioScalerView	*fAudioScaler;
		
		entry_ref			fFileRef;
		BFilePanel			*fFileSavePanel;
			
		// Sound Data Variables
		BSound			*fSound;					
		int32 			fCaretTime;
		bool			fIsPlaying;
		int32			fStartTime;
		int32			fSamplesPerPixel;	
		int32			fSampleRate;
		int32			fNumSamples;		// Number of sample frames
		int16			fNumChannels;
		int16			fSampleSize;		// 8 or 16 bit
		int32			fSDataOffset;		// offset into file for sample data
		int32			fSelectStart;		// Selection points in samples frames
		int32			fSelectEnd;
			
		// Member Functions
		void	Init();		
		void	InitSoundfileAttributes();
		
		void 	ShowFileSavePanel();
		int32 	SaveAlert();
		void 	Save(BMessage *message);
};

#endif
