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
BBitmap 		*GetCicnFromResource(const char *theResource);
BBitmap 		*GetIcon8FromResource(const char *theResource);
BBitmap 		*GetIcon16FromResource(const char *theResource);
BBitmap 		*GetIcon32FromResource(const char *theResource);

BBitmap 		*GetMICNFromResource(const char *theResource);
BBitmap 		*GetICONFromResource(const char *theResource);

BMessage 		*GetWindowFromResource(const char *theResource);
BMessage 		*GetMenuFromResource(const char *theResource);
bool 			GetCursorFromResource(const char *theResource, Cursor *theCursor);

TMuseumIcons 	*GetAppIcons();

bool 			GetString(const char *theResource, int32 id, char *theStr);

// Resource Structures

typedef struct // cicn ABH
{
	int16	width;
	int16	height;
	void 	*data;	
} cicn;

#endif
