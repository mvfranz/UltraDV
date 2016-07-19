//-------------------------------------------------------------------
//
//	File:	TVideoSettingsSlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Derivative of TSlider for live updates of video settings
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"
#include "MuseumApp.h"
#include "AppUtils.h"

#include "TMuseumSlider.h"

#include "TVideoSettingsSlider.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Default constructor
//

TVideoSettingsSlider::TVideoSettingsSlider( BRect frame, const char *name, const char *label, BMessage *message, int32 minValue,
                                            int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) :
	TMuseumSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{
	// Set up member variables
	//fParent = parent;

}



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TVideoSettingsSlider::TVideoSettingsSlider(BMessage *data) : TMuseumSlider(data)
{
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TVideoSettingsSlider::~TVideoSettingsSlider()
{
}



//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
//
//

void TVideoSettingsSlider::SetValue(int32 value)
{
	// Pass to parent
	TMuseumSlider::SetValue(value);

	Invoke();

	/*
	   //	Perform update based on VideoSlider label.  This determine what it is...
	   //	Brightness, contrast, hue, saturation

	   if (strcmp("Brightness", Name()) == 0)
	   {
	        fParent->GetParent()->fTempVideoSettings.fVideoImageSettings.fBrightnessValue = theMessage->FindInt32("be:value");
	        fVideoSource->VideoControls()->SetBrightness(theMessage->FindInt32("be:value"));
	   }
	   else if (strcmp("Contrast", Name()) == 0)
	   {
	   fillColor.green = Value();
	   viewColor.green = Value();
	   }
	   else if (strcmp("Hue", Name()) == 0)
	   {
	   fillColor.blue = Value();
	   viewColor.blue = Value();
	   }
	   else if (strcmp("Saturation", Name()) == 0)
	   {
	   fillColor.blue = Value();
	   viewColor.blue = Value();
	   }
	 */
}


/*
   case BRIGHTNESS_SLIDER_MSG:
                break;

        case CONTRAST_SLIDER_MSG:
                GetParent()->fTempVideoSettings.fVideoImageSettings.fContrastValue = theMessage->FindInt32("be:value");
                fVideoSource->VideoControls()->SetContrast(theMessage->FindInt32("be:value"));
                break;

        case HUE_SLIDER_MSG:
                GetParent()->fTempVideoSettings.fVideoImageSettings.fHueValue = theMessage->FindInt32("be:value");
                fVideoSource->VideoControls()->SetHue(theMessage->FindInt32("be:value"));
                break;

        case SATURATION_SLIDER_MSG:
                GetParent()->fTempVideoSettings.fVideoImageSettings.fSaturationValue = theMessage->FindInt32("be:value");
                fVideoSource->VideoControls()->SetSaturation(theMessage->FindInt32("be:value"));
                break;
 */