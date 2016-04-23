//---------------------------------------------------------------------
//
//	File:	TVideoCodec.cpp
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

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"

#include "TVideoCodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TVideoCodec::TVideoCodec( TRIFFReader *reader)
{
	m_Reader = reader;
	
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TVideoCodec::~TVideoCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TVideoCodec::Init()
{
}


//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TVideoCodec::GetDescription(char *descText)
{

	return false;
}



#pragma mark -
#pragma mark === CODEC Utilities ===

//-------------------------------------------------------------------
//	GetTrueColor
//-------------------------------------------------------------------
//
//	Convert RGB values into a 32-bit BGRA uint32
//

uint32 GetTrueColor(uint32 red, uint32 green, uint32 blue, uint32 bits)
{
	uint32 newColor = 0;
	
	switch(bits)
	{
		case 16:
			{
				red	  = (red   << 3) | (red   >> 2);
				green = (green << 3) | (green >> 2);
				blue  = (blue  << 3) | (blue  >> 2);
								
				uint32 alpha = 255;
				
				//	Create new color
				#if B_HOST_IS_LENDIAN
					newColor = ( ((alpha << 24) & kAlphaMask) | ((red << 16) & kRedMask) | ((green << 8) & kGreenMask) | (blue & kBlueMask) ); 					
				#else
					newColor = ( ((blue << 24) & kBlueMask) | ((green << 16) & kGreenMask) | ((red << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
				#endif								
			}
			break;
						
		default:
			TRESPASS();
			break;
	
	}
		
	return newColor;
}


//-------------------------------------------------------------------
//	AVIGetColor
//-------------------------------------------------------------------
//
//	Convert value into a 32-bit BGRA uint32
//

uint32	AVIGetColor(uint32 srcColor)
{ 
	register uint32 alpha, newColor, ra, ga, ba, tr, tg, tb;
	
	ra = (srcColor >> 10) & 0x1f;
	ga = (srcColor >>  5) & 0x1f;
	ba =  srcColor & 0x1f;
	
	tr = (ra << 3) | (ra >> 2);
	tg = (ga << 3) | (ga >> 2);
	tb = (ba << 3) | (ba >> 2);
	
	alpha = 255;
	
	//	Create new BGRA uint32 color
	//newColor = ( (tb << 24) & kBlueMask) | ( (tg << 16) & kGreenMask) | ((tr << 8) & kRedMask) | (alpha & kAlphaMask); 

	//	Create new color
	#if B_HOST_IS_LENDIAN
		newColor = ( ((alpha << 24) & kAlphaMask) | ((tr << 16) & kRedMask) | ((tg << 8) & kGreenMask) | (tb & kBlueMask) ); 					
	#else
		newColor = ( ((tb << 24) & kBlueMask) | ((tg << 16) & kGreenMask) | ((tr << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
	#endif

	return(newColor);
}

//-------------------------------------------------------------------
//	ConvertRBG16toRGB32
//-------------------------------------------------------------------
//
//	Return a BGRA pixel
//

uint32 ConvertRGB16toRGB32(uint16 srcColor)
{ 
	uchar  red, green, blue, alpha;
	uint32 newColor;
	
	//	Get red value
	red	= (srcColor >> 10) & 0x1f; 
	red	= (red << 3) | (red >> 2);
	
	//	Get green value
	green = (srcColor >>  5) & 0x1f; 
	green = (green << 3) | (green >> 2);
	
	//	Get blue value
	blue  =  srcColor & 0x1f;        
	blue  = (blue << 3) | (blue >> 2);
	
	alpha = 255;
	
	//	Create new color
	#if B_HOST_IS_LENDIAN
		newColor = ( ((alpha << 24) & kAlphaMask) | ((red << 16) & kRedMask) | ((green << 8) & kGreenMask) | (blue & kBlueMask) ); 					
	#else
		newColor = ( ((blue << 24) & kBlueMask) | ((green << 16) & kGreenMask) | ((red << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
	#endif


	return(newColor);	
}


//-------------------------------------------------------------------
//	ConvertRBG16toRGB32
//-------------------------------------------------------------------
//
//	Return a BGRA pixel
//

uint32 ConvertRGB24toRGB32(uint32 srcColor)
{ 
	uchar  red, green, blue, alpha;
	
	//	Get red value
	red	= srcColor >> 24;
	
	//	Get green value
	green = srcColor >>  16;
	
	//	Get blue value
	blue  =  srcColor >> 8;
	
	//	Setup alpha
	alpha = 255;
	
	//	Create new color
	#if B_HOST_IS_LENDIAN
		uint32 newColor = ( ((alpha << 24) & kAlphaMask) | ((red << 16) & kRedMask) | ((green << 8) & kGreenMask) | (blue & kBlueMask) ); 					
	#else
		uint32 newColor = ( ((blue << 24) & kBlueMask) | ((green << 16) & kGreenMask) | ((red << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
	#endif
	
	return(newColor);	
}
