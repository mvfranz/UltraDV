//---------------------------------------------------------------------
//
//	File:	TVideoCodec.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.14.98
//
//	Desc:	Video CODEC Object
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOCODEC_H__
#define __TVIDEOCODEC_H__

//	Includes
#include "RIFFConstants.h"
#include "RIFFTypes.h"

//	Forward Declarations
class TRIFFReader;

//	Class Definition
class TVideoCodec
{
	public:
		TVideoCodec(TRIFFReader *reader);
		virtual ~TVideoCodec();
		
		virtual bool 	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth) = 0;
		virtual bool	GetDescription(char *descStr) = 0;
		
	private:
		virtual void Init();
						
	protected:
		TRIFFReader *m_Reader;
};


// Utility Routines
uint32 	GetTrueColor(uint32 r, uint32 g, uint32 b, uint32 bits);
uint32	AVIGetColor(uint32 srcColor);
uint32 	ConvertRGB16toRGB32(uint16 srcColor);
uint32 ConvertRGB24toRGB32(uint32 srcColor);

// Constants
#if B_HOST_IS_LENDIAN
	#define kBlueMask 	0x000000FF
	#define kGreenMask 	0x0000FF00
	#define kRedMask	0x00FF0000
	#define kAlphaMask	0xFF000000
#else
	#define kBlueMask 	0xFF000000
	#define kGreenMask 	0x00FF0000
	#define kRedMask	0x0000FF00
	#define kAlphaMask	0x000000FF
#endif

#endif