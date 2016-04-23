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
	uint8 		m_ImageSize;	// size of bitmap
	uint8 		m_ImageDepth;	// bit depth of image
	uint8 		m_HotSpotX;		// x location of hotspot
	uint8 		m_HotSpotY;		// y location of hotspot
	CursorData 	m_Image;		// image data	
	CursorData 	m_Mask;			// image mask	
} Cursor;		
																
// Class Definition
class TCursor
{
	
	
	public:
		TCursor(int16 cursor);
		~TCursor();
		
	protected:
				
		uint8 	m_ImageSize;	// size of bitmap
		uint8 	m_ImageDepth;	// bit depth of image
		uint8 	m_HotSpotX;		// x location of hotspot
		uint8 	m_HotSpotY;		// y location of hotspot
		Cursor 	m_Image;		// image data	
		Cursor 	m_Mask;			// image mask	
		
};

#endif
