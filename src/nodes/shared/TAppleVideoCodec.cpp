//---------------------------------------------------------------------
//
//	File:	AppleVideoCodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.14.98
//
//	Desc:	Apple Video CODEC Routines "Road Pizza"
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"
#include "TVideoCodec.h"

#include "TAppleVideoCodec.h"

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TAppleVideoCodec::TAppleVideoCodec(TRIFFReader* reader) : TVideoCodec(reader)
{

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TAppleVideoCodec::~TAppleVideoCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TAppleVideoCodec::Init()
{

}


//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TAppleVideoCodec::GetDescription(char* descText)
{
	return false;
}


//-------------------------------------------------------------------
//	DecodeFrame
//-------------------------------------------------------------------
//
//	Decode frame passed in buffer
//

bool TAppleVideoCodec::DecodeFrame(AVIVideoFrame* theFrame, void* dstBuffer, uint16 dstDepth)
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
	bool retVal = DecodeAppleVideo(m_Reader->Width(), m_Reader->Height(), srcBuffer, bufSize, dstBuffer);

	return retVal;
}



//-------------------------------------------------------------------
//	DecodeAppleVideo
//-------------------------------------------------------------------
//
//	Decode Apple Quicktime Video "Road Pizza" Algorithm 'rpza'
//

bool TAppleVideoCodec::DecodeAppleVideo(uint32 width, uint32 height, void* srcBuffer, int32 bufSize, void* dstBuffer)
{
	uint32 changed;
	int32 x, y, length, row_inc, blk_inc;
	int32 minX, maxX, minY, maxY;
	uchar* im0, * im1, * im2, * im3;

	//	Setup pointers
	uchar* srcPtr   = (uchar*)srcBuffer;
	uchar* dstPtr   = (uchar*)dstBuffer;

	// skip 0xe1 byte
	srcPtr++;

	// Read length
	length  = (*srcPtr++) << 16;
	length |= (*srcPtr++) << 8;
	length |= (*srcPtr++);

	//	Check for corrupt frame.  Common with Apple Video codec
	if (length != bufSize) {
		#ifdef DEBUG
		printf("Corrupt frame... skipping...%x %x\n", bufSize, length);
		#endif

		return false;
	}

	// read 4 bytes already
	length -= 4;

	maxX    = 0;
	maxY    = 0;
	minX    = width;
	minY    = height;
	changed = 0;
	x               = 0;
	y               = 0;
	blk_inc = 4;

	row_inc = blk_inc * width;
	blk_inc *= 4;
	im1 = im0 = dstPtr;
	im1 += row_inc;
	im2 = im1;
	im2 += row_inc;
	im3 = im2;
	im3 += row_inc;

	// skip 3 rows at a time
	row_inc *= 3;

	while(length > 0) {
		uint32 code = *srcPtr++;
		length--;

		//	Single
		if ( (code >= 0xa0) && (code <= 0xbf) ) {
			uint32 color, skip;
			changed = 1;
			color = (*srcPtr++) << 8;
			color |= *srcPtr++;
			length -= 2;
			skip = (code - 0x9f);


			color = QuickTimeGetColor(color);

			while(skip--) {

				uint32* ip0 = (uint32*)im0;
				uint32* ip1 = (uint32*)im1;
				uint32* ip2 = (uint32*)im2;
				uint32* ip3 = (uint32*)im3;
				AppleVideoC1(ip0,ip1,ip2,ip3,color,uint32);

				QuickTimeMinMaxCheck(x, y, minX, minY, maxX, maxY);
				AppleVideoBlockInc(x, y, width, im0, im1, im2, im3, blk_inc, row_inc);
			}
		}
		// Skip
		else if ( (code >= 0x80) && (code <= 0x9f) ) {
			uint32 skip = (code-0x7f);

			while(skip--)
				AppleVideoBlockInc(x, y, width, im0, im1, im2, im3, blk_inc, row_inc);
		}
		//	Four or Sixteen color block
		else if ( (code < 0x80) || ((code >= 0xc0) && (code <= 0xdf)) ) {
			uint32 cA,cB;
			changed = 1;

			// Get 1st two colors
			if (code >= 0xc0) {
				cA = (*srcPtr++) << 8;
				cA |= *srcPtr++;
				length -= 2;
			} else {
				cA = (code << 8) | *srcPtr++;
				length -= 1;
			}

			cB = (*srcPtr++) << 8;
			cB |= *srcPtr++;
			length -= 2;

			//	Sixteen color block
			if ( (code < 0x80) && ((cB & 0x8000)==0) ) {
				uint32 i,d,* clr,c[16];
				uchar r[16],g[16],b[16];

				clr = c;
				*clr++ = QuickTimeGetColor(cA);
				*clr++ = QuickTimeGetColor(cB);

				for(i = 2; i < 16; i++) {
					d = (*srcPtr++) << 8;
					d |= *srcPtr++;
					length -= 2;

					*clr++ = QuickTimeGetColor(d);
				}

				clr = c;

				uint32* ip0 = (uint32*)im0;
				uint32* ip1 = (uint32*)im1;
				uint32* ip2 = (uint32*)im2;
				uint32* ip3 = (uint32*)im3;
				AppleVideoColor16(ip0, ip1, ip2, ip3, clr, uint32);

				QuickTimeMinMaxCheck(x,y,minX,minY,maxX,maxY);
				AppleVideoBlockInc(x, y, width, im0, im1, im2, im3, blk_inc, row_inc);
			}
			// Four color block
			else{
				uint32 m_cnt, msk0, msk1, msk2, msk3, c[4];
				uchar r[4], g[4], b[4];

				if (code < 0x80)
					m_cnt = 1;
				else
					m_cnt = code - 0xbf;

				QuickTimeGetAVColors(c, cA, cB);

				while(m_cnt--) {
					msk0 = *srcPtr++;
					msk1 = *srcPtr++;
					msk2 = *srcPtr++;
					msk3 = *srcPtr++;
					length -= 4;

					uint32* ip0= (uint32*)im0;
					uint32* ip1= (uint32*)im1;
					uint32* ip2= (uint32*)im2;
					uint32* ip3= (uint32*)im3;

					AppleVideoC4(ip0, c, msk0, uint32);
					AppleVideoC4(ip1, c, msk1, uint32);
					AppleVideoC4(ip2, c, msk2, uint32);
					AppleVideoC4(ip3, c, msk3, uint32);

					QuickTimeMinMaxCheck(x, y, minX, minY, maxX, maxY);
					AppleVideoBlockInc(x, y, width, im0, im1, im2, im3, blk_inc, row_inc);
				}
			}
		}
		//	Unknown
		else{
			#ifdef DEBUG
			printf("Apple Video: Unknown %x\n", code);
			#endif

			return false;
		}
	}

	return true;
}


//-------------------------------------------------------------------
//	QuickTimeGetColor
//-------------------------------------------------------------------
//
//

uint32 TAppleVideoCodec::QuickTimeGetColor(uint32 color)
{
	register uint32 newColor, ra, ga, ba, ra5, ga5, ba5;

	ra5 = (color >> 10) & 0x1f;
	ga5 = (color >>  5) & 0x1f;
	ba5 =  color & 0x1f;

	//ra = qt_gamma_adj[ra5];
	//ga = qt_gamma_adj[ga5];
	//ba = qt_gamma_adj[ba5];

	ra = ra5;
	ga = ga5;
	ba = ba5;

	newColor = GetTrueColor(ra, ga, ba, 16);

	return(newColor);
}

//-------------------------------------------------------------------
//	QuickTimeGetRGBColor
//-------------------------------------------------------------------
//
//

void TAppleVideoCodec::QuickTimeGetRGBColor( uchar* r, uchar* g, uchar* b, uint32 color)
{
	uint32 ra, ga, ba;

	ra = (color >> 10) & 0x1f;
	ra = (ra << 3) | (ra >> 2);
	ga = (color >>  5) & 0x1f;
	ga = (ga << 3) | (ga >> 2);

	ba =  color & 0x1f;
	ba = (ba << 3) | (ba >> 2);

	*r = ra;
	*g = ga;
	*b = ba;
}


//-------------------------------------------------------------------
//	QuickTimeGetAVColors
//-------------------------------------------------------------------
//
//

void TAppleVideoCodec::QuickTimeGetAVColors(uint32* c, uint32 cA, uint32 cB)
{
	uint32 clr,rA,gA,bA,rB,gB,bB,r0,g0,b0,r1,g1,b1;
	uint32 rA5,gA5,bA5,rB5,gB5,bB5;
	uint32 r05,g05,b05,r15,g15,b15;

	//	color 3
	rA5 = (cA >> 10) & 0x1f;
	gA5 = (cA >>  5) & 0x1f;
	bA5 =  cA & 0x1f;

	//	color 0
	rB5 = (cB >> 10) & 0x1f;
	gB5 = (cB >>  5) & 0x1f;
	bB5 =  cB & 0x1f;

	//	color 2
	r05 = (21 * rA5 + 11 * rB5) >> 5;
	g05 = (21 * gA5 + 11 * gB5) >> 5;
	b05 = (21 * bA5 + 11 * bB5) >> 5;

	//	color 1
	r15 = (11 * rA5 + 21 * rB5) >> 5;
	g15 = (11 * gA5 + 21 * gB5) >> 5;
	b15 = (11 * bA5 + 21 * bB5) >> 5;

	//	adj and scale to 16 bits
	/*rA = qt_gamma_adj[rA5];
	   gA = qt_gamma_adj[gA5];
	   bA = qt_gamma_adj[bA5];

	   rB = qt_gamma_adj[rB5];
	   gB = qt_gamma_adj[gB5];
	   bB = qt_gamma_adj[bB5];

	   r0 = qt_gamma_adj[r05];
	   g0 = qt_gamma_adj[g05];
	   b0 = qt_gamma_adj[b05];

	   r1 = qt_gamma_adj[r15];
	   g1 = qt_gamma_adj[g15];
	   b1 = qt_gamma_adj[b15];

	   rA = qt_gamma_adj[rA5];
	   gA = qt_gamma_adj[gA5];
	   bA = qt_gamma_adj[bA5];*/

	rA = rA5;
	gA = gA5;
	bA = bA5;

	rB = rB5;
	gB = gB5;
	bB = bB5;

	r0 = r05;
	g0 = g05;
	b0 = b05;

	r1 = r15;
	g1 = g15;
	b1 = b15;

	//	1st Color
	clr = GetTrueColor(rA, gA, bA, 16);
	c[3] = clr;

	// 2nd Color
	clr = GetTrueColor(rB, gB, bB, 16);
	c[0] = clr;

	//	1st Av
	clr = GetTrueColor(r0, g0, b0, 16);
	c[2] = clr;

	//	2nd Av
	clr = GetTrueColor(r1, g1, b1, 16);
	c[1] = clr;
}


//-------------------------------------------------------------------
//	QuickTimeGetAVRGBColors
//-------------------------------------------------------------------
//
//

void TAppleVideoCodec::QuickTimeGetAVRGBColors(uint32* c, uchar* r, uchar* g, uchar* b, uint32 cA, uint32 cB)
{
	uint32 rA, gA, bA, rB, gB, bB, ra, ga, ba;

	//	Color 3
	rA       = (cA >> 10) & 0x1f;
	r[3] = (rA << 3) | (rA >> 2);

	gA       = (cA >>  5) & 0x1f;
	g[3] = (gA << 3) | (gA >> 2);

	bA       =  cA & 0x1f;
	b[3] = (bA << 3) | (bA >> 2);

	//	Color 0
	rB   = (cB >> 10) & 0x1f;
	r[0] = (rB << 3) | (rB >> 2);

	gB   = (cB >>  5) & 0x1f;
	g[0] = (gB << 3) | (gB >> 2);

	bB       =  cB & 0x1f;
	b[0] = (bB << 3) | (bB >> 2);

	//	Color 2
	ra       = (21*rA + 11*rB) >> 5;
	r[2] = (ra << 3) | (ra >> 2);

	ga   = (21*gA + 11*gB) >> 5;
	g[2] = (ga << 3) | (ga >> 2);

	ba   = (21*bA + 11*bB) >> 5;
	b[2] = (ba << 3) | (ba >> 2);

	//	Color 1
	ra   = (11*rA + 21*rB) >> 5;
	r[1] = (ra << 3) | (ra >> 2);

	ga   = (11*gA + 21*gB) >> 5;
	g[1] = (ga << 3) | (ga >> 2);

	ba   = (11*bA + 21*bB) >> 5;
	b[1] = (ba << 3) | (ba >> 2);
}