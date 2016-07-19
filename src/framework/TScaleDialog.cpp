//-------------------------------------------------------------------
//
//	File:	TScaleDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.21.98
//
//	Desc:	Dialog for modifying cue opacity
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

#include "TScaleDialog.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TScaleDialog::TScaleDialog(TVisualCue *theCue, BMessage *theMessage) : BWindow(theMessage)
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

TScaleDialog::~TScaleDialog()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TScaleDialog::Init()
{
	//	Get Aspect ratio
	BRect aspectRect = fCue->GetArea();

	fAspectWidth 	= aspectRect.Width()  / aspectRect.Height();
	fAspectHeight	= aspectRect.Height() / aspectRect.Width();

	//	Get current dimensions
	fSavedScale = fCue->GetScale();

	//	Maintain aspect ratio
	fAspectRatio = true;

	//	Set units to pixels
	fWidthUnitType  = kUnitsPixels;
	fHeightUnitType = kUnitsPixels;

	//
	// Get dialog items
	//

	fBackView = (BView *)FindView("ScaleView");

	//	Get TextControls and swap them out for our own
	//

	//	Width
	BTextControl *widthControl	= (BTextControl *)FindView("WidthTextControl");
	fWidthText = new TNumberTextControl( widthControl->Frame(), widthControl->Label(), widthControl->Name(),
									  NULL, new BMessage(SCALE_WIDTH_TEXT_MSG));
	fWidthText->SetDivider(widthControl->Divider());
	fWidthText->TextView()->SetMaxBytes(4);
	fBackView->RemoveChild(widthControl);
	fBackView->AddChild(fWidthText);
	delete widthControl;

	//	Height
	BTextControl *heightControl	= (BTextControl *)FindView("HeightTextControl");
	fHeightText = new TNumberTextControl( heightControl->Frame(), heightControl->Label(), heightControl->Name(),
									  NULL, new BMessage(SCALE_HEIGHT_TEXT_MSG));
	fHeightText->SetDivider(heightControl->Divider());
	fHeightText->TextView()->SetMaxBytes(4);
	fBackView->RemoveChild(heightControl);
	fBackView->AddChild(fHeightText);
	delete heightControl;

	//
	// Setup menus
	//

	BMenu *theMenu;

	fWidthMenu	 = (BMenuField *)FindView("WidthMenu");
	fHeightMenu = (BMenuField *)FindView("HeightMenu");

	//	Width Menu
	theMenu = fWidthMenu->Menu();
	if (theMenu)
	{
		// Pixels
		BMenuItem *pixelsItem = new BMenuItem("Pixels", new BMessage(SCALE_W_PIXELS_MSG));
		theMenu->AddItem(pixelsItem);

		//	Inches
		BMenuItem *inchesItem = new BMenuItem("Inches", new BMessage(SCALE_W_INCHES_MSG));
		theMenu->AddItem(inchesItem);

		// Percent
		BMenuItem *percentItem = new BMenuItem("Percent", new BMessage(SCALE_W_PERCENT_MSG));
		theMenu->AddItem(percentItem);

		// Set the target
		theMenu->SetTargetForItems(this);

		// Select the first item
		pixelsItem->SetMarked(true);
	}

	//	Height Menu
	theMenu = fHeightMenu->Menu();
	if (theMenu)
	{
		// Pixels
		BMenuItem *pixelsItem = new BMenuItem("Pixels", new BMessage(SCALE_H_PIXELS_MSG));
		theMenu->AddItem(pixelsItem);

		//	Inches
		BMenuItem *inchesItem = new BMenuItem("Inches", new BMessage(SCALE_H_INCHES_MSG));
		theMenu->AddItem(inchesItem);

		// Percent
		BMenuItem *percentItem = new BMenuItem("Percent", new BMessage(SCALE_H_PERCENT_MSG));
		theMenu->AddItem(percentItem);

		// Set the target
		theMenu->SetTargetForItems(this);

		// Select the first item
		pixelsItem->SetMarked(true);
	}

	//	Get checkboxes
	fAspectCheck 	= (BCheckBox *)FindView("AspectCheck");

	//	Get buttons
	fOKButton 		= (BButton *)FindView("OKButton");
	fApplyButton	= (BButton *)FindView("ApplyButton");
	fCancelButton	= (BButton *)FindView("CancelButton");

	//
	//	Set controls
	//

	//	Width and Height
	BRect drawArea = fCue->GetScaledArea();

	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerWidth() + 1);
	fWidthText->SetText(numText);

	sprintf(numText, "%d", drawArea.IntegerHeight() + 1);
	fHeightText->SetText(numText);

}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	MessageReceived
//-------------------------------------------------------------------
//
//

void TScaleDialog::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		//	Aspect ratio checkbox
		case SCALE_ASPECT_MSG:
			fAspectRatio = fAspectCheck->Value();
			break;


		//	Apply changes
		case APPLY_MSG:
			{
				//	Get width text value
				float width  = GetNewWidth();
				float height = GetNewHeight();

				BPoint newScale(width, height);
				fCue->SetScale(newScale);

				UpdateCue();
			}
			break;

		//	The user has modified the width text
		case SCALE_WIDTH_TEXT_MSG:
			{
				//	Get text value
				const char *numStr = fWidthText->Text();
				int32 theVal = atoi(numStr);

				//	Do we need to maintain aspect ratio?
				if (fAspectRatio)
				{
					//	Get width text value
					const char *numText = fWidthText->Text();
					float width = atof(numText);

					//	Get width in pixels
					switch(fWidthUnitType)
					{
						case kUnitsPixels:
							break;

						case kUnitsInches:
							width *= 72;
							break;

						case kUnitsPercent:
							width /= 100;
							break;

						default:
							break;

					}

					//	Calculate new height in pixels
					int32 newHeight = width / fAspectWidth;

					//	Set the new height
					switch(fHeightUnitType)
					{
						case kUnitsPixels:
							break;

						case kUnitsInches:
							width *= 72;
							break;

						case kUnitsPercent:
							width /= 100;
							break;

						default:
							break;

					}
				}
			}
			break;

		//	The user has modified the height text
		case SCALE_HEIGHT_TEXT_MSG:
			{
				//	Get text value
				const char *numStr = fHeightText->Text();
				int16 theVal = atoi(numStr);

				//	Do we need to maintain aspect ratio?
				if (fAspectRatio)
				{
					//	Set height to same aspect ratio

				}
			}
			break;

		//	Covert width to pixels
		case SCALE_W_PIXELS_MSG:
			fWidthUnitType = kUnitsPixels;
			ConvertWidthToPixels();
			break;

		//	Covert width to inches
		case SCALE_W_INCHES_MSG:
			fWidthUnitType = kUnitsInches;
			ConvertWidthToInches();
			break;

		//	Covert width to percent
		case SCALE_W_PERCENT_MSG:
			fWidthUnitType = kUnitsPercent;
			ConvertWidthToPercent();
			break;

		//	Covert height to pixels
		case SCALE_H_PIXELS_MSG:
			fHeightUnitType = kUnitsPixels;
			ConvertHeightToPixels();
			break;

		//	Covert height to inches
		case SCALE_H_INCHES_MSG:
			fHeightUnitType = kUnitsInches;
			ConvertHeightToInches();
			break;

		//	Covert height to percent
		case SCALE_H_PERCENT_MSG:
			fHeightUnitType = kUnitsPercent;
			ConvertHeightToPercent();
			break;

		// User pressed OK button.
		case OK_MSG:
			{
				Hide();

				//
				// Get new cue scale

				//	Get width text value
				float width  = GetNewWidth();
				float height = GetNewHeight();

				BPoint newScale(width, height);

				//	Determine if cue scale needs to be updated
				if (newScale != fSavedScale)
				{
					fCue->SetScale(newScale);

					// Inform stage to redraw
					UpdateCue();
				}

				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(SCALE_CLOSE_MSG);
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
				Hide();

				//	Get current dimensions
				BPoint curScale	= fCue->GetScale();

				//	Restore to saved settings
				if (curScale != fSavedScale)
				{
					fCue->SetScale(curScale);

					// Inform stage to redraw
					UpdateCue();
				}


				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(SCALE_CLOSE_MSG);
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
//	ConvertWidthToPixels
//-------------------------------------------------------------------
//
//	Convert image width to pixels

void TScaleDialog::ConvertWidthToPixels()
{
	//	Get width
	BRect drawArea = fCue->GetScaledArea();

	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerWidth() + 1);
	fWidthText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertWidthToInches
//-------------------------------------------------------------------
//
//	Convert image width to inches

void TScaleDialog::ConvertWidthToInches()
{
	//	Get width
	BRect drawArea = fCue->GetScaledArea();

	char numText[4];
	sprintf(numText, "%f", (drawArea.Width() + 1) / 72);
	fWidthText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertWidthToPercent
//-------------------------------------------------------------------
//
//	Convert image width to percent

void TScaleDialog::ConvertWidthToPercent()
{
	//	Get width
	BRect originalArea 	= fCue->GetArea();
	BRect scaleArea 	= fCue->GetScaledArea();

	float percent 	 = scaleArea.Width() / originalArea.Width();
	int32 percentInt = percent * 100;

	char numText[4];
	sprintf(numText, "%d", percentInt);
	fWidthText->SetText(numText);
}


//-------------------------------------------------------------------
//	ConvertHeightToPixels
//-------------------------------------------------------------------
//
//	Convert image width to pixels

void TScaleDialog::ConvertHeightToPixels()
{
	//	Get width
	BRect drawArea = fCue->GetScaledArea();

	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerHeight() + 1);
	fHeightText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertHeightToInches
//-------------------------------------------------------------------
//
//	Convert image height to inches

void TScaleDialog::ConvertHeightToInches()
{
	//	Get width
	BRect drawArea = fCue->GetScaledArea();

	char numText[4];
	sprintf(numText, "%f", (drawArea.Height() + 1) / 72);
	fHeightText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertHeightToPercent
//-------------------------------------------------------------------
//
//	Convert image height to percent

void TScaleDialog::ConvertHeightToPercent()
{
	//	Get width
	BRect originalArea 	= fCue->GetArea();
	BRect scaleArea 	= fCue->GetScaledArea();

	float percent 	 = scaleArea.Height() / originalArea.Height();
	int32 percentInt = percent * 100;

	char numText[4];
	sprintf(numText, "%d", percentInt);
	fHeightText->SetText(numText);
}


//-------------------------------------------------------------------
//	UpdateCue
//-------------------------------------------------------------------
//
//	Force a redraw to the stage
//

void TScaleDialog::UpdateCue()
{
	if (fCue->IsOnStage())
	{
		TStageWindow *theStage	= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
		TStageView 	 *stageView = theStage->GetStageView();

		theStage->Lock();
		stageView->StageDraw( fCue->GetDrawArea(), GetCurrentTime());
		stageView->Draw( fCue->GetDrawArea());
		theStage->Unlock();
	}
}


//-------------------------------------------------------------------
//	GetNewWidth
//-------------------------------------------------------------------
//
//	Calculate new cue width
//

float TScaleDialog::GetNewWidth()
{
	float retVal = 0.0;

	const BRect scaleArea = fCue->GetScaledArea();
	const BRect area 	  = fCue->GetArea();

	const char *numStr = fWidthText->Text();
	const float theVal = atof(numStr);

	//	Calculate return value as a percent
	switch(fWidthUnitType)
	{
		case kUnitsPixels:
			retVal = theVal / area.Width();
			break;

		case kUnitsInches:
			{
				int32 pixels = theVal * 72;
				retVal = pixels / area.Width();
			}
			break;

		case kUnitsPercent:
			retVal = theVal / 100;
			break;

		default:
			break;

	}

	return retVal;
}


//-------------------------------------------------------------------
//	GetNewHeight
//-------------------------------------------------------------------
//
//	Calculate new cue height
//

float TScaleDialog::GetNewHeight()
{
	float retVal = 0.0;

	const BRect scaleArea = fCue->GetScaledArea();
	const BRect area 	  = fCue->GetArea();

	const char *numStr = fHeightText->Text();
	const float theVal = atof(numStr);

	//	Calculate return value as a percent
	switch(fWidthUnitType)
	{
		case kUnitsPixels:
			retVal = theVal / area.Height();
			break;

		case kUnitsInches:
			{
				int32 pixels = theVal * 72;
				retVal = pixels / area.Height();
			}
			break;

		case kUnitsPercent:
			retVal = theVal / 100;
			break;

		default:
			break;

	}

	return retVal;
}