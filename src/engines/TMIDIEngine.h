//---------------------------------------------------------------------
//
//	File:	TMIDIEngine.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.24.98
//
//	Desc:	MIDI playback engine
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TMIDIEngine__
#define __TMIDIEngine__

// Class Definitions
class TMIDIEngine
{ 
	
	public: 	
		// Member Functions
		TMIDIEngine(BMidiStore *midiStore);
		~TMIDIEngine();
		
		void	Play();
		void	Stop();
		void	Pause();
		
	private: 		
		// Member Functions
		BMidiStore	*m_MIDIStore;
		BMidiPort	*m_MIDIPort;
      		
};

#endif
