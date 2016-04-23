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

#include "TRotateDialog.h"

#include "TRotationEffect.h"
#include "TNumberTextControl.h"
#include "AddOnUtils.h"				// for color functions

//	Constants
const BRect kRotateDialogBounds(0, 0, 315, 125);

#define	ROTATE_MSG			'ROTm'
#define	ROTATE_TOOL_MSG		'ROTs'
#define	ROTATE_TEXT_MSG		'ROTt'
#define ROTATE_UPDATE_MSG	'ROTu'
#define ROTATE_APPLY_MSG	'ROTa'


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TRotateDialog::TRotateDialog(TRotationEffect* effect, int32 initialVal) :
	BWindow(kRotateDialogBounds, "Rotation", B_FLOATING_WINDOW_LOOK, 
			B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE | B_NOT_RESIZABLE |
			B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE, 0)
{
	//	Save reference to the effect
	m_effect = effect;
	// Save the value to init to and cancel back to
	m_savedRotation = initialVal;
	
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
	//
	// Create dialog elements
	//
	
	const BRect bounds = Bounds();
	
	// Create background view
	m_BackView = new BView(bounds, "RotationDialogView", B_FOLLOW_ALL, B_WILL_DRAW);
	m_BackView->SetViewColor(BackgroundGrey());
	AddChild(m_BackView);
	
	// Create Slider
	BRect sliderBounds;
	
	sliderBounds.left 	= bounds.left + 10;
	sliderBounds.top 	= bounds.top + 10;
	sliderBounds.right 	= sliderBounds.left + 255;
	sliderBounds.bottom = sliderBounds.top + 55;
	
	m_Slider = new BSlider(sliderBounds, "RotationSlider", "Rotation", 
			new BMessage(ROTATE_MSG), 0, 359, B_TRIANGLE_THUMB); 	
	m_BackView->AddChild(m_Slider); 	
	m_Slider->SetLimitLabels("0°", "359°");
	m_Slider->SetHashMarks(B_HASH_MARKS_TOP); 
	m_Slider->SetHashMarkCount(10);
	m_Slider->SetBarColor(DefaultDarkGrey());
	rgb_color fill = DefaultBlue();
	m_Slider->UseFillColor(true, &fill);
	
	//	Percentage Text Control
	BRect textBounds;
	textBounds.left 	= m_Slider->Frame().right + 5; 
	textBounds.top 		= bounds.top + 10 + ((55/2) - (25/2));
	textBounds.right	= textBounds.left + 30;
	textBounds.bottom	= textBounds.top +19;
	m_RotateText = new TNumberTextControl(textBounds, NULL, "RotationText",  
			"0", new BMessage(ROTATE_TEXT_MSG)); 	
	m_BackView->AddChild(m_RotateText);
		
	// Set up max number of characters
	m_RotateText->TextView()->SetMaxBytes(3);
	
	//	Realtime Preview Checkbox
	BRect checkRect;
	checkRect.left		= m_Slider->Frame().left;
	checkRect.right		= checkRect.left + 100;
	checkRect.top		= m_Slider->Frame().bottom + 5;
	checkRect.bottom	= checkRect.top + 15;
	
	m_UpdateCheckbox = new BCheckBox( checkRect, "RealtimeUpdate", 
			"Realtime Update", new BMessage(ROTATE_UPDATE_MSG));
	m_BackView->AddChild(m_UpdateCheckbox);
	m_UpdateCheckbox->SetValue(true);

	//
	// Cancel, Apply and OK buttons
	//

	// Create the Cancel button
	BRect cancelFrame = Bounds();
	cancelFrame.left 	+= 35;
	cancelFrame.right 	= cancelFrame.left + 60;
	cancelFrame.bottom  -= 15;
	cancelFrame.top 	= cancelFrame.bottom - 20;
	m_CancelButton = new BButton(cancelFrame, "Cancel", "Cancel", 
			new BMessage(B_CANCEL), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_CancelButton);
	
	// Create the Apply button
	BRect applyFrame = Bounds();
	applyFrame.left  	= (Bounds().Width() /2) - (60 / 2);
	applyFrame.right 	= applyFrame.left + 60;	
	applyFrame.bottom  	-= 15;
	applyFrame.top 		= applyFrame.bottom - 20;
	m_ApplyButton = new BButton(applyFrame, "Apply", "Apply", 
			new BMessage(ROTATE_APPLY_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_ApplyButton);
	
	// Create the OK button
	BRect okFrame = Bounds();
	okFrame.right 	-= 35;
	okFrame.left 	=  okFrame.right - 60;	
	okFrame.bottom  -= 15;
	okFrame.top 	= okFrame.bottom - 20;
	m_OKButton = new BButton(okFrame, "OK", "OK", new BMessage((uint32) B_OK), 
			B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	m_BackView->AddChild(m_OKButton);
	m_OKButton->MakeDefault(true);
	
	//	Set realtime preview to true
	m_UpdateCheckbox->SetValue(true);
	// Set the initial rotation
	SetRotation(m_savedRotation);
}

//-------------------------------------------------------------------
//	SetRotation
//-------------------------------------------------------------------
//
//

void TRotateDialog::SetRotation(float value)
{
	if (m_Slider) {
		if (value != m_Slider->Value())
			m_Slider->SetValue(value);
	}
	
	if (m_RotateText) {
		const char *numStr = m_RotateText->Text();
		int16 theVal = atoi(numStr);

		if (theVal != value) {
			char newStr[4];
			sprintf(newStr, "%d", value);
			m_RotateText->SetText(newStr);
		}
	}
}

//-------------------------------------------------------------------
//	LiveUpdate
//-------------------------------------------------------------------
//
//

bool TRotateDialog::LiveUpdate() const
{
	if (m_UpdateCheckbox)
		return m_UpdateCheckbox->Value()? true : false;
	return false;
}

//-------------------------------------------------------------------
//	Rotation
//-------------------------------------------------------------------
//
//

int32 TRotateDialog::Rotation() const
{
	if (m_Slider)
		return m_Slider->Value();
	return 0;
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
		//	Result of slider operation.  Inform effect of new setting
		case ROTATE_MSG:
			{
				//	Get new value
				int32 sliderVal = m_Slider->Value();
				
				//	Get text value
				const char *numStr = m_RotateText->Text();
				int16 theVal = atoi(numStr);
				
				//	Update text if needed
				if (theVal != m_Slider->Value())
				{
					char newStr[3];
					sprintf(newStr, "%d", m_Slider->Value());
					m_RotateText->SetText(newStr);

					// tell the cue effect about the change
					m_effect->DialogValueChanged(LiveUpdate());
				}
			}
			break;
		
		//	The user has modified the opacity text
		case ROTATE_TEXT_MSG:
			{
				//	Get text value
				const char *numStr = m_RotateText->Text();
				int16 theVal = atoi(numStr);
				
				//	Clip number to acceptable range
				if (theVal > 359)
					theVal = 359;
				
				//	Update text control and adjust slider
				if (m_Slider->Value() != theVal)
				{
					m_Slider->SetValue(theVal);
					m_Slider->Invoke();

					// tell the cue effect about the change
					m_effect->DialogValueChanged(LiveUpdate());
				}
			}
			break;
		
		//	Apply changes to cue
		case ROTATE_APPLY_MSG:
			// tell the cue effect about the change
			m_effect->DialogValueChanged(LiveUpdate());
			break;
			
		// Turn on or off live update
		case ROTATE_UPDATE_MSG:
			// Show the changes thus far
			if (m_UpdateCheckbox->Value())
				m_effect->DialogValueChanged(LiveUpdate());
			break;
		
		// User pressed OK button. 
		case B_OK:
			{
				// If we were running without live updates, let the value be
				// set and the cue updated if so required.
				if (!LiveUpdate())
					m_effect->DialogValueChanged(true);
			
				// tell the cue effect we are going to close 'OK'
				m_effect->DialogIsClosing(true);

				// Close the dialog 
				Lock();
				Quit();
			}
			break;
			
		// User has canceled the dialog
		case B_CANCEL:
			{
				// Restore the saved value
				if (m_savedRotation != m_Slider->Value()) {
					SetRotation(m_savedRotation);
					// Update the effect to show the restored value
					m_effect->DialogValueChanged(true);
				}
			
				// tell the cue effect we are going to close 'cancel'
				m_effect->DialogIsClosing(false);

				// Close the dialog 
				Lock();
				Quit();
			}
			break;
						
		default:
			BWindow::MessageReceived(message);						
			break;
	}
}
