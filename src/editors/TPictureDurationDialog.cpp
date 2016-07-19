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
	fCue = theCue;

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
	   fTextParent = FindView("PictureDurationView");

	   // Find text view and swap it out for our own TNumberBevelTextView
	   BTextView *theView = (BTextView *)fTextParent->FindView("PictureDurationText");
	   BRect frame = theView->Frame();
	   frame.InsetBy(-1, -1);
	   fTextView = new TCueTimeText(this, START_TIME_TEXT_FOCUS_MSG, frame, "PictureDurationText",
	                                                                         theView->ResizingMode(), kTextBevel );

	   // Set up time values
	   char timeStr[64];

	   TimeToString(fCue->Duration(), GetCurrentTimeFormat(), timeStr, false);
	   fTextView->fTime = fCue->Duration();

	   fTextView->SetText(timeStr);
	   fTextParent->AddChild(fTextView);

	   // Free original text control
	   fTextParent->RemoveChild(theView);
	   delete theView;

	   // Make the text active focus
	   fTextView->MakeFocus(true);*/
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
		//fTextView->CheckLastEdit();

		// Get new rate
		//int32 theTime = fTextView->fTime;

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