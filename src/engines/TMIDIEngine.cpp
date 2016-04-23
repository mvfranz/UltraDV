//---------------------------------------------------------------------
//
//	File:	TMIDIEngine.cpp
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

// Includes
#include "BuildApp.h"

#include 	<app/Application.h>
// ABH #include	<media/Subscriber.h>
#include 	<support/Debug.h>
#include 	<support/ByteOrder.h>


#include "TMIDIEngine.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEngine::TMIDIEngine(BMidiStore *midiStore)
{		
	// Create engine's MIDI port and save BMIDIStore
	m_MIDIPort = new BMidiPort(); 
   	m_MIDIStore = midiStore;
   	
   	// Open MIDI port
	m_MIDIPort->Open("midi2");
   
	// Connect the output of the MIDIStore to the input of the MIDIPort. 
	m_MIDIStore->Connect(m_MIDIPort);
	

}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIEngine::~TMIDIEngine()
{
	//  Disconnect store from port
	m_MIDIPort->Disconnect(m_MIDIStore);		
	
	// Free MIDIPort
	delete m_MIDIPort;
	 	
}


//---------------------------------------------------------------------
//	Play
//---------------------------------------------------------------------
//
//	Begin playback of MIDI data
//

void TMIDIEngine::Play()
{
	if ( !m_MIDIStore->IsRunning() )		 	
		m_MIDIStore->Start();
}	


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop playback of MIDI data
//

void TMIDIEngine::Stop()
{
	if ( m_MIDIStore->IsRunning() )		 	
		m_MIDIStore->Stop();
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Pause playback of MIDI data
//

void TMIDIEngine::Pause()
{
	if ( m_MIDIStore->IsRunning() )		 	
		m_MIDIStore->Stop();
}	

