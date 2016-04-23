//---------------------------------------------------------------------
//
//	File:	TMSRLECodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.18.98
//
//	Desc:	Microsoft RLE Codec
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TMSRLECODEC_H__
#define __TMSRLECODEC_H__

//	Microsoft RLE Codec
class TMSRLECodec : public TVideoCodec
{
	public:
		TMSRLECodec(TRIFFReader *reader);
		~TMSRLECodec();
		
		bool 	GetDescription(char *descText);
		
		bool	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth);
		
	private:
		void Init();
		
		bool	DecodeMicrosoftRLE(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer);
		

	protected:

};

#endif