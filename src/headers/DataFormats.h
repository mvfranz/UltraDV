//	DataFormats.h

#pragma once


#ifndef _DATAFORMATS_H_
#define _DATAFORMATS_H_


#include <SupportDefs.h>

//	extensions used in the extension BMessage. Use of these 
//	are described in the documentation (MANUAL.txt)
//
#define kCommentExtension		"/comment"
#define kTimeExtension			"/time"
#define kFrameExtension			"/frame"
#define kBitsRectExtension		"bits/Rect"
#define kHeaderExtension		"/headerOnly"
#define kDataExtension			"/dataOnly"
#define kNoisChannelExtension	"nois/channel"
#define kNoisMonoExtension		"nois/mono"
#define kNoisMarkerExtension	"nois/marker"
#define kNoisLoopExtension		"nois/loop"


//	for each data group, there is a "standard" format that
//	all translators should support.
//
enum DATAGroups {
	DATA_BITMAP			=	'bits',		//	DATA_BITMAP
	DATA_PICTURE		=	'pict',		//	BPicture data
	DATA_TEXT			=	'TEXT',		//	B_ASCII_TYPE
	DATA_SOUND			=	'nois',		//	DATA_SOUND
	DATA_MIDI			=	'midi',		//	standard MIDI
	DATA_MEDIA			=	'mhi!',		//	a stream of stuff
	DATA_EXECUTABLE		=	'BAPP',		//	PEF container
	DATA_NONE			=	'none'
};
#define kAnyType 0


//	This is the standard bitmap format
struct DATABitmap {
	uint32		magic;		//	DATA_BITMAP
	BRect		bounds;
	uint32		rowBytes;
	color_space	colors;
	uint32		dataSize;
};	//	data follows, padded to rowBytes

//	This is the standard sound format
struct DATASound {
	uint32		magic;		//	DATA_SOUND
	uint32		channels;	//	Left should always be first
	float		sampleFreq;	//	16 bit linear is assumed
	uint32		numFrames;
};	//	data follows

//	for DATA_MEDIA stuff (use for streaming video and/or audio and/or 
//	other things) please see the header file DataMedia.h

#endif // _DATAFORMATS_H_

