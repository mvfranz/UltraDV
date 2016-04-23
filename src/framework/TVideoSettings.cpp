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
	m_VideoCompressionSettings.m_Compressor = kNone;

	// Image
	m_VideoImageSettings.m_HueValue 		= 0.5;
	m_VideoImageSettings.m_SaturationValue	= 0.5;
	m_VideoImageSettings.m_BrightnessValue 	= 0.5;
	m_VideoImageSettings.m_ContrastValue 	= 0.5;

	// Source
	m_VideoSourceSettings.m_Digitizer	= kTurboTVPhilips;
	m_VideoSourceSettings.m_Input		= kComposite;
	m_VideoSourceSettings.m_Format		= kNTSC_M;
	
	m_VideoSourceSettings.m_Filters.m_GammaCorrection	= true;
	m_VideoSourceSettings.m_Filters.m_LumaCoring		= true;
	m_VideoSourceSettings.m_Filters.m_ErrorDiffusion	= true;
	m_VideoSourceSettings.m_Filters.m_LumaComb			= true;
	m_VideoSourceSettings.m_Filters.m_ChromaComb		= true;
}