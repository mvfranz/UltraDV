//---------------------------------------------------------------------
//
//	File:	AppleVideoCodec.h
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

#ifndef __APPLEVIDEOCODEC_H__
#define __APPLEVIDEOCODEC_H__

//	Includes
#include "TVideoCodec.h"

//	Apple Video "Road Pizza" decoder
class TAppleVideoCodec : public TVideoCodec
{
	public:
		TAppleVideoCodec(TRIFFReader *reader);
		~TAppleVideoCodec();
		
		bool 	GetDescription(char *descText);
		
		bool	DecodeFrame(AVIVideoFrame *theFrame, void *dstBuffer, uint16 dstDepth);
		
	private:
		void Init();
		
		bool 	DecodeAppleVideo(uint32 width, uint32 height, void *buffer, int32 bufSize, void *dstBuffer);
		void 	QuickTimeGetRGBColor( uchar *r, uchar *g, uchar *b, uint32 color);
		void 	QuickTimeGetAVRGBColors(uint32 *c, uchar *r, uchar *g, uchar *b, uint32 cA, uint32 cB);
		uint32 	QuickTimeGetColor( uint32 color);
		void 	QuickTimeGetAVColors(uint32 *c, uint32 cA, uint32 cB);
		
	protected:

};

//	QuickTime Macros
#define QuickTimeMinMaxCheck(x,y,min_x,min_y,max_x,max_y) {	\
  if (x > max_x) max_x=x; if (y > max_y) max_y=y;	\
  if (x < min_x) min_x=x; if (y < min_y) min_y=y;  }

#define AppleVideoC1(ip0,ip1,ip2,ip3,c,CAST) { \
 ip0[0] = ip0[1] = ip0[2] = ip0[3] = (CAST)c; \
 ip1[0] = ip1[1] = ip1[2] = ip1[3] = (CAST)c; \
 ip2[0] = ip2[1] = ip2[2] = ip2[3] = (CAST)c; \
 ip3[0] = ip3[1] = ip3[2] = ip3[3] = (CAST)c; }

#define AppleVideoRGBC1(ip,r,g,b) { \
 ip[0] = ip[3] = ip[6] = ip[9]  = r; \
 ip[1] = ip[4] = ip[7] = ip[10] = g; \
 ip[2] = ip[5] = ip[8] = ip[11] = b; }

#define AppleVideoC4(ip,c,mask,CAST); { \
 ip[0] =(CAST)(c[((mask>>6)&0x03)]); ip[1] =(CAST)(c[((mask>>4)&0x03)]); \
 ip[2] =(CAST)(c[((mask>>2)&0x03)]); ip[3] =(CAST)(c[ (mask & 0x03)]); }
 
#define AppleVideoRGBC4(ip,r,g,b,mask); { uint32 _idx; \
 _idx = (mask>>6)&0x03; ip[0] = r[_idx]; ip[1] = g[_idx]; ip[2] = b[_idx]; \
 _idx = (mask>>4)&0x03; ip[3] = r[_idx]; ip[4] = g[_idx]; ip[5] = b[_idx]; \
 _idx = (mask>>2)&0x03; ip[6] = r[_idx]; ip[7] = g[_idx]; ip[8] = b[_idx]; \
 _idx =  mask    &0x03; ip[9] = r[_idx]; ip[10] = g[_idx]; ip[11] = b[_idx]; }


#define AppleVideoRGBC16(ip,r,g,b) { \
 ip[0]= *r++; ip[1]= *g++; ip[2]= *b++; \
 ip[3]= *r++; ip[4]= *g++; ip[5]= *b++; \
 ip[6]= *r++; ip[7]= *g++; ip[8]= *b++; \
 ip[9]= *r++; ip[10]= *g++; ip[11]= *b++; }


#define AppleVideoBlockInc(x, y, imagex, im0, im1, im2, im3, binc, rinc) \
{ x += 4; im0 += binc; im1 += binc; im2 += binc; im3 += binc;	\
  if (x>=imagex)					\
  { x=0; y += 4; im0 += rinc; im1 += rinc; im2 += rinc; im3 += rinc; } }

#define AppleVideoColor16(ip0,ip1,ip2,ip3,c,CAST) { \
 ip0[0] = (CAST)(*c++); ip0[1] = (CAST)(*c++); \
 ip0[2] = (CAST)(*c++); ip0[3] = (CAST)(*c++); \
 ip1[0] = (CAST)(*c++); ip1[1] = (CAST)(*c++); \
 ip1[2] = (CAST)(*c++); ip1[3] = (CAST)(*c++); \
 ip2[0] = (CAST)(*c++); ip2[1] = (CAST)(*c++); \
 ip2[2] = (CAST)(*c++); ip2[3] = (CAST)(*c++); \
 ip3[0] = (CAST)(*c++); ip3[1] = (CAST)(*c++); \
 ip3[2] = (CAST)(*c++); ip3[3] = (CAST)(*c  ); }
 
#endif