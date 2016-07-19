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

#include        <app/Application.h>
// ABH #include	<media/Subscriber.h>
#include        <support/Debug.h>
#include        <support/ByteOrder.h>


#include "TMIDIEngine.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEngine::TMIDIEngine(BMidiStore *midiStore)
{
	// Create engine's MIDI port and save BMIDIStore
	fMIDIPort = new BMidiPort();
	fMIDIStore = midiStore;

	// Open MIDI port
	fMIDIPort->Open("midi2");

	// Connect the output of the MIDIStore to the input of the MIDIPort.
	fMIDIStore->Connect(fMIDIPort);


}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIEngine::~TMIDIEngine()
{
	//  Disconnect store from port
	fMIDIPort->Disconnect(fMIDIStore);

	// Free MIDIPort
	delete fMIDIPort;

}


//---------------------------------------------------------------------
//	Play
//---------------------------------------------------------------------
//
//	Begin playback of MIDI data
//

void TMIDIEngine::Play()
{
	if ( !fMIDIStore->IsRunning() )
		fMIDIStore->Start();
}


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop playback of MIDI data
//

void TMIDIEngine::Stop()
{
	if ( fMIDIStore->IsRunning() )
		fMIDIStore->Stop();
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Pause playback of MIDI data
//

void TMIDIEngine::Pause()
{
	if ( fMIDIStore->IsRunning() )
		fMIDIStore->Stop();
}

