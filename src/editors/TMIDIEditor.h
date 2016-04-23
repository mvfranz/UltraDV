//---------------------------------------------------------------------
//
//	File:	TMIDIEditor.h
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

#ifndef __TMIDIEditor__
#define __TMIDIEditor__
#include <interface/Window.h>

// Constants
const float kMIDIEditorHeight 	= 300;


// Forward declarations
class 	TMIDICue;
class	TMIDIEditorView;

// Class definitions
class TMIDIEditor: public BWindow
{
	public:
		TMIDIEditor(BRect bounds, TMIDICue *theCue);
		~TMIDIEditor();
		
		void		MessageReceived(BMessage* message);	
		void		Refresh();	
		
		// Accessor Functions
		inline 	TMIDIEditorView *GetEditorView(){ return m_EditorView; }	
		
	private:			
		// Member Variables
		TMIDICue			*m_MIDICue;
		BView				*m_Background;
		TMIDIEditorView		*m_EditorView;
		//TMIDIMixerView	*m_MIDIMixerView;
		BScrollBar			*m_HScroll;
		
		int32				m_TotalTracks;
					
		// Member Functions
		void		Init();		
};

#endif
