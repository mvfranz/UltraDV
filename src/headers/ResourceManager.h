//---------------------------------------------------------------------
//
//	File:	ResourceManager.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.25.98
//
//	Desc:	Routines for resource management
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "TCursor.h"
#include "TMuseumIcons.h"

// Resource Handling
__declspec(dllexport) BBitmap 		*GetCicnFromResource(const char *theResource);
__declspec(dllexport) BBitmap 		*GetIcon8FromResource(const char *theResource);
__declspec(dllexport) BBitmap 		*GetIcon16FromResource(const char *theResource);
__declspec(dllexport) BBitmap 		*GetIcon32FromResource(const char *theResource);

__declspec(dllexport) BBitmap 		*GetMICNFromResource(const char *theResource);
__declspec(dllexport) BBitmap 		*GetICONFromResource(const char *theResource);

__declspec(dllexport) BMessage 		*GetWindowFromResource(const char *theResource);
__declspec(dllexport) BMessage 		*GetMenuFromResource(const char *theResource);
__declspec(dllexport) bool 			GetCursorFromResource(const char *theResource, Cursor *theCursor);

__declspec(dllexport) TMuseumIcons 	*GetAppIcons();

__declspec(dllexport) bool 			GetString(const char *theResource, int32 id, char *theStr);

// Resource Structures

typedef struct // cicn ABH
{
	int16	width;
	int16	height;
	void 	*data;	
} cicn;

#endif
