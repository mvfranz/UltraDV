//---------------------------------------------------------------------
//
//	File:	AppAlerts.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
//	Desc:	Alert Handling Routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __APPALERTS_H__
#define __APPALERTS_H__

// Function protoypes
__declspec(dllexport) void 	CueBoundsAlert();
__declspec(dllexport) void 	UnhandledImageFormat(entry_ref *ref);
__declspec(dllexport) void 	UnhandledMIDIFormat(entry_ref *ref);
__declspec(dllexport) int32 SaveAlert();
__declspec(dllexport) int32	ErrorAlert(char *theError);


#endif
