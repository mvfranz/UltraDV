//---------------------------------------------------------------------
//
//	File:	TCinepakCodec.cpp
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

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TCinepakCodec.h"



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TCinepakCodec::TCinepakCodec(TRIFFReader *reader) : TVideoCodec(reader)
{	
	m_UBTable = 0;
	m_VRTable = 0;
	m_UGTable = 0;
	m_VGTable = 0;
	
	m_JPEGSampleLimit = 0;
	
	//	Default Initialization
	Init();

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TCinepakCodec::~TCinepakCodec()
{
	//	Free YUV Tables
	if (m_UBTable)
		free(m_UBTable);
	
	if (m_VRTable)
		free(m_VRTable);
	
	if (m_UGTable)
		free(m_UGTable);
	
	if (m_VGTable)
		free(m_VGTable);
	
	if (m_JPEGSampleLimit)
		free(m_JPEGSampleLimit);
}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TCinepakCodec::Init()
{		
	m_JPEGSampleLimit = 0;
	
	m_NumColorMaps = 0;
	
	//	Create YUV lookup tables
	CreateYUVTables();
	
	//	Create JPEG Sample limit table
	SetupJPEGSampleLimitTable();
}

//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TCinepakCodec::GetDescription(char *descText)
{		
	return false;
}



//-------------------------------------------------------------------
//	DecodeFrame
//-------------------------------------------------------------------
//
//	Decode frame passed in buffer
//

bool TCinepakCodec::DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth)
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
	bool retVal = DecodeCinepakFrame(m_Reader->Width(), m_Reader->Height(), srcBuffer, bufSize, dstBuffer);
	
	//	Clean up
	free(srcBuffer);
	
	return retVal;
}


//-------------------------------------------------------------------
//	DecodeCinepak
//-------------------------------------------------------------------
//
//	Decode Cinepak "Compact Video" Algorithm 'cvid'
//

bool TCinepakCodec::DecodeCinepakFrame(uint32 width, uint32 height, void *srcBuffer, 
			int32 bufSize, void *dstBuffer)
{
	int32 	cr, cg, cb; 
	int32 	Y, red, green, blue;
	uint32 	j, U, V;
	uint32	mask, flag, mcode;
	uchar 	*yPtr;
	uint32 	data, index;
	
	CinepakColor *colorMap;
	
	//int32  numColorMaps = 0;
	
	//CinepakColor *cinepakMaps0[kCinepakMaxStrips];
	//CinepakColor *cinepakMaps1[kCinepakMaxStrips];
	
	uint32 strip, stripNum;
	
	uint32 y 	= 0;
	uint32 yTop = 0;
	
	//	Setup pointers
	uchar *srcPtr 	= (uchar *)srcBuffer;
	uchar *dstPtr 	= (uchar *)dstBuffer; 	
	
	// Main Codec header
	int32 	length = *srcPtr++;
	uint32 	copyFlag = (length & 0x01) ? (false) : (true);
	
	//	Get length
	length =  (*srcPtr++) << 16; 
	length |= (*srcPtr++) << 8; 
	length |= (*srcPtr++);
	
	printf("DecodeCinepakFrame: length=%d, bufSize=%d\n", length, bufSize);
	
	//	Check for corrupt frame
	if (length != bufSize){ 
		//	Pad
		if (length & 0x01) 
			length++;
			
		if (length != bufSize){
			#ifdef DEBUG
				printf("Data corruption.  Skipping frame... %x %x\n", bufSize, length);
			#endif
			return false;
		}
	}
	
	//	Get X size
	uint32 xSize;	
	xSize = (*srcPtr++) << 8;  
	xSize |= *srcPtr++;
	
	//	Get Y size
	uint32 ySize;	
	ySize = (*srcPtr++) << 8;  
	ySize |= *srcPtr++;
	
	//	Get strip
	stripNum = (*srcPtr++) << 8;  
	stripNum |= *srcPtr++;
	
	#ifdef DEBUG
		printf("Cinepak <%ld %ld> strips %ld\n", xSize, ySize, stripNum);
	#endif
	
	if (stripNum > m_NumColorMaps){ 
		if (stripNum >= kCinepakMaxStrips) {
			#ifdef DEBUG
				printf("Cinepak strip overflow\n");
			#endif
			
			return false;
		}
		
		for( index = m_NumColorMaps; index < stripNum; index++){
			colorMap = (CinepakColor *)malloc( 1040 * sizeof(CinepakColor) );
			
			if (colorMap == 0) {
				#ifdef DEBUG
					printf("Color map allocation failure\n");
				#endif
				return false;
			}			
			m_CinepakMaps0[index] = colorMap;
			
			colorMap = (CinepakColor *)malloc( 1040 * sizeof(CinepakColor) );
			
			if (colorMap == 0) {
				#ifdef DEBUG
					printf("Color map allocation failure\n");
				#endif
				return false;			
			}			
			m_CinepakMaps1[index] = colorMap;
		}
	}
	m_NumColorMaps = stripNum;
	
	
	//	Decode strips
	for( strip = 0; strip < stripNum; strip++ ){ 
		CinepakColor *colorMap0, *colorMap1;
		int32 topSize, x = 0;
	
		colorMap0 = m_CinepakMaps0[strip];
		colorMap1 = m_CinepakMaps1[strip];
		
		
		if ( strip && (copyFlag == true)){ 			
			CinepakColor *src,*dst;
			
			src = m_CinepakMaps0[strip-1]; 
			dst = m_CinepakMaps0[strip];
			
			for(index = 0; index < 1024; index++) 
				dst[index] = src[index];
				
			src = m_CinepakMaps1[strip-1]; 
			dst = m_CinepakMaps1[strip];
			
			for( index = 0; index < 1024; index++) 
				dst[index] = src[index];
		}
	
		//	Read strip header
		uint32 top_cid, x0, y0, x1, y1;
		top_cid = (*srcPtr++) << 8;   
		top_cid |= *srcPtr++;
		topSize	= (*srcPtr++) << 8;   
		topSize |= *srcPtr++;
		y0      = (*srcPtr++) << 8;   
		y0      |= *srcPtr++;
		x0      = (*srcPtr++) << 8;   
		x0      |= *srcPtr++;
		y1      = (*srcPtr++) << 8;   
		y1      |= *srcPtr++;
		x1      = (*srcPtr++) << 8;   
		x1      |= *srcPtr++;

		yTop 	+= y1;
		topSize -= 12;
		
		if (x1 != width) {
			#ifdef DEBUG
				printf("Cinepak Warning x1(%lx) != width(%lx)\n",x1,width);
			#endif
		}
	
		while(topSize > 0){ 
			uint32 	chunkID; 
			int32 	chunkSize;
			
			chunkID		=  (*srcPtr++) << 8;  
			chunkID   	|= *srcPtr++;
			chunkSize 	=  (*srcPtr++) << 8;  
			chunkSize 	|= *srcPtr++;
		
			topSize 	-= chunkSize;
			chunkSize 	-= 4;
			
			switch(chunkID)	{
				//	Color Palette
				case 0x2000: 
				case 0x2200: 
				{ 
					uint32 chunkNum;		
					uchar *rangeLimit = m_JPEGSampleLimit + 256;
					
					if (chunkID == 0x2000)	
						colorMap = colorMap0; 
					else
						colorMap = colorMap1; 
					
					chunkNum = chunkSize / 6;  
					
					for(index = 0; index < chunkNum; index++) { 
						yPtr = srcPtr;
						
						//	Skip luma
						srcPtr += 4;
						
						U = (uint32)(*srcPtr++) ^ 0x80;	
						V = (uint32)(*srcPtr++) ^ 0x80;
						cr = m_VRTable[V];		
						cb = m_UBTable[U];
						cg = (m_UGTable[U] + m_VGTable[V])>>16;
				
						for(j = index; j < 1024; j += 256){ 
							Y 	  = (int32)*yPtr++;	
							red   = rangeLimit[Y + cr]; 
							green = rangeLimit[Y + cg]; 
							blue  = rangeLimit[Y + cb];
						
							colorMap[j].color = GetQuickTimeColor24(red, green, blue);							
						}
					}
				}
				break;
				
				//	Color Palette
				case 0x2100: 
				case 0x2300: 
					{ 
						uint32 ci = 0;
						
						uchar *rangeLimit = m_JPEGSampleLimit + 256;
						
						if (chunkID == 0x2100)	
							colorMap = colorMap0; 
						else		
							colorMap = colorMap1; 
						
						while(chunkSize > 0)
						{ 
							mask  = 0x80000000;
							flag  = (*srcPtr++) << 24;  
							flag |= (*srcPtr++) << 16;
							flag |= (*srcPtr++) <<  8;  
							flag |= *srcPtr++; 
							chunkSize -= 4;
						
							while(mask)
							{ 
								if (mask & flag)
								{ 																		
									yPtr = srcPtr;									
									srcPtr += 4;	
									chunkSize -= 6;							
									U 	= (uint32)(*srcPtr++) ^ 0x80;	
									V 	= (uint32)(*srcPtr++) ^ 0x80;
									cr 	= m_VRTable[V];		
									cb 	= m_UBTable[U];
									cg 	= (m_UGTable[U] + m_VGTable[V]) >> 16;
							
									for(j = ci; j < 1024; j += 256)
									{ 										
										Y 	  = (int32)*yPtr++;	
										red   = rangeLimit[Y + cr];
										green = rangeLimit[Y + cg];	
										blue  = rangeLimit[Y + cb];
																				
										colorMap[j].color = GetQuickTimeColor24(red, green, blue);
									}
								}
														
								ci++; 
								mask >>= 1;
							}
						}
					
						#ifdef DEBUG
							if (chunkSize != 0) 
								printf("Cinepak 21 err sizz %04lx\n", chunkSize);
						#endif
					}
					break;
			
				//
				case 0x3000: 
				{ 				
					while( (chunkSize > 0) && (y < yTop) )
					{ 
						int32 j;
						
						flag  = (*srcPtr++) << 24;  
						flag |= (*srcPtr++) << 16;
						flag |= (*srcPtr++) <<  8;  
						flag |= *srcPtr++; 
						chunkSize -= 4;
				
						mask = 0x80000000; 
						j = 32;
						
						for(j = 0; j < 32; j++)
						{ 
							if (y >= yTop) 
								break;
						
							//	Update blocks 4 bytes map 0
							if (mask & flag) 
							{ 
								uint32 d0,d1,d2,d3;
								d0 = *srcPtr++; 
								d1 = *srcPtr++; 
								d2 = *srcPtr++; 
								d3 = *srcPtr++; 
								chunkSize -= 4;
								
								DecodeC4(dstPtr, x, y, width, d0, d1, d2, d3, colorMap0);								
							}
							//	1 byte map 1
							else 
							{ 
								uint32 data = *srcPtr++; 
								chunkSize--;
								
								DecodeC1(dstPtr, x, y, width, data, colorMap1);
							}
						
							x += 4; 
						
							if (x >= width) 
							{
								x = 0; 								
								y += 4;
							}
						
							mask >>= 1;
						}
					
						if (chunkSize < 4) 
						{ 
							srcPtr += chunkSize;  
							chunkSize = 0; 
						}
					} 
				
					if (chunkSize) 
						srcPtr += chunkSize;
				}
				break;
				
				
				// Every Byte is C1
				case 0x3200:
					{ 
						while( (chunkSize > 0) && (y < yTop) )
						{ 
							data = *srcPtr++; 
							chunkSize--;
							
							DecodeC1(dstPtr, x, y, width, data, colorMap1);
							#ifdef DEBUG
								printf("X: %d  Y: %d  Width: %d\n", x, y, width);
							#endif
							x += 4; 
							
							//	Check for width bounds violation
							if (x >= width)
							{
								x = 0; 							
								y += 4;
							}
						}
						
						if (chunkSize) 
							srcPtr += chunkSize;
					}
					break;
					
					
				//	0   SKIP      if(nextbit) { if(nextbit) C4 else C1 }
				//	10  C1
				//	11  C4
				case 0x3100: 
				{
					while( (chunkSize > 0) && (y < yTop) )
					{ 
						mask  = 0x80000000;
						mcode = (*srcPtr++) << 24;  
						mcode |= (*srcPtr++) << 16;
						mcode |= (*srcPtr++) <<  8;  
						mcode |= *srcPtr++; 
						chunkSize -= 4;
						mask = 0x80000000;
						
						while( (mask) && (y < yTop) )
						{ 
							if (mcode & mask)							
							{ 
								//	need more bits
								if (mask == 1) 
								{ 
									if (chunkSize < 0) 
										break;
										
									mcode  = (*srcPtr++) << 24;  
									mcode |= (*srcPtr++) << 16;
									mcode |= (*srcPtr++) <<  8;  
									mcode |= *srcPtr++; 
									chunkSize -= 4;
									mask = 0x80000000;
								} 
								else 
									mask >>= 1;
				
								//	C4
								if (mcode & mask)
								{ 
									uint32 d0,d1,d2,d3;
									d0 = *srcPtr++; 
									d1 = *srcPtr++;
									d2 = *srcPtr++; 
									d3 = *srcPtr++; 
									chunkSize -= 4;
									
									DecodeC4(dstPtr,x,y,width, d0, d1, d2, d3, colorMap0);
								}
								//	C1
								else
								{ 
									data = *srcPtr++; 
									chunkSize--;
									
									DecodeC1(dstPtr, x, y, width, data, colorMap1);
								}
							}
							
							mask >>= 1;
							x += 4;
							
							if (x >= width)
							{
								x = 0; 
								y += 4;
							}
						}
					}
					
					if (chunkSize) 
						srcPtr += chunkSize;
				}
				break;
				
				//	
				case 0x2400: 
				case 0x2600: 
				{ 
					uint32 chunkNum, i; 
					
					if (chunkID == 0x2400)	
						colorMap = colorMap0; 
					else
						colorMap = colorMap1; 

					chunkNum = chunkSize >> 2;  
					
					for(i = 0; i < chunkNum; i++) 
					{ 
						colorMap[i].color     = (uint32)(*srcPtr++);
						colorMap[i+256].color = (uint32)(*srcPtr++);
						colorMap[i+512].color = (uint32)(*srcPtr++);
						colorMap[i+768].color = (uint32)(*srcPtr++);						
					}
				}
				break;
				
				//	
				case 0x2500: 
				case 0x2700: 
				{ 
					uint32 ci = 0; 
					
					if (chunkID == 0x2500)	
						colorMap = colorMap0; 
					else
						colorMap = colorMap1; 
				
					while(chunkSize > 0)
					{ 
						uint32 mask = 0x80000000;
						flag  = (*srcPtr++) << 24;  
						flag |= (*srcPtr++) << 16;
						flag |= (*srcPtr++) <<  8;  
						flag |= *srcPtr++; chunkSize -= 4;
				
						while(mask)
						{ 
							if (mask & flag)
							{ 
								colorMap[ci].color     = (uint32)(*srcPtr++);
								colorMap[ci+256].color = (uint32)(*srcPtr++);
								colorMap[ci+512].color = (uint32)(*srcPtr++);
								colorMap[ci+768].color = (uint32)(*srcPtr++);
								
								chunkSize -= 4;
							}
							
							ci++; 
							mask >>= 1;
						} 
					}
					
					if (chunkSize != 0) 
					{
						#ifdef DEBUG						
							printf("Cinepak 25 error size %04lx\n", chunkSize);
						#endif
					}
				}
				break;
				
				default:
				{
					#ifdef DEBUG						
						printf("Cinepak unknown chunkID %08lx\n", chunkID);
					#endif
					return false;
				}
				break;					
			}
		}
	}
	
	return true;
}

//-------------------------------------------------------------------
//	CreateYUVTables
//-------------------------------------------------------------------
//
//	R = Y + 1.40200 * V
//	G = Y - 0.34414 * U - 0.71414 * V
//	B = Y + 1.77200 * U
//

void TCinepakCodec::CreateYUVTables()
{ 
	int32 index;
  	float tableUB, tableVR, tableUG, tableVG;

	if (m_UBTable == 0)
	{
		m_UBTable = (int32 *)malloc( 256 * sizeof(int32) );
		m_VRTable = (int32 *)malloc( 256 * sizeof(int32) );
		m_UGTable = (int32 *)malloc( 256 * sizeof(int32) );
		m_VGTable = (int32 *)malloc( 256 * sizeof(int32) );
	
		ASSERT(m_UBTable);
		ASSERT(m_VRTable);
		ASSERT(m_UGTable);
		ASSERT(m_VGTable);		
	}
	
	tableUB = (1.77200 / 2.0) * (float)(1 << 16) + 0.5;
	tableVR = (1.40200 / 2.0) * (float)(1 << 16) + 0.5;
	tableUG = (0.34414 / 2.0) * (float)(1 << 16) + 0.5;
	tableVG = (0.71414 / 2.0) * (float)(1 << 16) + 0.5;
	
	for(index = 0; index < 256; index++)
	{
		float x = (float)(2 * index - 255);
	
		m_UBTable[index] = (int32)( (tableUB * x) + (1 << 15)) >> 16;
		m_VRTable[index] = (int32)( (tableVR * x) + (1 << 15)) >> 16;
		m_UGTable[index] = (int32)( -tableUG * x);
		m_VGTable[index] = (int32)( -tableVG * x) + (1 << 15);
	}
}

//-------------------------------------------------------------------
//	SetupJPEGSampleLimitTable
//-------------------------------------------------------------------
//
//

const int32 MAXJSAMPLE 		= 255;
const int32 CENTERJSAMPLE 	= 128;

void TCinepakCodec::SetupJPEGSampleLimitTable()
{	
	int32 index;
	
	if (m_JPEGSampleLimit == 0)
	{
		m_JPEGSampleLimit = (uchar *)malloc((5 * (MAXJSAMPLE+1) + CENTERJSAMPLE));
		ASSERT(m_JPEGSampleLimit);
	}
	else 
		return;
	
	uchar *table = m_JPEGSampleLimit;
	
	//	Allow negative subscripts of simple table
	table += (MAXJSAMPLE + 1);   
	
	//	First segment of "simple" table: limit[x] = 0 for x < 0
	memset(table - (MAXJSAMPLE + 1), 0, (MAXJSAMPLE + 1));
	
	//	Main part of "simple" table: limit[x] = x
	for ( index = 0; index <= MAXJSAMPLE; index++) 
		table[index] = (uchar) index;
	
	//	Point to where post-IDCT table starts
	table += CENTERJSAMPLE;       
	
	//	End of simple table, rest of first half of post-IDCT table
	for (index = CENTERJSAMPLE; index < 2 * (MAXJSAMPLE + 1); index++) 
		table[index] = MAXJSAMPLE;
	
	//	Second half of post-IDCT table
	memset(table + (2 * (MAXJSAMPLE + 1)), 0, (2 * (MAXJSAMPLE + 1) - CENTERJSAMPLE));
	memcpy(table + (4 * (MAXJSAMPLE + 1) - CENTERJSAMPLE), (char *)(m_JPEGSampleLimit + (MAXJSAMPLE + 1)), CENTERJSAMPLE);
}

#pragma mark -
#pragma mark === Utility Routines ===

//-------------------------------------------------------------------
//	GetQuickTimeColor24
//-------------------------------------------------------------------
//
//	Dither Cinepak C4 Color Block OR Dither Four C1 Color Blocks 
//

uint32 GetQuickTimeColor24( uint32 red, uint32 green, uint32 blue)
{ 	
	uint32 color, ra, ga, ba;
	
	//	gzr: to do ...
	//	Adjust gamma in future...
	//ra = xa_gamma_adj[red]; 
	//ga = xa_gamma_adj[green]; 
	//ba = xa_gamma_adj[blue];
	uint32 alpha = 255;
	
	//	Create new color
	#if B_HOST_IS_LENDIAN
		color = ( ((alpha << 24) & kAlphaMask) | ((red << 16) & kRedMask) | ((green << 8) & kGreenMask) | (blue & kBlueMask) ); 					
	#else
		color = ( ((blue << 24) & kBlueMask) | ((green << 16) & kGreenMask) | ((red << 8) & kRedMask) | (alpha & kAlphaMask) ); 	
	#endif								

 	return(color);
}

//-------------------------------------------------------------------
//	DecodeC1
//-------------------------------------------------------------------
//
//

void TCinepakCodec::DecodeC1(uchar *dstPtr, uint32 x, uint32 y, uint32 width, int32 data, CinepakColor *colorMap)
{ 
	//printf("C1 Data: %d\n", data);
	
	//	Setup image pointer
	uint32 *imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	register uint32 d0, d1;
	
	*imagePtr++ = d0 = (uint32)(colorMap[data].color);
	*imagePtr++ = d0;
	data += 256;
		
	*imagePtr++ = d1 = (uint32)(colorMap[data].color);
	*imagePtr   = d1;
	data  += 256;
	
	//	Increment row
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	//	Load colors
	*imagePtr++ = d0;
	*imagePtr++ = d0;
	*imagePtr++ = d1;
	*imagePtr 	= d1;
	
	//	Increment row
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	*imagePtr++ = d0 = (uint32)(colorMap[data].color);
	*imagePtr++ = d0; 
	data += 256;
	
	*imagePtr++ = d1 = (uint32)(colorMap[data].color);
	*imagePtr 	= d1;
	
	//	Increment row
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	//	Load colors
	*imagePtr++ = d0;
	*imagePtr++ = d0;
	*imagePtr++ = d1;
	*imagePtr 	= d1;		
}

//-------------------------------------------------------------------
//	DecodeC4
//-------------------------------------------------------------------
//
//

void TCinepakCodec::DecodeC4(uchar *dstPtr, uint32 x, uint32 y, uint32 width, int32 d0, int32 d1, int32 d2, int32 d3, CinepakColor *colorMap)
{ 
	//printf("C4 Data: %d %d %d %d\n", d0, d1, d2, d3);
	
	//	Setup image pointer
	uint32 *imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);

	//	First block
	*imagePtr++ = (uint32)(colorMap[d0].color); d0 += 256;
	*imagePtr++ = (uint32)(colorMap[d0].color); d0 += 256;
	
	*imagePtr++ = (uint32)(colorMap[d1].color); d1 += 256;	
	*imagePtr   = (uint32)(colorMap[d1].color); d1 += 256;
	
	//	Increment row
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
 	*imagePtr++ = (uint32)(colorMap[d0].color); d0 += 256;	
	*imagePtr++ = (uint32)(colorMap[d0].color);	
	*imagePtr++ = (uint32)(colorMap[d1].color); d1 += 256;	
	*imagePtr	= (uint32)(colorMap[d1].color);
	
	//	Increment
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	//	Second block
	*imagePtr++ = (uint32)(colorMap[d2].color); d2 += 256;
	*imagePtr++ = (uint32)(colorMap[d2].color); d2 += 256;
	
	*imagePtr++ = (uint32)(colorMap[d3].color); d3 += 256;	
	*imagePtr   = (uint32)(colorMap[d3].color); d3 += 256;
	
	//	Increment row
	y++;
	imagePtr = (uint32 *)dstPtr;
	imagePtr += x;
	imagePtr += (y * width);
	
	*imagePtr++ = (uint32)(colorMap[d2].color); d2 += 256;	
	*imagePtr++ = (uint32)(colorMap[d2].color);	
	*imagePtr++ = (uint32)(colorMap[d3].color); d3 += 256;	
	*imagePtr	= (uint32)(colorMap[d3].color);
}
