//---------------------------------------------------------------------
//
//	File:	TRGBCodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.18.98
//
//	Desc:	RGB Video CODEC Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TRGBCODEC_H__
#define __TRGBCODEC_H__

//	RGB Video decoder
class TRGBCodec : public TVideoCodec
{
	public:
		TRGBCodec(TRIFFReader *reader);
		~TRGBCodec();
		
		bool 	GetDescription(char *descText);
		
		bool	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth);
		
	private:
		void Init();
		
		bool DecodeRGBVideoFrame(uint32 chunkID, uint32 width, uint32 height, uint16 depth, void *buffer, int32 bufSize, void *dstBuffer, uint16 dstDepth);
		bool DecodeRGB4(uint32 width, uint32 height, void *srcBuffer,  int32 bufSize, void *dstBuffer, uint16 dstDepth);
		bool DecodeRGB8(uint32 width, uint32 height, void *srcBuffer,  int32 bufSize, void *dstBuffer, uint16 dstDepth);
		bool DecodeRGB16(uint32 width, uint32 height, void *srcBuffer, int32 bufSize, void *dstBuffer, uint16 dstDepth);
		bool DecodeRGB32(uint32 width, uint32 height, void *srcBuffer, int32 bufSize, void *dstBuffer, uint16 dstDepth);		

	protected:

};


#endif