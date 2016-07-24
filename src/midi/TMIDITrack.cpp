//---------------------------------------------------------------------
//
//	File:	TMIDITrack.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.25.98
//
//	Desc:	Container for MIDI track data
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "TMIDITrack.h"
#include "TCueView.h"
#include "TMIDICue.h"


// Defines for accessing elements in array
#define ITEM_OFFSET(index) ((index-1) * elementSize)
#define ITEM_PTR(index) (&(*hItems)[ ITEM_OFFSET(index) ])

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDITrack::TMIDITrack(TMIDICue* theCue)
{

	// Save parent cue
	fCue = theCue;

	fTrackName[0] = 0;
	fCurrentEvent = 0;
	fCurrentDelta = 0;

	// Set our list to NULL
	fExternalData = NULL;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//	Disposes the exended data handles
//

TMIDITrack::~TMIDITrack()
{
	// Dispose of extended data list/buffers
	if (fExternalData) {
		//fExternalData->DoForEach(DoDisposeHandle);
		delete fExternalData;
	}
}



//---------------------------------------------------------------------
//	ExtractInfo
//---------------------------------------------------------------------
//
//	Extracts initial information from the track including volume, pan,
//	program change and bank select
//


void TMIDITrack::ExtractInfo()
{
	Event* theEvent;
	int32 index = 1;
	int32 dTime, totalTime = 0;
	//char	bankStr[256];


	// First, set all values to default
	fMuted          = false;
	fDrumTrack      = false;
	fDevice                 = 2;                            // Default to general midi
	fBank                   = -1;
	fProgram                = -1;
	fChannel                = -1;
	fOctave                 = 0;
	fVolume                 = 127;
	fPan                    = -1;
	fReverb                 = -1;
	fChorus                 = -1;
	fSrcPBRange     = 2;
	fDstPBRange     = 2;
	fSrcDrumMap     = 0;
	fDstDrumMap     = 0;

	// Next, get initial track info
	theEvent = (Event*)ItemAt(index++);
	if (theEvent)
		fChannel = theEvent->d.bytes[0] & 0x0F;

	while (theEvent && theEvent->time == 0) {

		// Channel
		if ((fChannel != -1) && ((theEvent->d.bytes[0] & 0x0F) != fChannel))
			fChannel = -1;

		// Program
		if ((theEvent->d.bytes[0] & STATUS_MASK) == PROG_CHANGE) {
			fProgram = theEvent->d.bytes[1];
			RemoveItem(--index);
		}
		// Controllers
		else if ((theEvent->d.bytes[0] & STATUS_MASK) == CTRL_CHANGE) {
			if (theEvent->d.bytes[1] == VOLUME) {
				fVolume = theEvent->d.bytes[2];
				RemoveItem(--index);
			} else if (theEvent->d.bytes[1] == PAN && (fPan == -1)) {
				fPan = theEvent->d.bytes[2];
				RemoveItem(--index);
			} else if (theEvent->d.bytes[1] == BANK_CHANGE_MSB && fBank == -1) {
				fBank = theEvent->d.bytes[2] << 7;
				RemoveItem(--index);
			} else if (theEvent->d.bytes[1] == BANK_CHANGE_LSB && fBank != -1) {
				fBank |= theEvent->d.bytes[2];
				RemoveItem(--index);
			} else if (theEvent->d.bytes[1] == REVERB && (fReverb == -1)) {
				fReverb = theEvent->d.bytes[2];
				RemoveItem(--index);
			} else if (theEvent->d.bytes[1] == CHORUS && (fChorus == -1)) {
				fChorus = theEvent->d.bytes[2];
				RemoveItem(--index);
			}
		}

		theEvent = (Event*)ItemAt(index++);
	}

	while (theEvent && theEvent->time != END_OF_TRACK) {
		totalTime += theEvent->time;

		if ((theEvent->d.bytes[0] & 0x0F) != fChannel) {
			fChannel = -1;
			break;
		}

		if (((theEvent->d.bytes[0] & STATUS_MASK) == CTRL_CHANGE) && theEvent->d.bytes[1] == PAN && (fPan == -1)) {
			fPan = theEvent->d.bytes[2];
			dTime = theEvent->time;
			RemoveItem(--index);
			theEvent->time += dTime;
		}

		theEvent = (Event*)ItemAt(index++);
	}

	if (fPan == -1)
		fPan = 64;

	if (fChannel == 9)
		fDrumTrack = TRUE;

	fLastVol = fVolume;

	if (fBank == -1) {
		//GetIndString(bankStr, kDEVICE_STR + device, kBANK_NUMBER_STR);
		//StringToNum(bankStr, &dTime);
		//bank = dTime - 1;
	}
}

//---------------------------------------------------------------------
//	SetTrackName
//---------------------------------------------------------------------
//
//	Sets track name to name.
//

void TMIDITrack::SetTrackName(char* name)
{
	strcpy(fTrackName, name);
}


//---------------------------------------------------------------------
//	GetTrackName
//---------------------------------------------------------------------
//
//	Retrieve track name
//

void TMIDITrack::GetTrackName(char* name)
{
	strcpy(name, fTrackName);
}


//---------------------------------------------------------------------
//	InsertExtended
//---------------------------------------------------------------------
//
//	Insert extended MIDI data into track
//

void TMIDITrack::InsertExtended(short type, long time, ExtDataBuffer data)
{
	Event e;

	if (fExternalData == NULL)
		fExternalData = new BList();

	e.time          = time;
	e.d.data        = data;
	e.flags         = 0;
	e.extType       = type;

	AddItem(&e);

	// Append to extended data list
	fExternalData->AddItem(data);

}


//---------------------------------------------------------------------
//	SetPosition
//---------------------------------------------------------------------
//
//	Sets play pointers to where.
//

void TMIDITrack::SetPosition(long where)
{
	Event* theEvent;

	fCurrentEvent = 0;
	theEvent = (Event*)ItemAt(++fCurrentEvent);
	fCurrentDelta = theEvent->time;

	while (where > theEvent->time && theEvent->time != END_OF_TRACK) {
		where -= theEvent->time;
		theEvent = (Event*)ItemAt(++fCurrentEvent);
		fCurrentDelta = theEvent->time;
	}

	if (theEvent->time == END_OF_TRACK)
		return;

	fCurrentDelta -= where;

}


//---------------------------------------------------------------------
//	Reset
//---------------------------------------------------------------------
//
//	Sets track pointers to beginning.  (same as SetPosition(0)).
//

void TMIDITrack::Reset()
{
	Event* theEvent;

	fCurrentEvent = 1;
	theEvent = (Event*)ItemAt(1);
	fCurrentDelta = theEvent->time;
}
