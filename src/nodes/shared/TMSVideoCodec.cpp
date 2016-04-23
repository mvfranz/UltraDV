//---------------------------------------------------------------------
//
//	File:	TMSVideoCodec.cpp
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

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TMSVideoCodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TMSVideoCodec::TMSVideoCodec(TRIFFReader *reader) : TVideoCodec(reader)
{

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TMSVideoCodec::~TMSVideoCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TMSVideoCodec::Init()
{
	
}


//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TMSVideoCodec::GetDescription(char *descText)
{		
	return false;
}


//-------------------------------------------------------------------
//	DecodeFrame
//-------------------------------------------------------------------
//
//	Decode frame passed in buffer
//

bool TMSVideoCodec::DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth)
{	
	//	Get file and save position
	BFile *theFile = m_Reader->GetFile();
	off_t position = theFile->Position();
	theFile->Seek(0, SEEK_SET);

	//	Allocate buffer to contain frame data
	const int32 bufSize = theFrame->Length;
	void *srcBuffer = malloc(bufSize);
	ASSERT(srcBuffer);
	
	//	Load bits from current frame		
	theFile->Seek(theFrame->Offset, SEEK_SET);
	theFile->Read(srcBuffer, bufSize);
	
	//	Restore file position
	theFile->Seek(position, SEEK_SET);
	
	//	Decode the frame			
	bool retVal = DecodeMSVideo(theFrame->ChunkID, m_Reader->Width(), m_Reader->Height(), m_Reader->VideoDepth(), srcBuffer, bufSize, dstBuffer);
		
	return retVal;
}


//-------------------------------------------------------------------
//	DecodeMSVideo
//-------------------------------------------------------------------
//
//	Decode Microsoft Video 01 'CRAM'
//

bool TMSVideoCodec::DecodeMSVideo(uint32 chunkID, uint32 width, uint32 height, uint16 depth, void *srcBuffer, int32 bufSize, void *dstBuffer)
{ 
	bool retVal = false;
	
	switch(depth)
	{					
		case 8:
			retVal = DecodeMSVideo8(width, height, srcBuffer, bufSize, dstBuffer);
			break;

		case 16:
			retVal = DecodeMSVideo16(width, height, srcBuffer, bufSize, dstBuffer);
			break;
		
		default:
			#ifdef DEBUG
				printf("Unhandled video frame bit depth.\n");
			#endif
			break;	
	}
	
	//	Uncompressed bitmap
	//case kRiff_00db_Chunk:
	//	retVal = DecodeRawVideoFrame(buffer, bufSize, dstBuffer);
	//	break;
			
	//	Compressed bitmap
	//case kRiff_00dc_Chunk:
	//	retVal = DecodeCompressedVideoFrame(buffer, bufSize, dstBuffer);
	//	break;


	return retVal;	
}
	


//-------------------------------------------------------------------
//	DecodeMSVideo8
//-------------------------------------------------------------------
//
//	Decode Microsoft Video 01 8-bit 'CRAM' and return BBitmap
//

bool TMSVideoCodec::DecodeMSVideo8(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer)
{ 
	uint32 	changed = 0;
	int32 	maxX 	= 0;
	int32	maxY 	= 0;	
	int32	minX 	= width;	
	int32	minY 	= height;	
	uint32	row_dec = width + 3;
	int32 	x 		= 0;
	int32	y 		= height - 1;
	
	uint32 	blockCount 	= ((width * height) >> 4) + 1;
	bool	exitFlag 	= false;
	
	//	Setup pointers
	uchar *bufPtr 	= (uchar *)buffer;
	uchar *bits 	= (uchar *)dstBuffer; 	
	
	while(!exitFlag)
	{
		uint32 code0 = *bits++;	
		uint32 code1 = *bits++;	
		blockCount--;
		
		if ( ((code1 == 0) && (code0 == 0) && !blockCount) || (y < 0)) 
			exitFlag = true;
		else
		{
			//	Skip
			if ((code1 >= 0x84) && (code1 <= 0x87)) 
			{ 
				uint32 skip = ((code1 - 0x84) << 8) + code0;
				blockCount -= (skip-1); 
				
				while(skip--) 
					MSBlockInclude(x, y, width);
			//	Single block encoded 
			} 
			else 
			{
				//	8 color quadrant encoding
				if (code1 >= 0x90) 
				{
					uchar cA0, cA1, cB0, cB1;
					
					uchar *i_ptr = (uchar *)(bits + y * width + x);
					cB0 = (uchar)*bufPtr++;  
					cA0 = (uchar)*bufPtr++;
					cB1 = (uchar)*bufPtr++;  
					cA1 = (uchar)*bufPtr++;						
					
					MSVideoC4( i_ptr, code0, cA0, cA1, cB0, cB1, row_dec); 
					i_ptr -=row_dec;
					
					cB0 = (uchar)*bufPtr++;  
					cA0 = (uchar)*bufPtr++;
					cB1 = (uchar)*bufPtr++;  
					cA1 = (uchar)*bufPtr++;
					MSVideoC4(i_ptr, code1, cA0, cA1, cB0, cB1, row_dec);
				} 
				//	2 color encoding
				else if (code1 < 0x80) 
				{ 
					uchar clr_A,clr_B;
					uchar *i_ptr = (uchar *)(bits + y * width + x);
					
					clr_B = (uchar)*bufPtr++;   
					clr_A = (uchar)*bufPtr++;
					
					MSVideoC2( i_ptr, code0, clr_A, clr_B, row_dec); 
					i_ptr -= row_dec;
					MSVideoC2(i_ptr, code1, clr_A, clr_B, row_dec);
				}
				//	1 color encoding
				else
				{
					uchar clr 	 = (uchar)code0;
					uchar *i_ptr = (uchar *)(bits + y * width + x);
					MSVideoC1(i_ptr,clr,row_dec);
				}
	
				MSMinMaxCheck(x, y, minX, maxX, minY, maxY);
				changed = 1; 
				MSBlockInclude(x, y, width);
			}
		}
	}
			
	return true;
}



//-------------------------------------------------------------------
//	DecodeMSVideo16
//-------------------------------------------------------------------
//
//	Decode Microsoft Video 01 16-bit 'CRAM' and return BBitmap
//

bool TMSVideoCodec::DecodeMSVideo16(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer)
{ 
	int32 bufferSize = bufSize;
	
	width 	= 4 * ((width  + 3) /4);
	height 	= 4 * ((height + 3) /4);

	uint32 code0, code1;
		
	int32 	maxX 	= 0;
	int32 	maxY 	= 0;	
	int32 	minX 	= width;	
	int32 	minY 	= height;
	int32	x 		= 0;
	int32 	y 		= height - 1;
	
	uint32 	rowDecrement = width + 3; 	
	
	bool changed  = false;
	bool exitFlag = false;
	
	uint32 blockCount = ((width * height) >> 4) + 1;
	
	//	Setup pointers
	uchar *srcPtr 	= (uchar *)buffer;
	uchar *dstPtr 	= (uchar *)dstBuffer; 	
		
	while(exitFlag == false)
	{
		code0 =  *srcPtr++;	
		code1 =  *srcPtr++;	
		blockCount--;
		
		if ( (code1==0) && (code0==0) && !blockCount) 
		{ 
			exitFlag = true; 
			continue; 
		}
		
		if (y < 0) 
		{
			exitFlag = true; 
			continue; 
		}
		
		//	Skip
		if ((code1 >= 0x84) && (code1 <= 0x87))
		{ 
			uint32 skip = ((code1 - 0x84) << 8) + code0;
			blockCount -= (skip-1); 
			
			while(skip--) 
				MSBlockInclude(x, y, width);
		}
		//	Don't skip
		else
		{ 
			uint32 *rowPtr = (uint32 *)(dstPtr + ((y * width + x) << 2) );
			
			//	2 or 8 color encoding
			if (code1 < 0x80)
			{ 
				uint32 cA,cB,cA0,cB0;
				
				MSGet16(cB, srcPtr); 
				MSGet16(cA, srcPtr);
				cB0 = AVIGetColor(cB);
				cA0 = AVIGetColor(cA);
				
				//	Eight color encoding
				if (cB & 0x8000)
				{ 
					uint32 cA1,cB1;
					MSGet16(cB, srcPtr); 
					MSGet16(cA, srcPtr);
					cB1 = AVIGetColor(cB);
					cA1 = AVIGetColor(cA);
					
					MSVideoC4(rowPtr, code0, cA0, cA1, cB0, cB1, rowDecrement); 
					rowPtr -= rowDecrement;
					
					MSGet16(cB, srcPtr); 
					MSGet16(cA, srcPtr);
					cB0 = AVIGetColor(cB);
					cA0 = AVIGetColor(cA);
					
					MSGet16(cB, srcPtr); 
					MSGet16(cA, srcPtr);
					cB1 = AVIGetColor(cB);
					cA1 = AVIGetColor(cA);
					MSVideoC4(rowPtr, code1, cA0, cA1, cB0, cB1, rowDecrement);
				} 
				//	Two color encoding
				else
				{
					MSVideoC2(rowPtr,code0,cA0,cB0, rowDecrement); 
					rowPtr -= rowDecrement;
					MSVideoC2(rowPtr, code1, cA0, cB0, rowDecrement);
				}
			}
			//	One color encoding
			else
			{ 
				uint32 cA 	 = (code1 << 8) | code0;
				uint32 color = AVIGetColor(cA);
				MSVideoC1(rowPtr, color, rowDecrement);
			}
			
			changed = true; 
			MSMinMaxCheck(x, y, minX, maxX, minY, maxY);
			MSBlockInclude(x, y, width);
		}
	}

	return true;
}


