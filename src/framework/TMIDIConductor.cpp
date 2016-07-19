//---------------------------------------------------------------------
//
//	File:	TMIDIConductor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.25.98
//
//	Desc:	MIDI conductor class
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
#include "TMIDIConductor.h"


#define ITEM_OFFSET(index) ((index-1) * elementSize)
#define ITEM_PTR(index) (&(*hItems)[ ITEM_OFFSET(index) ])


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIConductor::TMIDIConductor(TMIDICue *parent)
{
	// Save parent cue
	fParent = parent;

	// Current delta
	fCurrentDelta = 0;

	// Absolute ticks into file
	fCurrentLocation = 0;

	fTotalTime      = 0;
	fPlayTime               = 0;
	fLastTempo      = 0;
	fTempoRatio     = kTempoDivisor;

	// Allocate marker list
	fMarkerList = new BList();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIConductor::~TMIDIConductor()
{
	// Clean up event list
	for (int32 index = 0; index < CountItems(); index++) {
		ConductorEvent *theEvent = (ConductorEvent *)ItemAt(index);

		if (theEvent)
			free(theEvent);
	}

	// Clean up marker list
	for (int32 index = 0; index < fMarkerList->CountItems(); index++) {
		MarkerPtr theMarker = (MarkerPtr)fMarkerList->ItemAt(index);

		if (theMarker)
			free(theMarker);
	}

}

//---------------------------------------------------------------------
//	InsertTempo
//---------------------------------------------------------------------
//
//	Inserts tempo at the end of the conductor track.
//

void TMIDIConductor::InsertTempo(long tempo, long time)
{
	ConductorEvent  *theEvent;

	// Allocate event
	theEvent = (ConductorEvent *)malloc( sizeof(ConductorEvent) );

	fCurrentDelta += time;
	fCurrentLocation += time;

	theEvent->time = fCurrentDelta;
	theEvent->type = tempoChange;
	theEvent->d.tempo = tempo;

	fCurrentDelta = 0;

	AddItem(theEvent);
}


//---------------------------------------------------------------------
//	InsertMeter
//---------------------------------------------------------------------
//
//	Inserts meter at end of the conductor track.
//

void TMIDIConductor::InsertMeter(short num, short den, long time)
{
	ConductorEvent  *theEvent;

	// Allocate event
	theEvent = (ConductorEvent *)malloc( sizeof(ConductorEvent) );

	fCurrentDelta += time;
	fCurrentLocation += time;

	theEvent->time = fCurrentDelta;
	theEvent->type = meterChange;
	theEvent->d.meter.num = num;
	theEvent->d.meter.den = den;

	fCurrentDelta = 0;

	AddItem(theEvent);

}


//---------------------------------------------------------------------
//	SetStartTime
//---------------------------------------------------------------------
//
//	Converts sTime (SMPTE) to milliseconds and sets fStartTime.
//

void TMIDIConductor::SetStartTime(long sTime, long time)
{
	short format, hours, minutes, seconds, frames;
	long startTime;

	fCurrentDelta           += time;
	fCurrentLocation        += time;

	format = (sTime & 0x60000000) >> 29;

	switch (format)
	{
	case 0:
		format = 24;
		break;

	case 1:
		format = 25;
		break;

	case 2:
		format = 29;
		break;

	case 3:
		format = 30;
		break;
	}

	sTime &= ~0x60000000;

	hours = (sTime >> 24) & 0xFF;
	minutes = (sTime >> 16) & 0xFF;
	seconds = (sTime >> 8) & 0xFF;
	frames = sTime & 0xFF;

	startTime = hours * 3600;
	startTime += minutes * 60;
	startTime += seconds;
	startTime *= 1000;
	startTime += (frames * 1000 / format);

}



//---------------------------------------------------------------------
//	StartTime
//---------------------------------------------------------------------
//
//	returns fStartTime.
//

long TMIDIConductor::StartTime(void)
{
	return fParent->StartTime();
}


//---------------------------------------------------------------------
//	SetEndTime
//---------------------------------------------------------------------
//
//	Sets fPlayTime to eTime (total ticks in song), also inserts an
//	endOfTrack event into the conductor track.
//

void TMIDIConductor::SetEndTime(long eTime)
{
	ConductorEvent  *theEvent;

	// Allocate event
	theEvent = (ConductorEvent *)malloc( sizeof(ConductorEvent) );

	fCurrentDelta += eTime;
	fCurrentLocation += eTime;

	theEvent->time = fCurrentDelta;
	theEvent->type = endOfTrack;

	fCurrentDelta = 0;

	fTotalTime = eTime;

	AddItem(theEvent);

}



//---------------------------------------------------------------------
//	Reset
//---------------------------------------------------------------------
//
//	Resets play pointers to beginning. Same as LocateToMSec(0).
//

void TMIDIConductor::Reset()
{
	ConductorEvent  *e;

	fCurrentEvent = 1;
	e = (ConductorEvent *)ItemAt(1);
	fCurrentDelta = e->time;

	fPlayTime = fTotalTime;

}


//---------------------------------------------------------------------
//	GetStartDelta
//---------------------------------------------------------------------
//
//	Returns number of ticks from beginning of song to mSecLocation.
//	mSecLocation is relative to start time.
//


long TMIDIConductor::GetStartDelta(long mSecLocation)
{
	long pos = 0;
	short done = FALSE;
	long uSecLocation, uSecsPerTick;
	long nextTempo = 0;
	long theIndex;
	ConductorEvent  *e;

	uSecLocation = 0;
	theIndex = 1;
	mSecLocation *= 1000;                   // Convert to uSecs

	while (!done) {
		if (nextTempo) {
			uSecsPerTick = nextTempo;
			nextTempo = 0;
		}
		e = (ConductorEvent *)ItemAt(theIndex++);

		if (e->type == tempoChange)
			nextTempo = e->d.tempo;

		uSecLocation += (e->time * uSecsPerTick);
		pos += e->time;

		if (uSecLocation >= mSecLocation)
			done = TRUE;

		if (e->type == endOfTrack)
			done = TRUE;
	}

	if (nextTempo) {
		uSecsPerTick = nextTempo;
		nextTempo = 0;
	}

	// First see if we went far enough
	while (uSecLocation < mSecLocation) {
		pos++;
		uSecLocation += uSecsPerTick;
	}

	// Next, see if we went too far
	while (uSecLocation > mSecLocation) {
		pos--;
		uSecLocation -= uSecsPerTick;           // uSecsPerTick is last tempo
	}

	return pos;

}



//---------------------------------------------------------------------
//	SetPosition
//---------------------------------------------------------------------
//
//	Sets play pointers to where ticks into song.  Also remembers last
//	tempo change encountered so it will be properly set at play time.
//

void TMIDIConductor::SetPosition(long where)
{
	ConductorEvent  *e;

	fPlayTime = fTotalTime - where;                 // Moved from below 1.5a31
	fCurrentEvent = 0;
	e = (ConductorEvent *)ItemAt(++fCurrentEvent);
	fCurrentDelta = e->time;

	fLastTempo = 0;

	while (where > e->time && e->type != endOfTrack) {
		if (e->type == tempoChange)
			fLastTempo = e->d.tempo;

		where -= e->time;
		e = (ConductorEvent *)ItemAt(++fCurrentEvent);
		fCurrentDelta = e->time;

	}

	fCurrentDelta -= where;

}


#pragma mark -
#pragma mark === Timeing Routines ===

//---------------------------------------------------------------------
//	GetTotalTimeMSec
//---------------------------------------------------------------------
//
//	Returns total time in milliseconds for the song
//

int32 TMIDIConductor::GetTotalTimeMSec()
{
	ConductorEvent  *theEvent;
	int32 currentUsecsPerTick = 0;
	int32 index = 1;
	int32 timeCountdown = fTotalTime;
	int32 mSecTime = 0;

	for (int32 index = 0; index < CountItems(); index++) {
		theEvent = (ConductorEvent *)ItemAt(index);

		// Check for valid event
		if (theEvent) {
			// Make sure we are not at end of track
			if (theEvent->type != endOfTrack) {
				mSecTime += (theEvent->time * currentUsecsPerTick / 1000);

				if (theEvent->type == tempoChange) {
					currentUsecsPerTick = theEvent->d.tempo;
				}
				timeCountdown -= theEvent->time;
			}
		}
	}

	mSecTime += (timeCountdown * currentUsecsPerTick / 1000);

	return mSecTime;
}


//---------------------------------------------------------------------
//	TicksToMSec
//---------------------------------------------------------------------
//
//	Returns the millisecond value for given ticks from beginning of song
//

int32 TMIDIConductor::TicksToMSec(int32 location)
{
	int32 pos = 0;
	int32 uSecsPerTick = 0, uSecAccum = 0;
	int32 index = 0;
	ConductorEvent  *theEvent;

	theEvent = (ConductorEvent *)ItemAt(index);

	while (pos < location) {

		pos += theEvent->time;
		if (pos > location) {

			// Adjust e.time to coorespond with location.
			theEvent->time -= (pos - location);
			pos = location;
		}

		uSecAccum += theEvent->time * uSecsPerTick;

		if (theEvent->type == tempoChange)
			uSecsPerTick = (int32)theEvent->d.tempo * fTempoRatio / kTempoDivisor;

		if (index < CountItems())
			theEvent = (ConductorEvent *)ItemAt(index++);
	}

	return (uSecAccum + 500L) / 1000;
}


#pragma mark -
#pragma mark === Tempo Routines ===

//---------------------------------------------------------------------
//	GetTempoRatio
//---------------------------------------------------------------------
//
//	Returns total time in milliseconds for the song
//

int16 TMIDIConductor::GetTempoRatio()
{
	return fTempoRatio;
}


//---------------------------------------------------------------------
//	SetTempoRatio
//---------------------------------------------------------------------
//
//	Sets the current tempo ratio
//

void TMIDIConductor::SetTempoRatio(int16 ratio)
{
	fTempoRatio = ratio;

	// Since the tempo ratio changed, we need to recalc the marker times
	CalcMarkerTimes();
}


#pragma mark -
#pragma mark === Marker Routines ===

//---------------------------------------------------------------------
//	CalcMarkerTimes
//---------------------------------------------------------------------
//
//	Calculates absolute (millisecond) times for markers
//

void TMIDIConductor::CalcMarkerTimes()
{
	// Iterate through marker list
	for(int32 index = 0; index < fMarkerList->CountItems(); index++ ) {
		MarkerPtr theMarker = (MarkerPtr)fMarkerList->ItemAt(index);

		if (theMarker) {
			theMarker->time = TicksToMSec(theMarker->location);
		}
	}
}


//---------------------------------------------------------------------
//	InsertMarker
//---------------------------------------------------------------------
//
//	Add the marker to the marker array
//

void TMIDIConductor::InsertMarker( char *str, int32 time)
{
	// Allocate marker
	MarkerPtr marker;
	marker = (MarkerPtr)malloc( sizeof (MarkerRec) );
	ASSERT(marker);

	fCurrentDelta           += time;
	fCurrentLocation        += time;

	marker->location = fCurrentLocation;

	// Truncate string
	if ( strlen(str) > 32)
		str[32] = '\0';

	strcpy( marker->text, str);

	fMarkerList->AddItem(marker);
}
