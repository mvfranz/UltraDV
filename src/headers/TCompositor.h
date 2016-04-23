//---------------------------------------------------------------------
//
//	File:	TCompositor.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.02.98
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TCOMPOSITOR_H__
#define __TCOMPOSITOR_H__


//
//	Structs
//

//	Struct used in biilnear interpolation optimization
typedef struct
{
	int32 	xInt;;
	float 	p;
	float	p1;
} RowValues;

//	Struct used in bicubic scaline
typedef struct
{
	int32 	srcOffset;
	int32	coefficient[5];
	
} CubicTable;


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

#define ZERO		0.00000000001

//	Macros
#define ROUNDDOWN(IX, X)\
		IX = (int32) X;\
		if ((X < ZERO) && (X != (float)IX))\
		IX -= 1;

//	Function Prototypes
int32 GetBitmapPixelSize(color_space colorSpace);

// Pixel Conversion Routines
rgb_color ConvertPixelGRAY8to32(uint8 srcPixel);
rgb_color ConvertPixelCMAP8to32(uint8 srcPixel);
rgb_color ConvertPixelRGB15to32(int16 srcPixel);
rgb_color ConvertPixelRGBA15to32(int16 srcPixel);
rgb_color ConvertPixelRGB16to32(uint16 srcPixel);

//	Scaling Routines
void 	ScaleBitmapNN(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center);
void	ScaleBitmapBilinear(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center);
void	ScaleBitmapBicubic(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center);
void 	ScaleBitmapBicubicH(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, CubicTable *hTable);
void 	ScaleBitmapBicubicV(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, CubicTable *vTable);
void 	CreateBicubicTableH( const uint32 dstColumns, const uint32 srcColumns, CubicTable *tableH);
void 	CreateBicubicTableV( const uint32 dstRows, const uint32 srcRows, CubicTable *tableV);
float 	GetCubicFloat(float x);

//	Rotation Routines
void	RotateBitmapNN(BBitmap *srcBitmap, BBitmap *dstBitmap, float degrees, BPoint originPt);
void 	RotateBitmapBilinear(BBitmap *srcBitmap, BBitmap *dstBitmap, float degrees, BPoint originPt);

//	Mirroring Functions
BBitmap *MirrorBitmapH(BBitmap *srcBitmap, bool center);
BBitmap *MirrorBitmapV(BBitmap *srcBitmap, bool center);

//	Utility Functions
int32 	ClipChar(int32 theValue);
int 	sgn(int32 a);
BPoint 	PlotLine( BPoint startPt, BPoint endPt, int32 numSteps);

#endif
