//---------------------------------------------------------------------
//
//	File:	TAudioEngine.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.18.98
//
//	Desc:	Sound playback engine
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>
#include <ByteOrder.h>

#include "TAudioEngine.h"

// Constants
const int32 RATE_SCALE = 1.0;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioEngine::TAudioEngine() : BSoundPlayer("AudioEngine")
{	
	//	Start Engine
	Start();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioEngine::~TAudioEngine()
{
	//	Stop playback off all sounds
	
	//	Stop node
	Stop();		
}
