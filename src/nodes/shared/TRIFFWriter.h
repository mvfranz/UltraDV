//---------------------------------------------------------------------
//
//	File:	TRIFFWriter.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	RIFF File Format
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TRIFFWRITER_H__
#define __TRIFFWRITER_H__

//	Includes
#include "RIFFConstants.h"
#include "RIFFTypes.h"

const char kRiffWriteMaxStreams = 2;

//	Class Definition
class TRIFFWriter
{
	public:
		TRIFFWriter(BFile *file);
		~TRIFFWriter();
		
		bool 	InitAVIFile();
		bool 	CompleteAVIFile();
						
		bool 	Write00DCChunk(uint32 chunkID, uint32 length, void *data);
		bool 	Init00WBChunk(uint32 chunkID, uint32 length);
		bool 	Write00WBChunk(uint32 chunkID, uint32 length, void *data);

		bool	SetStreamCount(int32 streamCount);
		
		//	Accessors
		inline AVIHeader		*GetAVIHeader(){ return &m_AVIHeader; }
		inline AVIStreamHeader	*GetStreamOneHeader(){ return &m_StreamHeaderOne; }
		inline AVIStreamHeader	*GetStreamTwoHeader(){ return &m_StreamHeaderTwo; }
		inline AVIVIDSHeader	*GetVIDSHeader(){ return &m_VIDSHeader; }
		inline AVIAUDSHeader	*GetAUDSHeader(){ return &m_AUDSHeader; }
		inline uint32			GetTotalFrames(){ return m_TotalFrames; }
		
	private:
		//	Member Functions
		void	Init();
				
		//	Chunk handling
		bool 	WriteRIFFChunk(uint32 size);
		bool	WriteLISTChunk(uint32 size);
		bool	WriteavihChunk();
		bool	WriteAVIHeader();
		bool	WriteStreamHeader(AVIStreamHeader *header);
		bool	WriteStreamFormat(AVIStreamHeader *header);
		bool 	WritevidsChunk();
		bool 	WriteaudsChunk();
		bool 	Writeidx1Chunk();
								
		//	Member Variables
		BFile		*m_File;
		
		int32		m_AVISize;
		int32		m_StreamCount;
		int32		m_MoviChunkOffset;
		
		bool		m_UseIndexFlag;
		
		//	Video Variables
		AVIVIDSHeader	m_VIDSHeader;
		uint32			m_IndexCount;
		uint32			m_TotalFrames;
		
		//	Audio Variables
		AVIAUDSHeader	m_AUDSHeader;		
		uint32			m_AudioType;
		uint32			m_AudioFrequency;
		uint32			m_AudioChannels;
		uint32			m_AudioBPS;
		uint32			m_AudioEnd;
		uint32			m_TotalAudioTime;
		
		//	Chunk headers
		AVIHeader		m_AVIHeader;		
		AVIStreamHeader	m_StreamHeaderOne;
		AVIStreamHeader	m_StreamHeaderTwo;
};

#endif