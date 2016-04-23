//---------------------------------------------------------------------
//
//	File:	TAudioCodec.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.23.98
//
//	Desc:	Audop CODEC Object
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TPCMCODEC_H__
#define __TPCMCODEC_H__

#include "TAudioCodec.h"

//	Class Definition
class TPCMCodec: public TAudioCodec
{
	public:
		TPCMCodec(TRIFFReader *reader);
		~TPCMCodec();
		
		bool DecodeBuffer(void *srcBuffer, size_t bufSize, void *dstBuffer);
		bool GetDescription(char *descStr);
		
	private:
		void Init();
		
		bool	DecompressPcm8Signed(void *srcBuffer, size_t length, void *dstBuffer);
		bool	DecompressPcm8Unsigned(void *srcBuffer, size_t length, void *dstBuffer);
		bool	DecompressPcm16MsbSigned(void *srcBuffer, size_t length, void *dstBuffer);
		bool	DecompressPcm16LsbSigned(void *srcBuffer, size_t length, void *dstBuffer);
						
	protected:
		AVIAUDSHeader 	*m_Header;
};

#endif