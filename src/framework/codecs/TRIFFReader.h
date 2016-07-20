//---------------------------------------------------------------------
//
//	File:	TRIFFReader.h
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

#ifndef __TRIFFREADER_H__
#define __TRIFFREADER_H__

//	Includes
#include "RIFFConstants.h"
#include "RIFFTypes.h"
#include "TVideoCodec.h"

//	Constants
const int32 kRiffMaxStreams = 2;

// Prototypes
bool IsRIFFFile(BFile *file);

//	Forward Declarations
class TVideoCodec;

//	Class Definition
class TRIFFReader
{
	public:
		TRIFFReader(BFile *file);
		~TRIFFReader();
		
		bool	InitCheck();
		
		bool	ReadVideoFrame(uint32 frameNum, void *buffer);
		bool	ReadAudioFrame(uint32 frameNum, void *buffer);
		
		//	Header Queries
		uint16	VideoDepth();
		uint32 	Width();
		uint32 	Height();
		bool 	IsBottomUp();
		uint32 	UsPerFrame();
		
		//	Inlines
		BFile			*GetFile(){ return m_File; }
		AVIIndex 		*GetIndex(){ return m_HeaderIndex; }
		AVIHeader		*GetAVIHeader(){ return &m_AVIHeader; };
		AVIVIDSHeader	*GetVIDSHeader(){ return &m_VIDSHeader; }
		AVIAUDSHeader	*GetAUDSHeader(){ return &m_AUDSHeader; }
		BList			*GetVideoFrameList(){ return m_VideoFrameList; }
		BList			*GetAudioFrameList(){ return m_AudioFrameList; }
		uint32			VideoFrameCount(){ return m_VideoFrameList->CountItems(); }
		uint32			AudioFrameCount(){ return m_AudioFrameCount; }
		uint32			IndexCount(){ return m_IndexCount; }
		bool			HasAudio(){ return m_HasAudio; }
		bool			HasVideo(){ return m_HasVideo; }
		int32			GetStreamCount(){ return m_StreamCount; }
		
		AVIStreamHeader *GetStreamHeaderOne(){ return &m_StreamHeaderOne; }
		AVIStreamHeader *GetStreamHeaderTwo(){ return &m_StreamHeaderTwo; }
				
	private:
		//	Member Functions
		void	Init();
		bool 	ParseAVIFile();
		
		//	Chunk handling
		bool 	ReadRIFFChunk(uint32 size);
		bool	ReadLISTChunk(uint32 size);
		bool	ReadavihChunk(uint32 size);
		bool	ReadStreamHeader(uint32 size);
		bool	ReadstrdChunk(uint32 size);		
		bool	ReadStreamFormat(uint32 size);
		bool 	ReadvidsChunk(uint32 size);
		bool 	ReadaudsChunk(uint32 size);
		bool 	Readidx1Chunk(uint32 size);
		bool 	ReadpcChunk(uint32 size);
		bool 	Read00DCChunk(uint32 chunkID, uint32 offset, uint32 length, uint32 streamNum);
		bool 	Read00WBChunk(uint32 chunkID, uint32 offset, uint32 length, uint32 streamNum);
		
		//	Header Reading
		bool	ReadAVIHeader();
		
		//	Codecs
		bool	InitCodecs();
		bool	InitVideoCodec();
		bool	InitAudioCodec();
							
		//	Member Variables
		BFile		*m_File;
		bool		m_InitCheck;
		
		int32		m_AVISize;
		int32		m_StreamCount;
		int32		m_MoviChunkOffset;
		
		uint32		m_StreamType[kRiffMaxStreams];
		bool		m_StreamOK[kRiffMaxStreams];

		bool		m_UseIndexFlag;
		
		//	Video Variables
		bool			m_HasVideo;
		AVIVIDSHeader	m_VIDSHeader;
		uint32			m_IndexCount;
		TVideoCodec		*m_VideoCodec;
		
		//	Audio Variables
		bool			m_HasAudio;		
		AVIAUDSHeader	m_AUDSHeader;		
		uint32			m_AudioType;
		uint32			m_AudioFrequency;
		uint32			m_AudioChannels;
		uint32			m_AudioBPS;
		uint32			m_AudioFrameSize;
		uint32			m_AudioEnd;		
		uint32			m_TotalAudioTime;
		uint32			m_AudioFrameCount;

		uchar 		*m_STRDBuffer;
		uint32 		m_STRDSize;
		uint32		m_STRDCurSize;
		
		//	Chunk headers
		AVIHeader		m_AVIHeader;
		AVIIndex 		*m_HeaderIndex;
		AVIStreamHeader	m_StreamHeaderOne;
		AVIStreamHeader	m_StreamHeaderTwo;		
						
		//	Data Lists
		BList *m_VideoFrameList;
		BList *m_AudioFrameList;
};

#endif