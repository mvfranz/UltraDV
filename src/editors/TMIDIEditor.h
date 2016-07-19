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
		inline 	TMIDIEditorView *GetEditorView(){ return fEditorView; }	
		
	private:			
		// Member Variables
		TMIDICue			*fMIDICue;
		BView				*fBackground;
		TMIDIEditorView		*fEditorView;
		//TMIDIMixerView	*fMIDIMixerView;
		BScrollBar			*fHScroll;
		
		int32				fTotalTracks;
					
		// Member Functions
		void		Init();		
};

#endif
