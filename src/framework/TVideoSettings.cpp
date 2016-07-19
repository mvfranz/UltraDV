//---------------------------------------------------------------------
//
//	File:	TVideoSettings.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.14.98
//
//	Desc:	Video Setting Class.  Stores video settings.
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "TVideoSettings.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoSettings::TVideoSettings()
{			
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoSettings::~TVideoSettings()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Setup defaults
//

void TVideoSettings::Init()
{	
	// Compression
	fVideoCompressionSettings.fCompressor = kNone;

	// Image
	fVideoImageSettings.fHueValue 		= 0.5;
	fVideoImageSettings.fSaturationValue	= 0.5;
	fVideoImageSettings.fBrightnessValue 	= 0.5;
	fVideoImageSettings.fContrastValue 	= 0.5;

	// Source
	fVideoSourceSettings.fDigitizer	= kTurboTVPhilips;
	fVideoSourceSettings.fInput		= kComposite;
	fVideoSourceSettings.fFormat		= kNTSC_M;
	
	fVideoSourceSettings.fFilters.fGammaCorrection	= true;
	fVideoSourceSettings.fFilters.fLumaCoring		= true;
	fVideoSourceSettings.fFilters.fErrorDiffusion	= true;
	fVideoSourceSettings.fFilters.fLumaComb			= true;
	fVideoSourceSettings.fFilters.fChromaComb		= true;
}