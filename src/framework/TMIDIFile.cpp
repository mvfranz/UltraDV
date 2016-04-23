//---------------------------------------------------------------------
//
//	File:	TMIDIFile.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "MuseumMidiDefs.h"
#include "TMIDIFile.h"
#include "TMIDIConductor.h"
#include "TMIDITrack.h"


// static
HeaderChunk		sgHdr;			// Copy of header chunk


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIFile::TMIDIFile(entry_ref *ref, uint32 openMode) : BFile(ref, openMode)
{		
	//m_DiskBuffer = NULL;
	// Initialize header
	ReadHeader(&m_Header);		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIFile::~TMIDIFile()
{
	//if (m_DiskBuffer)
	//	free(m_DiskBuffer);
}


#pragma mark -
#pragma mark === Reading Functions ===

//---------------------------------------------------------------------
//	ReadHeader
//---------------------------------------------------------------------
//
//	Rewinds file and reads in the header chunk.
//

void TMIDIFile::ReadHeader(HeaderChunk *header)
{
	char	buf[100];
	
	// Read from beginning of file
	Seek(0, SEEK_SET);						
	
	ReadMidiChunkHeader();
	Read( header, sizeof(HeaderChunk));	
	
	// Make local copy
	sgHdr = *header;									
		
	// Read any excess header info
	if (m_MidiChunkHeader.length > sizeof(HeaderChunk))		
		Read(buf, m_MidiChunkHeader.length - sizeof(HeaderChunk));		
} 




//---------------------------------------------------------------------
//	ReadConductor
//---------------------------------------------------------------------
//
//	Reads conductor track and inserts data into condTrack.
//	Data events are ignored.
//

int32 TMIDIFile::ReadConductor(TMIDIConductor *condTrack)
{
	int16	lastStatus,				// Last MIDI status byte read
			status,					// First byte read
			type,					// META_EVENT type
			dataByte1,				// Second byte read
			dataByte2;				// Third byte read, if any
	
	int16	done, 					// Flag for loop
			i;
			
	int32	time,					// Time of event
			tempo,					// Tempo event
			num,					// Numerator of time signature
			den,					// Denominator of time signature
			length,					// Length of event
			totalTime;				// Total time of conductor track
			
	char	cBuf[C_BUFF_SIZE];		// Buffer for reading text events
			
	// Load in header
	ReadMidiChunkHeader();
	
	if ( m_MidiChunkHeader.chunkType != TRACK_CHUNK_TYPE )
		return -1;
	
	m_BufferRemainder = m_MidiChunkHeader.length;
	
	try
	{	
		// Read track data into m_DiskBuffer
		FillBuffer();					
	}
	catch(...)
	{
		return -1;
	}
		
	done = false;
	totalTime = time = 0;
			
	while (!done) 
	{
		time 		+= ReadVariableLength();
		totalTime 	+= time;
		status 		= ReadByte();
		
		if (status == META_EVENT) 
		{
			type 	= ReadByte();
			length 	= ReadVariableLength();
			
			switch (type) 
			{
				case MF_TEXT:
				case MF_MARKER:
				case MF_CUE_POINT:
					for (i = 0; i < length; i++) 
					{
						cBuf[i] = ReadByte();
						if (i >= C_BUFF_SIZE)
							break;
					}
					cBuf[i] = '\0';			// Terminate string
					
					condTrack->InsertMarker(cBuf, time);
					break;
					
				case MF_TEMPO:
					tempo = ReadByte();
					tempo <<= 8;
					tempo += ReadByte();
					tempo <<= 8;
					tempo += ReadByte();
					
					tempo /= sgHdr.division;
					
					condTrack->InsertTempo(tempo, time);
					break;
					
				case MF_SMPTE:
					if (length == 5) 
					{
						for (i = 0; i < 5; i++)
							cBuf[i] = ReadByte();
						condTrack->SetStartTime(*(int32*)cBuf, time);
					} 
					else
						SkipNBytes(length);
					break;
					
				case MF_METER:
					num = ReadByte();
					den = (1 << ReadByte());
					dataByte1 = ReadByte();			// Click
					dataByte2 = ReadByte();			// 32nd notes per 1/4 note
					
					condTrack->InsertMeter(num, den, time);
					break;
					
				case MF_END_TRK:
					done = true;
					break;
					
				// All others are ignored by conductor tracks
				default:					
					SkipNBytes(length);
					break;					
			}
						
			time = 0;			
		} 
		else if (status == SYS_EXCL || status == END_SYS_EXCL) 
		{		
			length = ReadVariableLength();
			// Ignore sys-ex messages			
			SkipNBytes(length);				
		} 
		else 
		{			
			// running status
			if (status < 0x80) 
			{			
				dataByte1 = status;
				status = lastStatus;
			} 
			else 
			{
				dataByte1 = ReadByte();
				lastStatus = status;
			}
			
			if ((status & 0xE0) != 0xC0) 	// 3-byte message
				dataByte2 = ReadByte();
			else
				dataByte2 = 0;
			
			// Since this is a conductor track, ignore any MIDI data						
		}		
	}
	
	// Clean up buffer
	//if (m_DiskBuffer)
	//	free(m_DiskBuffer);
	
	return totalTime;					
	
} 


//---------------------------------------------------------------------
//	ReadType0
//---------------------------------------------------------------------
//
//	Reads a type 0 MIDI File into condTrack and dataTrack.
//


int32 TMIDIFile::ReadType0(TMIDIConductor *condTrack, TMIDITrack *dataTrack)
{
	HeaderChunk		hdr;
	int32			totalTime;
	
	ReadHeader(&hdr);						// ReadHeader sets file mark to top
	
	totalTime = ReadConductor(condTrack);	// Pass through once reading conductor data
	
	if (totalTime == -1)
		return -1;
		
	ReadHeader(&hdr);						// Back to the top
	
	totalTime = ReadNextTrack(dataTrack);	// Pass through again reading MIDI data
	
	return totalTime;
	
} 


//---------------------------------------------------------------------
//	ReadVariableLength
//---------------------------------------------------------------------
//
//	Reads/returns a variable length number.
//

int32 TMIDIFile::ReadVariableLength()
{
	register int32	value;
	register int16	c;
	
	value = 0;
	
	do 
	{
		c = ReadByte();
		value <<= 7;
		value += (c & 0x7F);
	} while (c & 0x80);
	
	return (value);	
} 



//---------------------------------------------------------------------
//	ReadNextByte
//---------------------------------------------------------------------
//
//	Returns next byte from disk buffer.
//	If buffer needs refilling, calls FillBuffer().
//


int16 TMIDIFile::ReadByte()
{
	int16 	value;
	
	if (m_DiskBuffer) 
	{
		// Index into disk buffer
		value = *((unsigned char *)m_DiskBuffer + m_BufferLocation);
		
		if (++m_BufferLocation >= m_BufferSize && m_BufferRemainder)
			FillBuffer();									
			
		return (value);
	} 
	else
		return -1;
		
} 


//---------------------------------------------------------------------
//	FillBuffer
//---------------------------------------------------------------------
//
//	Fills the disk buffer.
//

void	TMIDIFile::FillBuffer()
{
	int32	howMuch;
	
	//if (m_DiskBuffer)
	//{
		//free(m_DiskBuffer);
		//m_DiskBuffer = NULL;
	//}	
	
	// Allocate a new disk buffer
	//m_DiskBuffer = malloc(BUFF_SIZE);
	//ASSERT(m_DiskBuffer);
	//m_DiskBuffer = new BMallocIO();
	//m_DiskBuffer->SetSize(BUFF_SIZE);
	
	howMuch = m_BufferRemainder > BUFF_SIZE ? BUFF_SIZE : m_BufferRemainder;
	
	Read(m_DiskBuffer, howMuch);
	
	m_BufferRemainder 	-= howMuch;
	m_BufferSize 		= howMuch;
	m_BufferLocation 	= 0;			// Reset index	
} 


//---------------------------------------------------------------------
//	SkipNBytes
//---------------------------------------------------------------------
//
//	Skips n bytes in disk buffer.
//	Refills the buffer if neccesary.
//

void	TMIDIFile::SkipNBytes(int32 nBytes)
{
	int32 	remainder;
	
	// Just bump index
	m_BufferLocation += nBytes;									
	
	if (m_BufferLocation > m_BufferSize) 
	{
		remainder = m_BufferLocation - m_BufferSize;
		FillBuffer();
		m_BufferLocation = remainder;
	}		
} 


//---------------------------------------------------------------------
//	ReadMIDIChunkHeader
//---------------------------------------------------------------------
//
//	Reads in the chunk header.
//


void TMIDIFile::ReadMidiChunkHeader()
{
	Read( &m_MidiChunkHeader, sizeof(MidiChunkHeader) );
}



//---------------------------------------------------------------------
//	ReadNextTrack
//---------------------------------------------------------------------
//
//	Reads next data track and inserts data into dataTrack.
//	Conductor events are ignored.
//

int32 TMIDIFile::ReadNextTrack(TMIDITrack *dataTrack)
{
	int16			lastStatus,				// Last MIDI status byte read
					status,					// First byte read
					type,					// META_EVENT type
					dataByte1,				// Second byte read
					dataByte2;				// Third byte read, if any
	
	int16			done, 					// Flag for loop
					i;
			
	int32			time,					// Time of event
					length,					// Length of event
					totalTime;				// Total time of track
			
	char			cBuf[C_BUFF_SIZE];		// Buffer for reading text events
	
	ExtDataBuffer	dataHandle = NULL;		// Used to load in sysex events
	unsigned char	*tempPtr;				
	
	Event			*theEvent;				// Event buffer
	int32			numItems = 0;			// total number of events in track
	void			*itemBuffer;			// Buffer of items
		
	// Read in header info	
	ReadMidiChunkHeader();
	
	if (m_MidiChunkHeader.chunkType != TRACK_CHUNK_TYPE)
		return -1;
		
	m_BufferRemainder = m_MidiChunkHeader.length;
	

	try
	{	
		// Read track data into m_DiskBuffer
		FillBuffer();					
	}
	catch(...)
	{
		return -1;
	}
	
	totalTime = time = 0;
	done = false;
		
	// Make a buffer big enough for 50,000 events
	size_t theSize = sizeof(Event) * 5000 ;	 
	itemBuffer = malloc( theSize);
	theEvent = (Event *)itemBuffer;
	ASSERT(theEvent);
		
	try
	{
		while (!done) 
		{
			time += ReadVariableLength();
			totalTime += time;
			status = ReadByte();
			
			if (status == META_EVENT) 
			{
				type = ReadByte();
				length = ReadVariableLength();
				
				switch (type) 
				{
					case MF_TEXT:
					case MF_TRK_NAME:
					case MF_INST_NAME:
						for (i = 0; i < length; i++) 
						{
							if (i < C_BUFF_SIZE)
								cBuf[i] = ReadByte();
							else 
								ReadByte();		// discard byte
						}
						if (i > C_BUFF_SIZE)
							i = C_BUFF_SIZE - 1;
							
						cBuf[i] = '\0';			// Terminate string
						
						dataTrack->SetTrackName(cBuf);
						time = 0;
						break;
						
					case MF_END_TRK:
						done = true;
						break;
						
					default:					// All others are ignored by data tracks
						SkipNBytes(length);
						break;
						
				}		// switch (type)
				
				// time = 0;
				
			} 
			else if (status == SYS_EXCL || status == END_SYS_EXCL) 
			{
			
				length = ReadVariableLength();
				// length doesn't include 0xF0
				length++;								
				dataHandle = (ExtDataBuffer)malloc(length + 2);
				ASSERT(dataHandle);
				
				
				(*dataHandle)->length = length;
				
				tempPtr = (*dataHandle)->data;
				
				tempPtr[0] = status;
				for (i = 1; i < length; i++)			// Read the message
					tempPtr[i] = ReadByte();									
				
				theEvent->time 		= time;
				theEvent->d.data 	= dataHandle;
				theEvent->flags 	= 0;
				theEvent->extType 	= kSysexType;
				
				theEvent++;
				numItems++;
				
				if (numItems >= 5000)
				{
					if (!(numItems % 1000)) 
					{
						// Reallocate buffer
						realloc( itemBuffer, (sizeof(Event) * 5000)  + 10000); 
												  												
						// Get event info
						theEvent = (Event*)itemBuffer + numItems;											
					}
				}
				time = 0;											
			} 
			else 
			{				
				// running status
				if (status < 0x80) 
				{			
					dataByte1 = status;
					status = lastStatus;
				} 
				else 
				{
					dataByte1 = ReadByte();
					lastStatus = status;
				}
				
				// 3-byte message
				if ((status & 0xE0) != 0xC0) 	
					dataByte2 = ReadByte();
				else
					dataByte2 = 0;
					
				// Stuff the packet
				theEvent->time = time;
				theEvent->d.bytes[0] 	= status;
				theEvent->d.bytes[1] 	= dataByte1;
				theEvent->d.bytes[2] 	= dataByte2;
				theEvent->d.bytes[3] 	= 0;
				theEvent->flags 		= 0;
				theEvent->extType 		= kNotExtended;
							
				theEvent++;
				numItems++;
				
				if (numItems >= 5000) 
				{
					// allocate enough for 1000 more events
					if (!(numItems % 1000)) 
					{							
						// Reallocate buffer
						realloc( itemBuffer, (sizeof(Event) * 5000)  + 10000); 
												  												
						// Get event info
						theEvent = (Event*)itemBuffer + numItems;																				
					}
				}				
				time = 0;				
			}			
		}
		
		theEvent->time = END_OF_TRACK;		
		numItems++;
		
		// Allocate space for one extra slot the Array needs as a holding tank
		//realloc( itemBuffer, (numItems+1) * sizeof(Event) ); 
	}
	catch(...)
	{
		// signal error
		totalTime = -1;		
		if (itemBuffer) 
			free(itemBuffer);
	}
	
	free(itemBuffer);

	// Clean up buffers
	//if (m_DiskBuffer)
	//	free(m_DiskBuffer);

	return totalTime;

} 


#pragma mark -
#pragma mark === Writing Functions ===
