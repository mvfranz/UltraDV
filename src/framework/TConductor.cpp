//---------------------------------------------------------------------
//
//	File:	TConductor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.23.98
//
//	Desc:	Handle displaying of markers and stretching tempos
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include <app/Application.h>
#include <assert.h>
#include <MidiKit.h>


/********************************************************************************/
/*																				*/
/*		CProducerConductor.c									                                */
/*																				*/
/*		SUPERCLASS:	CConductor													*/
/*																				*/
/*		Copyright  1992 Passport Designs										*/
/*		Written By: Glenn Ruehle												*/
/*																				*/
/*																				*/
/********************************************************************************/

// ABH missing
//#include "CProducerConductor.h"
//#include "CMIDICue.h"

//#include <CArray.h>
//#include <TBUtilities.h>



/********************************************************************************
        IProducerConductor

                Initialize the conductor.
********************************************************************************/

void CProducerConductor::IProducerConductor(CMIDICue *aSupervisor)
{
	CConductor::IConductor(aSupervisor);

	itsTempoRatio = kTEMPO_DIVISOR;

	itsMarkers = new CArray;
	itsMarkers->IArray(sizeof(MarkerRec));
}



/********************************************************************************
        Dispose

                Dispose of the marker array before going.
********************************************************************************/

void CProducerConductor::Dispose(void)
{
	ForgetObject(itsMarkers);
	inherited::Dispose();
}



/********************************************************************************
        InsertMarker

                Add the marker to the marker array.
********************************************************************************/

void CProducerConductor::InsertMarker(char *str, long time)
{
	MarkerRec data;

	inherited::InsertMarker(str, time);                     // inherited updates curLocation

	data.location = curLocation;
	CtoPstr(str);

	// gzr: fix this...
	//if (StrLength((StringPtr)str) > 32)
	//	StrLength((StringPtr)str) = 32;
	CopyPString((StringPtr)str, data.text);

	itsMarkers->InsertAtIndex(&data, AT_END);
}



/********************************************************************************
        CalcMarkerTimes

                Calculates absolute (millisecond) times for markers.
********************************************************************************/

void CProducerConductor::CalcMarkerTimes(void)
{
	MarkerPtr aMarker;
	long i;

	// Directly de-reference the marker item handle for efficiency

	HLock(itsMarkers->hItems);
	aMarker = (MarkerPtr)*itsMarkers->hItems;

	for (i = 0; i < itsMarkers->GetNumItems(); i++, aMarker++)
		aMarker->time = TicksToMSec(aMarker->location);

	HUnlock(itsMarkers->hItems);
}



/********************************************************************************
        TicksToMSec

                Returns the millisecond value for given ticks from beginning of song.
********************************************************************************/

long CProducerConductor::TicksToMSec(long location)
{
	long pos = 0;
	long uSecsPerTick = 0, uSecAccum = 0;
	long index = 1;
	CondEvent e;

	GetArrayItem(&e, index++);

	while (pos < location) {

		pos += e.time;
		if (pos > location) {

			// Adjust e.time to coorespond with location.

			e.time -= (pos - location);
			pos = location;
		}

		uSecAccum += e.time * uSecsPerTick;

		if (e.type == tempoChange)
			uSecsPerTick = (long)e.d.tempo * itsTempoRatio / kTEMPO_DIVISOR;

		if (index <= numItems)
			GetArrayItem(&e, index++);
	}

	return (uSecAccum + 500L) / 1000;
}



/********************************************************************************
        GetTempoRatio

                Returns the current tempo ratio (in 1/1000ths).
********************************************************************************/

short CProducerConductor::GetTempoRatio(void)
{
	return itsTempoRatio;
}



/********************************************************************************
        SetTempoRatio

                Sets the current tempo ratio.
********************************************************************************/

void CProducerConductor::SetTempoRatio(short aRatio)
{
	itsTempoRatio = aRatio;

	// Since the tempo ratio changed, we need to recalc the marker times
	CalcMarkerTimes();
}



/********************************************************************************
        CheckPlay

                Called from TimerProc with the number of ticks that have elapsed since
                the last time we were called.
                {OVERRIDE} to adjust tempo using tempoRatio.
********************************************************************************/

long CProducerConductor::CheckPlay(long dTime)
{
	CondEvent       *e;


	// First check for tracks total time
	itsPlayTime -= dTime;

	if (itsPlayTime <= 0)
		return END_OF_TRACK;

	// Next, see if there is a stored tempo change we need to send
	if (itsLastTempo) {
		itsDoc->itsUsecsPerTick = (long)itsLastTempo * itsTempoRatio / kTEMPO_DIVISOR;
		itsLastTempo = 0;
	}

	// Next, see if there are any more conductor events
	if (curDelta == END_OF_TRACK)
		return AT_END;

	curDelta -= dTime;

	// See if there's anything to do
	if (curDelta <= 0) {

		e = GetItemPtr(curEvent);

		// Handle all conductor events with 0 delta time
		do {

			switch (e->type) {
			case meterChange:
				break;

			case tempoChange:
				itsDoc->itsUsecsPerTick = (long)e->d.tempo *
				                          itsTempoRatio / kTEMPO_DIVISOR;
				break;

			case endOfTrack:
				curDelta = END_OF_TRACK;

				// end of conductor data, set next wake up for end of play
				return itsPlayTime;
				break;
			}

			e = GetItemPtr(++curEvent);

		} while (e->time == 0);

		curDelta = e->time;
	}

	return curDelta;

}


