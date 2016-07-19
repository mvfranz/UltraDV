//-------------------------------------------------------------------
//
//	File:	TOpacitySlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Derivative of TSlider for live updates of stage cue opacity
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"
#include "MuseumApp.h"
#include "AppUtils.h"

#include "TNumberTextControl.h"
#include "TVisualCue.h"

#include "TCueSheetWindow.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TOpacityDialog.h"

#include "TOpacitySlider.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Default constructor
//

TOpacitySlider::TOpacitySlider( TOpacityDialog *dialog, TNumberTextControl *rotateText, TVisualCue *theCue, BRect frame, const char *name, const char *label, BMessage *message, int32 minValue,
				  int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) :
				  TSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{
	// Set up member variables
	fDialog 		= dialog;
	fTextControl 	= rotateText;
	fCue 			= theCue;
	fStage			= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
	fStageView 	= fStage->GetStageView();

}



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TOpacitySlider::TOpacitySlider(BMessage *data) : TSlider(data)
{
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TOpacitySlider::~TOpacitySlider()
{
}



//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
//
//

void TOpacitySlider::SetValue(int32 value)
{
	// Pass to parent
	TSlider::SetValue(value);

	//	Get new value
	float sliderVal = ((float)(value)) / 100;

	//	Update cue
	fCue->SetTransparency(sliderVal);

	//	Update text
	if (fTextControl != NULL)
	{
		char newStr[3];
		sprintf(newStr, "%d", value);
		fTextControl->SetText(newStr);
	}

	// Update cue
	if (fCue != NULL)
	{
		//	Draw transformed bitmap
		if ( fDialog->GetRealtimePreview() == true )
		{
			if (fCue->IsOnStage())
			{
				fStage->Lock();
				fStageView->StageDraw( fCue->GetDrawArea(), GetCurrentTime());
				fStageView->Draw( fCue->GetDrawArea());
				fStage->Unlock();
			}
		}
		//	Draw bounding box
		else
		{
			//fCue->DrawSelectionRect
		}
	}
}


//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
//
//	Set TNumberTextControl member variable
//

void TOpacitySlider::SetTextControl(TNumberTextControl *textControl)
{
	fTextControl = textControl;
}

