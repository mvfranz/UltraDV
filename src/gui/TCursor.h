//---------------------------------------------------------------------
//
//	File:	TCursor.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Basic cursor class
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCURSOR_H__
#define __TCURSOR_H__


// Cursors Definition
typedef struct CursorData 
{
	uint16 data[16];
} CursorData;

typedef struct Cursor 
{
	uint8 		fImageSize;	// size of bitmap
	uint8 		fImageDepth;	// bit depth of image
	uint8 		fHotSpotX;		// x location of hotspot
	uint8 		fHotSpotY;		// y location of hotspot
	CursorData 	fImage;		// image data	
	CursorData 	fMask;			// image mask	
} Cursor;		
																
// Class Definition
class TCursor
{
	
	
	public:
		TCursor(int16 cursor);
		~TCursor();
		
	protected:
				
		uint8 	fImageSize;	// size of bitmap
		uint8 	fImageDepth;	// bit depth of image
		uint8 	fHotSpotX;		// x location of hotspot
		uint8 	fHotSpotY;		// y location of hotspot
		Cursor 	fImage;		// image data	
		Cursor 	fMask;			// image mask	
		
};

#endif
