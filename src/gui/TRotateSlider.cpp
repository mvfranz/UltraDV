//-------------------------------------------------------------------
//
//	File:	TRotateSlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Derivative of TSlider for live updates of stage cue rotation
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
#include "TRotateDialog.h"

#include "TRotateSlider.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Default constructor
//

TRotateSlider::TRotateSlider( TRotateDialog* dialog, TNumberTextControl* rotateText, TVisualCue* theCue, BRect frame, const char* name, const char* label, BMessage* message, int32 minValue,
                              int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) :
	TSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{
	// Set up member variables
	fDialog                 = dialog;
	fTextControl    = rotateText;
	fCue                    = theCue;
	fStage                  = static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetStage();
	fStageView      = fStage->GetStageView();
}



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TRotateSlider::TRotateSlider(BMessage* data) : TSlider(data)
{
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TRotateSlider::~TRotateSlider()
{
}



//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
//
//

void TRotateSlider::SetValue(int32 value)
{
	// Pass to parent
	TSlider::SetValue(value);

	//	Update cue
	fCue->SetRotation(value);

	//	Update text
	if (fTextControl != NULL) {
		char newStr[3];
		sprintf(newStr, "%d", value);
		fTextControl->SetText(newStr);
	}

	// Update cue
	if (fCue != NULL) {
		//	Draw transformed bitmap
		if ( fDialog->GetRealtimePreview() == true ) {
			if (fCue->IsOnStage()) {
				fStage->Lock();
				fStageView->UpdateStage( fCue->GetDrawArea(), GetCurrentTime());
				fStageView->Invalidate( fCue->GetDrawArea());
				fStage->Unlock();
			}
		}
		//	Draw bounding box
		else{
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

void TRotateSlider::SetTextControl(TNumberTextControl* textControl)
{
	fTextControl = textControl;
}

