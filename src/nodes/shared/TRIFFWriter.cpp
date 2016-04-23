//---------------------------------------------------------------------
//
//	File:	TRIFFWriter.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	RIFF File Format Writer
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "AVIUtils.h"
#include "DebugUtils.h"

#include "AVINodeConstants.h"

#include "TRIFFWriter.h"
    
//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TRIFFWriter::TRIFFWriter(BFile *file)
{
	
	//	Save pointer to file
	m_File = file;
			
	//	Default initialization
	Init();	
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TRIFFWriter::~TRIFFWriter() 
{
	//	Clean up
}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//	Perform defualt initialization
//

void TRIFFWriter::Init() 
{
	//	Set stream count to 0
	m_StreamCount 		= 0;
	m_IndexCount 		= 0;
	m_TotalFrames		= 0;
	m_MoviChunkOffset 	= -1;
			
	//	Rewind file
	m_File->Seek(0, SEEK_SET);
	
	//	Clear out headers
	m_AUDSHeader.ByteCount = 0;
			
}

#pragma mark -
#pragma mark === File Handling ===

//-------------------------------------------------------------------
//	InitAVIFile
//-------------------------------------------------------------------
//
//	Write AVI file headers and other info that can be 
//	determined at start of write activity.
//

bool TRIFFWriter::InitAVIFile()
{	
	
	//	We cannot write out if all elements are not initialized...
	
	off_t	savePos;	
	bool 	retVal 	= true;
	ssize_t size 	= 0x4949;
	
	//	Rewind the file
	m_File->Seek(0, SEEK_SET);
		
	//	Write RIFF header
	WriteRIFFChunk(size);
	
	//	Write AVIHeader LIST chunk
	retVal = WriteLISTChunk(size);
	if (retVal == false)
		return retVal;
	
	//	Save position for later calculation.  Back over size...
	off_t listHeaderPos = m_File->Position();
	listHeaderPos -= sizeof(uint32);

	WriteIntMsb(m_File, kRiff_hdrl_Chunk, sizeof(uint32)); 	

	//	Write AVIHeader chunk
	retVal = WriteavihChunk();
	if (retVal == false)
		return retVal;
	
	//
	//	Write stream data List Chunk
	//
	
	//	Make sure we have streams to write
	if ( (m_StreamCount == 0) || (m_StreamCount > kRiffWriteMaxStreams) )
		return false;
	
	//
	//	Write out first stream
	//
		
	retVal = WriteLISTChunk(size);
	if (retVal == false)
		return retVal;
			
	//	Save position for later size calculation
	off_t streamOnePos = m_File->Position();

	//	Write out 'strl' chunk
	WriteIntMsb(m_File, kRiff_strl_Chunk, sizeof(uint32));
		
	//	First stream header
	retVal = WriteStreamHeader(&m_StreamHeaderOne);
	
	//	First stream format
	retVal = WriteStreamFormat(&m_StreamHeaderOne);
	
	//	Update streamOne chunk size
	savePos = m_File->Position();
	uint32 streamOneSize = (uint32)(savePos - streamOnePos);
	
	//	Update list header chunk size
	B_HOST_TO_LENDIAN_INT32(streamOneSize);
	streamOnePos -= sizeof(uint32);
	m_File->WriteAt(streamOnePos, &streamOneSize, sizeof(uint32));
	
	//	Return to saved postion
	m_File->Seek(savePos, SEEK_SET);
	
	//
	//	Write out second stream
	//
	
	if (m_StreamCount == 2)
	{
		retVal = WriteLISTChunk(size);
		if (retVal == false)
			return retVal;
	
		//	Save position for later size calculation
		off_t streamTwoPos = m_File->Position();

		//	Write out 'strl' chunk
		WriteIntMsb(m_File, kRiff_strl_Chunk, sizeof(uint32)); 	
		
		//	Second stream header
		retVal = WriteStreamHeader(&m_StreamHeaderTwo);
		
		//	Second stream format
		retVal = WriteStreamFormat(&m_StreamHeaderTwo);
		
		//	Update streamTwo chunk size
		savePos = m_File->Position();
		uint32 streamTwoSize = savePos - streamTwoPos;
		
		//	Update list header chunk size
		B_HOST_TO_LENDIAN_INT32(streamTwoSize);
		streamTwoPos -= sizeof(uint32);
		m_File->WriteAt(streamTwoPos, &streamTwoSize, sizeof(uint32));
		
		//	Restore position
		m_File->Seek(savePos, SEEK_SET);
	}
			
	//	Save position and go back and update unitialized chunk sizes
	savePos = m_File->Position();
	uint32 listHeaderSize = savePos - listHeaderPos;
	
	//	Update list header chunk size
	B_HOST_TO_LENDIAN_INT32(listHeaderSize);
	m_File->WriteAt(listHeaderPos, &listHeaderSize, sizeof(uint32));
	
	//	Restore position
	m_File->Seek(savePos, SEEK_SET);
	
	//
	//	Write movi Chunk.  We don't know the true size yet...
	//
	
	retVal = WriteLISTChunk(size);
	if (retVal == false)
		return retVal;
		
	WriteIntMsb(m_File, kRiff_movi_Chunk, sizeof(uint32)); 	
	
	//	Save offset for later index chunk calculation
	m_MoviChunkOffset = m_File->Position();
	
	//	All done
	return retVal;
	
}



//-------------------------------------------------------------------
//	CompleteAVIFile
//-------------------------------------------------------------------
//
//	Complete update of unfilled header fields
//

bool TRIFFWriter::CompleteAVIFile()
{
	bool 	retVal = true;
	uint32 	streamSize;
	
	//	Save our position
	off_t savePos = m_File->Position();
	
	//	Rewind
	m_File->Seek(0, SEEK_SET);
					
	//	Skip over RIFF form
	m_File->Seek(sizeof(uint32), SEEK_CUR);
	
	//	Write file size and bump pointer
	off_t fileSize;
	m_File->GetSize(&fileSize);
	uint32 size = (uint32)fileSize;		
	B_HOST_TO_LENDIAN_INT32(size);
	m_File->WriteAt(m_File->Position(), &size, sizeof(uint32));
	m_File->Seek( sizeof(uint32), SEEK_CUR);
	
	//	Skip AVI form, LIST chunk and hdrl chunk
	m_File->Seek( sizeof(uint32) * 6, SEEK_CUR);
	
	//
	//	Update unitialized AVIHeader fields
	//
			
	//	Skip filled fields and update fields needing info
	m_File->Seek( sizeof(uint32) * 4, SEEK_CUR);
	WriteAtIntLsb(m_File, m_TotalFrames, sizeof(uint32));
	m_File->Seek( sizeof(uint32), SEEK_CUR);
	m_File->Seek( sizeof(uint32) * 8, SEEK_CUR);
	WriteAtIntLsb(m_File, m_TotalFrames, sizeof(uint32));
	m_File->Seek( sizeof(uint32), SEEK_CUR);
	
	//
	//	Update unitialized stream header and format fields
	//	
	
	//	The first should always be the video stream
	
	//	Skip LIST chunk
	m_File->Seek( sizeof(uint32) * 3, SEEK_CUR);
	
	//	Skip stream header chunkID and size
	m_File->Seek( sizeof(uint32) * 2, SEEK_CUR);
	
	//	Skip filled fields and update DataLength.  Skip over rest of chunk...
	m_File->Seek( sizeof(uint32) * 8, SEEK_CUR);
	WriteAtIntLsb(m_File, m_TotalFrames, sizeof(uint32));
	m_File->Seek( sizeof(uint32) * 4, SEEK_CUR);
	
	//	Skip vids StreamFormat chunk
	m_File->Seek( sizeof(uint32), SEEK_CUR);	
	m_File->Read( &streamSize, sizeof(uint32));
	m_File->Seek(streamSize, SEEK_CUR);
	
	//	Is there an audio chunk to update?  If so, it will always be the second stream
	if ( m_StreamCount > 1)
	{
		//	Skip LIST chunk
		m_File->Seek( sizeof(uint32) * 3, SEEK_CUR);
	
		//	Skip stream header
		m_File->Seek( sizeof(uint32), SEEK_CUR);
		m_File->Read( &streamSize, sizeof(uint32));
		m_File->Seek(streamSize, SEEK_CUR);

		//	Skip stream format header and size
		m_File->Seek( sizeof(uint32) * 2, SEEK_CUR);

		//	Skip filled fields and update ByteCount
		m_File->Seek( 28, SEEK_CUR);
		WriteAtIntLsb(m_File, m_AUDSHeader.ByteCount, sizeof(uint32));
		m_File->Seek( sizeof(uint32), SEEK_CUR);		
	}
	
	//
	//	Write out size of 'movi' chunk
	//
	
	//	Seek to start of 'movi' index
	off_t seekPos = m_MoviChunkOffset - ( sizeof(uint32) * 2);
	m_File->Seek(seekPos, SEEK_SET);
	
	//	Calculate movi chunk size and write it out
	m_File->GetSize(&fileSize);
	size = (uint32)fileSize;
	uint32 moviChunkSize = size - m_MoviChunkOffset;
	moviChunkSize += sizeof(uint32);
	WriteAtIntLsb(m_File, moviChunkSize, sizeof(uint32));
	m_File->Seek( sizeof(uint32), SEEK_CUR);
				
	//	Go to end of file and write out index chunk
	m_File->Seek(0, SEEK_END);
	retVal = Writeidx1Chunk();
	
	return retVal;

}
				
#pragma mark -
#pragma mark === Chunk Routines ===

//-------------------------------------------------------------------
//	WriteRIFFChunk
//-------------------------------------------------------------------
//
//

bool TRIFFWriter::WriteRIFFChunk(uint32 size) 
{	
	#ifdef DEBUG
		printf("Writing RIFF chunk...\n");
	#endif
					
	// Seek to beginning of file
	m_File->Seek(0, SEEK_SET); 
		
	//	Write RIFF form
	WriteIntMsb(m_File, kRiff_RIFF_Chunk, sizeof(uint32)); 	
		
	//	Write file size
	WriteIntLsb(m_File, size, sizeof(int32)); 	
		
	//	Write AVI form
	WriteIntMsb(m_File, kRiff_AVI, sizeof(uint32)); 	
		
	return true;		
}

//-------------------------------------------------------------------
//	WriteLISTChunk
//-------------------------------------------------------------------
//
//

bool TRIFFWriter::WriteLISTChunk(uint32 size) 
{	
	#ifdef DEBUG
		printf("Writing LIST...\n");
	#endif
		
	//	Write LIST chunk
	WriteIntMsb(m_File, kRiff_LIST_Chunk, sizeof(uint32)); 	
		
	//	Write chunk size
	WriteIntLsb(m_File, size, sizeof(int32)); 	
		
	return true; 	
}


//-------------------------------------------------------------------
//	WriteavihChunk
//-------------------------------------------------------------------
//
//	Write AVIHeader chunk
//

bool TRIFFWriter::WriteavihChunk() 
{			
	#ifdef DEBUG
		printf("Writing 'avif' chunk...\n");
	#endif
			
	//	Write out 'avih' chunkID
	WriteIntMsb(m_File, kRiff_avih_Chunk, sizeof(uint32)); 	
	
	//	Write out size of avih data
	uint32 size = sizeof(AVIHeader);
	WriteIntLsb(m_File, size, sizeof(uint32)); 	
		
	//	Write out the header
	bool retVal = WriteAVIHeader();
		
	return retVal;
}


//-------------------------------------------------------------------
//	WriteStreamHeader
//-------------------------------------------------------------------
//
//

bool TRIFFWriter::WriteStreamHeader(AVIStreamHeader *streamHeader)
{		
	#ifdef DEBUG
		printf("Writing strh chunk...\n");
	#endif
		
	//	Write chunkID
	WriteIntMsb(m_File, kRiff_strh_Chunk, sizeof(uint32));
	
	//	Write chunkSize
	int32 size = sizeof(AVIStreamHeader);	
	WriteIntLsb(m_File, size, sizeof(uint32));
	
	//	Write header
	WriteIntMsb(m_File, streamHeader->DataType, sizeof(uint32));
	WriteIntMsb(m_File, streamHeader->DataHandler, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->Flags, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->Priority, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->InitialFrames, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->TimeScale, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->DataRate, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->StartTime, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->DataLength, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->SuggestedBufferSize, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->Quality, sizeof(uint32));
	WriteIntLsb(m_File, streamHeader->SampleSize, sizeof(uint32));
  
	#ifdef DEBUG
		//DumpAVIStreamHeader(streamHeader);
	#endif
	
	return true;
}


//-------------------------------------------------------------------
//	WriteStreamFormat
//-------------------------------------------------------------------
//
//	Write out StreamFormat chunk.  We handle the following types:
//		
//		--	'auds'	(Audio)
//		--	'vids'	(Video) 
//

bool TRIFFWriter::WriteStreamFormat(AVIStreamHeader *streamHeader) 
{	
	#ifdef DEBUG
		printf("Writing strf...\n");
	#endif
	
	bool retVal = false;
	

	//	Write out stream based on previouslly determined format
	switch(streamHeader->DataType)
	{
		case kRiff_vids_Chunk:			
			retVal = WritevidsChunk();
			break;

		case kRiff_auds_Chunk:			
			retVal = WriteaudsChunk();
			break;
									
		//	Unknown
		default:							
			break;
	}

	return retVal;
}


//-------------------------------------------------------------------
//	WritevidsChunk
//-------------------------------------------------------------------
//
//	Write 'vids' Chunk
//

bool TRIFFWriter::WritevidsChunk() 
{		
	#ifdef DEBUG
		printf("Writing vids...\n");
	#endif
		
	bool retVal = true;
	
	//	Write chunkID
	WriteIntMsb(m_File, kRiff_strf_Chunk, sizeof(uint32));
	
	//	Write chunkSize
	int32 size = sizeof(AVIVIDSHeader);	
	WriteIntLsb(m_File, size, sizeof(uint32));
	
	//	Write out header
	WriteIntLsb(m_File, m_VIDSHeader.Size, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.Width, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.Height, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.Planes, sizeof(uint16));
	WriteIntLsb(m_File, m_VIDSHeader.BitCount, sizeof(uint16));
	WriteIntMsb(m_File, m_VIDSHeader.Compression, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.ImageSize, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.XPelsPerMeter, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.YPelsPerMeter, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.NumColors, sizeof(uint32));
	WriteIntLsb(m_File, m_VIDSHeader.ImpColors, sizeof(uint32));
	
	#ifdef DEBUG
		DumpVIDSHeader(&m_VIDSHeader);
	#endif
	
	return retVal;
}


//-------------------------------------------------------------------
//	WriteaudsChunk
//-------------------------------------------------------------------
//
//	Write 'auds' Chunk
//

bool TRIFFWriter::WriteaudsChunk() 
{		
	#ifdef DEBUG
		printf("Writing auds...\n");
	#endif
	
	bool retVal = true;
	
	//	Write chunkID
	WriteIntMsb(m_File, kRiff_strf_Chunk, sizeof(uint32));
	
	//	Write chunkSize
	//int32 size = sizeof(AVIAUDSHeader);	
	WriteIntLsb(m_File, 32, sizeof(uint32));
	
	//	Write out header info
	WriteIntLsb(m_File, m_AUDSHeader.Format, sizeof(uint16));
	WriteIntLsb(m_File, m_AUDSHeader.Channels, sizeof(uint16));       			
	WriteIntLsb(m_File, m_AUDSHeader.Rate, sizeof(uint32));          		
	WriteIntLsb(m_File, m_AUDSHeader.AverageDataRate, sizeof(uint32));
	WriteIntLsb(m_File, m_AUDSHeader.BlockAlign, sizeof(uint16));	
	WriteIntLsb(m_File, m_AUDSHeader.Size, sizeof(uint16));
	WriteIntLsb(m_File, m_AUDSHeader.ExtensionSize, sizeof(uint16));	
	WriteIntLsb(m_File, m_AUDSHeader.SamplesPerBlock, sizeof(uint16));
	WriteIntLsb(m_File, m_AUDSHeader.NumCoefficients, sizeof(uint16));	
	//	Write nothing for coefficients for now...		
	//WriteIntLsb(m_File, m_AUDSHeader.Coefficients->Coef1, sizeof(int16));	
	WriteIntLsb(m_File, 0, sizeof(uint32));
	WriteIntLsb(m_File, m_AUDSHeader.Style, sizeof(uint16));	
	WriteIntLsb(m_File, m_AUDSHeader.ByteCount, sizeof(uint32));
	
	
	#ifdef DEBUG
		//DumpAUDSHeader(&m_AUDSHeader);
	#endif
	
	return retVal;	
}

//-------------------------------------------------------------------
//	Writeidx1Chunk
//-------------------------------------------------------------------
//
//	Write 'idx1' Chunk
//

bool TRIFFWriter::Writeidx1Chunk() 
{		
	bool retVal = true;

	AVIIndex headerIndex;
	
	//	Write header info
	WriteIntMsb(m_File, kRiff_idx1_Chunk, sizeof(uint32));
	WriteIntLsb(m_File, 0x00000000, sizeof(uint32));
	
	//	Save start of index chunk
	off_t indexPos = m_File->Position();
	
	//	Set up next chunk to seek to
	off_t nextChunk = m_MoviChunkOffset;
		
	//	Iterate through and write index information
	for (uint32 index = 0; index < m_IndexCount; index++)
	{
		//	Seek to next chunk offset
		m_File->Seek(nextChunk, SEEK_SET);
		
		//	Get offset
		off_t chunkOffset = m_File->Position();

		//	Read chunk items
		headerIndex.ChunkID = ReadIntMsb(m_File, sizeof(uint32));
		headerIndex.Flags 	= 0x00000000;
		headerIndex.Offset 	= (uint32) chunkOffset;
		headerIndex.Length 	= ReadIntLsb(m_File, sizeof(uint32));
		
		//printf("Writeidx1Chunk:ChunkID: %x\n", headerIndex.ChunkID);
		//printf("Writeidx1Chunk:Flags: %x\n", headerIndex.Flags);
		//printf("Writeidx1Chunk:Offset: %x\n", headerIndex.Offset);
		//printf("Writeidx1Chunk:Length: %x\n", headerIndex.Length);
				
		//	Skip over data length
		m_File->Seek(headerIndex.Length, SEEK_CUR);
		
		//	Save position
		nextChunk = m_File->Position();
		
		//	Seek to end of file
		m_File->Seek(0, SEEK_END);	
		
		//	Write it out
		WriteIntMsb(m_File, headerIndex.ChunkID, sizeof(uint32));
		WriteIntLsb(m_File, headerIndex.Flags, sizeof(uint32));
		WriteIntLsb(m_File, headerIndex.Offset, sizeof(uint32));
		WriteIntLsb(m_File, headerIndex.Length, sizeof(uint32));		
	}
	
	//	Update size of index chunk
	m_File->Seek(0, SEEK_END);	
	off_t curPos = m_File->Position();
	
	uint32 chunkSize = curPos - indexPos;
	printf("Index Size: %d\n", chunkSize);
	
	indexPos -= sizeof(uint32);
	m_File->Seek( indexPos, SEEK_SET);
	WriteAtIntLsb(m_File, chunkSize, sizeof(uint32));
	
	return retVal;	
}

#pragma mark -
#pragma mark === AVI Header Routines ===

//-------------------------------------------------------------------
//	WriteAVIHeader
//-------------------------------------------------------------------
//
//	Write out AVIHeader
//

bool TRIFFWriter::WriteAVIHeader()
{		
	WriteIntLsb(m_File, m_AVIHeader.TimeBetweenFrames, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.MaximumDataRate, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.PaddingGranularity, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.Flags, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.TotalNumberOfFrames, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.NumberOfInitialFrames, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.NumberOfStreams, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.SuggestedBufferSize, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.Width, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.Height, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.TimeScale, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.DataRate, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.StartTime, sizeof(uint32));
	WriteIntLsb(m_File, m_AVIHeader.DataLength, sizeof(uint32));

	#ifdef DEBUG
		//DumpAVIHeader(&m_AVIHeader);
	#endif
		
	//	Success
	return true;
}

#pragma mark -
#pragma mark === AVI Stream Routines ===

//-------------------------------------------------------------------
//	Write00DCChunk
//-------------------------------------------------------------------
//
//

bool TRIFFWriter::Write00DCChunk(uint32 chunkID, uint32 length, void *data)
{
	bool retVal = true;
	
	//printf("Write00DCChunk ENTER\n");
	
	//	Write chunk
	WriteIntMsb(m_File, chunkID, sizeof(uint32));
	WriteIntLsb(m_File, length, sizeof(uint32));
		
	//	Write video data
	ssize_t written = m_File->Write(data, length);
	
	//	Increment our internal frame counter
	m_TotalFrames++;
	
	//	Increment our internal index counter
	m_IndexCount++;
	
	//printf("Write00DCChunk EXIT\n");
	
	return(retVal);
}



//-------------------------------------------------------------------
//	Init00WBChunk
//-------------------------------------------------------------------
//
//	Init waveform data chunk
//

bool TRIFFWriter::Init00WBChunk(uint32 chunkID, uint32 length)
{
	bool retVal = true;
			
	//	Write chunk
	WriteIntMsb(m_File, chunkID, sizeof(uint32));
	WriteIntLsb(m_File, length, sizeof(uint32));

	//	Increment audio byte count
	m_AUDSHeader.ByteCount += length;
	
	//	Increment our internal index counter
	m_IndexCount++;

	return(retVal);
}


//-------------------------------------------------------------------
//	Write00WBChunk
//-------------------------------------------------------------------
//
//	Compressed waveform data
//

bool TRIFFWriter::Write00WBChunk(uint32 chunkID, uint32 length, void *data)
{
	bool retVal = true;
			
	//	Write chunk
	WriteIntMsb(m_File, chunkID, sizeof(uint32));
	WriteIntLsb(m_File, length, sizeof(uint32));

	//	Write audio data
	m_File->Write(data, length);

	//	Increment audio byte count
	m_AUDSHeader.ByteCount += length;
	
	//	Increment our internal index counter
	m_IndexCount++;

	return(retVal);
}


#pragma mark -
#pragma mark === Utility Routines ===

//-------------------------------------------------------------------
//	SetStreamCount
//-------------------------------------------------------------------
//
//

bool TRIFFWriter::SetStreamCount(int32 streamCount)
{
	//	Verify we have at least one stream
	if (streamCount <= 0)
		return false;
		
	//	Verify we have two streams or less
	if (streamCount > 2)
		return false;
	
	//	Set streams		
	m_StreamCount = streamCount;
	
	return true;

}

