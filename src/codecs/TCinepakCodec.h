//---------------------------------------------------------------------
//
//	File:	TCinepakCodec.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.14.98
//
//	Desc:	Cinepak CODEC Routines "Radius Compact Video"
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TCINEPAKCODEC_H__
#define __TCINEPAKCODEC_H__

//	Structs
typedef struct
{
	uint32 	color;
} CinepakColor;


//	Constants
const int32 kCinepakMaxStrips = 16;

//	RGB Video decoder
class TCinepakCodec : public TVideoCodec
{
	public:
		TCinepakCodec(TRIFFReader *reader);
		~TCinepakCodec();
		
		bool 	GetDescription(char *descText);
		
		bool	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth);
		
	private:
		void Init();
		
		bool 	DecodeCinepakFrame(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer);
		void 	CreateYUVTables();
		void 	SetupJPEGSampleLimitTable();
		
		void 	DecodeC1(uchar *image, uint32 x, uint32 y, uint32 imagex, int32 d, CinepakColor *colorMap);
		void 	DecodeC4(uchar *image, uint32 x, uint32 y, uint32 imagex, int32 d0, int32 d1, int32 d2, int32 d3, CinepakColor *colorMap);

		
	protected:		
		uchar  *m_JPEGSampleLimit;
		
		CinepakColor *m_CinepakMaps0[kCinepakMaxStrips];
		CinepakColor *m_CinepakMaps1[kCinepakMaxStrips];
		int32  		 m_NumColorMaps;
		
		//	YUV cache tables
		int32 *m_UBTable;
		int32 *m_VRTable;
		int32 *m_UGTable;
		int32 *m_VGTable;
};


//	Prototypes
uint32 	GetQuickTimeColor24( uint32 r, uint32 g, uint32 b);

#endif