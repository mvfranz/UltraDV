//---------------------------------------------------------------------
//
//	File:	TMuseumSlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Custom BSlider
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "TMuseumSlider.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMuseumSlider::TMuseumSlider( BRect frame, const char* name, const char* label, BMessage* message, int32 minValue,
                              int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) :
	BSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{
}


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TMuseumSlider::TMuseumSlider(BMessage* data) : BSlider(data)
{
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TMuseumSlider::~TMuseumSlider()
{
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	DrawText
//---------------------------------------------------------------------
//
//	Draw text in custom location
//

void TMuseumSlider::DrawText()
{
	if (Looper()->Lock() ) {
		Looper()->Unlock();
	}
}
