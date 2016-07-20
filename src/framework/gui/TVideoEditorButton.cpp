//---------------------------------------------------------------------
//
//	File:	TVideoEditorButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//	Desc:	Video Editor Bitmap button object
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
#include "MuseumApp.h"

#include "TVideoEditorButton.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//
// TODO: eliminate this class and just create a TBitmapButton (mo 8/11/98)

TVideoEditorButton::TVideoEditorButton(BRect bounds, const char* name,
                                       BBitmap* offBitmap, BBitmap* onBitmap, BHandler* handler,
                                       BMessage* message) :
	TBitmapButton(bounds, name, offBitmap, onBitmap, handler, message)
{
}
