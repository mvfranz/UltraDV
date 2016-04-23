//---------------------------------------------------------------------
//
//	File:	AVIUtils.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	Application Utilities
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#if !defined(__AVIUTILS_H__)
#define __AVIUTILS_H__

#include "RIFFConstants.h"
#include "RIFFTypes.h"

//	Byte swapping Utilities
int32 	ReadIntMsb(BFile *in, int bytes);
int32 	BytesToIntMsb(void *buff, int bytes);
int32 	ReadIntLsb(BFile *in, int bytes);
int32 	BytesToIntLsb(void *buff, int bytes);
void 	SkipBytes(BFile *in, int bytes);
void	WriteIntMsb(BFile *out, int32 l, int bytes);
ssize_t WriteAtIntMsb(BFile *out, int32 value, int size);
void 	WriteIntLsb(BFile *out, int32 l, int bytes);
ssize_t WriteAtIntLsb(BFile *out, int32 value, int size);

//	RIFF Utilities
void	DebugAlert(char *theString);
void	DumpAVIHeader(AVIHeader *theHeader);
void 	DumpAVIStreamHeader(AVIStreamHeader *theHeader);
void 	DumpVIDSHeader(AVIVIDSHeader *theHeader);
void 	DumpAUDSHeader(AVIAUDSHeader *theHeader);
void 	DumpRIFFID(int32 theID);
void 	DumpAVIHeaderFlags(AVIHeader *theHeader);
void 	DumpAudioType(uint16 type);

#endif
