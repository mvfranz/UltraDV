//---------------------------------------------------------------------
//
//	File:	AVIUtils.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	Application Utilities
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include "BuildApp.h"

#include <string.h>

#include "AppUtils.h"
#include "TCompositor.h"
#include "RIFFConstants.h"
#include "RIFFTypes.h"

#include "AVIUtils.h"


#pragma mark -
#pragma mark === Byte Order Utiities ===

//-------------------------------------------------------------------
//	ReadIntMsb
//-------------------------------------------------------------------
//
//

long ReadIntMsb(BFile* in, int size)
{
	long value;

	if (size <= 0)
		return 0;

	//	Clear out bits
	switch(size)
	{
	case 1:
	{
		char charBuf;
		in->Read(&charBuf, size);
		value = charBuf;
	}
	break;

	case 2:
	{
		int16 shortBuf;
		in->Read(&shortBuf, size);
		swap_data(B_INT16_TYPE, &shortBuf, sizeof(int16), B_SWAP_HOST_TO_BENDIAN);
		value = shortBuf;
	}
	break;

	case 3:
	{
		int32 intBuf;
		in->Read(&intBuf, size);
		//	Clear out last byte
		intBuf &= 0xFFFFFF00;
		swap_data(B_INT32_TYPE, &intBuf, sizeof(int32), B_SWAP_HOST_TO_BENDIAN);
		value = intBuf;
	}
	break;

	default:
	{
		in->Read(&value, size);
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_BENDIAN);
	}
	break;
	}

	return value;
}

//-------------------------------------------------------------------
//	BytesToIntMsb
//-------------------------------------------------------------------
//
//

long BytesToIntMsb(void* vBuff, int size)
{
	unsigned char* buff = reinterpret_cast<unsigned char*>(vBuff);

	if (size <= 0)
		return 0;

	long l = BytesToIntMsb(buff,size-1) << 8;

	l |= static_cast<long>(buff[size-1]) & 255;

	return l;
}


//-------------------------------------------------------------------
//	ReadIntLsb
//-------------------------------------------------------------------
//
//

long ReadIntLsb(BFile* in, int size)
{
	long value;

	if (size <= 0)
		return 0;

	//	Clear out bits
	switch(size)
	{
	case 1:
	{
		char charBuf;
		in->Read(&charBuf, size);
		value = charBuf;
	}
	break;

	case 2:
	{
		int16 shortBuf;
		in->Read(&shortBuf, size);
		swap_data(B_INT16_TYPE, &shortBuf, sizeof(int16), B_SWAP_HOST_TO_LENDIAN);
		value = shortBuf;
	}
	break;

	case 3:
	{
		int32 intBuf;
		in->Read(&intBuf, size);
		//	Clear out last byte
		intBuf &= 0xFFFFFF00;
		swap_data(B_INT32_TYPE, &intBuf, sizeof(int32), B_SWAP_HOST_TO_LENDIAN);
		value = intBuf;
	}
	break;

	default:
	{
		in->Read(&value, size);
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_LENDIAN);
	}
	break;
	}

	return value;
}

//-------------------------------------------------------------------
//	BytesToIntMsb
//-------------------------------------------------------------------
//
//

long BytesToIntLsb(void* vBuff, int size)
{
	unsigned char* buff = reinterpret_cast<unsigned char*>(vBuff);
	if (size <= 0) return 0;
	long l = static_cast<long>(*buff) & 255;
	l |= BytesToIntLsb(buff+1,size-1)<<8;
	return l;
}


//-------------------------------------------------------------------
//	SkipBytes
//-------------------------------------------------------------------
//
//

void SkipBytes(BFile* in, int size)
{
	char getChar;

	while (size-- > 0)
		in->Read(&getChar, sizeof(char));
}


//-------------------------------------------------------------------
//	WriteIntMsb
//-------------------------------------------------------------------
//
//

void WriteIntMsb(BFile* out, long l, int size)
{
	if (size <= 0)
		return;

	// Write MS Bytes
	WriteIntMsb(out, l>>8, size-1);

	// Write LS Byte
	char putChar = l & 255;
	out->Write( &putChar, sizeof(char));
}

//-------------------------------------------------------------------
//	WriteAtIntMsb
//-------------------------------------------------------------------
//
//

ssize_t WriteAtIntMsb(BFile* out, int32 value, int size)
{
	if (size <= 0)
		return 0;

	ssize_t bytesWritten = 0;

	switch(size)
	{
	//	char
	case 1:
	{
		char charBuf = value;
		bytesWritten = out->WriteAt(out->Position(), &charBuf, size);
	}
	break;

	case 2:
	{
		swap_data(B_INT16_TYPE, &value, sizeof(int16), B_SWAP_HOST_TO_BENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);
	}
	break;

	case 3:
	{
		//	Clear out last byte
		value &= 0xFFFFFF00;
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_BENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);

	}
	break;

	default:
	{
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_BENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);
	}
	break;
	}

	return bytesWritten;
}


//-------------------------------------------------------------------
//	WriteIntLsb
//-------------------------------------------------------------------
//
//

void WriteIntLsb(BFile* out, long l, int size)
{
	if (size <= 0)
		return;

	// Write LS Byte
	char putChar = l & 255;
	out->Write( &putChar, sizeof(char));

	// Write rest
	WriteIntLsb(out, l>>8, size-1);
}

//-------------------------------------------------------------------
//	WriteAtIntLsb
//-------------------------------------------------------------------
//
//

ssize_t WriteAtIntLsb(BFile* out, int32 value, int size)
{
	if (size <= 0)
		return 0;

	ssize_t bytesWritten = 0;

	switch(size)
	{
	//	char
	case 1:
	{
		char charBuf = value;
		bytesWritten = out->WriteAt(out->Position(), &charBuf, size);
	}
	break;

	case 2:
	{
		swap_data(B_INT16_TYPE, &value, sizeof(int16), B_SWAP_HOST_TO_LENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);
	}
	break;

	case 3:
	{
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_LENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);
	}
	break;

	default:
	{
		swap_data(B_INT32_TYPE, &value, sizeof(int32), B_SWAP_HOST_TO_LENDIAN);
		bytesWritten = out->WriteAt(out->Position(), &value, size);
	}
	break;
	}

	return bytesWritten;
}

//---------------------------------------------------------------------
//	DumpAVIHeader
//---------------------------------------------------------------------
//
//

void DumpAVIHeader(AVIHeader* theHeader)
{
	#ifdef DEBUG
	printf("=== Start AVIHeader ===\n");
	printf("TimeBetweenFrames: %d\n", theHeader->TimeBetweenFrames);
	printf("MaximumDataRate: %d\n", theHeader->MaximumDataRate);
	printf("PaddingGranularity: %d\n", theHeader->PaddingGranularity);
	printf("Flags: %d\n", theHeader->Flags);
	printf("TotalNumberOfFrames: %d\n", theHeader->TotalNumberOfFrames);
	printf("NumberOfInitialFrames: %d\n", theHeader->NumberOfInitialFrames);
	printf("NumberOfStreams: %d\n", theHeader->NumberOfStreams);
	printf("SuggestedBufferSize: %d\n", theHeader->SuggestedBufferSize);
	printf("Width: %d\n", theHeader->Width);
	printf("Height: %d\n", theHeader->Height);
	printf("TimeScale: %d\n", theHeader->TimeScale);
	printf("DataRate: %d\n", theHeader->DataRate);
	printf("StartTime: %d\n", theHeader->StartTime);
	printf("DataLength: %d\n", theHeader->DataLength);
	printf("=== End AVIHeader ===\n\n\n");
	#endif
}

//-------------------------------------------------------------------
//	DumpAVIStreamHeader
//-------------------------------------------------------------------
//
//

void DumpAVIStreamHeader(AVIStreamHeader* theHeader)
{
	#ifdef DEBUG
	printf("=== Start AVIStreamHeader ===\n");
	//printf("DataType: %d\n", theHeader->DataType);
	printf("DataType: ");
	DumpRIFFID(theHeader->DataType);
	//printf("DataHandler: %d\n", theHeader->DataHandler);
	printf("DataHandler: ");
	DumpRIFFID(theHeader->DataHandler);
	printf("Flags: %d\n", theHeader->Flags);
	printf("Priority: %d\n", theHeader->Priority);
	printf("InitialFrames: %d\n", theHeader->InitialFrames);
	printf("TimeScale: %d\n", theHeader->TimeScale);
	printf("DataRate: %d\n", theHeader->DataRate);
	printf("StartTime: %d\n", theHeader->StartTime);
	printf("DataLength: %d\n", theHeader->DataLength);
	printf("SuggestedBufferSize: %d\n", theHeader->SuggestedBufferSize);
	printf("Quality: %d\n", theHeader->Quality);
	printf("SampleSize: %d\n", theHeader->SampleSize);
	printf("=== End AVIStreamHeader ===\n\n\n");
	#endif
}


//-------------------------------------------------------------------
//	DumpVIDSHeader
//-------------------------------------------------------------------
//
//

void DumpVIDSHeader(AVIVIDSHeader* theHeader)
{
	#ifdef DEBUG
	printf("=== Start AVIVIDSHeader ===\n");
	printf("Size: %d\n", theHeader->Size);
	printf("Width: %d\n", theHeader->Width);
	printf("Height: %d\n", theHeader->Height);
	printf("Planes: %d\n", theHeader->Planes);
	printf("BitCount: %d\n", theHeader->BitCount);
	//printf("Compression: %d\n", theHeader->Compression);
	printf("Compression: ");
	DumpRIFFID(theHeader->Compression);
	printf("ImageSize: %d\n", theHeader->ImageSize);
	printf("XPelsPerMeter: %d\n", theHeader->XPelsPerMeter);
	printf("YPelsPerMeter: %d\n", theHeader->YPelsPerMeter);
	printf("Number of Colors: %d\n", theHeader->NumColors);
	printf("Important Colors: %d\n", theHeader->ImpColors);
	printf("=== End AVIVIDSHeader ===\n\n\n");
	#endif
}

//-------------------------------------------------------------------
//	DumpAUDSHeader
//-------------------------------------------------------------------
//
//

void DumpAUDSHeader(AVIAUDSHeader* theHeader)
{
	#ifdef DEBUG
	printf("=== Start AVIAUDSHeader ===\n");
	printf("Format: %d\n", theHeader->Format);
	printf("Channels: %d\n", theHeader->Channels);
	printf("Rate: %d\n", theHeader->Rate);
	printf("AverageDataRate: %d\n", theHeader->AverageDataRate);
	printf("BlockAlign: %d\n", theHeader->BlockAlign);
	printf("Size: %d\n", theHeader->Size);
	printf("ExtensionSize: %d\n", theHeader->ExtensionSize);
	printf("SamplesPerBlock: %d\n", theHeader->SamplesPerBlock);
	printf("NumCoefficients: %d\n", theHeader->NumCoefficients);
	//WaveCoefficient *Coefficients
	printf("Style: %d\n", theHeader->Style);
	printf("ByteCount: %d\n", theHeader->ByteCount);
	printf("=== End AVIAUDSHeader ===\n\n\n");
	#endif
}


//-------------------------------------------------------------------
//	DumpRIFFID
//-------------------------------------------------------------------
//
//	Print RIFF ID ie: 'RIFF'
//

void DumpRIFFID(int32 theID)
{
	printf("DumpRIFFID: %4X\n",theID);
#ifdef DEBUG
	printf("%c",     (char)((theID >> 24) & 0xff)   );
	printf("%c",     (char)((theID >> 16) & 0xff)   );
	printf("%c",     (char)((theID >>  8) & 0xff)   );
	printf("%c(%x)", (char) (theID        & 0xff), theID);
	printf(".\n");
#endif
}

//-------------------------------------------------------------------
//	DumpAVIHeaderFlags
//-------------------------------------------------------------------
//
//	Dump bit flag setting in AVIHeader
//

void DumpAVIHeaderFlags(AVIHeader* theHeader)
{
	#ifdef DEBUG
	printf(" AVI flags: ");
	if (theHeader->Flags & kAVIHasIndexFlag) printf("Has_Index ");
	if (theHeader->Flags & kAVIMustUseIndexFlag) printf("Use_Index ");
	if (theHeader->Flags & kAVIIsInterleavedFlag) printf("Interleaved ");
	if (theHeader->Flags & kAVIWasCaptureFileFlag) printf("Captured ");
	if (theHeader->Flags & kAVICopyrightedFlag) printf("Copyrighted ");
	printf("\n");
	#endif
}

//-------------------------------------------------------------------
//	DumpAudioType
//-------------------------------------------------------------------
//
//	Dump bit flag setting in AVIHeader
//

void DumpAudioType(uint16 type)
{
	#ifdef DEBUG
	switch(type)
	{
	case WAVE_FORMAT_PCM: printf("PCM"); break;
	case WAVE_FORMAT_ADPCM: printf("MS ADPCM"); break;
	case WAVE_FORMAT_DVI_ADPCM: printf("DVI ADPCM"); break;
	case WAVE_FORMAT_ALAW: printf("ALAW"); break;
	case WAVE_FORMAT_MULAW: printf("ULAW"); break;
	case WAVE_FORMAT_OKI_ADPCM: printf("OKI_ADPCM"); break;
	case IBM_FORMAT_MULAW: printf("IBM_ULAW"); break;
	case IBM_FORMAT_ALAW: printf("IBM_ALAW"); break;
	case IBM_FORMAT_ADPCM: printf("IBM_ADPCM"); break;
	case WAVE_FORMAT_GSM610: printf("GSM 6.10"); break;
	case WAVE_FORMAT_DSP_TRUESPEECH: printf("DSP TrueSpeech"); break;
	default: printf("Unknown(%x)",type); break;
	}
	#endif
}
