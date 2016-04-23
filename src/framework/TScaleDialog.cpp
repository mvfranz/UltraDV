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
	m_Cue = theCue;
	
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
	BRect aspectRect = m_Cue->GetArea();
	
	m_AspectWidth 	= aspectRect.Width()  / aspectRect.Height();
	m_AspectHeight	= aspectRect.Height() / aspectRect.Width();
	
	//	Get current dimensions
	m_SavedScale = m_Cue->GetScale();
	
	//	Maintain aspect ratio
	m_AspectRatio = true;
	
	//	Set units to pixels
	m_WidthUnitType  = kUnitsPixels;
	m_HeightUnitType = kUnitsPixels;
	
	//
	// Get dialog items			
	//
	
	m_BackView = (BView *)FindView("ScaleView");
	
	//	Get TextControls and swap them out for our own
	//
	
	//	Width
	BTextControl *widthControl	= (BTextControl *)FindView("WidthTextControl");
	m_WidthText = new TNumberTextControl( widthControl->Frame(), widthControl->Label(), widthControl->Name(), 
									  NULL, new BMessage(SCALE_WIDTH_TEXT_MSG));
	m_WidthText->SetDivider(widthControl->Divider());
	m_WidthText->TextView()->SetMaxBytes(4);
	m_BackView->RemoveChild(widthControl);
	m_BackView->AddChild(m_WidthText);
	delete widthControl;
		
	//	Height
	BTextControl *heightControl	= (BTextControl *)FindView("HeightTextControl");
	m_HeightText = new TNumberTextControl( heightControl->Frame(), heightControl->Label(), heightControl->Name(), 
									  NULL, new BMessage(SCALE_HEIGHT_TEXT_MSG));	
	m_HeightText->SetDivider(heightControl->Divider());
	m_HeightText->TextView()->SetMaxBytes(4);
	m_BackView->RemoveChild(heightControl);
	m_BackView->AddChild(m_HeightText);
	delete heightControl;
	
	//
	// Setup menus
	//
	
	BMenu *theMenu;
	
	m_WidthMenu	 = (BMenuField *)FindView("WidthMenu");
	m_HeightMenu = (BMenuField *)FindView("HeightMenu");
	
	//	Width Menu
	theMenu = m_WidthMenu->Menu();
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
	theMenu = m_HeightMenu->Menu();
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
	m_AspectCheck 	= (BCheckBox *)FindView("AspectCheck");
	
	//	Get buttons
	m_OKButton 		= (BButton *)FindView("OKButton");
	m_ApplyButton	= (BButton *)FindView("ApplyButton");
	m_CancelButton	= (BButton *)FindView("CancelButton");
	
	//
	//	Set controls
	//
	
	//	Width and Height
	BRect drawArea = m_Cue->GetScaledArea();
	
	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerWidth() + 1);
	m_WidthText->SetText(numText);

	sprintf(numText, "%d", drawArea.IntegerHeight() + 1);
	m_HeightText->SetText(numText);
	
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
			m_AspectRatio = m_AspectCheck->Value();
			break;
			
		
		//	Apply changes
		case APPLY_MSG:
			{
				//	Get width text value
				float width  = GetNewWidth();
				float height = GetNewHeight();
				
				BPoint newScale(width, height);
				m_Cue->SetScale(newScale);
				
				UpdateCue();
			}
			break;
				
		//	The user has modified the width text
		case SCALE_WIDTH_TEXT_MSG:
			{
				//	Get text value
				const char *numStr = m_WidthText->Text();
				int32 theVal = atoi(numStr);
				
				//	Do we need to maintain aspect ratio?
				if (m_AspectRatio)
				{
					//	Get width text value
					const char *numText = m_WidthText->Text();
					float width = atof(numText);
						
					//	Get width in pixels
					switch(m_WidthUnitType)
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
					int32 newHeight = width / m_AspectWidth;
					
					//	Set the new height
					switch(m_HeightUnitType)
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
				const char *numStr = m_HeightText->Text();
				int16 theVal = atoi(numStr);
				
				//	Do we need to maintain aspect ratio?
				if (m_AspectRatio)
				{
					//	Set height to same aspect ratio
				
				}				
			}
			break;
		
		//	Covert width to pixels
		case SCALE_W_PIXELS_MSG:
			m_WidthUnitType = kUnitsPixels;
			ConvertWidthToPixels();
			break;
			
		//	Covert width to inches
		case SCALE_W_INCHES_MSG:
			m_WidthUnitType = kUnitsInches;
			ConvertWidthToInches();
			break;
			
		//	Covert width to percent
		case SCALE_W_PERCENT_MSG:
			m_WidthUnitType = kUnitsPercent;
			ConvertWidthToPercent();
			break;
		
		//	Covert height to pixels
		case SCALE_H_PIXELS_MSG:
			m_HeightUnitType = kUnitsPixels;
			ConvertHeightToPixels();
			break;
			
		//	Covert height to inches
		case SCALE_H_INCHES_MSG:
			m_HeightUnitType = kUnitsInches;
			ConvertHeightToInches();
			break;
			
		//	Covert height to percent
		case SCALE_H_PERCENT_MSG:
			m_HeightUnitType = kUnitsPercent;
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
				if (newScale != m_SavedScale)
				{
					m_Cue->SetScale(newScale);
				
					// Inform stage to redraw
					UpdateCue();
				}
								
				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(SCALE_CLOSE_MSG);
				m_Cue->MessageReceived(theMessage);
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
				BPoint curScale	= m_Cue->GetScale();

				//	Restore to saved settings
				if (curScale != m_SavedScale)
				{
					m_Cue->SetScale(curScale);
				
					// Inform stage to redraw
					UpdateCue();
				}
						
					
				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(SCALE_CLOSE_MSG);
				m_Cue->MessageReceived(theMessage);
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
	BRect drawArea = m_Cue->GetScaledArea();
	
	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerWidth() + 1);
	m_WidthText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertWidthToInches
//-------------------------------------------------------------------
//
//	Convert image width to inches

void TScaleDialog::ConvertWidthToInches()
{
	//	Get width
	BRect drawArea = m_Cue->GetScaledArea();
	
	char numText[4];
	sprintf(numText, "%f", (drawArea.Width() + 1) / 72);
	m_WidthText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertWidthToPercent
//-------------------------------------------------------------------
//
//	Convert image width to percent

void TScaleDialog::ConvertWidthToPercent()
{
	//	Get width
	BRect originalArea 	= m_Cue->GetArea();
	BRect scaleArea 	= m_Cue->GetScaledArea();
	
	float percent 	 = scaleArea.Width() / originalArea.Width();
	int32 percentInt = percent * 100; 
	 
	char numText[4];
	sprintf(numText, "%d", percentInt);
	m_WidthText->SetText(numText);
}


//-------------------------------------------------------------------
//	ConvertHeightToPixels
//-------------------------------------------------------------------
//
//	Convert image width to pixels

void TScaleDialog::ConvertHeightToPixels()
{
	//	Get width
	BRect drawArea = m_Cue->GetScaledArea();
	
	char numText[4];
	sprintf(numText, "%d", drawArea.IntegerHeight() + 1);
	m_HeightText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertHeightToInches
//-------------------------------------------------------------------
//
//	Convert image height to inches

void TScaleDialog::ConvertHeightToInches()
{
	//	Get width
	BRect drawArea = m_Cue->GetScaledArea();
	
	char numText[4];
	sprintf(numText, "%f", (drawArea.Height() + 1) / 72);
	m_HeightText->SetText(numText);
}

//-------------------------------------------------------------------
//	ConvertHeightToPercent
//-------------------------------------------------------------------
//
//	Convert image height to percent

void TScaleDialog::ConvertHeightToPercent()
{
	//	Get width
	BRect originalArea 	= m_Cue->GetArea();
	BRect scaleArea 	= m_Cue->GetScaledArea();
	
	float percent 	 = scaleArea.Height() / originalArea.Height();
	int32 percentInt = percent * 100; 
	 
	char numText[4];
	sprintf(numText, "%d", percentInt);
	m_HeightText->SetText(numText);
}


//-------------------------------------------------------------------
//	UpdateCue
//-------------------------------------------------------------------
//
//	Force a redraw to the stage
//

void TScaleDialog::UpdateCue()
{
	if (m_Cue->IsOnStage())
	{
		TStageWindow *theStage	= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
		TStageView 	 *stageView = theStage->GetStageView();
		
		theStage->Lock();
		stageView->StageDraw( m_Cue->GetDrawArea(), GetCurrentTime());
		stageView->Draw( m_Cue->GetDrawArea());
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
	
	const BRect scaleArea = m_Cue->GetScaledArea();
	const BRect area 	  = m_Cue->GetArea();
	
	const char *numStr = m_WidthText->Text();
	const float theVal = atof(numStr);

	//	Calculate return value as a percent
	switch(m_WidthUnitType)
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
	
	const BRect scaleArea = m_Cue->GetScaledArea();
	const BRect area 	  = m_Cue->GetArea();
	
	const char *numStr = m_HeightText->Text();
	const float theVal = atof(numStr);

	//	Calculate return value as a percent
	switch(m_WidthUnitType)
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