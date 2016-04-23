//---------------------------------------------------------------------
//
//	File:	AppTypes.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.25.98
//
//	Desc:	Application defined types
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __APPTYPES_H__
#define __APPTYPES_H__

#if __GNUC__ > 2
#ifndef dllexport
#define dllexport
#endif
#endif

// Current file version
#define kCurrentFileVersion 0x0100

//
// ---- Chunk Definitions
//

//
// Chunk header -- same as AIFF and MIDI files
//

typedef struct 
{
	int32	chunkID;			// 4 character code
	int32	chunkSize;			// length of data following the header
} ChunkHeader;


//
// Chunk ID of the first chunk in file, the container chunk for the whole file
//

//#define kCueSheetDocChunkIntelID 	0x4A4A4A4A
//#define kCueSheetDocChunkMotorolaID	0x4C4C4C4C
const int32 kCueSheetDocChunkIntelID	= 0x4A4A4A4A;
const int32 kCueSheetDocChunkMotorolaID = 0x4C4C4C4C;

typedef struct 
{
	ChunkHeader	header;		// 	Length in chunk header is length of entire cue 
	int32		chunkSize;	// 	Sheet file chunk - sizeof(ChunkHeader)
	int16		version;	//	Version number of file	
} CueSheetHeaderChunk;

//	Cue Display types
typedef enum
{
	kWireframeQuality,
	kPreviewQuality,
	kBetterQuality,
	kBestQuality

} DisplayQuality;

//	Define current edit mode of keyframes
typedef enum
{
	kKeyframeRotation,
	kKeyframeCrop,
	kKeyframeScale,
	kKeyframeShear,
	kKeyframeSkew,
	kKeyframePerpsective,
	kKeyframeMirror,
	kKeyframeOpacity
} KeyframeMode;

#endif
