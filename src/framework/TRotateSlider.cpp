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

TRotateSlider::TRotateSlider( TRotateDialog *dialog, TNumberTextControl *rotateText, TVisualCue *theCue, BRect frame, const char *name, const char *label, BMessage *message, int32 minValue,
				  int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) : 
				  TSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{
	// Set up member variables
	m_Dialog 		= dialog;
	m_TextControl 	= rotateText;
	m_Cue 			= theCue;
	m_Stage			= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
	m_StageView 	= m_Stage->GetStageView();
}



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TRotateSlider::TRotateSlider(BMessage *data) : TSlider(data)
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
	m_Cue->SetRotation(value);
	
	//	Update text
	if (m_TextControl != NULL)
	{
		char newStr[3];
		sprintf(newStr, "%d", value);
		m_TextControl->SetText(newStr);
	}
	
	// Update cue
	if (m_Cue != NULL)
	{
		//	Draw transformed bitmap
		if ( m_Dialog->GetRealtimePreview() == true )
		{
			if (m_Cue->IsOnStage())
			{
				m_Stage->Lock();
				m_StageView->StageDraw( m_Cue->GetDrawArea(), GetCurrentTime());
				m_StageView->Draw( m_Cue->GetDrawArea());
				m_Stage->Unlock();
			}		
		}
		//	Draw bounding box
		else
		{
			//m_Cue->DrawSelectionRect
		}
	}
}


//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
//
//	Set TNumberTextControl member variable
//

void TRotateSlider::SetTextControl(TNumberTextControl *textControl)
{
	m_TextControl = textControl;
}

