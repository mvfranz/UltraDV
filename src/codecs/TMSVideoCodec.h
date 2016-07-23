//---------------------------------------------------------------------
//
//	File:	TMSVideoCodec.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.14.98
//
//	Desc:	Microsoft Video CODEC Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TMSVIDEOCODEC_H__
#define __TMSVIDEOCODEC_H__

//	Microsoft Video 01 "CRAM" decoder
class TMSVideoCodec : public TVideoCodec
{
	public:
		TMSVideoCodec(TRIFFReader *reader);
		~TMSVideoCodec();
		
		bool 	GetDescription(char *descText);
		
		bool	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth);
		
	private:
		void Init();
		
		bool	DecodeMSVideo(uint32 chunkID, uint32 width, uint32 height, uint16 depth, void *srcBuffer, int32 bufSize, void *dstBuffer);
		bool	DecodeMSVideo8(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer);
		bool	DecodeMSVideo16(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer);
		
	protected:

};


//	Microsoft Macros
#define MSBlockInclude(x, y, width) { x += 4; if ( x >= width) { x = 0; y -= 4; } }

#define MSVideoC1(ip,clr,rdec) { \
	*ip++ = clr; *ip++ = clr; *ip++ = clr; *ip = clr; ip -= rdec; \
	*ip++ = clr; *ip++ = clr; *ip++ = clr; *ip = clr; ip -= rdec; \
	*ip++ = clr; *ip++ = clr; *ip++ = clr; *ip = clr; ip -= rdec; \
	*ip++ = clr; *ip++ = clr; *ip++ = clr; *ip = clr; }

#define MSVideoC2(ip,flag,cA,cB,rdec) { \
	 *ip++ =(flag&0x01)?(cB):(cA); *ip++ =(flag&0x02)?(cB):(cA); \
	 *ip++ =(flag&0x04)?(cB):(cA); *ip   =(flag&0x08)?(cB):(cA); ip-=rdec; \
	 *ip++ =(flag&0x10)?(cB):(cA); *ip++ =(flag&0x20)?(cB):(cA); \
	 *ip++ =(flag&0x40)?(cB):(cA); *ip   =(flag&0x80)?(cB):(cA); }

#define MSVideoC4(ip,flag,cA0,cA1,cB0,cB1,rdec) { \
	 *ip++ =(flag&0x01)?(cB0):(cA0); *ip++ =(flag&0x02)?(cB0):(cA0); \
	 *ip++ =(flag&0x04)?(cB1):(cA1); *ip   =(flag&0x08)?(cB1):(cA1); ip-=rdec; \
	 *ip++ =(flag&0x10)?(cB0):(cA0); *ip++ =(flag&0x20)?(cB0):(cA0); \
	 *ip++ =(flag&0x40)?(cB1):(cA1); *ip   =(flag&0x80)?(cB1):(cA1); }

#define MSMinMaxCheck(x,y,min_x,max_x,min_y,max_y) { \
	if (x < min_x) min_x = x; if (y > max_y) max_y = y; \
	if (x > max_x) max_x = x; if (y < min_y) min_y = y; } 

#define MSGet16(data, dptr) { data = *dptr++; data |= (*dptr++) << 8; }

#endif