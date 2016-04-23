//-------------------------------------------------------------------
//
//	File:	TOpacityDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.15.98
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
#include "TOpacitySlider.h"

#include "TOpacityDialog.h"

//	Constants
const BRect kOpacityDialogBounds(0, 0, 315, 125);

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TOpacityDialog::TOpacityDialog(TVisualCue *theCue) : BWindow( kOpacityDialogBounds, "Opacity", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE | B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE, 0)
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

TOpacityDialog::~TOpacityDialog()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TOpacityDialog::Init()
{
	//	Set realtime preview to false while initing
	m_RealtimePreview = false;

	//	Get current cue opacity
	m_SavedOpacity = m_Cue->GetTransparency();
	
	//
	// Create dialog elements
	//
	
	const BRect bounds = Bounds();
	
	// Create background view
	m_BackView = new BView(bounds, "OpacityDialogView", B_FOLLOW_ALL, B_WILL_DRAW);
	m_BackView->SetViewColor(kGrey);
	AddChild(m_BackView);
	
	
	// Create Slider
	BRect sliderBounds;
	
	sliderBounds.left 	= bounds.left + 10;
	sliderBounds.top 	= bounds.top + 10;
	sliderBounds.right 	= sliderBounds.left + 255;
	sliderBounds.bottom = sliderBounds.top + 55;
	
	m_Slider = new TOpacitySlider(this, NULL, m_Cue, sliderBounds, "OpacitySlider", "Opacity", new BMessage(OPACITY_MSG), 0, 100, B_TRIANGLE_THUMB); 	
	m_BackView->AddChild(m_Slider); 	
	m_Slider->SetLimitLabels("0%", "100%");
	m_Slider->SetHashMarks(B_HASH_MARKS_TOP); 
	m_Slider->SetHashMarkCount(10);
	m_Slider->SetBarColor(kHeaderGrey);
	m_Slider->UseFillColor(true, &kMediumSteelBlue);
	
	//	Set slider value
	int32 sliderVal = m_SavedOpacity * 100;
	m_Slider->SetValue(sliderVal);	
	
	//	Percentage Text Control
	BRect textBounds;
	textBounds.left 	= m_Slider->Frame().right + 5; 
	textBounds.top 		= bounds.top + 10 + ((55/2) - (25/2));
	textBounds.right	= textBounds.left + 30;
	textBounds.bottom	= textBounds.top +19;
	m_OpacityText = new TNumberTextControl(textBounds, NULL, "OpacityText",  "0", new BMessage(OPACITY_TEXT_MSG)); 	
	m_BackView->AddChild(m_OpacityText);
	
	// Set up max number of characters
	m_OpacityText->TextView()->SetMaxBytes(3);
	
	//	Set text value
	char numStr[4];
	sprintf(numStr, "%d", sliderVal);
	m_OpacityText->SetText(numStr);
	
	//	Realtime Preview Checkbox
	BRect checkRect;
	checkRect.left		= m_Slider->Frame().left;
	checkRect.right		= checkRect.left + 100;
	checkRect.top		= m_Slider->Frame().bottom + 5;
	checkRect.bottom 	= checkRect.top + 15;
	
	m_UpdateCheckbox = new BCheckBox( checkRect, "RealtimeUpdate", "Realtime Update", new BMessage(OPACITY_UPDATE_MSG));
	m_BackView->AddChild(m_UpdateCheckbox);
	m_UpdateCheckbox->SetValue(m_RealtimePreview);
    
	//
	// Cancel, Apply and OK buttons
	//

	// Create the Cancel button
	BRect cancelFrame = Bounds();
	cancelFrame.left 	+= 35;
	cancelFrame.right 	= cancelFrame.left + 60;
	cancelFrame.bottom  -= 15;
	cancelFrame.top 	= cancelFrame.bottom - 20;
	m_CancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_CancelButton);
	
	// Create the Apply button
	BRect applyFrame = Bounds();
	applyFrame.left  	= (Bounds().Width() /2) - (60 / 2);
	applyFrame.right 	= applyFrame.left + 60;	
	applyFrame.bottom  	-= 15;
	applyFrame.top 		= applyFrame.bottom - 20;
	m_ApplyButton = new BButton(applyFrame, "Apply", "Apply", new BMessage(APPLY_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_ApplyButton);
	
	// Create the OK button
	BRect okFrame = Bounds();
	okFrame.right 	-= 35;
	okFrame.left 	=  okFrame.right - 60;	
	okFrame.bottom  -= 15;
	okFrame.top 	= okFrame.bottom - 20;
	m_OKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_OKButton);
	m_OKButton->MakeDefault(true);
	
	//  Update slider with text control
	m_Slider->SetTextControl(m_OpacityText);
	
	//	Set realtime preview to true
	m_RealtimePreview = true;
	m_UpdateCheckbox->SetValue(m_RealtimePreview);
		
}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	MessageReceived
//-------------------------------------------------------------------
//
//

void TOpacityDialog::MessageReceived(BMessage* message)
{
		
	switch(message->what)
	{
		//	Result of slider operation.  Inform cue of new setting
		case OPACITY_MSG:
			{
				//	Get new value
				float sliderVal = (float)(m_Slider->Value()) / 100;
				
				//	Get text value
				const char *numStr = m_OpacityText->Text();
				int16 theVal = atoi(numStr);
				
				//	Update text if needed
				if (theVal != m_Slider->Value())
				{
					char newStr[3];
					sprintf(newStr, "%d", m_Slider->Value());
					m_OpacityText->SetText(newStr);
				}
				
				if (m_RealtimePreview)				
				{				
					m_Cue->SetTransparency(sliderVal) ;
					
					// Inform stage to redraw
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
			}
			break;
		
		//	Set state of Realtime Updating
		case OPACITY_UPDATE_MSG:
			{
				m_RealtimePreview = m_UpdateCheckbox->Value();
			}
			break;
				
		//	The user has modified the opacity text
		case OPACITY_TEXT_MSG:
			{
				//	Get text value
				const char *numStr = m_OpacityText->Text();
				int16 theVal = atoi(numStr);
				
				//	Clip number to acceptable range
				if (theVal > 100)
					theVal = 100;
				
				//	Update text control and adjust slider
				if (m_Slider->Value() != theVal)
				{
					m_Slider->SetValue(theVal);
					m_Slider->Invoke();
				}
			}
			break;
		
		//	Apply changes to cue
		case APPLY_MSG:
			{
				// Get new cue opacity				
				float opacity =  (float)( m_Slider->Value() ) / 100;
				
				//	Determine if cue opacity needs to be updated
				if (opacity != m_SavedOpacity)
				{
					m_SavedOpacity = opacity;
					
					m_Cue->SetTransparency(opacity);
				
					// Redraw if necessary
					UpdateCue();
				}
			}
			break;
			
		// User pressed OK button. 
		case OK_MSG:
			{
				// Get new cue opacity				
				float opacity =  (float)( m_Slider->Value() ) / 100;
				
				//	Determine if cue opacity needs to be updated
				if (opacity != m_SavedOpacity)
				{
					m_Cue->SetTransparency(opacity);
				
					// Inform stage to redraw
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
				
				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(OPACITY_CLOSE_MSG);
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
				//	Get current opacity
				float currentOpacity = m_Cue->GetTransparency();
				
				//	Restore to saved opacity
				if (currentOpacity != m_SavedOpacity)
				{
					m_Cue->SetTransparency(m_SavedOpacity);
				
					// Inform stage to redraw
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
								
				//	Inform cue we are gone
				BMessage *theMessage = new BMessage(OPACITY_CLOSE_MSG);
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
//	UpdateCue
//-------------------------------------------------------------------
//
//	Force a redraw to the stage
//

void TOpacityDialog::UpdateCue()
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