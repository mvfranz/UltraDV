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

TProjectSettings::TProjectSettings(TCueSheetTimeView *parent, TCueSheetView *cueSheet,  BMessage *theMessage) : BWindow(theMessage)
{
	
	// Save parent view and cue sheet
	m_Parent = parent;
	m_CueSheet = cueSheet;
	
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
	m_Parent->m_ProjectSettings = false;
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TProjectSettings::Init()
{
	// Tell parent we have been constructed
	m_Parent->m_ProjectSettings = true;
		
	// Get dialog items	
	m_RealTimeButton 	= (BRadioButton *)FindView("RealTime");
	m_24FPSButton 		= (BRadioButton *)FindView("24FPS");
	m_25FPSButton 		= (BRadioButton *)FindView("25FPS");
	m_2997FPSButton 	= (BRadioButton *)FindView("2997FPS");
	m_30FPSButton 		= (BRadioButton *)FindView("30FPS");
		
	m_StartTime = (BTextControl *)FindView("StartTime");
	m_Duration 	= (BTextControl *)FindView("Duration");
	m_EndTime 	= (BTextControl *)FindView("EndTime");
	
	// Set up time values
	char timeStr[256];
	TimeToString(m_CueSheet->StartTime(), m_CueSheet->GetTimeFormat(), timeStr, false);
	m_StartTime->SetText(timeStr);
	
	TimeToString(m_CueSheet->Duration(), m_CueSheet->GetTimeFormat(), timeStr, false);
	m_Duration->SetText(timeStr);
	
	TimeToString( m_CueSheet->Duration() - m_CueSheet->StartTime(), m_CueSheet->GetTimeFormat(), timeStr, false);
	m_EndTime->SetText(timeStr);
	
	// Set to current project settings
	m_TimeFormat = GetCurrentTimeFormat();
	switch(m_TimeFormat)
	{
		//case B_TIMECODE_30_DROP_2:
		//	m_RealTimeButton->SetValue(1);
		//	break;
			
		case B_TIMECODE_24:
			m_24FPSButton->SetValue(1);
			break;
			
		case B_TIMECODE_25:
			m_25FPSButton->SetValue(1);
			break;
			
		case B_TIMECODE_30_DROP_2:
			m_2997FPSButton->SetValue(1);
			break;
			
		case B_TIMECODE_30:
			m_30FPSButton->SetValue(1);
			break;
	
		default:
			m_RealTimeButton->SetValue(1);
			break;
	}
		
	m_StartTime->MakeFocus(true);		
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
			m_TimeFormat = B_TIMECODE_30_DROP_2;
			break;
			
		case TWOFOUR_BUTTON_MSG:
			m_TimeFormat = B_TIMECODE_24;
			break;
			
		case TWOFIVE_BUTTON_MSG:
			m_TimeFormat = B_TIMECODE_25;
			break;
			
		case TWONINE_BUTTON_MSG:
			m_TimeFormat = B_TIMECODE_30_DROP_2;
			break;
			
		case THIRTY_BUTTON_MSG:
			m_TimeFormat = B_TIMECODE_30;
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
	BMessage *message = new BMessage(TIMEFORMAT_CHANGED_MSG);
	short format = m_TimeFormat;
	message->AddInt16("TimeFormat", format);
	TCueSheetWindow *theWindow = static_cast<MuseumApp *>(be_app)->GetCueSheet();
	TCueSheetView *theView = theWindow->GetCueSheetView();
	theWindow->PostMessage( message, theView); 
	delete message;
	
	// Set new start time
	
	// Now set duration.  We can use this and ignore the end time because it has
	// already been verified as good at entry time
	
}

