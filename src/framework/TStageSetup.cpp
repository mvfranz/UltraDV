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

	fStage = theStage;

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
	fStage->Lock();

	// Save the current stade color
	fSaveColor = fStage->GetStageView()->ViewColor();

	// Get dialog items
	fDimesionsBox 	= (BBox *)FindView("DimensionBox");
	fSettingsBox	= (BBox *)FindView("SettingsBox");
	fColorBox		= (BBox *)FindView("ColorBox");

	fWidth 		= (BTextControl *)FindView("WidthText");
	fHeight 		= (BTextControl *)FindView("HeightText");
	fXPos  		= (BTextControl *)FindView("XPosText");
	fYPos 			= (BTextControl *)FindView("YPosText");

	fTitleText 	= (BTextControl *)FindView("TitleText");

	fCenterCheck 	= (BCheckBox *)FindView("CenterCheck");
	fTitleCheck	= (BCheckBox *)FindView("TitleCheck");

	fStageColor 	= (BColorControl *)FindView("StageColor");

	// Set up dialog items
	char 	tmpStr[64];
	int32	tmpNum;

	// Width
	tmpNum = fStage->Bounds().Width();
	sprintf(tmpStr, "%d", tmpNum);
	fWidth->SetText( tmpStr );

	// Height
	tmpNum = fStage->Bounds().Height();
	sprintf(tmpStr, "%d", tmpNum);
	fHeight->SetText( tmpStr );

	// X Position
	tmpNum = fStage->Frame().left;
	sprintf(tmpStr, "%d", tmpNum);
	fXPos->SetText( tmpStr );

	// Y Position
	tmpNum = fStage->Frame().top;
	sprintf(tmpStr, "%d", tmpNum);
	fYPos->SetText( tmpStr );

	fStage->Unlock();
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
				fStage->Lock();
				rgb_color newColor = fStage->GetStageView()->ViewColor();
				fStage->GetStageView()->SetViewColor(fSaveColor);
				fStage->GetStageView()->Invalidate();
				fStage->Unlock();
				Lock();
				Quit();
			}
			break;

		case CENTER_CHECK_MSG:
		case TITLE_CHECK_MSG:
			break;

		// Update the stage color
		case STAGE_COLOR_MSG:
			fStage->Lock();
			fStage->GetStageView()->SetViewColor( fStageColor->ValueAsColor() );
			fStage->GetStageView()->Invalidate();
			fStage->Unlock();
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

	width = atoi(fWidth->Text());
	height = atoi(fHeight->Text());
	xpos = atoi(fXPos->Text());
	ypos = atoi(fYPos->Text());

	fStage->ResizeTo(width, height);
	fStage->MoveTo(xpos, ypos);

}

