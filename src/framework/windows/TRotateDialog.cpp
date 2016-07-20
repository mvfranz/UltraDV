//-------------------------------------------------------------------
//
//	File:	TRotateDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.22.98
//
//	Desc:	Dialog for modifying cue rotation angle
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <Application.h>
#include <StringView.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TCueSheetView.h"
#include "TNumberTextControl.h"
#include "TCueView.h"
#include "TVisualCue.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TRotateSlider.h"

#include "TRotateDialog.h"

//	Constants
const BRect kRotateDialogBounds(0, 0, 315, 125);

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TRotateDialog::TRotateDialog(TVisualCue* theCue) : BWindow( kRotateDialogBounds, "Rotation", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE | B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE, 0)
{
	//	Save reference to PictureCue
	fCue = theCue;

	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TRotateDialog::~TRotateDialog()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TRotateDialog::Init()
{
	//	Set realtime preview to false while initing
	fRealtimePreview = false;

	//	Get current cue rotation
	fLastRotation = fSavedRotation = fCue->GetRotation();

	//	Set real time preview to true
	fRealtimePreview = true;

	//
	// Create dialog elements
	//

	const BRect bounds = Bounds();

	// Create background view
	fBackView = new BView(bounds, "RotationDialogView", B_FOLLOW_ALL, B_WILL_DRAW);
	fBackView->SetViewColor(kGrey);
	AddChild(fBackView);


	// Create Slider
	BRect sliderBounds;

	sliderBounds.left       = bounds.left + 10;
	sliderBounds.top        = bounds.top + 10;
	sliderBounds.right      = sliderBounds.left + 255;
	sliderBounds.bottom = sliderBounds.top + 55;

	fSlider = new TRotateSlider(this, NULL, fCue, sliderBounds, "RotationSlider", "Rotation", new BMessage(ROTATE_MSG), 0, 359, B_TRIANGLE_THUMB);
	fBackView->AddChild(fSlider);
	fSlider->SetLimitLabels("0°", "359°");
	fSlider->SetHashMarks(B_HASH_MARKS_TOP);
	fSlider->SetHashMarkCount(10);
	fSlider->SetBarColor(kHeaderGrey);
	fSlider->UseFillColor(true, &kMediumSteelBlue);

	//	Set slider value
	int32 sliderVal = fSavedRotation;
	fSlider->SetValue(sliderVal);

	//	Percentage Text Control
	BRect textBounds;
	textBounds.left         = fSlider->Frame().right + 5;
	textBounds.top          = bounds.top + 10 + ((55/2) - (25/2));
	textBounds.right        = textBounds.left + 30;
	textBounds.bottom       = textBounds.top +19;
	fRotateText = new TNumberTextControl(textBounds, NULL, "RotationText",  "0", new BMessage(ROTATE_TEXT_MSG));
	fBackView->AddChild(fRotateText);

	// Set up max number of characters
	fRotateText->TextView()->SetMaxBytes(3);

	//	Set text value
	char numStr[4];
	sprintf(numStr, "%d", sliderVal);
	fRotateText->SetText(numStr);


	//	Realtime Preview Checkbox
	BRect checkRect;
	checkRect.left          = fSlider->Frame().left;
	checkRect.right         = checkRect.left + 100;
	checkRect.top           = fSlider->Frame().bottom + 5;
	checkRect.bottom        = checkRect.top + 15;

	fUpdateCheckbox = new BCheckBox( checkRect, "RealtimeUpdate", "Realtime Update", new BMessage(ROTATE_UPDATE_MSG));
	fBackView->AddChild(fUpdateCheckbox);
	fUpdateCheckbox->SetValue(fRealtimePreview);

	//
	// Cancel, Apply and OK buttons
	//

	// Create the Cancel button
	BRect cancelFrame = Bounds();
	cancelFrame.left        += 35;
	cancelFrame.right       = cancelFrame.left + 60;
	cancelFrame.bottom  -= 15;
	cancelFrame.top         = cancelFrame.bottom - 20;
	fCancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fBackView->AddChild(fCancelButton);

	// Create the Apply button
	BRect applyFrame = Bounds();
	applyFrame.left         = (Bounds().Width() /2) - (60 / 2);
	applyFrame.right        = applyFrame.left + 60;
	applyFrame.bottom       -= 15;
	applyFrame.top          = applyFrame.bottom - 20;
	fApplyButton = new BButton(applyFrame, "Apply", "Apply", new BMessage(APPLY_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fBackView->AddChild(fApplyButton);

	// Create the OK button
	BRect okFrame = Bounds();
	okFrame.right   -= 35;
	okFrame.left    =  okFrame.right - 60;
	okFrame.bottom  -= 15;
	okFrame.top     = okFrame.bottom - 20;
	fOKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fBackView->AddChild(fOKButton);
	fOKButton->MakeDefault(true);

	//  Update slider with text control
	fSlider->SetTextControl(fRotateText);

	//	Set realtime preview to true
	fRealtimePreview = true;
	fUpdateCheckbox->SetValue(fRealtimePreview);

}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	MessageReceived
//-------------------------------------------------------------------
//
//

void TRotateDialog::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
	//	Result of slider operation.  Inform cue of new setting
	case ROTATE_MSG:
	{
		//	Get new value
		int32 sliderVal = fSlider->Value();

		//	Get text value
		const char* numStr = fRotateText->Text();
		int16 theVal = atoi(numStr);

		//	Update text if needed
		if (theVal != fSlider->Value()) {
			char newStr[3];
			sprintf(newStr, "%d", fSlider->Value());
			fRotateText->SetText(newStr);
		}
	}
	break;

	//	We have been rotated by the stage tool.  Update controls
	case ROTATE_TOOL_MSG:
	{
		int32 rotation;

		message->FindInt32("RotateDegrees", &rotation);

		//	Update text control and adjust slider
		if (fSlider->Value() != rotation) {
			fSlider->SetValue(rotation);
			fSlider->Invoke();
		}
	}
	break;


	//	The user has modified the opacity text
	case ROTATE_TEXT_MSG:
	{
		//	Get text value
		const char* numStr = fRotateText->Text();
		int16 theVal = atoi(numStr);

		//	Clip number to acceptable range
		if (theVal > 359)
			theVal = 359;

		//	Update text control and adjust slider
		if (fSlider->Value() != theVal) {
			fSlider->SetValue(theVal);
			fSlider->Invoke();
		}
	}
	break;

	//	Set state of Realtime Updating
	case ROTATE_UPDATE_MSG:
	{
		fRealtimePreview = fUpdateCheckbox->Value();

		//	Update stage if neccessary
		if (fRealtimePreview)
			UpdateCue();
	}
	break;

	//	Apply changes to cue
	case APPLY_MSG:
	{
		// Get new cue opacity
		float rotation =  fSlider->Value();

		//	Determine if cue opacity needs to be updated
		if (rotation != fLastRotation) {
			fLastRotation = rotation;

			fCue->SetRotation(rotation);

			// Redraw if necessary
			UpdateCue();
		}
	}
	break;

	// User pressed OK button.
	case OK_MSG:
	{
		// Get new cue opacity
		float rotation =  fSlider->Value();

		Hide();

		//	Determine if cue opacity needs to be updated
		if (rotation != fSavedRotation) {
			fCue->SetRotation(rotation);

			// Redraw if necessary
			UpdateCue();
		}

		//	Inform cue we are gone
		BMessage* theMessage = new BMessage(ROTATE_CLOSE_MSG);
		fCue->MessageReceived(theMessage);
		delete theMessage;

		// Close the dialog
		Lock();
		Quit();
	}
	break;

	// User has canceled the dialog
	case CANCEL_MSG:
	{
		//	Get current opacity
		float currentRotation = fCue->GetRotation();

		Hide();

		//	Restore to saved opacity
		if (currentRotation != fSavedRotation) {
			fCue->SetRotation(fSavedRotation);

			// Inform stage to redraw
			UpdateCue();
		}

		//	Inform cue we are gone
		BMessage* theMessage = new BMessage(ROTATE_CLOSE_MSG);
		fCue->MessageReceived(theMessage);
		delete theMessage;

		//	Close dialog
		Lock();
		Quit();

	}
	break;

	default:
		BWindow::MessageReceived(message);
		break;

	}

}


#pragma mark -
#pragma mark === Utility Routines ===

//-------------------------------------------------------------------
//	UpdateCue
//-------------------------------------------------------------------
//
//	Force a redraw to the stage
//

void TRotateDialog::UpdateCue()
{
	if (fCue->IsOnStage()) {
		TStageWindow* theStage  = static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetStage();
		TStageView* stageView = theStage->GetStageView();

		theStage->Lock();
		stageView->StageDraw( fCue->GetDrawArea(), GetCurrentTime());
		stageView->Draw( fCue->GetDrawArea());
		theStage->Unlock();
	}
}