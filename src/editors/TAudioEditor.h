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
		inline 	TAudioTimelineView *GetTimeline(){ return m_Timeline; }	
		inline 	TAudioEditorView *GetEditorView(){ return m_EditorView; }	
		
	private:	
		friend class TAudioEditorView;
		
		// Member Functions
		void 	HandleFileMenu(BMessage *message);
		void 	HandleEditMenu(BMessage *message);
		
		// Member Variables
		TAudioEditorMenus	*m_EditorMenu;
		bool				m_NewFile;
		bool				m_IsClosing;
							
		TAudioCue			*m_AudioCue;
		BView				*m_Background;
		TAudioEditorToolbar	*m_Toolbar;
		TAudioTimelineView	*m_Timeline;
		TAudioEditorView	*m_EditorView;
		
		BScrollBar			*m_VScroll;
		BScrollBar			*m_HScroll;
		
		TAudioScalerView	*m_AudioScaler;
		
		entry_ref			m_FileRef;
		BFilePanel			*m_FileSavePanel;
			
		// Sound Data Variables
		BSound			*m_Sound;					
		int32 			m_CaretTime;
		bool			m_IsPlaying;
		int32			m_StartTime;
		int32			m_SamplesPerPixel;	
		int32			m_SampleRate;
		int32			m_NumSamples;		// Number of sample frames
		int16			m_NumChannels;
		int16			m_SampleSize;		// 8 or 16 bit
		int32			m_SDataOffset;		// offset into file for sample data
		int32			m_SelectStart;		// Selection points in samples frames
		int32			m_SelectEnd;
			
		// Member Functions
		void	Init();		
		void	InitSoundfileAttributes();
		
		void 	ShowFileSavePanel();
		int32 	SaveAlert();
		void 	Save(BMessage *message);
};

#endif
