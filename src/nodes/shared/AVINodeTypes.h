//---------------------------------------------------------------------
//
//	File:	AVINodeTypes.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	Definition of AVIProducer types
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __AVINODETYPES_H__
#define __AVINODETYPES_H__

//	AVIVideoFrame
typedef struct 
{ 
	uint32	ChunkID;
	uint32 	Offset;      // Position of chunk in file
	uint32 	Length;      // Length of chunk in bytes
} AVIVideoFrame; 


//	AVIAudioFrame
typedef struct 
{ 
	uint32	ChunkID;
	uint32 	Offset;      // Position of chunk in file
	uint32 	Length;      // Length of chunk in bytes
} AVIAudioFrame; 

#endif
