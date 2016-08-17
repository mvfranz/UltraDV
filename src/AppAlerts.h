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
void 	CueBoundsAlert();
void 	UnhandledImageFormat(entry_ref *ref);
void 	UnhandledMIDIFormat(entry_ref *ref);
int32 SaveAlert();
int32	ErrorAlert(char *theError);


#endif
