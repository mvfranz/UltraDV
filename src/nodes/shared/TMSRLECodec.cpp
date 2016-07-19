//---------------------------------------------------------------------
//
//	File:	TMSRLECodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.18.98
//
//	Desc:	Microsoft Run Length Encoding CODEC Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TMSRLECodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TMSRLECodec::TMSRLECodec(TRIFFReader* reader) : TVideoCodec(reader)
{

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TMSRLECodec::~TMSRLECodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TMSRLECodec::Init()
{

}

//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TMSRLECodec::GetDescription(char* descText)
{
	return false;
}


//-------------------------------------------------------------------
//	DecodeFrame
//-------------------------------------------------------------------
//
//	Decode frame passed in buffer
//

bool TMSRLECodec::DecodeFrame(AVIVideoFrame* theFrame, void* dstBuffer, uint16 dstDepth)
{
	//	Get file and save position
	BFile* theFile = m_Reader->GetFile();
	off_t position = theFile->Position();
	theFile->Seek(0, SEEK_SET);

	//	Allocate buffer to contain frame data
	const int32 bufSize = theFrame->Length;
	void* srcBuffer = malloc(bufSize);
	ASSERT(srcBuffer);

	//	Load bits from current frame
	theFile->Seek(theFrame->Offset, SEEK_SET);
	theFile->Read(srcBuffer, bufSize);

	//	Restore file position
	theFile->Seek(position, SEEK_SET);

	//	Decode the frame
	bool retVal = DecodeMicrosoftRLE(m_Reader->Width(), m_Reader->Height(), srcBuffer, bufSize, dstBuffer);

	return retVal;
}


//-------------------------------------------------------------------
//	DecodeMicrosoftRLE
//-------------------------------------------------------------------
//
//	Decode Microsoft RLE and return BBitmap
//

bool TMSRLECodec::DecodeMicrosoftRLE(uint32 width, uint32 height, void* buffer, int32 bufSize, void* dstBuffer)
{
	uint32 opcode, mod;
	int32 x, y, minX, maxX, minY, maxY;

	int32 dataSize = bufSize;

	maxX    = 0;
	maxY    = 0;
	minX    = width;
	minY    = height;
	x               = 0;
	y               = height - 1;

	//	Setup pointers
	uchar* bufPtr = (uchar*)buffer;
	uchar* bits = (uchar*)dstBuffer;

	while( (y >= 0) && (dataSize > 0) ) {
		mod      = *bufPtr++;
		opcode   = *bufPtr++;
		dataSize -= 2;

		#ifdef DEBUG
		printf("MOD %x OPCODE %x <%d,%d>\n", mod, opcode, x, y);
		#endif

		//	End of line
		if (mod == 0x00) {
			if (opcode==0x00) {
				while( x > width) {
					x -= width;
					y--;
				}

				x = 0;
				y--;

				#ifdef DEBUG
				printf("EOL <%d,%d>\n",x,y);
				#endif
			}
			//	End of image
			else if (opcode==0x01) {
				y = -1;

				#ifdef DEBUG
				printf("EOI <%d,%d>\n",x,y);
				#endif
			}
			//	Skip
			else if (opcode==0x02) {
				uint32 yskip,xskip;
				xskip = *bufPtr++;
				yskip = *bufPtr++;  dataSize-=2;
				x += xskip;
				y -= yskip;

				#ifdef DEBUG
				printf("SKIP <%d,%d>\n",x,y);
				#endif
			}
			//	Absolute
			else{
				int cnt = opcode;

				dataSize-=cnt;
				while(x >= width) {
					x -= width;
					y--;
				}

				if (y > maxY)
					maxY = y;

				if (x < minX)
					minX = x;

				uchar* iptr = (uchar*)(bits + (y * width + x) );

				while(cnt--) {
					if (x >= width) {
						maxX = width;
						minX = 0;
						x -= width;
						y--;
						iptr = (uchar*)(bits + y * width + x);
					}

					*iptr++ = (uchar)(*bufPtr++);
					x++;
				}

				#ifdef DEBUG
				printf("Absolute <%d,%d>\n",x,y);
				#endif

				//	Pad to int16
				if (opcode & 0x01) {
					bufPtr++;
					dataSize--;
				}

				if (y < minY)
					minY = y;

				if (x > maxX)
					maxX = x;
			}
		}
		//	Encoded
		else{
			int color,cnt;

			while(x >= width) {
				x -= width;
				y--;
			}

			if (y > maxY)
				maxY = y;

			if (x < minX)
				minX = x;

			cnt   = mod;
			color = opcode;


			uint32* iptr = (uint32*)(bits + ((y * width + x)<<2) );
			uint32 clr = (uint32)color;

			while(cnt--) {
				if (x >= width) {
					maxX = width;
					minX = 0;
					x -= width;
					y--;
					iptr = (uint32*)(bits + y * width + x);
				}
				*iptr++ = clr;
				x++;
			}

			if (y < minY)
				minY = y;

			if (x > maxX)
				maxX = x;

			#ifdef DEBUG
			printf("Encoded <%d,%d>\n",x,y);
			#endif
		}
	}

	#ifdef DEBUG
	{
		printf("dataSize %d\n  ",dataSize);
		while(dataSize) {
			int d = *bufPtr++;
			printf("<%02x> ",d);
			dataSize--;
		}
		printf("\n");
	}
	#endif

	return true;
}