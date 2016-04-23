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


#ifndef __TMIDIFILE_H__
#define __TMIDIFILE_H__

#include "MuseumMidiDefs.h"

// Constants
#define C_BUFF_SIZE	100			// Size of text buffer
#define BUFF_SIZE	1024*16		// Size of disk buffer (16k)

// Class Definition
class TMIDIFile : public BFile 
{

	public:
		// Member Functions
		TMIDIFile(entry_ref *ref, uint32 openMode);
		~TMIDIFile();
	
		void		Dispose();
		void		ReadHeader(HeaderChunk *header);
		int32		ReadConductor(TMIDIConductor *condTrack);
		int32		ReadNextTrack(TMIDITrack *theTrack);
		int32		ReadType0(TMIDIConductor *condTrack, TMIDITrack *dataTrack);
		
		// Inlines
		inline int32	GetTotalTracks() { return m_Header.numTracks; }
					
		// Member Variables
		// gzr: to do... Figure out why we have malloc problem
		//void			*m_DiskBuffer;		// RAM buffer to hold a track at a time 
		char			m_DiskBuffer[BUFF_SIZE];		// RAM buffer to hold a track at a time 
		int32			m_BufferLocation;	// Current diskBuffer position
		int32			m_BufferSize;		// Current buffer size
		int32			m_BufferRemainder;	// Remainder of track to be read in
		HeaderChunk		m_Header;			// Copy of the files header chunk
		MidiChunkHeader	m_MidiChunkHeader;	// Chunk header of current chunk
	

	protected:
		int32	ReadVariableLength();
		int16	ReadByte();
		void	FillBuffer();
		void	SkipNBytes(int32 nBytes);
		void	ReadMidiChunkHeader();
	
		void	CreateDiskBuffer(int32 size);
		
};

#endif
