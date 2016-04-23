//---------------------------------------------------------------------
//
//	File:	RGBCodec.cpp
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

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TRGBCodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TRGBCodec::TRGBCodec(TRIFFReader *reader) : TVideoCodec(reader)
{
	#ifdef DEBUG
		printf("New TRGBCodec\n");
	#endif
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TRGBCodec::~TRGBCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TRGBCodec::Init()
{
	
}


//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TRGBCodec::GetDescription(char *descText)
{		
	sprintf(descText, "Decode uncompressed RGB data.");
	
	return false;
}


//-------------------------------------------------------------------
//	DecodeFrame
//-------------------------------------------------------------------
//
//	Decode frame passed in buffer
//

bool TRGBCodec::DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth)
{	
	//	Get file and save position
	off_t position = m_Reader->GetFile()->Position();
	m_Reader->GetFile()->Seek(0, SEEK_SET);

	//	Allocate buffer to contain frame data
	const int32 bufSize = theFrame->Length;
	void *srcBuffer = malloc(bufSize);
	ASSERT(srcBuffer);
	
	//	Load bits from current frame		
	m_Reader->GetFile()->Seek(theFrame->Offset, SEEK_SET);
	m_Reader->GetFile()->Read(srcBuffer, bufSize);
	
	//	Restore file position
	m_Reader->GetFile()->Seek(position, SEEK_SET);
	
	//	Decode the frame		
	bool retVal = DecodeRGBVideoFrame(theFrame->ChunkID, m_Reader->Width(), m_Reader->Height(), m_Reader->VideoDepth(), srcBuffer, bufSize, dstBuffer, dstDepth);
	
	//	Clean up
	free(srcBuffer);
	
	return retVal;
}


//-------------------------------------------------------------------
//	DecodeRGBVideoFrame
//-------------------------------------------------------------------
//
//	Determine RGB routine to decompress data
//

bool TRGBCodec::DecodeRGBVideoFrame(uint32 chunkID, uint32 width, uint32 height, uint16 depth, void *srcBuffer, int32 bufSize, void *dstBuffer, uint16 dstDepth)
{ 
	bool retVal = false;
	
	switch(depth)
	{					
		case 4:
			retVal = DecodeRGB4(width, height, srcBuffer, bufSize, dstBuffer, dstDepth);
			break;
		
		case 8:
			retVal = DecodeRGB8(width, height, srcBuffer, bufSize, dstBuffer, dstDepth);
			break;

		case 16:
			retVal = DecodeRGB16(width, height, srcBuffer, bufSize, dstBuffer, dstDepth);
			break;
		
		case 24:
		case 32:
			retVal = DecodeRGB32(width, height, srcBuffer, bufSize, dstBuffer, dstDepth);
			break;
		
		default:
			#ifdef DEBUG
				printf("Unhandled video frame bit depth.\n");
			#endif
			break;	
	}

	return retVal;	
}
	

//-------------------------------------------------------------------
//	DecodeRGB4
//-------------------------------------------------------------------
//
//	Return a 4-bit BBitmap containing decompressed data
//

bool TRGBCodec::DecodeRGB4(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer, uint16 dstDepth)
{ 
	uint32 oddCount = ((width & 0x03) == 1) ? (1) : (0);
	
	int32 x, y = height - 1;
	
	//	Setup pointers
	uchar *srcPtr = (uchar *)buffer;
	uchar *dstPtr = (uchar *)dstBuffer; 	

	//	Traverse rows
	while ( y >= 0 )
	{ 
		uchar *rowPtr = (uchar *)(dstPtr + y * width); 
		y--; 
		
		x = width; 
		
		//	Traverse columns
		while(x) 
		{ 
			uchar d = *srcPtr++; 
			x -= 2;
			
			*rowPtr++ = (uchar)(d >> 4); 
			*rowPtr++ = (uchar)(d & 0xf); 
		}
	
		x = oddCount; 
		
		while(x--) 
			srcPtr++; 
	}
	
	return true;
}

//-------------------------------------------------------------------
//	DecodeRGB8
//-------------------------------------------------------------------
//
//	Return an 8-bit BBitmap containing decompressed data
//

bool TRGBCodec::DecodeRGB8(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer, uint16 dstDepth)
{ 
	//	Setup pointers
	uchar *srcPtr = (uchar *)buffer;
	uchar *dstPtr = (uchar *)dstBuffer; 	

	uint32 oddCount = 4 - (width & 0x03);  
	
	if (oddCount == 4) 
		oddCount = 0;
		
	int32 x, y = height - 1;
	
	while ( y >= 0 )
	{ 
		uchar *rowPtr = (uchar *)(dstPtr + y * width); 
		y--; 
		
		x = width; 
		while(x--) 
			*rowPtr++ = (uchar)*srcPtr++;
			
		x = oddCount; 
		
		while(x--) 
			srcPtr++; 
	}
	
	return true;
}

//-------------------------------------------------------------------
//	DecodeRGB16
//-------------------------------------------------------------------
//
//	Return a 16-bit BBitmap containing decompressed data
//

bool TRGBCodec::DecodeRGB16(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer, uint16 dstDepth)
{ 
	//#ifdef DEBUG
	//	printf("DecodeRGB16\n");
	//#endif

	//	Handle depth conversion.  IF source and depth are same, just do memcpy, 
	//	otherwise we must convert data.
	if (dstDepth == 16)
		memcpy(dstBuffer, buffer, (width * height) * sizeof(uint16));
	else
	{
		int32 x, y = height - 1;
		
		//	Setup pointers
		uchar *srcPtr = (uchar *)buffer;
		uchar *dstPtr = (uchar *)dstBuffer; 	
	
		//	Traverse height
		while ( y >= 0 )
		{ 
			//	Get pointer to row data
			uint32 *rowPtr = (uint32 *)(dstPtr + ((y * width) << 2) ); 
			y--; 
			x = width; 
			
			//	Traverse width
			while(x--) 
			{ 
				uint32 color = *srcPtr++; 
				color |= (*srcPtr++) << 8;
				
				//	Convert to proper depth
				switch( dstDepth)
				{
					default:
						return false;
				}
			}
		}
	}
	
	return true;
}


//-------------------------------------------------------------------
//	DecodeRGB32
//-------------------------------------------------------------------
//
//	Return a 32-bit BBitmap containing decompressed data
//

bool TRGBCodec::DecodeRGB32(uint32 width, uint32 height, void *srcBuffer, int32 bufSize, void *dstBuffer, uint16 dstDepth)
{
	//#ifdef DEBUG
	//	printf("DecodeRGB32\n");
	//#endif

	int32 x, y = height - 1;
	
	//	Setup pointers
	uchar  *srcPtr = (uchar *)srcBuffer;
	uint32 *dstPtr = (uint32 *)dstBuffer; 	

	//	Traverse height
	for (uint32 rows = 0; rows < height; rows++) 
	{ 						
		//	Traverse width
		for (uint32 columns = 0; columns < width; columns++) 
		{ 
			//	Get 24-bit rgb values
			uint32 red 	 = *srcPtr++;
			uint32 green = *srcPtr++;
			uint32 blue  = *srcPtr++;
			uint32 alpha = 255;
						
			//	Construct uint32 RGB value			
			#if B_HOST_IS_LENDIAN
				uint32 color = ( ((alpha << 24) & kAlphaMask) | ((red << 16) & kRedMask) | ((green << 8) & kGreenMask) | (blue & kBlueMask) ); 					
			#else
				uint32 color = ( ((blue << 24) & kBlueMask) | ((green << 16) & kGreenMask) | ((red << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
			#endif
						
			*dstPtr++ = color;
		}		
	}
	   
	return true;			
}
