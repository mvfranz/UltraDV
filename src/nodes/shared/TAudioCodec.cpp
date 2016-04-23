//---------------------------------------------------------------------
//
//	File:	TAudioCodec.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.22.98
//
//	Desc:	Audio CODEC Object
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TRIFFReader.h"

#include "TAudioCodec.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TAudioCodec::TAudioCodec( TRIFFReader *reader)
{
	m_Reader = reader;
	
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TAudioCodec::~TAudioCodec()
{

}

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TAudioCodec::Init()
{
}


//-------------------------------------------------------------------
//	GetDescription
//-------------------------------------------------------------------
//
//	Return text description of codec
//

bool TAudioCodec::GetDescription(char *descText)
{
	sprintf(descText, "Basic Audio CODEC");
	
	return true;
}

#pragma mark -
#pragma mark === CODEC Utilities ===

