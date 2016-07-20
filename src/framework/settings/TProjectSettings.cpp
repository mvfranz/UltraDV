//-------------------------------------------------------------------
//
//	File:	TProjectSettings.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.15.98
//
//	Desc:	Dialog for the setting of project defaults
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <Application.h>
#include <StringView.h>
#include <TimeCode.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TProjectSettings.h"
#include "TCueSheetTimeView.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TProjectSettings::TProjectSettings(TCueSheetTimeView* parent, TCueSheetView* cueSheet,  BMessage* theMessage) : BWindow(theMessage)
{

	// Save parent view and cue sheet
	fParent = parent;
	fCueSheet = cueSheet;

	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TProjectSettings::~TProjectSettings()
{
	// Tell parent we have been destructed
	fParent->fProjectSettings = false;
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TProjectSettings::Init()
{
	// Tell parent we have been constructed
	fParent->fProjectSettings = true;

	// Get dialog items
	fRealTimeButton         = (BRadioButton*)FindView("RealTime");
	f24FPSButton            = (BRadioButton*)FindView("24FPS");
	f25FPSButton            = (BRadioButton*)FindView("25FPS");
	f2997FPSButton  = (BRadioButton*)FindView("2997FPS");
	f30FPSButton            = (BRadioButton*)FindView("30FPS");

	fStartTime = (BTextControl*)FindView("StartTime");
	fDuration       = (BTextControl*)FindView("Duration");
	fEndTime        = (BTextControl*)FindView("EndTime");

	// Set up time values
	char timeStr[256];
	TimeToString(fCueSheet->StartTime(), fCueSheet->GetTimeFormat(), timeStr, false);
	fStartTime->SetText(timeStr);

	TimeToString(fCueSheet->Duration(), fCueSheet->GetTimeFormat(), timeStr, false);
	fDuration->SetText(timeStr);

	TimeToString( fCueSheet->Duration() - fCueSheet->StartTime(), fCueSheet->GetTimeFormat(), timeStr, false);
	fEndTime->SetText(timeStr);

	// Set to current project settings
	fTimeFormat = GetCurrentTimeFormat();
	switch(fTimeFormat)
	{
	//case B_TIMECODE_30_DROP_2:
	//	fRealTimeButton->SetValue(1);
	//	break;

	case B_TIMECODE_24:
		f24FPSButton->SetValue(1);
		break;

	case B_TIMECODE_25:
		f25FPSButton->SetValue(1);
		break;

	case B_TIMECODE_30_DROP_2:
		f2997FPSButton->SetValue(1);
		break;

	case B_TIMECODE_30:
		f30FPSButton->SetValue(1);
		break;

	default:
		fRealTimeButton->SetValue(1);
		break;
	}

	fStartTime->MakeFocus(true);
}


//-------------------------------------------------------------------
//	Function:	MessageReceived
//-------------------------------------------------------------------
//
//

void TProjectSettings::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
	// User pressed OK button.  Update channel name with text in control field
	case OK_MSG:
		GetDialogSettings();
		Lock();
		Quit();
		break;

	case CANCEL_MSG:
		Lock();
		Quit();
		break;

	case RT_BUTTON_MSG:
		fTimeFormat = B_TIMECODE_30_DROP_2;
		break;

	case TWOFOUR_BUTTON_MSG:
		fTimeFormat = B_TIMECODE_24;
		break;

	case TWOFIVE_BUTTON_MSG:
		fTimeFormat = B_TIMECODE_25;
		break;

	case TWONINE_BUTTON_MSG:
		fTimeFormat = B_TIMECODE_30_DROP_2;
		break;

	case THIRTY_BUTTON_MSG:
		fTimeFormat = B_TIMECODE_30;
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

void TProjectSettings::GetDialogSettings()
{
	// Set cue sheet's TimeFormat based on user changes
	BMessage* message = new BMessage(TIMEFORMAT_CHANGED_MSG);
	short format = fTimeFormat;
	message->AddInt16("TimeFormat", format);
	TCueSheetWindow* theWindow = static_cast<MuseumApp*>(be_app)->GetCueSheet();
	TCueSheetView* theView = theWindow->GetCueSheetView();
	theWindow->PostMessage( message, theView);
	delete message;

	// Set new start time

	// Now set duration.  We can use this and ignore the end time because it has
	// already been verified as good at entry time

}

