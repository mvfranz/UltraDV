//---------------------------------------------------------------------
//
//	File:	TPCMCodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.23.98
//
//	Desc:	PCM Audio compression/decompression rotuines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include "BuildApp.h"

#include "TRIFFReader.h"

#include "TPCMCodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TPCMCodec::TPCMCodec( TRIFFReader* reader) : TAudioCodec(reader)
{
	// Defualt initialization
	Init();

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TPCMCodec::~TPCMCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TPCMCodec::Init()
{
	//	Get AVIAUDSHeader
	m_Header = m_Reader->GetAUDSHeader();
}


//-------------------------------------------------------------------
//	DecodeBuffer
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TPCMCodec::DecodeBuffer(void* srcBuffer, size_t bufSize, void* dstBuffer)
{
	bool retVal = false;

	switch(m_Header->Size)
	{
	case 8:
		retVal = DecompressPcm8Signed(srcBuffer, bufSize, dstBuffer);
		break;

	case 16:
		retVal = DecompressPcm16MsbSigned(srcBuffer, bufSize, dstBuffer);
		break;

	//	Unhandled format
	default:
		break;
	}

	return retVal;
}

//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TPCMCodec::GetDescription(char* descText)
{
	return false;
}


//---------------------------------------------------------------------
//	GetSamples
//---------------------------------------------------------------------
//
//

bool TPCMCodec::DecompressPcm8Signed(void* srcBuffer, size_t length, void* dstBuffer)
{
	uchar* byteBuff = reinterpret_cast<uchar*>(srcBuffer);
	uint16* shortBuf = reinterpret_cast<uint16*>(dstBuffer);

	for( int32 i = length - 1; i >= 0; i--)
		shortBuf[i] = static_cast<uint16>(byteBuff[i]) << ( (sizeof(uint16)-1) * 8);

	return true;
}


//---------------------------------------------------------------------
//	DecompressPcm8Unsigned
//---------------------------------------------------------------------
//
//

bool TPCMCodec::DecompressPcm8Unsigned(void* srcBuffer, size_t length, void* dstBuffer)
{
	uchar* byteBuff = reinterpret_cast<uchar*>(srcBuffer);
	uint16* shortBuf = reinterpret_cast<uint16*>(dstBuffer);

	for( long i = length - 1; i >= 0; i--)
		shortBuf[i] = static_cast<uint16>(byteBuff[i] ^ 0x80) << ((sizeof(uint16)-1)*8);

	return true;
}


//---------------------------------------------------------------------
//	GetSamples
//---------------------------------------------------------------------
//
//

bool TPCMCodec::DecompressPcm16MsbSigned(void* srcBuffer, size_t length, void* dstBuffer)
{
	uchar* byteBuff =  reinterpret_cast<uchar*>(srcBuffer);
	uint16* shortBuf = reinterpret_cast<uint16*>(dstBuffer);
	//size_t read = ReadBytes(byteBuff, length * 2) / 2;

	size_t samplesRead = length;

	for( long i = samplesRead - 1; i >= 0; i-- ) {
		int16 s = static_cast<uint16>(byteBuff[2*i]) << 8;
		s |= static_cast<uint16>(byteBuff[2*i+1]) & 255;
		shortBuf[i] = static_cast<uint16>(s) << ((sizeof(uint16)-2)*8);
	}

	return true;
}


//---------------------------------------------------------------------
//	GetSamples
//---------------------------------------------------------------------
//
//

bool TPCMCodec::DecompressPcm16LsbSigned(void* srcBuffer, size_t length, void* dstBuffer)
{
	uchar* byteBuff = reinterpret_cast<uchar*>(srcBuffer);
	uint16* shortBuf = reinterpret_cast<uint16*>(dstBuffer);

	//size_t read = ReadBytes(byteBuff,length*2)/2;
	size_t read = length;

	for(long i = read-1; i>=0; i--) {
		int16 s = static_cast<uint16>(byteBuff[2*i+1]) << 8;
		s |= static_cast<uint16>(byteBuff[2*i]) & 255;
		shortBuf[i] = static_cast<uint16>(s) << ((sizeof(uint16)-2)*8);
	}

	return true;
}
