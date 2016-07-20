//---------------------------------------------------------------------
//
//	File:	TAudioSettingsView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Background view of TAudioSettingsDialog
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppAlerts.h"

#include "TAudioSettingsDialog.h"

#include "TAudioSettingsView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioSettingsView::TAudioSettingsView(BRect bounds, TAudioSettingsDialog* parent) : BView(bounds, "VideoSettingsView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	fParent = parent;

	// Perform default intitialization
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TAudioSettingsView::Init()
{

	// Grey background
	SetViewColor(kBeGrey);

	// Create OK and Cancel Buttons
	//

	BRect bounds = Bounds();

	// Set up dimensions of our buttons
	const BRect cancelFrame( bounds.left + 30, bounds.bottom - 35, bounds.left + 90, bounds.bottom - 55);
	const BRect okFrame( bounds.right - 90, bounds.bottom - 35, bounds.right - 30, bounds.bottom - 55);

	// Create the Cancel button
	fCancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	AddChild(fCancelButton);

	// Create the OK button
	fOKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	AddChild(fOKButton);
	fOKButton->MakeDefault(true);

}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioSettingsView::MessageReceived(BMessage* theMessage)
{

	switch( theMessage->what)
	{
	case CANCEL_MSG:
	case OK_MSG:
		fParent->MessageReceived(theMessage);
		break;

	default:
		BView::MessageReceived(theMessage);
		break;
	}
}



#pragma mark -
#pragma mark === Video Capture Handling ===
