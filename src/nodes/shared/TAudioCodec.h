//---------------------------------------------------------------------
//
//	File:	TAudioCodec.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.23.98
//
//	Desc:	Audio CODEC Object
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TAUDIOCODEC_H__
#define __TAUDIOCODEC_H__

#include "TRIFFReader.h"

//	Class Definition
class TAudioCodec
{
	public:
		TAudioCodec(TRIFFReader *reader);
		virtual ~TAudioCodec();
		
		virtual bool DecodeBuffer(void *srcBuffer, size_t bufSize, void *dstBuffer) = 0;
		virtual bool GetDescription(char *descStr) = 0;
		
	private:
		virtual void Init();
						
	protected:
		TRIFFReader *m_Reader;
};

#endif
