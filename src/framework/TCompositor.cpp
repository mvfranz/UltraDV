//---------------------------------------------------------------------
//
//	File:	TCompositor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.02.98
//
//	Desc:	Bitmap compositing routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

//#include "math.h"

#include "TCompositor.h"


//---------------------------------------------------------------------
//	GetBitmapPixelSize
//---------------------------------------------------------------------
//
//	Return size of bitmap pixel based on color space

int32 GetBitmapPixelSize(color_space colorSpace)
{
	switch(colorSpace)
	{
		// Unused and non-values
		case B_NO_COLOR_SPACE:
		case B_RGB24:
		case B_RGB24_BIG:
		case B_YUV422:
		case B_YUV411:
		case B_YUV420:
		case B_YUV444:
		case B_YUV9:
		case B_YUV12:
			return(-1);
			break;

		// 32-bit
		case B_RGB32:
		case B_RGBA32:
		case B_RGB32_BIG:
		case B_RGBA32_BIG:
			return sizeof(uint32);
			break;

		// 16-bit
		case B_RGB16:
		case B_RGB15:
		case B_RGBA15:
		case B_RGB16_BIG:
		case B_RGB15_BIG:
		case B_RGBA15_BIG:
			return sizeof(uint16);
			break;


		// 8-bit
		case B_CMAP8:
		case B_GRAY8:
			return sizeof(uchar);
			break;

		// Monochrome
		case B_GRAY1:
			return sizeof(uchar);
			break;

		default:
			return(-1);
			break;

	}
}


#pragma mark -
#pragma mark === Pixel Conversion Routines ===

//---------------------------------------------------------------------
//	ConvertPixelGRAY8to32
//---------------------------------------------------------------------
//
//	Convert an 8 bit grayscale pixel to 32-bit.
//

rgb_color ConvertPixelGRAY8to32(uint8 srcPixel)
{
	rgb_color color;

	color.red 	= srcPixel;
	color.green = srcPixel;
	color.blue 	= srcPixel;
	color.alpha = 0;

	return color;
}



//---------------------------------------------------------------------
//	ConvertPixelCMAP8to32
//---------------------------------------------------------------------
//
//	Convert an 8 bit pixel to 32-bit.  Get index into CLUT and return
//	32-bit value
//

rgb_color ConvertPixelCMAP8to32(uint8 srcPixel)
{
	BScreen screen;

	return screen.ColorForIndex(srcPixel);
}


//---------------------------------------------------------------------
//	ConvertPixelRGB15to32
//---------------------------------------------------------------------
//
//	Convert a 15 bit pixel to 32-bit.  5-bits for red, blue
//	and green
//

// 32768
rgb_color ConvertPixelRGB15to32(int16 srcPixel)
{
	uint8 red 	= ( (uint8) (srcPixel >> 7) ) & 0xF8;
	uint8 green = ( (uint8) (srcPixel >> 2) ) & 0xF8;
	uint8 blue 	= ( (uint8) (srcPixel << 3) ) & 0xF8;
	uint8 alpha = 0;

	// 	Now adjust index of values.  Normalize from 0-248 to
	//	0-255

	// Set up rgb_color and return
	rgb_color color;
	color.red 	= red;
	color.green = green;
	color.blue 	= blue;
	color.alpha = alpha;

	return color;
}


//---------------------------------------------------------------------
//	ConvertPixelRGBA15to32
//---------------------------------------------------------------------
//
//	Convert a 15 bit pixel with alpha to 32-bit.  5-bits for red, blue
//	and green.  1 bit alpha ignored
//

rgb_color ConvertPixelRGBA15to32(int16 srcPixel)
{
	uint8 red 	= ( (uint8) (srcPixel >> 7) ) & 0xF8;
	uint8 green = ( (uint8) (srcPixel >> 2) ) & 0xF8;
	uint8 blue 	= ( (uint8) (srcPixel << 3) ) & 0xF8;
	uint8 alpha = 0;

	// 	Now adjust index of values.  Normalize from 0-248 to
	//	0-255

	// Set up rgb_color and return
	rgb_color color;
	color.red 	= red;
	color.green = green;
	color.blue 	= blue;
	color.alpha = alpha;

	return color;
}

//---------------------------------------------------------------------
//	ConvertPixelRGB16to32
//---------------------------------------------------------------------
//
//	Convert a 16 bit pixel with alpha to 32-bit.  5-bits for red and
//	blue, 6-bits for green
//

rgb_color ConvertPixelRGB16to32(uint16 srcPixel)
{
	uint8 red 	= ( (uint8) (srcPixel >> 8) ) & 0xF8;
	uint8 green = ( (uint8) (srcPixel >> 3) ) & 0xF8;
	uint8 blue 	= ( (uint8) (srcPixel << 2) ) & 0xF8;
	uint8 alpha = 0;

	// 	Now adjust index of values.  Normalize from 0-248 to
	//	0-255

	// Set up rgb_color and return
	rgb_color color;
	color.red 	= red;
	color.green = green;
	color.blue 	= blue;
	color.alpha = alpha;

	return color;
}

/*
//---------------------------------------------------------------------
//	ConvertDepth
//---------------------------------------------------------------------
//
//

#define  DEPTH  3
#define  XSIZE  640
#define  YSIZE  480

unsigned char *ir,*ig,*ib;
unsigned char *outRed,*og,*ob;

void ConvertDepth(float depth, BBitmap *srcBitmap, *dstBitmap)
{
	long int x, y;
	float 	 scale;

	// allocate input and output memory buffers
	ir = (unsigned char *) malloc (XSIZE*YSIZE);
	ig = (unsigned char *) malloc (XSIZE*YSIZE);
	ib = (unsigned char *) malloc (XSIZE*YSIZE);
	outRed = (unsigned char *) malloc (XSIZE*YSIZE);
	og = (unsigned char *) malloc (XSIZE*YSIZE);
	ob = (unsigned char *) malloc (XSIZE*YSIZE);

	// read the input image (8-bit resolution)
	read_image("input",ir,ig,ib);

	//	Calculate the scale factor that will reduce 8-bit values
	//	(0-255, inclusive) to evenly spaced values of 0-255 for
	//	the requested depth resolution.
	//	Example: if DEPTH = 2, values are 0, 85, 171, and 255
	scale = 255.0 / (powf (2.0, (float)DEPTH) - 1.0);

	// reduce depth resolution by generating new 8-bit pixel values
	for (y = 0; y < YSIZE; y = y++)
	{
		for (x = 0; x < XSIZE; x = x++)
		{
	    	outRed[(y*XSIZE)+x] = (ir[(y*XSIZE)+x] >> (8 - DEPTH)) * scale;
	    	og[(y*XSIZE)+x] = (ig[(y*XSIZE)+x] >> (8 - DEPTH)) * scale;
	    	ob[(y*XSIZE)+x] = (ib[(y*XSIZE)+x] >> (8 - DEPTH)) * scale;
		}
	}

	// write the output image
	write_image("output",outRed,og,ob);

	// free memory buffers
	free (ir);
	free (ig);
	free (ib);
	free (outRed);
	free (og);
	free (ob);
}
*/

#pragma mark -
#pragma mark === Scaling Routines ===


//---------------------------------------------------------------------
//	ScaleBitmapNN
//---------------------------------------------------------------------
//
//	Scale a bitmap using bilinear interpolation to create new pixels
//

void ScaleBitmapNN(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center)
{
	uint32 	sourceX, sourceY; 	// 	X and Y address of source pixel

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows		= dstBitmap->Bounds().Height();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	//	Perform scale by inverse mapping from destination to source
	//

	uchar	*dstRowPtr, *srcRowPtr;
	uint32	srcYBPR;

	// 	Traverse through bitmap scanlines
    for( uint32 y = 0; y < dstRows; y++)
	{
		// Set up row pointers
		dstRowPtr = dstBits;

		//	Get Y offset
		sourceY = (uint32)((y / scaleBy.y) + 0.5);
		srcYBPR = srcBPR * sourceY;

		//	Locate first pixel of the source row for this destination row
		//srcRowPtr = srcBits + srcYBPR;

		// 	Traverse through pixel rows.  Traverse the visible width of the image
		for( uint32 x = 0; x < dstColumns; x++)
	    {
		 	//	Get X offset
			sourceX = (uint32)((x / scaleBy.x) + 0.5);

			//	Index into bits
			srcRowPtr = srcBits + srcYBPR + (sourceX * srcIncrement);
			//srcRowPtr += sourceX * srcIncrement;

			//	Copy data
			*(uint32 *)dstRowPtr = *(uint32 *)srcRowPtr;

			//	Increment line pointer
			dstRowPtr += dstIncrement;
	    }

		// Offset pixel rows
		dstBits += dstBPR;
    }
}


//---------------------------------------------------------------------
//	ScaleBitmapBilinear
//---------------------------------------------------------------------
//
//	Scale a bitmap using bilinear interpolation to create new pixels
//

void ScaleBitmapBilinear(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center)
{
	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows		= dstBitmap->Bounds().Height();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	//	Calculate scale factors and inverse scale factors
	float scaleByXInv = 1.0 / scaleBy.x;
	float scaleByYInv = 1.0 / scaleBy.y;

	//	Allocate buffer for storing the values of m and m1
	RowValues 	*row, *rowPtr;
	float		xFloat;

	row = rowPtr = (RowValues *)malloc( sizeof(RowValues) * dstColumns);

	//	Fill the buffer once, to be used for each row
	for (int32 index = 0; index < dstColumns; index++)
	{
		xFloat = index * scaleByXInv;
		rowPtr->xInt = (int32)xFloat;
		rowPtr->p = xFloat - (float)rowPtr->xInt;
		rowPtr->p1 = 1.0 - rowPtr->p;
		rowPtr++;
	}

	//	Perform scale by inverse mapping from destination to source
	//

	float 	srcYFloat;
	int32	srcXInt, srcYInt;
	float 	p,q, p1, q1;
	uchar	*dstRowPtr, *srcPixel01, *srcPixel02;
	uint32	pixelA, pixelB, pixelC, pixelD;
	uint32	newPixel;
	float	newPixelBlue, newPixelGreen, newPixelRed;
	uchar	pixelABlue, pixelBBlue, pixelCBlue, pixelDBlue;
	uchar	pixelARed, pixelBRed, pixelCRed, pixelDRed;
	uchar	pixelAGreen, pixelBGreen, pixelCGreen, pixelDGreen;
	uint32 	addPixBlue, addPixGreen, addPixRed;

	// 	Traverse through bitmap scanlines
    for( uint32 y = 0; y < dstRows - 1; y++)
	{
		// Set up row pointers
		dstRowPtr = dstBits;

		srcYFloat = y * scaleByYInv;
 		srcYInt	  = (int32) srcYFloat;

		q  = srcYFloat - (float) srcYInt;
		q1 = 1.0 - q;

		srcPixel01 = srcBits + srcYInt * srcBPR;
		srcPixel02 = srcBits + (srcYInt + srcIncrement) * srcBPR;
		rowPtr 	   = row;

		// 	Traverse through pixel rows.  Traverse the visible width of the image
		for( uint32 x = 0; x < dstColumns - 1; x++, rowPtr++)
	    {
			//	Init these values from our precalculated struct
			srcXInt = rowPtr->xInt;

			p  = rowPtr->p;
			p1 = rowPtr->p1;

			//	Get the four pixels to interpolate from
			pixelA = *(uint32 *)(srcPixel01 + (srcXInt * srcIncrement));
			pixelB = *(uint32 *)(srcPixel01 + (srcXInt * srcIncrement) + srcIncrement);
			pixelC = *(uint32 *)(srcPixel02 + (srcXInt * srcIncrement));
			pixelD = *(uint32 *)(srcPixel02 + (srcXInt * srcIncrement) + srcIncrement);

			//	Seperate pixel components
			pixelABlue 	= (pixelA & kBlueMask)	>> 24;
			pixelAGreen = (pixelA & kGreenMask) >> 16;
			pixelARed 	= (pixelA & kRedMask) 	>> 8;

			pixelBBlue 	= (pixelA & kBlueMask)	>> 24;
			pixelBGreen = (pixelA & kGreenMask) >> 16;
			pixelBRed 	= (pixelA & kRedMask) 	>> 8;

			pixelCBlue 	= (pixelA & kBlueMask)	>> 24;
			pixelCGreen = (pixelA & kGreenMask) >> 16;
			pixelCRed 	= (pixelA & kRedMask) 	>> 8;

			pixelDBlue 	= (pixelA & kBlueMask)	>> 24;
			pixelDGreen = (pixelA & kGreenMask) >> 16;
			pixelDRed 	= (pixelA & kRedMask) 	>> 8;

			//	Compute interpolated pixel value
			newPixelBlue  = ( ((float) pixelABlue * p1 + (float) pixelBBlue * p) * q1 +
							  ((float) pixelCBlue * p1 + (float) pixelDBlue * p) * q );

			newPixelGreen = ( ((float) pixelAGreen * p1 + (float) pixelBGreen * p) * q1 +
							  ((float) pixelCGreen * p1 + (float) pixelDGreen * p) * q );

			newPixelRed   = ( ((float) pixelARed * p1 + (float) pixelBRed * p) * q1 +
							  ((float) pixelCRed * p1 + (float) pixelDRed * p) * q );

			//	Cast and shift to get uchar pixel values
			addPixBlue 	= (uint32)newPixelBlue;
			addPixBlue 	<<= 24;
			addPixGreen = (uint32)newPixelGreen;
			addPixGreen <<= 16;
			addPixRed 	= (uint32)newPixelRed;
			addPixRed 	<<= 8;

			// 	Logical OR pixel components to create new pixel
			newPixel = addPixBlue | addPixGreen | addPixRed;

			//	Assign to destination
			*(uint32 *)dstRowPtr = newPixel;

			//	Increment destination
			dstRowPtr += dstIncrement;
		}

		// Offset pixel row
		dstBits += dstBPR;
    }
}


//---------------------------------------------------------------------
//	ScaleBitmapBicubic
//---------------------------------------------------------------------
//
//	Scale a bitmap using bilinear interpolation to create new pixels
//

//	Defines
#define SPACC	5		//	SubPixelACCuracy
#define	SUPPORT	5		// 	Region of support for filter
#define	SCA		4096	//	Filter coefficient scaled to 12-bits
#define SHIFT	12
#define HALF	(1<<11)
#define	EDGE	2		//	Two edge pixels at boundary - no write

void ScaleBitmapBicubic(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, bool center)
{
	CubicTable	*hTablePtr, *vTablePtr;

	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows		= dstBitmap->Bounds().Height();

	//	Allocate intermediate image
	BRect interArea(0, 0, dstColumns, srcRows);
	BBitmap *interBitmap = new BBitmap(interArea, B_RGB32);

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	//	Allocate memory for tables
	hTablePtr = (CubicTable *) malloc( sizeof(CubicTable) * dstColumns );
	vTablePtr = (CubicTable *) malloc( sizeof(CubicTable) * dstRows );

	//	Compute table for one row
	CreateBicubicTableH(srcColumns, dstColumns, hTablePtr);

	//	Perform horizontal scale
	ScaleBitmapBicubicH(srcBitmap, interBitmap, scaleBy, hTablePtr);

	//	Compute table for one column
	CreateBicubicTableV( interBitmap->Bounds().Height(), dstRows, vTablePtr);

	//	Perform vertical scale
	ScaleBitmapBicubicV(interBitmap, dstBitmap, scaleBy, vTablePtr);

	//	Clean up
	delete interBitmap;
	free(hTablePtr);
	free(vTablePtr);
}


//---------------------------------------------------------------------
//	ScaleBitmapBicubicH
//---------------------------------------------------------------------
//
//	Scale horizontally using a table of filters
//

void ScaleBitmapBicubicH(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, CubicTable *hTable)
{
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows		= dstBitmap->Bounds().Height();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	//
	//	Perform horizontal scale
	//

	CubicTable 	*tablePtr;
	uchar 		*srcRowPtr, *dstRowPtr;
	int32		resultBlue, resultGreen, resultRed;
	uint32		newPixel;
	uint32 		addPixBlue, addPixGreen, addPixRed;

	int32		pixel01, pixel02, pixel03, pixel04, pixel05;
	uchar		blue01, blue02, blue03, blue04, blue05;
	uchar		green01, green02, green03, green04, green05;
	uchar		red01, red02, red03, red04, red05;

	//	Precalculate pointer offsets
	const int32 src1X = 1 * srcIncrement;
	const int32 src2X = 2 * srcIncrement;

	// 	Traverse through bitmap scanlines
	for( uint32 y = 0; y < srcRows; y++)
	{
		//	No write condition at edge
		tablePtr = hTable + EDGE;

		dstRowPtr = dstBits;

		// 	Traverse through pixel rows.  Traverse the width of the image
		for( uint32 x = 2; x < dstColumns; x++)
	    {
			//	Note that srcOffset corresponds to the offset of the first source
			//	pixel to the left of the current subpixel position (i.e., dest mapped
			//	back to source)
			srcRowPtr = srcBits + (tablePtr->srcOffset * srcIncrement);

			//	Load pixels from source
			pixel01 = *(int32 *) (srcRowPtr - src2X);
			pixel02 = *(int32 *) (srcRowPtr - src1X);
			pixel03 = *(int32 *) (srcRowPtr);
			pixel04 = *(int32 *) (srcRowPtr + src1X);
			pixel05 = *(int32 *) (srcRowPtr + src2X);

			//	Compute convolution using info from table

			//	Calculate new blue pixel
			blue01 = (pixel01 & kBlueMask) >> 24;
			blue02 = (pixel02 & kBlueMask) >> 24;
			blue03 = (pixel03 & kBlueMask) >> 24;
			blue04 = (pixel04 & kBlueMask) >> 24;
			blue05 = (pixel05 & kBlueMask) >> 24;

			resultBlue = 	blue01 * tablePtr->coefficient[0] +
							blue02 * tablePtr->coefficient[1] +
							blue03 * tablePtr->coefficient[2] +
							blue04 * tablePtr->coefficient[3] +
							blue05 * tablePtr->coefficient[4];

			//	Calculate new green pixel
			green01 = (pixel01 & kGreenMask) >> 16;
			green02 = (pixel02 & kGreenMask) >> 16;
			green03 = (pixel03 & kGreenMask) >> 16;
			green04 = (pixel04 & kGreenMask) >> 16;
			green05 = (pixel05 & kGreenMask) >> 16;

			resultGreen = 	green01 * tablePtr->coefficient[0] +
							green02 * tablePtr->coefficient[1] +
							green03 * tablePtr->coefficient[2] +
							green04 * tablePtr->coefficient[3] +
							green05 * tablePtr->coefficient[4];

			//	Calculate new red pixel
			red01 = (pixel01 & kRedMask) >> 8;
			red02 = (pixel02 & kRedMask) >> 8;
			red03 = (pixel03 & kRedMask) >> 8;
			red04 = (pixel04 & kRedMask) >> 8;
			red05 = (pixel05 & kRedMask) >> 8;

			resultRed = 	red01 * tablePtr->coefficient[0] +
							red02 * tablePtr->coefficient[1] +
							red03 * tablePtr->coefficient[2] +
							red04 * tablePtr->coefficient[3] +
							red05 * tablePtr->coefficient[4];


			//	Convert results to 8-bit
			resultBlue 	= (resultBlue  + HALF) >> SHIFT;
			resultGreen = (resultGreen + HALF) >> SHIFT;
			resultRed 	= (resultRed   + HALF) >> SHIFT;

			//	Clip results if needed
			resultBlue 	= ClipChar(resultBlue);
			resultGreen = ClipChar(resultGreen);
			resultRed 	= ClipChar(resultRed);

			//	Cast and shift to get uchar pixel values
			addPixBlue 	= (uchar)resultBlue;
			addPixBlue 	<<= 24;
			addPixGreen = (uchar)resultGreen;
			addPixGreen <<= 16;
			addPixRed 	= (uchar)resultRed;
			addPixRed 	<<= 8;

			//	Create new pixel
			newPixel = addPixBlue | addPixGreen | addPixRed;

			//	Assign new pixel
			*(uint32 *)dstRowPtr = newPixel;

			//	Increment rows and table
			dstRowPtr += dstIncrement;
			tablePtr++;
		}

		srcBits += srcBPR;
		dstBits += dstBPR;

    }
}


//---------------------------------------------------------------------
//	ScaleBitmapBicubicV
//---------------------------------------------------------------------
//
//	Scale vertically using a table of filters
//

void ScaleBitmapBicubicV(BBitmap *srcBitmap, BBitmap *dstBitmap, BPoint scaleBy, CubicTable *vTable)
{
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows		= dstBitmap->Bounds().Height();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	//
	//	Perform vertical scale
	//

	//	Precalculate pointer offsets
	const int32 src1Y = 1 * srcBPR;
	const int32 src2Y = 2 * srcBPR;

	CubicTable 	*tablePtr;
	uchar 		*srcRowPtr, *dstRowPtr;
	int32		resultBlue, resultGreen, resultRed;
	uint32		newPixel;
	uint32 		addPixBlue, addPixGreen, addPixRed;

	int32		pixel01, pixel02, pixel03, pixel04, pixel05;
	uchar		blue01, blue02, blue03, blue04, blue05;
	uchar		green01, green02, green03, green04, green05;
	uchar		red01, red02, red03, red04, red05;

	// 	Traverse through bitmap scanlines
    for( uint32 x = 0; x < srcColumns; x++)
	{
		//	No write condition at edge
		tablePtr = vTable + EDGE;

		dstRowPtr = dstBits;

		// 	Traverse through pixel rows.  Traverse the height of the image
		for( uint32 y = 2; y < dstRows-EDGE; y++)
	    {
			//	Note that srcOffset corresponds to the offset of the first source
			//	pixel to the left of the current subpixel position (i.e., dest mapped
			//	back to source)
			srcRowPtr = srcBits + tablePtr->srcOffset * srcBPR;

			//	Load pixels from source
			pixel01 = *(int32 *) (srcRowPtr - src2Y);
			pixel02 = *(int32 *) (srcRowPtr - src1Y);
			pixel03 = *(int32 *) (srcRowPtr);
			pixel04 = *(int32 *) (srcRowPtr + src1Y);
			pixel05 = *(int32 *) (srcRowPtr + src2Y);

			//	Compute convolution using info from table

			//	Calculate new blue pixel
			blue01 = (pixel01 & kBlueMask) >> 24;
			blue02 = (pixel02 & kBlueMask) >> 24;
			blue03 = (pixel03 & kBlueMask) >> 24;
			blue04 = (pixel04 & kBlueMask) >> 24;
			blue05 = (pixel05 & kBlueMask) >> 24;

			resultBlue = 	blue01 * tablePtr->coefficient[0] +
							blue02 * tablePtr->coefficient[1] +
							blue03 * tablePtr->coefficient[2] +
							blue04 * tablePtr->coefficient[3] +
							blue05 * tablePtr->coefficient[4];

			//	Calculate new green pixel
			green01 = (pixel01 & kGreenMask) >> 16;
			green02 = (pixel02 & kGreenMask) >> 16;
			green03 = (pixel03 & kGreenMask) >> 16;
			green04 = (pixel04 & kGreenMask) >> 16;
			green05 = (pixel05 & kGreenMask) >> 16;

			resultGreen = 	green01 * tablePtr->coefficient[0] +
							green02 * tablePtr->coefficient[1] +
							green03 * tablePtr->coefficient[2] +
							green04 * tablePtr->coefficient[3] +
							green05 * tablePtr->coefficient[4];

			//	Calculate new red pixel
			red01 = (pixel01 & kRedMask) >> 8;
			red02 = (pixel02 & kRedMask) >> 8;
			red03 = (pixel03 & kRedMask) >> 8;
			red04 = (pixel04 & kRedMask) >> 8;
			red05 = (pixel05 & kRedMask) >> 8;

			resultRed = 	red01 * tablePtr->coefficient[0] +
							red02 * tablePtr->coefficient[1] +
							red03 * tablePtr->coefficient[2] +
							red04 * tablePtr->coefficient[3] +
							red05 * tablePtr->coefficient[4];


			//	Convert results to 8-bit
			resultBlue 	= (resultBlue  + HALF) >> SHIFT;
			resultGreen = (resultGreen + HALF) >> SHIFT;
			resultRed 	= (resultRed   + HALF) >> SHIFT;

			//	Clip results if needed
			resultBlue 	= ClipChar(resultBlue);
			resultGreen = ClipChar(resultGreen);
			resultRed 	= ClipChar(resultRed);

			//	Cast and shift to get uchar pixel values
			addPixBlue 	= (uchar)resultBlue;
			addPixBlue 	<<= 24;
			addPixGreen = (uchar)resultGreen;
			addPixGreen <<= 16;
			addPixRed 	= (uchar)resultRed;
			addPixRed 	<<= 8;

			//	Create new pixel
			newPixel = addPixBlue | addPixGreen | addPixRed;

			//	Assign new pixel
			*(uint32 *)dstRowPtr = newPixel;

			//	Increment rows and table
			dstRowPtr += dstBPR;
			tablePtr++;
		}

		srcBits += srcIncrement;
		dstBits += dstIncrement;

    }
}

//---------------------------------------------------------------------
//	CreateBicubicTableH
//---------------------------------------------------------------------
//
//	Create filter table for horizontal interpolation
//

void CreateBicubicTableH( const uint32 srcColumns, const uint32 dstColumns, CubicTable *tableH)
{
	float	factorXInv;
	float	xSrcFloat;				// 	Dst column mapped back to source
	float	fraction;				//	Fractional part in source
	float	subPixel;				//	Subpixel position of filter

	//	Get inverse factor
	factorXInv = (float) srcColumns / (float) dstColumns;

	//	Fill up table
	for (int32 index = 0; index < dstColumns; index++)
	{
		xSrcFloat = index * factorXInv;
		fraction = xSrcFloat - (int32) xSrcFloat;

		//	Find the closest subpixel for the fraction
		subPixel = (float) ((int32) (fraction * SPACC + 0.5) / (float)SPACC );

		tableH->srcOffset 		= xSrcFloat;
		tableH->coefficient[0] 	= (int32) ( GetCubicFloat(-subPixel-2)  * SCA);
		tableH->coefficient[1] 	= (int32) ( GetCubicFloat(-subPixel-1)  * SCA);
		tableH->coefficient[2] 	= (int32) ( GetCubicFloat(subPixel)  	* SCA);
		tableH->coefficient[3] 	= (int32) ( GetCubicFloat(1 - subPixel) * SCA);
		tableH->coefficient[4] 	= (int32) ( GetCubicFloat(2 - subPixel) * SCA);

		tableH++;
	}
}


//---------------------------------------------------------------------
//	CreateBicubicTableV
//---------------------------------------------------------------------
//
//	Create filter table for vertical interpolation
//

void CreateBicubicTableV( const uint32 srcRows, const uint32 dstRows, CubicTable *tableV)
{
	float	factorYInv;
	float	ySrcFloat;				// 	Dst row mapped back to source
	float	fraction;				//	Fractional part in source
	float	subPixel;				//	Subpixel position of filter

	factorYInv = (float) srcRows / (float) dstRows;

	//	Fill up table
	for (int32 index = 0; index < dstRows; index++)
	{
		ySrcFloat = index * factorYInv;
		fraction = ySrcFloat - (int32) ySrcFloat;

		//	Find the closest subpixel for the fraction
		subPixel = (float) ((int32) (fraction * SPACC + 0.5) / (float)SPACC );

		tableV->srcOffset 		= ySrcFloat;
		tableV->coefficient[0] 	= (int32) ( GetCubicFloat(-subPixel-2)  * SCA);
		tableV->coefficient[1] 	= (int32) ( GetCubicFloat(-subPixel-1)  * SCA);
		tableV->coefficient[2] 	= (int32) ( GetCubicFloat(subPixel)     * SCA);
		tableV->coefficient[3] 	= (int32) ( GetCubicFloat(1 - subPixel) * SCA);
		tableV->coefficient[4] 	= (int32) ( GetCubicFloat(2 - subPixel) * SCA);

		tableV++;
	}
}


//---------------------------------------------------------------------
//	GetCubicFloat
//---------------------------------------------------------------------
//
//	Routine to generate filter coefficients.  Modify this routine to use
//	other types of filters
//

float GetCubicFloat(float x)
{
	//#define A -1
	const float A = -1;

	if ( x < 0)
		x = -x;

	if (x >= 2)
		return (0);
	else if ( x >= 1)
		return ( A*x*x*x - 5*A*x*x + 8*A*x - 4*A);
	else
		return ( (A+2)*x*x*x - (A+3)*x*x + 1);
}


#pragma mark -
#pragma mark === Rotation Routines ===


//---------------------------------------------------------------------
//	RotateBitmapNN
//---------------------------------------------------------------------
//
//	Rotate using the nearest neighbor algorithm
//

void RotateBitmapNN(BBitmap *srcBitmap, BBitmap *dstBitmap, float degrees, BPoint originPt)
{

    //	Calculate rotation angle
    double	angle 	= ((double) 3.14159265 / (double) 180.0) * (double) degrees;	// convert to RADs

	//	Calculate angle minus 90 degrees
	double	angle2 	= 1.570796327 - angle;

    //	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine	= sin(angle);
	float 	cosf 	= (float)cosine;
	float 	sinf 	= (float)sine;

	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows 	= dstBitmap->Bounds().Height();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	//	Calculate center points
	const int32 centerX = srcColumns / 2;
	const int32 centerY = srcRows / 2;

	const int32 xDiff = (dstColumns - srcColumns) / 2;
	const int32 yDiff = (dstRows - srcRows) / 2;

	//
	//	Inverse map the destination into the source
	//

	float floatX, floatY;
	int32 intX, intY;

	uchar	*dstRowPtr;

	dstRowPtr = dstBits;

	//	Handle all rows in image
	for (int32 y = -1 * yDiff; y < (dstRows - yDiff); y++)
	{
		//	Handle all columns in image
		for( int32 x = -1 * xDiff; x < (dstColumns - xDiff); x++)
		{

			floatX = (x - centerX) * cosine + (y - centerY) * sine;
			floatX += centerX;

			floatY = (y - centerY) * cosine - (x - centerX) * sine;
			floatY += centerY;

			intX = (int32) floatX;
			intY = (int32) floatY;

			//	Clip the source pixel
			if ( (intX >= 0) && (intX < srcColumns) && (intY >= 0) && (intY < srcRows) )
			{
				*(uint32 *)dstRowPtr = *(uint32 *)(srcBits + (intY * srcBPR) + (intX * srcIncrement) );
			}

			//	Increment
			dstRowPtr += dstIncrement;

		}

		dstRowPtr = (dstBits += dstBPR);
	}
}


//---------------------------------------------------------------------
//	RotateBitmapBilinear
//---------------------------------------------------------------------
//
//	Rotate using bilinear interpolation
//

void RotateBitmapBilinear(BBitmap *srcBitmap, BBitmap *dstBitmap, float degrees, BPoint originPt)
{

    //	Calculate rotation angle
    double	angle 	= ((double) 3.14159265 / (double) 180.0) * (double) degrees;	// convert to RADs

	//	Calculate angle minus 90 degrees
	double	angle2 	= 1.570796327 - angle;

    //	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine	= sin(angle);
	float 	cosf 	= (float)cosine;
	float 	sinf 	= (float)sine;

	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows 	= dstBitmap->Bounds().Height();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();

	//	Calculate center points
	const int32 centerX = srcColumns / 2;
	const int32 centerY = srcRows / 2;

	const int32 xDiff = (dstColumns - srcColumns) / 2;
	const int32 yDiff = (dstRows - srcRows) / 2;

	//
	//	Inverse map the destination into the source
	//

	float floatX, floatY;
	int32 intX, intY;

	uchar	*dstRowPtr;

	dstRowPtr = dstBits;

	//	Handle all rows in image
	for (int32 y = -1 * yDiff; y < (dstRows - yDiff); y++)
	{
		//	Handle all columns in image
		for( int32 x = -1 * xDiff; x < (dstColumns - xDiff); x++)
		{

			floatX = (x - centerX) * cosine + (y - centerY) * sine;
			floatX += centerX;

			floatY = (y - centerY) * cosine - (x - centerX) * sine;
			floatY += centerY;

			intX = (int32) floatX;
			intY = (int32) floatY;

			//	Clip the source pixel
			if ( (intX >= 0) && (intX < srcColumns) && (intY >= 0) && (intY < srcRows) )
			{
				*(uint32 *)dstRowPtr = *(uint32 *)(srcBits + (intY * srcBPR) + (intX * srcIncrement) );
			}

			//	Increment
			dstRowPtr += dstIncrement;

		}

		dstRowPtr = (dstBits += dstBPR);
	}
}

#pragma mark -
#pragma mark === Mirroring Routines ===

//---------------------------------------------------------------------
//	MirrorBitmapH
//---------------------------------------------------------------------
//
//	Mirror bitmap horizontaly
//

BBitmap *MirrorBitmapH(BBitmap *srcBitmap, bool center)
{
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Create bitmap to be returned by function.  Calculate new size based on scaled coodinates
	BRect newArea(0, 0, srcColumns, srcRows);
	BBitmap *dstBitmap = new BBitmap(newArea, B_RGB32);

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());

	//	Perform mirror by flipping bits from source to destination.
	//	Source and destination are identical in size and depth, hence
	//	we can use srcRows, srcBPR, etc.
	//

	uchar	*dstRowPtr, *srcRowPtr;

	// 	Traverse through bitmap scanlines
    for( uint32 y = 0; y < srcRows; y++)
	{
		// Set up row pointers
		srcRowPtr = srcBits;
		dstRowPtr = dstBits;

		//	Set dest row to start at end of row and work backwards
		dstRowPtr += srcBPR - srcIncrement;

		// 	Traverse through pixel rows.  Traverse the visible width of the image
		for( uint32 x = 0; x < srcColumns; x++)
	    {
			*(uint32 *)dstRowPtr = *(uint32 *)srcRowPtr;

			//	Increment line pointers
			srcRowPtr += srcIncrement;
			dstRowPtr -= srcIncrement;
	    }

		// Increment pixel rows
		srcBits += srcBPR;
		dstBits += srcBPR;
    }

    return dstBitmap;
}


//---------------------------------------------------------------------
//	MirrorBitmapV
//---------------------------------------------------------------------
//
//	Mirror bitmap vertically
//

BBitmap *MirrorBitmapV(BBitmap *srcBitmap, bool center)
{
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();

	// 	Create bitmap to be returned by function.  Calculate new size based on scaled coodinates
	BRect newArea(0, 0, srcColumns, srcRows);
	BBitmap *dstBitmap = new BBitmap(newArea, B_RGB32);

	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();

	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());

	//	Perform mirror by flipping bits from source to destination.
	//	Source and destination are identical in size and depth, hence
	//	we can use srcRows, srcBPR, etc.
	//

	uchar	*dstRowPtr, *srcRowPtr;

	// 	Traverse through bitmap scanlines
    for( uint32 y = 0; y < srcRows; y++)
	{
		// Set up row pointers
		srcRowPtr = srcBits;

		//	Set dest row to start at last row and work backwards
		dstRowPtr = dstBits + (srcRows - y) * srcBPR;

		// 	Traverse through pixel rows.  Traverse the visible width of the image
		for( uint32 x = 0; x < srcColumns; x++)
	    {
			*(uint32 *)dstRowPtr = *(uint32 *)srcRowPtr;

			//	Increment line pointers
			srcRowPtr += srcIncrement;
			dstRowPtr -= srcIncrement;
	    }

		// Increment pixel row
		srcBits += srcBPR;

    }

    return dstBitmap;
}


#pragma mark -
#pragma mark === Convolution Filter ===

#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	ClipChar
//---------------------------------------------------------------------
//
//	Clip value between 0 to 255.  Return new value
//


int32 ClipChar(int32 theValue)
{
	if (theValue < 0)
		return 0;

	if (theValue > 255)
		return 255;

	return theValue;
}


//---------------------------------------------------------------------
//	sgn
//---------------------------------------------------------------------
//
//

int sgn (int32 a)
{
	if (a > 0)
		return +1;
 	else if (a < 0)
 		return -1;
 	else
 		return 0;
}

//---------------------------------------------------------------------
//	PlotLine
//---------------------------------------------------------------------
//
//	Return point that line drawn from startPt to endPt will
//	be at, at given percentage of completion
//

BPoint PlotLine(BPoint startPt, BPoint endPt, int32 numSteps)
{
	int32 	u, s, v, d1x, d1y, d2x, d2y, m, n;

	u   = endPt.x - startPt.x;      // x2-x1
	v   = endPt.y - startPt.y;      // y2-y1
	d1x = sgn(u);   // d1x is the sign of u (x2-x1) (VALUE -1,0,1)
	d1y = sgn(v);   // d1y is the sign of v (y2-y1) (VALUE -1,0,1)
	d2x = sgn(u);   // d2x is the sign of u (x2-x1) (VALUE -1,0,1)
	d2y = 0;
	m   = abs(u);   // m is the distance between x1 and x2
	n   = abs(v);   // n is the distance between y1 and y2

	// if the x distance is greater than the y distance
	if (m <= n)
	{
		d2x = 0;
		d2y = sgn(v);	// d2y is the sign of v (x2-x1) (VALUE -1,0,1)
		m   = abs(v);	// m is the distance between y1 and y2
		n   = abs(u);	// n is the distance between x1 and x2
	}

	s = (int)(m / 2);	// s is the m distance (either x or y) divided by 2

	// repeat this loop until it
	for ( int32 index = 0; index < m; index++)
	{
		// is = to m (y or x distance)
		//Putpixel(startPt.x, startPt.y, col);	// plot a pixel at the original x1, y1
		s += n;            		// add n (dis of x or y) to s (dis of x of y)

		// if s is >= m (distance between y1 and y2)
		if (s >= m)
		{
			s -= m;
			startPt.x += d1x;
			startPt.y += d1y;
		}
		else
		{
			startPt.x += d2x;
			startPt.y += d2y;
		}
	}

	return startPt;
}
