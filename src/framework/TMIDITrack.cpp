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

#include "TArray.h"
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

TMIDITrack::TMIDITrack(TMIDICue *theCue)
{	
	
	// Save parent cue
	m_Cue = theCue;
	
	m_TrackName[0] = 0;
	m_CurrentEvent = 0;
	m_CurrentDelta = 0;
	
	// Set our list to NULL
	m_ExternalData = NULL;	
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
	if (m_ExternalData) 
	{
		//m_ExternalData->DoForEach(DoDisposeHandle);
		delete m_ExternalData;
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
	Event	*theEvent;
	int32	index = 1;				
	int32	dTime, totalTime = 0;
	//char	bankStr[256];
	
	
	// First, set all values to default
	m_Muted 		= false;
	m_DrumTrack 	= false;
	m_Device 		= 2;				// Default to general midi
	m_Bank 			= -1;
	m_Program 		= -1;
	m_Channel 		= -1;
	m_Octave 		= 0;
	m_Volume 		= 127;
	m_Pan 			= -1;
	m_Reverb 		= -1;
	m_Chorus 		= -1;
	m_SrcPBRange	= 2;
	m_DstPBRange 	= 2;
	m_SrcDrumMap 	= 0;
	m_DstDrumMap 	= 0;

	// Next, get initial track info		
	theEvent = (Event *)ItemAt(index++);
	if (theEvent)
		m_Channel = theEvent->d.bytes[0] & 0x0F;
	
	while (theEvent && theEvent->time == 0) 
	{
			
		// Channel
		if ((m_Channel != -1) && ((theEvent->d.bytes[0] & 0x0F) != m_Channel))
			m_Channel = -1;
			
		// Program
		if ((theEvent->d.bytes[0] & STATUS_MASK) == PROG_CHANGE) 
		{
			m_Program = theEvent->d.bytes[1];
			RemoveItem(--index);
		}
			
		// Controllers
		else if ((theEvent->d.bytes[0] & STATUS_MASK) == CTRL_CHANGE) 
		{
			if (theEvent->d.bytes[1] == VOLUME) 
			{
				m_Volume = theEvent->d.bytes[2];
				RemoveItem(--index);
			} 
			else if (theEvent->d.bytes[1] == PAN && (m_Pan == -1)) 
			{
				m_Pan = theEvent->d.bytes[2];
				RemoveItem(--index);
			} 
			else if (theEvent->d.bytes[1] == BANK_CHANGE_MSB && m_Bank == -1) 
			{
				m_Bank = theEvent->d.bytes[2] << 7;
				RemoveItem(--index);
			} 
			else if (theEvent->d.bytes[1] == BANK_CHANGE_LSB && m_Bank != -1) 
			{
				m_Bank |= theEvent->d.bytes[2];
				RemoveItem(--index);
			} 
			else if (theEvent->d.bytes[1] == REVERB && (m_Reverb == -1)) 
			{
				m_Reverb = theEvent->d.bytes[2];
				RemoveItem(--index);
			} 
			else if (theEvent->d.bytes[1] == CHORUS && (m_Chorus == -1)) 
			{
				m_Chorus = theEvent->d.bytes[2];
				RemoveItem(--index);
			}
		}
				
		theEvent = (Event *)ItemAt(index++);
	}
	
	while (theEvent && theEvent->time != END_OF_TRACK) 
	{
		totalTime += theEvent->time;
		
		if ((theEvent->d.bytes[0] & 0x0F) != m_Channel) 
		{
			m_Channel = -1;
			break;
		}
		
		if (((theEvent->d.bytes[0] & STATUS_MASK) == CTRL_CHANGE) && theEvent->d.bytes[1] == PAN && (m_Pan == -1)) 
		{
			m_Pan = theEvent->d.bytes[2];
			dTime = theEvent->time;
			RemoveItem(--index);
			theEvent->time += dTime;
		}
			
		theEvent = (Event *)ItemAt(index++);
	}
	
	if (m_Pan == -1)
		m_Pan = 64;
		
	if (m_Channel == 9)
		m_DrumTrack = TRUE;
		
	m_LastVol = m_Volume;
		
	if (m_Bank == -1) 
	{
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

void TMIDITrack::SetTrackName(char *name)
{
	strcpy(m_TrackName, name);	
}


//---------------------------------------------------------------------
//	GetTrackName
//---------------------------------------------------------------------
//
//	Retrieve track name
//

void TMIDITrack::GetTrackName(char *name)
{
	strcpy(name, m_TrackName);
}


//---------------------------------------------------------------------
//	InsertExtended
//---------------------------------------------------------------------
//
//	Insert extended MIDI data into track
//

void TMIDITrack::InsertExtended(short type, long time, ExtDataBuffer data)
{
	Event	e;
	
	if (m_ExternalData == NULL) 
		m_ExternalData = new BList();
			
	e.time 		= time;
	e.d.data 	= data;
	e.flags 	= 0;
	e.extType 	= type;
	
	AddItem(&e);
	
	// Append to extended data list
	m_ExternalData->AddItem(data);
	
} 


//---------------------------------------------------------------------
//	SetPosition
//---------------------------------------------------------------------
//
//	Sets play pointers to where.
//

void TMIDITrack::SetPosition(long where)
{
	Event *theEvent;
	
	m_CurrentEvent = 0;
	theEvent = (Event *)ItemAt(++m_CurrentEvent);
	m_CurrentDelta = theEvent->time;
	
	while (where > theEvent->time && theEvent->time != END_OF_TRACK) 
	{
		where -= theEvent->time;
		theEvent = (Event *)ItemAt(++m_CurrentEvent);
		m_CurrentDelta = theEvent->time;
	}

	if (theEvent->time == END_OF_TRACK) 
		return;
	
	m_CurrentDelta -= where;
	
} 


//---------------------------------------------------------------------
//	Reset
//---------------------------------------------------------------------
//
//	Sets track pointers to beginning.  (same as SetPosition(0)).
//

void  TMIDITrack::Reset()
{	
	Event *theEvent;
		
	m_CurrentEvent = 1;
	theEvent = (Event *)ItemAt(1);
	m_CurrentDelta = theEvent->time;	
} 
