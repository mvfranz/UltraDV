//---------------------------------------------------------------------
//
//	File:	DebugUtils.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	Debugging Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "DebugUtils.h"



//---------------------------------------------------------------------
//	DebugAlert
//---------------------------------------------------------------------
//
//

void DebugAlert(char *theString)
{
	BAlert *theAlert = new BAlert("Debug", theString, "Thanks");
	theAlert->Go();
}


//---------------------------------------------------------------------
//	DumpRect
//---------------------------------------------------------------------
//
// Dump coordinates of rect to stdout
//

void DumpRect(BRect theRect)
{
	#ifdef DEBUG
		printf("theRect = %f, %f, %f, %f\n", theRect.left, theRect.top, theRect.right, theRect.bottom  );
	#endif
}


//---------------------------------------------------------------------
//	DumpPoint
//---------------------------------------------------------------------
//
// Dump coordinates of point to stdout
//

void DumpPoint(BPoint thePoint)
{
	#ifdef DEBUG
		printf("thePoint = %f, %f\n", thePoint.x, thePoint.y);
	#endif
}
