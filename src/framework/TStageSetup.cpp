//-------------------------------------------------------------------
//
//	File:	TStageSetup.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.09.98
//
//	Desc:	Stage setting dialog
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <stdlib.h>

#include <Application.h>
#include <StringView.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TStageSetup.h"
#include "TStageWindow.h"
#include "TStageView.h"

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TStageSetup::TStageSetup(TStageWindow *theStage, BMessage *theMessage) : BWindow(theMessage)
{
		
	m_Stage = theStage;
	
	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TStageSetup::~TStageSetup()
{

}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TStageSetup::Init()
{		
	m_Stage->Lock();
	
	// Save the current stade color
	m_SaveColor = m_Stage->GetStageView()->ViewColor();
	
	// Get dialog items			
	m_DimesionsBox 	= (BBox *)FindView("DimensionBox");
	m_SettingsBox	= (BBox *)FindView("SettingsBox");
	m_ColorBox		= (BBox *)FindView("ColorBox");
	
	m_Width 		= (BTextControl *)FindView("WidthText");
	m_Height 		= (BTextControl *)FindView("HeightText");
	m_XPos  		= (BTextControl *)FindView("XPosText");
	m_YPos 			= (BTextControl *)FindView("YPosText");
	
	m_TitleText 	= (BTextControl *)FindView("TitleText");
	
	m_CenterCheck 	= (BCheckBox *)FindView("CenterCheck");
	m_TitleCheck	= (BCheckBox *)FindView("TitleCheck");	
	
	m_StageColor 	= (BColorControl *)FindView("StageColor");
	
	// Set up dialog items
	char 	tmpStr[64];
	int32	tmpNum;
	
	// Width
	tmpNum = m_Stage->Bounds().Width();
	sprintf(tmpStr, "%d", tmpNum);
	m_Width->SetText( tmpStr );
	
	// Height
	tmpNum = m_Stage->Bounds().Height();
	sprintf(tmpStr, "%d", tmpNum);
	m_Height->SetText( tmpStr );
	
	// X Position
	tmpNum = m_Stage->Frame().left;
	sprintf(tmpStr, "%d", tmpNum);
	m_XPos->SetText( tmpStr );

	// Y Position
	tmpNum = m_Stage->Frame().top;
	sprintf(tmpStr, "%d", tmpNum);
	m_YPos->SetText( tmpStr );
	
	m_Stage->Unlock();
}


//-------------------------------------------------------------------
//	Function:	MessageReceived		
//-------------------------------------------------------------------
//
//

void TStageSetup::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		// User pressed OK button.  Update channel name with text in control field
		case OK_MSG:	
			GetDialogSettings();		
			Lock();
			Quit();
			break;
			
		// Restore backgorund color if changed
		case CANCEL_MSG:
			{
				m_Stage->Lock();
				rgb_color newColor = m_Stage->GetStageView()->ViewColor();
				m_Stage->GetStageView()->SetViewColor(m_SaveColor);
				m_Stage->GetStageView()->Invalidate();
				m_Stage->Unlock();
				Lock();
				Quit();
			}
			break;
			
		case CENTER_CHECK_MSG:
		case TITLE_CHECK_MSG:
			break;
			
		// Update the stage color
		case STAGE_COLOR_MSG:
			m_Stage->Lock();
			m_Stage->GetStageView()->SetViewColor( m_StageColor->ValueAsColor() );
			m_Stage->GetStageView()->Invalidate();
			m_Stage->Unlock();
			break;
	
		default:
			BWindow::MessageReceived(message);						
			break;
	}
	
}


//-------------------------------------------------------------------
//	Function:	GetDialogSettings		
//-------------------------------------------------------------------
//
//	Retrieve dialog settings and apply them to the current cue sheet
//

void TStageSetup::GetDialogSettings()
{
	// Get new stage size
	int32 width, height, xpos, ypos;
	
	width = atoi(m_Width->Text());
	height = atoi(m_Height->Text());
	xpos = atoi(m_XPos->Text());
	ypos = atoi(m_YPos->Text());

	m_Stage->ResizeTo(width, height);
	m_Stage->MoveTo(xpos, ypos);
		
}

