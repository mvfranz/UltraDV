//---------------------------------------------------------------------
//
//	File:	FileIO.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.25.98
//
//	Desc:	File input and output routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

// Includes
#include <app/Application.h>
#include <assert.h>

#include "AppTypes.h"

#include "FileIO.h"
#include "MuseumApp.h" // ABH

//-------------------------------------------------------------------
//	OpenCueSheet
//-------------------------------------------------------------------
//
//	Load cue sheet from file.  Return true id successful
//

bool OpenCueSheet()
{
	// Display standard file open panel
	((MuseumApp *)be_app)->ShowFileOpenPanel(); // ABH added ::

	return true;
}





