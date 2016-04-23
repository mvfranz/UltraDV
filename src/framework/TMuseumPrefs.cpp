//---------------------------------------------------------------------
//
//	File:	TMuseumPrefs.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Object that handles all application preferences
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>
#include <TimeCode.h>

#include "AppConstants.h"
#include "AppTypes.h"

#include "TMuseumPrefs.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMuseumPrefs::TMuseumPrefs()
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMuseumPrefs::~TMuseumPrefs()
{
}

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMuseumPrefs::Init()
{
	m_Timescale = B_TIMECODE_24;
}
