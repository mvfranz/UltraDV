//---------------------------------------------------------------------
//
//	File:	TCueSheetPrefs.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.15.98
//
//	Desc:	Object that handles all cue sheet preferences
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "TCueSheetPrefs.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueSheetPrefs::TCueSheetPrefs()
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetPrefs::~TCueSheetPrefs()
{
}

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TCueSheetPrefs::Init()
{
	m_Timescale = B_TIMECODE_24;
}
