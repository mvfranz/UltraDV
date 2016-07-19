//---------------------------------------------------------------------
//
//	File:	TRIFFReader.cpp
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


//	Includes
#include <Debug.h>
//#include <iostream> // ABH

#include "BuildApp.h"

#include "AppUtils.h"
#include "DebugUtils.h"
#include "AVIUtils.h"

#include "AVINodeConstants.h"

#include "TAppleVideoCodec.h"
#include "TCinepakCodec.h"
#include "TMSRLECodec.h"
#include "TMSVideoCodec.h"
#include "TRGBCodec.h"
#include "InfoAlert.h"


#include "TRIFFReader.h"

//#define ABH

//	Macros
#define ChunkName(a,b,c,d) (                    \
		((static_cast<unsigned long>(a)&255)<<24)   \
		+ ((static_cast<unsigned long>(b)&255)<<16)   \
		+ ((static_cast<unsigned long>(c)&255)<<8)    \
		+ ((static_cast<unsigned long>(d)&255)))


//-------------------------------------------------------------------
//	IsRIFFFile
//-------------------------------------------------------------------
//
//	Determine file type
//

bool IsRIFFFile(BFile *file)
{
	// Seek to beginning of file
	file->Seek(0, SEEK_SET);

	// Seek to beginning of file
	file->Seek(0, SEEK_SET);

	uint32 form;

	//	Get first chunk
	form = ReadIntMsb(file, 4);

	//	Form should be 'RIFF'
	if (form != ChunkName('R','I','F','F'))
		return false;

	//	Skip size indicator
	file->Seek(sizeof(uint32), SEEK_CUR);

	//	Get next chunk
	form = ReadIntMsb(file, 4);

	//	Type should be 'AVI '
	if (form == ChunkName('A','V','I',' '))
		return true;

	//      RIFF file, but not AVI file
	printf("Unsupported RIFF file.\n");
	return false;
}


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TRIFFReader::TRIFFReader(BFile *file)
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

TRIFFReader::~TRIFFReader()
{
	//	Clean up
	if (m_HeaderIndex)
		free(m_HeaderIndex);

	//	Free video frame lists
	if (m_VideoFrameList) {
		for( uint32 index = 0; index < m_VideoFrameList->CountItems(); index++) {
			AVIVideoFrame *theFrame = (AVIVideoFrame *)m_VideoFrameList->ItemAt(index);

			if (theFrame)
				free(theFrame);
		}

		delete m_VideoFrameList;
	}

	//	Free audio frame lists
	if (m_AudioFrameList) {
		for( uint32 index = 0; index < m_AudioFrameList->CountItems(); index++) {
			AVIAudioFrame *theFrame = (AVIAudioFrame *)m_AudioFrameList->ItemAt(index);

			if (theFrame)
				free(theFrame);
		}

		delete m_AudioFrameList;
	}

	//	Codecs
	if (m_VideoCodec)
		delete m_VideoCodec;

	//	Clean up file
	m_File->Unset();
	delete m_File;
	m_File = NULL;

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//	Perform default initialization
//

void TRIFFReader::Init()
{
	//	Set stream count to 0
	m_StreamCount           = 0;
	m_InitCheck                     = false;
	m_HasVideo                      = false;
	m_HasAudio                      = false;
	m_IndexCount            = 0;
	m_HeaderIndex           = NULL;
	m_VideoCodec            = NULL;
	m_AudioFrameCount       = 0;

	for(int32 index = 0; index < kRiffMaxStreams; index++) {
		m_StreamType[index] = 0;
		m_StreamOK[index]       = false;
	}

	//	Init frame lists
	m_VideoFrameList = new BList();
	m_AudioFrameList = new BList();

	//	Rewind file
	m_File->Seek(0, SEEK_SET);

	//	Process
	bool retVal = ParseAVIFile();

	//	Abort if failure
	if (retVal == false) {
		// ABH display dialog for unhandled video format
		InfoAlert("Unsupported Video Format");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Init Codecs
	retVal = InitCodecs();

	if (retVal == false)
		m_InitCheck = false;
	else
		m_InitCheck = true;
}


//-------------------------------------------------------------------
//	InitCheck
//-------------------------------------------------------------------
//
//	Return state of initialization
//

bool TRIFFReader::InitCheck()
{
	return m_InitCheck;
}

#pragma mark -
#pragma mark === File Parsing ===

//-------------------------------------------------------------------
//	ParseAVIFile
//-------------------------------------------------------------------
//
//	Traverse thorugh AVI file.
//

bool TRIFFReader::ParseAVIFile()
{

	uint32 chunkID;
	uint32 chunkSize;

	m_MoviChunkOffset       = 0;
	m_AVISize                       = 1;
	m_UseIndexFlag          = false;
	bool retVal                     = false;

	//	Rewind the file
	m_File->Seek(0, SEEK_SET);

	//	Get file size
	off_t fileSize;
	m_File->GetSize(&fileSize);

	//	Traverse through entire file
	while( (m_File->Position() <= fileSize) && (m_AVISize > 0) ) {
		//	Read in chunk and size identifiers.  We are proceeding through switch
		//	in the most likely order of chunks appearence in the file
		chunkID         = ReadIntMsb(m_File, sizeof(uint32));
		chunkSize       = ReadIntLsb(m_File, sizeof(uint32));

		printf("ParseAVIFile ");
		DumpRIFFID(chunkID);

		//	Subtract from total RIFF size
		m_AVISize -= sizeof(uint32) * 2;

		//	Handle various chunk formats
		switch(chunkID)
		{
		//	'RIFF'
		case kRiff_RIFF_Chunk:
			retVal = ReadRIFFChunk(chunkSize);
			break;

		//	'LIST'
		case kRiff_LIST_Chunk:
			retVal = ReadLISTChunk(chunkSize);
			break;

		//	'avih'
		case kRiff_avih_Chunk:
			retVal = ReadavihChunk(chunkSize);
			break;

		//	'strh'
		case kRiff_strh_Chunk:
			retVal = ReadStreamHeader(chunkSize);
			break;

		//	'strd'
		case kRiff_strd_Chunk:
			retVal = ReadstrdChunk(chunkSize);
			break;

		//	'strf'
		case kRiff_strf_Chunk:
			retVal = ReadStreamFormat(chunkSize);
			break;

		//	'00pc'
		//	'01pc'
		case kRiff_00pc_Chunk:
		case kRiff_01pc_Chunk:
			break;

		//	'idx1'
		case kRiff_idx1_Chunk:
			//cerr << "Reading idx1...\n";
			retVal = Readidx1Chunk(chunkSize);
			break;

		//	Skip over these
		case kRiff_hdrl_Chunk:
		case kRiff_strl_Chunk:
		case kRiff_vedt_Chunk:
		case kRiff_vids_Chunk:
		case kRiff_00AM_Chunk:
		case kRiff_DISP_Chunk:
		case kRiff_ISBJ_Chunk:
		case kRiff_JUNK_Chunk:
		case kRiff_strn_Chunk:
		{
			//	Pad
			if (chunkSize & 0x01)
				chunkSize++;

			//	Skip these chunks
			m_File->Seek(chunkSize, SEEK_CUR);
		}
		break;

		//	Handle all others and end of file
		default: {
			//	Are we past the end of the file?
			if ( m_AVISize <= 0 ) {
				//	Seek to end of file
			}
			printf("TRIFFReader: end of file and failure\n");
			retVal = false;
			//	Handle stream chunks?

			//
			printf("ParseAVIFIle::Unknown RIFF Chunk ");
			//InfoAlert("Unknown RIFF Chunk");
			DumpRIFFID(chunkID);

		}
		break;
		}

		//	Check for errors
		if (retVal == false)
			return retVal;

		//	Deduct size
		m_AVISize -= chunkSize;
		if (chunkSize & 0x01)
			m_AVISize--;
	}

	//	Complete initialization
	if (m_HasAudio) {
		if (m_AUDSHeader.BlockAlign == 0)
			m_AUDSHeader.BlockAlign = 1;

		//	Use float to avoid uint32 overflow
		if (m_AUDSHeader.Rate) {
			m_TotalAudioTime = (uint32) ((((float) m_AUDSHeader.ByteCount * (float)m_AUDSHeader.SamplesPerBlock * 1000.0)
			                              / (float)m_AUDSHeader.BlockAlign) / (float)m_AUDSHeader.Rate);
		} else
			m_TotalAudioTime = 0;
	}


#ifdef DEBUG
	printf("Total Video Chunks: %d\n", m_VideoFrameList->CountItems());
	printf("Total Audio Chunks: %d\n", m_AudioFrameList->CountItems());
#endif

	//	Return success or failure
	if (retVal) {
		printf("TRIFFReader: end of file and success\n");
	} else {
		printf("TRIFFReader: end of file and failure\n");
	}
	return retVal;
}


#pragma mark -
#pragma mark === Data Retrieval ===

//-------------------------------------------------------------------
//	ReadVideoFrame
//-------------------------------------------------------------------
//
//	Return true if video frame can be read and allocated.  Caller
//	is responsible for freeing allocated memory
//

bool TRIFFReader::ReadVideoFrame(uint32 frameNum, void *buffer)
{
	bool retVal = false;

	//	Have we been initialized?
	if (!m_InitCheck)
		return retVal;

	//	No video chunk
	if (!m_HasVideo)
		return retVal;

	printf("ReadVideoFrame:\n");

	printf("ReadVideoFrame: frameNum = %d, VideoFrameCount = %d\n",frameNum, VideoFrameCount());

	//	Make sure we will not overflow number of video frames
	if (frameNum > VideoFrameCount() )
		return retVal;

	//	Get the frame
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_VideoFrameList->ItemAt(frameNum);
	if (theFrame == NULL)
		return false;

	//	Decode
	switch(theFrame->ChunkID)
	{
	//	Video data
	case kRiff_00db_Chunk:
	case kRiff_00dc_Chunk:
	case kRiff_01dc_Chunk:
		retVal = m_VideoCodec->DecodeFrame(theFrame, buffer, 16);
		break;

	default:
#ifdef DEBUG
		printf("Non-video chunk...\n");
		DumpRIFFID(theFrame->ChunkID);
#endif
		return retVal;
	}

	return retVal;
}


//-------------------------------------------------------------------
//	ReadVideoFrame
//-------------------------------------------------------------------
//
//	Return true if audio frame can be read and allocated.  Caller
//	is responsible for freeing allocated memory
//

bool TRIFFReader::ReadAudioFrame(uint32 frameNum, void *buffer)
{
	bool retVal = false;

	//	Have we been initialized?
	if (!m_InitCheck)
		return retVal;

	//	No audio chunk
	if (!m_HasAudio)
		return retVal;

	//	Make sure we will not overflow number of video frames
	//if (frameNum > FrameCount() )
	//	return retVal;

	return retVal;
}


#pragma mark -
#pragma mark === Chunk Routines ===

//-------------------------------------------------------------------
//	ReadRIFFChunk
//-------------------------------------------------------------------
//
//	Return true if this is a 'RIFF/AVI ' file

bool TRIFFReader::ReadRIFFChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading RIFF chunk...\n";
	#endif

	//	Save file size
	m_AVISize = (2 * size) - 4;

	//	Get form
	uint32 form = ReadIntMsb(m_File, 4);

	//	Type should be 'AVI '
	if (form == ChunkName('A','V','I',' '))
		return true;

	//      RIFF file, but not AVI file
	return false;
}

//-------------------------------------------------------------------
//	ReadListHeaderChunk
//-------------------------------------------------------------------
//
//	Read in ListHeader chunk.  This chunk contains the AVIHeader
//	and the StreamListHeader chunk defining the streams.
//

bool TRIFFReader::ReadLISTChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading LIST...\n";
	#endif

	uint32 chunkID = ReadIntMsb(m_File, sizeof(uint32));

	//	Is there an Index chunk?  If so, ship over
	//	movie list data.
	if ( (chunkID == kRiff_movi_Chunk) && m_UseIndexFlag ) {
		m_MoviChunkOffset = m_File->Position();

		//	Back up over type code
		m_MoviChunkOffset -= 4;
		size -= 4;

		//	Pad
		if (size & 0x01)
			size++;

		//	Skip over the list
		m_File->Seek( size,  SEEK_CUR);
	} else   {
		// Re-add list size minus size of type
		m_AVISize += (size - 4);
	}

	return true;
}


//-------------------------------------------------------------------
//	ReadavihChunk
//-------------------------------------------------------------------
//
//	Read in AVIHeader chunk
//

bool TRIFFReader::ReadavihChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading 'avif' chunk...\n";
	#endif

	//	Verify size of ChunkSize.	It should be 56 bytes
	if (size != 0x38)
		return false;

	//	Load in the header
	bool retVal = ReadAVIHeader();

	if (retVal == true) {
		//	Setup color map frame
		//avi->cmap_frame_num = avi_hdr->tot_frames / cmap_sample_cnt;

		//	Convert file time to milliseconds
		double fileTime = (double)((m_AVIHeader.TimeBetweenFrames)/1000.0);

		//	Setup node time
		//m_Node->GetSettings()->vid_time =  (uint32)(fileTime);
		//fileTime -= (double)(m_Node->GetSettings()->vid_time);
		//m_Node->GetSettings()->vid_timelo = ( fileTime * (double)(1 << 24) );

		//	Verify 2 streams or less
		if (m_AVIHeader.NumberOfStreams > 2) {
			printf("Unable to handle %d streams.", m_AVIHeader.NumberOfStreams);
			return false;
		}

		//	Check header flags
		if ( (m_AVIHeader.Flags & kAVIMustUseIndexFlag) || (m_AVIHeader.Flags & kAVIHasIndexFlag) )
			m_UseIndexFlag = true;
		else
			m_UseIndexFlag = false;

		//#ifdef DEBUG
		//	DumpAVIHeaderFlags(&m_AVIHeader);
		//#endif
	}

	return retVal;
}


//-------------------------------------------------------------------
//	ReadStreamHeaderChunk
//-------------------------------------------------------------------
//
//

bool TRIFFReader::ReadStreamHeader(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading strh chunk...\n";
	//cerr << "m_StreamCount: " << m_StreamCount << "\n";
	#endif

	//	Determine StreamHeader to use
	AVIStreamHeader *streamHeader;

	if (m_StreamCount == 0)
		streamHeader = &m_StreamHeaderOne;
	else
		streamHeader = &m_StreamHeaderTwo;

	//	Fill header
	streamHeader->DataType                          = ReadIntMsb(m_File, sizeof(uint32));
	streamHeader->DataHandler                       = ReadIntMsb(m_File, sizeof(uint32));
	streamHeader->Flags                             = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->Priority                          = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->InitialFrames                     = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->TimeScale                 = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->DataRate                  = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->StartTime                 = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->DataLength                = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->SuggestedBufferSize       = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->Quality                           = ReadIntLsb(m_File, sizeof(uint32));
	streamHeader->SampleSize                        = ReadIntLsb(m_File, sizeof(uint32));

	//	Adjust size and seek
	size -= sizeof(AVIStreamHeader);

	if (size > 0)
		m_File->Seek(size, SEEK_CUR);

	//#ifdef DEBUG
	//	DumpAVIStreamHeader(streamHeader);
	//#endif

	return true;
}


//-------------------------------------------------------------------
//	ReadstrdChunk
//-------------------------------------------------------------------
//
//	Read in 'strd' chunk.
//

bool TRIFFReader::ReadstrdChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading strd chunk...\n";
	#endif

	bool retVal = false;

	//	Save current strd size
	m_STRDCurSize = size;

	//	Pad
	if (size & 0x01)
		size++;

	//	Allocate STRD buffer
	if (m_STRDSize == 0) {
		m_STRDSize = size;
		m_STRDBuffer = (uchar *)malloc(size);
		ASSERT(m_STRDBuffer);
	}
	//	Reallocate...
	else if (size > m_STRDSize) {
		uchar *tmpBuf;

		m_STRDSize = size;
		tmpBuf = (uchar *)realloc(m_STRDBuffer, size);
		ASSERT(tmpBuf);
		m_STRDBuffer = tmpBuf;
	}

	//	Read in strd data
	m_File->Read(m_STRDBuffer, size);

	return retVal;
}

//-------------------------------------------------------------------
//	ReadStreamFormat
//-------------------------------------------------------------------
//
//	Read in StreamFormat chunk.  We handle the following types:
//
//		--	'auds'	(Audio)
//		--	'vids'	(Video)
//		--	'pads'	(Padding)
//		--	'txts'	(Text)
//

bool TRIFFReader::ReadStreamFormat(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading strf...\n";
	#endif

	bool retVal = true;

	//	Pad
	if (size & 0x01)
		size++;

	//	Determine stream header to use
	uint32 dataType;

	if (m_StreamCount == 0)
		dataType = m_StreamHeaderOne.DataType;
	else
		dataType = m_StreamHeaderTwo.DataType;

	m_StreamType[m_StreamCount] = dataType;

	//	Read in stream based on previouslly determined format
	switch(dataType)
	{
	case kRiff_vids_Chunk:
		retVal = ReadvidsChunk(size);
		m_StreamOK[m_StreamCount] = retVal;
		break;

	case kRiff_auds_Chunk:
		retVal = ReadaudsChunk(size);
		m_StreamOK[m_StreamCount] = retVal;
		break;

	//	Skip
	case kRiff_pads_Chunk:
		//cerr << "strf/pads ignored.\n";
		m_StreamOK[m_StreamCount] = false;
		m_File->Seek(size, SEEK_CUR);
		break;

	//	Skip
	case kRiff_txts_Chunk:
		//cerr << "strf/txts ignored.\n";
		m_StreamOK[m_StreamCount] = false;
		m_File->Seek(size, SEEK_CUR);
		break;

	//	Unknown.  Skip over
	default:
		//cerr << "Unknown strf/type ignored.\n";
		m_StreamOK[m_StreamCount] = false;
		m_File->Seek(size, SEEK_CUR);
		break;
	}

	//	Increment stream count
	m_StreamCount++;

	return retVal;
}

//-------------------------------------------------------------------
//	ReadVIDSChunk
//-------------------------------------------------------------------
//
//	Read 'vids' Chunk
//

bool TRIFFReader::ReadvidsChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading vids...\n";
	#endif

	bool retVal = false;

	//	Set our video flag to true
	m_HasVideo = true;

	//	Adjust size for padding
	if (size & 0x01)
		size++;

	//	Read in header
	m_VIDSHeader.Size               = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.Width              = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.Height             = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.Planes             = ReadIntLsb(m_File, sizeof(uint16));
	m_VIDSHeader.BitCount           = ReadIntLsb(m_File, sizeof(uint16));
	m_VIDSHeader.Compression        = ReadIntMsb(m_File, sizeof(uint32));
	m_VIDSHeader.ImageSize          = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.XPelsPerMeter      = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.YPelsPerMeter      = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.NumColors          = ReadIntLsb(m_File, sizeof(uint32));
	m_VIDSHeader.ImpColors          = ReadIntLsb(m_File, sizeof(uint32));

	//	Adjust size
	size -= sizeof(AVIVIDSHeader);

	//	Check for supported video codec
	switch(m_VIDSHeader.Compression)
	{

	default:
		retVal = true;
		break;
	}

	//	Calculate amount of data left and seek past it
	if (size > 0)
		m_File->Seek(size, SEEK_CUR);

	//#ifdef DEBUG
	//	DumpVIDSHeader(&m_VIDSHeader);
	//#endif

	return retVal;
}


//-------------------------------------------------------------------
//	ReadAUDSChunk
//-------------------------------------------------------------------
//
//	Read 'auds' Chunk
//

bool TRIFFReader::ReadaudsChunk(uint32 size)
{
	#ifdef DEBUG
	//cerr << "Reading auds...\n";
	#endif

	bool retVal = true;

	//	Set our audio flag to true
	m_HasAudio = true;

	//	Read in header info
	m_AUDSHeader.Format                             = ReadIntLsb(m_File, sizeof(uint16));
	m_AUDSHeader.Channels                   = ReadIntLsb(m_File, sizeof(uint16));
	m_AUDSHeader.Rate                               = ReadIntLsb(m_File, sizeof(uint32));
	m_AUDSHeader.AverageDataRate    = ReadIntLsb(m_File, sizeof(uint32));
	m_AUDSHeader.BlockAlign                 = ReadIntLsb(m_File, sizeof(uint16));
	m_AUDSHeader.SamplesPerBlock    = 1;
	m_AUDSHeader.ByteCount                  = 0;

	//	Adjust size for padding
	if (size & 0x01)
		size++;

	//	Get sample size
	if (size >= 0x10) {
		m_AUDSHeader.Size = ReadIntLsb(m_File, sizeof(uint16));
		size -= 0x10;
	}
	//	Old style.  Set to 8-bits per sample
	else{
		m_AUDSHeader.Size = 8;
		size -= 0x0e;
	}

	//	Figure out compression
	switch(m_AUDSHeader.Format)
	{
	case WAVE_FORMAT_PCM:
	{
		switch(m_AUDSHeader.Size)
		{
		case 8:
			m_AudioType = kAudioLinear;
			break;

		case 16:
			m_AudioType = kAudioSigned;
			break;

		default:
			m_AudioType = kAudioInvalid;
			break;
		}
	}
	break;



	case WAVE_FORMAT_ADPCM:
	{
		switch(m_AUDSHeader.Size)
		{
		case 4:
			m_AudioType = kAudioADPCM;
			break;

		default:
			m_AudioType = kAudioInvalid;
			break;
		}

		m_AUDSHeader.ExtensionSize   = ReadIntLsb(m_File, sizeof(uint16));
		m_AUDSHeader.SamplesPerBlock = ReadIntLsb(m_File, sizeof(uint16));
		m_AUDSHeader.NumCoefficients = ReadIntLsb(m_File, sizeof(uint16));
		size -= sizeof(uint16) * 3;

				#ifdef DEBUG
		printf("MSADPM_EXT: SamplesPerBlock %d NumCoefficients %d\n", m_AUDSHeader.SamplesPerBlock, m_AUDSHeader.NumCoefficients);
				#endif

		for( uint32 index = 0; index < m_AUDSHeader.NumCoefficients; index++) {
			int16 coefficient1 = ReadIntLsb(m_File, sizeof(uint16));
			int16 coefficient2 = ReadIntLsb(m_File, sizeof(uint16));
			size -= sizeof(uint16) * 2;

					#ifdef DEBUG
			printf("%d) coefficient1 %d coefficient2 %d\n", index, coefficient1, coefficient2);
					#endif
		}
	}
	break;

	case WAVE_FORMAT_DVI_ADPCM:
	{
		m_AudioType = kAudioDVI;
		m_AUDSHeader.ExtensionSize       = ReadIntLsb(m_File, sizeof(uint16));
		m_AUDSHeader.SamplesPerBlock = ReadIntLsb(m_File, sizeof(uint16));
		size -= sizeof(uint16) * 2;

				#ifdef DEBUG
		printf("DVI: samples per block %d\n", m_AUDSHeader.SamplesPerBlock);
				#endif
	}
	break;

	case WAVE_FORMAT_MULAW:
	{
		m_AudioType = kAudioULaw;
	}
	break;

	case WAVE_FORMAT_GSM610:
	{
		m_AudioType = kAudioMSGSM;
		m_AUDSHeader.ExtensionSize       = ReadIntLsb(m_File, sizeof(uint16));
		m_AUDSHeader.SamplesPerBlock = ReadIntLsb(m_File, sizeof(uint16));
		size -= sizeof(uint16) * 2;

				#ifdef DEBUG
		printf("GSM: Samples per block %d\n", m_AUDSHeader.SamplesPerBlock);
				#endif

		//	Init GSM tables here...
	}
	break;

	default:
		m_AudioType = kAudioInvalid;
		retVal = false;
		break;
	}

	//	Complete initialization
	m_AudioFrequency  = m_AUDSHeader.Rate;
	m_AudioChannels   = m_AUDSHeader.Channels;

	//	Detemrine BPS
	switch(m_AUDSHeader.Size)
	{
	case 4:
	case 8:
		m_AudioBPS = 1;
		break;

	case 16:
		m_AudioBPS = 2;
		break;

	case 32:
		m_AudioBPS = 4;
		break;

	default:
		m_AudioBPS = 1000 + m_AUDSHeader.Size;
		break;
	}

	m_AudioEnd   = 0;

	if (m_AudioChannels > 2)
		retVal = false;

	/*
	 #ifdef DEBUG
	   {
	        printf("Audio Codec: ");
	        DumpAudioType(m_AUDSHeader.Format);
	        printf(" Rate=%d Chans = %d bps = %d\n", m_AudioFrequency, m_AudioChannels, m_AUDSHeader.Size);
	        printf("block_align %d\n", m_AUDSHeader.BlockAlign);
	   }
	 #endif
	 */

	//	Modify type
	if (m_AudioChannels==2)
		m_AudioType |= kAudioStereoMask;

	if (m_AudioChannels==2)
		m_AudioType |= kAudioBPS2Mask;

	#ifdef DEBUG
	printf("size = %d retVal = %x\n", size, retVal);
	#endif

	//	Seek through rest of chunk
	m_File->Seek(size, SEEK_CUR);

	//#ifdef DEBUG
	//	DumpAUDSHeader(&m_AUDSHeader);
	//#endif

	return retVal;
}

//-------------------------------------------------------------------
//	Readidx1Chunk
//-------------------------------------------------------------------
//
//	Read 'idx1' Chunk
//

bool TRIFFReader::Readidx1Chunk(uint32 size)
{
	bool retVal = true;

	//	Pad
	if (size & 0x01)
		size++;

	//	Set up count
	m_IndexCount = size >> 4;

	// Allocate index entries
	m_HeaderIndex = (AVIIndex *)malloc(m_IndexCount * sizeof(AVIIndex));

	//	Verify allocation
	if (m_HeaderIndex == 0) {
		printf("Error allocating AVIIndex.\n");
		retVal = false;
		return retVal;
	}

	//	Initialize minumum offset
	uint32 minOffset = 0xFFFFFFFF;

	//	Iterate through and read index information
	for (uint32 index = 0; index < m_IndexCount; index++) {
		m_HeaderIndex[index].ChunkID    = ReadIntMsb(m_File, sizeof(uint32));
		m_HeaderIndex[index].Flags              = ReadIntLsb(m_File, sizeof(uint32));
		m_HeaderIndex[index].Offset     = ReadIntLsb(m_File, sizeof(uint32));
		m_HeaderIndex[index].Length     = ReadIntLsb(m_File, sizeof(uint32));

		//	Check mimumum offset and adjust if neccessary
		if ( m_HeaderIndex[index].Offset < minOffset)
			minOffset = m_HeaderIndex[index].Offset;

		//#ifdef DEBUG
		//	DumpRIFFID(m_HeaderIndex[index].ChunkID);
		//#endif
	}

	//	Calculate offset from either start of file or from
	//	start of the LIST movi.  Use best guess to determine
	//	as there is no set rule to do so.
	if (minOffset >= m_MoviChunkOffset)
		m_MoviChunkOffset = 0;

	//	Skip of ID and Size
	m_MoviChunkOffset += 8;

	//	Jump over any extra bytes
	size -= (m_IndexCount << 4);
	if (size > 0)
		m_File->Seek(size, SEEK_CUR);

	//	Save position for later restore
	int32 tmpPos = m_File->Position();

	//	Adjust offsets and get stream types
	int32 streamNum = -1;
	for (uint32 index = 0; index < m_IndexCount; index++) {
		m_HeaderIndex[index].Offset += m_MoviChunkOffset;

		uint32 streamID = m_HeaderIndex[index].ChunkID & kRiff_FF00_Chunk;

		//	Get stream types
		switch(streamID)
		{
		case kRiff_00_Chunk:
			streamNum = 0;
			break;

		case kRiff_01_Chunk:
			streamNum = 1;
			break;

		case kRiff_02_Chunk:
			streamNum = 2;
			break;

		case kRiff_03_Chunk:
			streamNum = 3;
			break;

		case kRiff_04_Chunk:
			streamNum = 4;
			break;

		case kRiff_05_Chunk:
			streamNum = 5;
			break;

		case kRiff_06_Chunk:
			streamNum = 6;
			break;

		case kRiff_07_Chunk:
			streamNum = 7;
			break;

		default:
			streamNum = -1;
			break;
		}

		uint32 streamType;
		bool streamOK;

		if (streamNum >= 0) {
			streamType = m_StreamType[streamNum];
			streamOK   = m_StreamOK[streamNum];
		} else   {
			streamType = 0;
			streamOK   = false;
		}
		// ABH cinepak error here

#ifdef DEBUG
		if (streamOK == false) {
			printf("Readidx1Chunk::streamOK == false\n");
			continue;
		}
#endif
		//	Handle streamType
		switch(streamType) {
		case kRiff_vids_Chunk: {
			//printf("idx1 vids stream...\n");
			retVal = Read00DCChunk(m_HeaderIndex[index].ChunkID, m_HeaderIndex[index].Offset, m_HeaderIndex[index].Length, streamNum);
		}
		break;

		case kRiff_auds_Chunk:
		{
			//printf("idx1 auds stream...\n");
			retVal = Read00WBChunk(m_HeaderIndex[index].ChunkID, m_HeaderIndex[index].Offset, m_HeaderIndex[index].Length, streamNum);
		}
		break;

		default:
			printf ("Readidx1Chunk::Unknown Stream Type\n");
			break;
		}
	}

	//	Restore file position
	m_File->Seek(tmpPos, SEEK_SET);

	return retVal;
}


//-------------------------------------------------------------------
//	ReadpcChunk
//-------------------------------------------------------------------
//
//	Read 'pc' Chunk
//

bool TRIFFReader::ReadpcChunk(uint32 size)
{
	size = 0;

	return false;
}

#pragma mark -
#pragma mark === AVI Header Routines ===

//-------------------------------------------------------------------
//	ReadAVIHeader
//-------------------------------------------------------------------
//
//

bool TRIFFReader::ReadAVIHeader()
{
	m_AVIHeader.TimeBetweenFrames           = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.MaximumDataRate             = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.PaddingGranularity          = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.Flags                               = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.TotalNumberOfFrames         = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.NumberOfInitialFrames       = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.NumberOfStreams             = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.SuggestedBufferSize         = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.Width                               = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.Height                              = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.TimeScale                   = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.DataRate                    = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.StartTime                   = ReadIntLsb(m_File, sizeof(uint32));
	m_AVIHeader.DataLength                  = ReadIntLsb(m_File, sizeof(uint32));

	//#ifdef DEBUG
	//	DumpAVIHeader(&m_AVIHeader);
	//#endif

	//	Success
	return true;
}

#pragma mark -
#pragma mark === AVI Stream Routines ===

//-------------------------------------------------------------------
//	Read00DCChunk
//-------------------------------------------------------------------
//
//

bool TRIFFReader::Read00DCChunk(uint32 chunkID, uint32 offset, uint32 length, uint32 streamNum)
{
	bool retVal = true;

	AVIVideoFrame *videoFrame = NULL;

	//	Alocate frame
	videoFrame = (AVIVideoFrame *) malloc(sizeof(AVIVideoFrame));
	ASSERT(videoFrame);

	//	Initialize
	videoFrame->ChunkID = chunkID;
	videoFrame->Offset      = offset;
	videoFrame->Length      = length;

	//	Add to frame list
	m_VideoFrameList->AddItem(videoFrame);

	return(retVal);
}


//-------------------------------------------------------------------
//	Read00WCChunk
//-------------------------------------------------------------------
//
//	Compressed waveform data
//

bool TRIFFReader::Read00WBChunk(uint32 chunkID, uint32 offset, uint32 length, uint32 streamNum)
{
	bool retVal = true;

	AVIAudioFrame *audioFrame = NULL;

	//	Alocate frame
	audioFrame = (AVIAudioFrame *) malloc(sizeof(AVIAudioFrame));
	ASSERT(audioFrame);

	//	Initialize
	audioFrame->ChunkID = chunkID;
	audioFrame->Offset      = offset;
	audioFrame->Length      = length;

	//	Increment audio byte count
	m_AUDSHeader.ByteCount += length;

	//	Add to frame list
	m_AudioFrameList->AddItem(audioFrame);

	return(retVal);
}


#pragma mark -
#pragma mark === Codec Handling ===

//-------------------------------------------------------------------
//	InitCodecs
//-------------------------------------------------------------------
//
//	Init audio and video decoders
//

bool TRIFFReader::InitCodecs()
{

	bool retVal = false;

	if (m_HasVideo)
		retVal = InitVideoCodec();

	if (m_HasAudio)
		retVal = InitAudioCodec();

	return retVal;

}


//-------------------------------------------------------------------
//	InitVideoCodec
//-------------------------------------------------------------------
//
//	Initialize video decoders
//

bool TRIFFReader::InitVideoCodec()
{

	if (!m_HasVideo)
		return false;

	//	Determine compression and create codec
	switch(m_VIDSHeader.Compression)
	{
	//	Uncompressed RGB
	case RIFF_RGB:
	case RIFF_rgb:
		m_VideoCodec = new TRGBCodec(this);
		break;

	//	Apple Video "Road Pizza"
	case RIFF_rpza:
	case RIFF_azpr:
		m_VideoCodec = new TAppleVideoCodec(this);
		break;

	//	Microsoft Video 01
	case RIFF_wham:
	case RIFF_WHAM:
	case RIFF_cram:
	case RIFF_CRAM:
	case kRiff_msvc_Chunk:
	case kRiff_MSVC_Chunk:
		m_VideoCodec = new TMSVideoCodec(this);
		break;

	//	Microsoft RLE
	case RIFF_RLE8:
	case RIFF_rle8:
		m_VideoCodec = new TMSRLECodec(this);
		break;

	//	Radius Cinepak "Compact Video"
	case RIFF_cvid:
	case RIFF_CVID:
		m_VideoCodec = new TCinepakCodec(this);
		break;

	case RIFF_MJPG:
	case RIFF_mJPG:
	case RIFF_jpeg:
	case RIFF_JPEG:
		InfoAlert("Unhandled JPEG codec\n");
		DumpRIFFID(m_VIDSHeader.Compression);

		return false;
		break;

	case RIFF_iv31:
	case RIFF_IV31:
	case RIFF_iv32:
	case RIFF_IV32:
	case RIFF_iv41:
	case RIFF_IV41:
//		case RIFF_iv50:
//		case RIFF_IV50:
		InfoAlert("Unhandled Indeo codec\n");
		DumpRIFFID(m_VIDSHeader.Compression);

		return false;
		break;

	default:
		InfoAlert("Unhandled Video Codec");

			#ifdef DEBUG
		printf("Unhandled CODEC: ");
		DumpRIFFID(m_VIDSHeader.Compression);
			#endif
		return false;
		break;
	}

	if (m_VideoCodec == NULL)
		return false;

	return true;
}


//-------------------------------------------------------------------
//	InitAudioCodec
//-------------------------------------------------------------------
//
//	Initialize audio decoder
//

bool TRIFFReader::InitAudioCodec()
{
	if (!m_HasAudio)
		return false;

	//	Calculate audio frame size
	m_AudioFrameSize = m_AudioBPS * m_AudioChannels;

	//	Calculate number of audio frames
	m_AudioFrameCount = m_AUDSHeader.ByteCount / m_AudioFrameSize;

	return true;
}

#pragma mark -
#pragma mark === Header Queries ===

//-------------------------------------------------------------------
//	VideoDepth
//-------------------------------------------------------------------
//
//

uint16 TRIFFReader::VideoDepth()
{
	if (m_HasVideo) {
		return m_VIDSHeader.BitCount;
	}
	//	No video stream
	else{
		return 0;
	}
}


//-------------------------------------------------------------------
//	Width
//-------------------------------------------------------------------
//
//

uint32 TRIFFReader::Width()
{
	if (m_HasVideo) {
		return m_VIDSHeader.Width;
	}
	//	No video stream
	else{
		return 0;
	}
}


//-------------------------------------------------------------------
//	Height
//-------------------------------------------------------------------
//
//

uint32 TRIFFReader::Height()
{
	if (m_HasVideo) {
		return m_VIDSHeader.Height;
	}
	//	No video stream
	else{
		return 0;
	}
}


//-------------------------------------------------------------------
//	IsBottomUp
//-------------------------------------------------------------------
//
//

bool TRIFFReader::IsBottomUp()
{
	if ( m_VIDSHeader.Height > 0)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------
//	UsPerFrame
//-------------------------------------------------------------------
//
//	Milliseconds per frame
//

uint32 TRIFFReader::UsPerFrame()
{
	if (m_HasVideo) {
		return m_AVIHeader.TimeBetweenFrames;
	}
	//	No video stream
	else{
		return 0;
	}
}
