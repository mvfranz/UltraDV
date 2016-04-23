//---------------------------------------------------------------------
//
//	File:	TMIDICue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header file
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TMIDICUE_H__
#define __TMIDICUE_H__

#include "MuseumMidiDefs.h"

// Forward Declaration
class TMIDIEditor;

// Class Definition
class	TMIDICue : public TCueView
{
	public:
		// Member Functions
		TMIDICue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TMIDICue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime);
		~TMIDICue();
		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);		
		void 	MessageReceived(BMessage *message);
								
		// Accessor Functions
		inline TMIDIFile *GetMidiFile(){ return m_MIDIFile; }
		
		// Member Variables
		
		
	private:
		// Member Functions
		void 	Init();
		bool 	LoadMIDIFile(BMessage *message);
		void 	OpenEditor();
		
		void 	LoadCueIcon();
				
		// MIDI playback thread functions
		static int32 	play_midi_file(void *arg);
		int32 			PlayMidiFile();
		void 			PlayCue(uint32 theTime);
		void 			StopCue(uint32 theTime);
		void 			PauseCue();
		void 			ResumeCue();
		static void 	CompletionHook(int32 arg);

	protected:
				
		// Member Variables
		void 	ShowPanel();
		void	HidePanel();
		
		void 	CalcDuration();
		
		// Member Variables		
		TMIDIEditor		*m_Editor;	
		BMidiPort		*m_MIDIPort;
		BMidiStore		*m_MIDIStore;	
		BList			*m_TrackList;
		
		TMIDIConductor	*m_MIDIConductor;
		TMIDIFile		*m_MIDIFile;
		TMIDIEngine		*m_MIDIEngine;
		BMidiSynthFile 	*m_SynthFile; 
		
		thread_id		m_PlaybackThread;
					
};

#endif
