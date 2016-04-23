//-------------------------------------------------------------------
//
//	File:	TPictureDurationDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.11.98
//
//	Desc:	Dialog for the entry of picture duration
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/StringView.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"

#include "TCueView.h"
#include "TCueTimeText.h"

#include "TPictureDurationDialog.h"

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TPictureDurationDialog::TPictureDurationDialog(BMessage *theMessage, TCueView *theCue) : BWindow(theMessage)
{	
	// Save transition rate
	m_Cue = theCue;
	
	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TPictureDurationDialog::~TPictureDurationDialog()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TPictureDurationDialog::Init()
{
	/*// Get main view
	m_TextParent = FindView("PictureDurationView");
	
	// Find text view and swap it out for our own TNumberBevelTextView
	BTextView *theView = (BTextView *)m_TextParent->FindView("PictureDurationText");
	BRect frame = theView->Frame();
	frame.InsetBy(-1, -1);
	m_TextView = new TCueTimeText(this, START_TIME_TEXT_FOCUS_MSG, frame, "PictureDurationText", 
										 theView->ResizingMode(), kTextBevel );		

	// Set up time values
	char timeStr[64];
	
	TimeToString(m_Cue->Duration(), GetCurrentTimeFormat(), timeStr, false);
	m_TextView->m_Time = m_Cue->Duration();
	
	m_TextView->SetText(timeStr);
	m_TextParent->AddChild(m_TextView);
	
	// Free original text control
	m_TextParent->RemoveChild(theView);
	delete theView;
			
	// Make the text active focus
	m_TextView->MakeFocus(true);*/	
}

		
//-------------------------------------------------------------------
//
//	Function:	MessageReceived
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

void TPictureDurationDialog::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		// User pressed OK button.  Update transition duration
		case OK_MSG:
			{
				// Verify that all values are good
				//m_TextView->CheckLastEdit();
				
				// Get new rate
				//int32 theTime = m_TextView->m_Time;		
							
				// Close dialog
				Hide();
			}
			break;
			
		case CANCEL_MSG:
			Hide();
			break;
			
		default:
			BWindow::MessageReceived(message);						
			break;

	}
	
}