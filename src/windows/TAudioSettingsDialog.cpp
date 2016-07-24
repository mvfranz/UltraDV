//---------------------------------------------------------------------
//
//	File:	TAudioSettingsDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Audio Settings Dialog
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
#include "ResourceManager.h"

#include "TAudioSettingsView.h"
#include "TAudioSettingsTabView.h"

#include "TAudioSettingsDialog.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioSettingsDialog::TAudioSettingsDialog(BRect bounds) : BWindow( bounds, "Audio Settings", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)
{
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TAudioSettingsDialog::Init()
{

	// Create background view
	fBGView = new TAudioSettingsView(Bounds(), this);
	AddChild(fBGView);

	//
	// Add tabs to the window
	//

	// AudioSettings
	BRect bounds(0, 0, Bounds().Width(), 200);
	fTabView = new TAudioSettingsTabView(fBGView, bounds, "AudioSettingsTabView");
	ASSERT(fTabView);
	fTabView->SetViewColor(kBeGrey);

	fBGView->AddChild(fTabView);

}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioSettingsDialog::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	case CANCEL_MSG:
		Lock();
		Quit();
		break;

	case OK_MSG:
		// Restore Settings
		Lock();
		Quit();
		break;

	default:
		BWindow::MessageReceived(message);
		break;
	}
}


//------------------------------------------------------------------
//	QuitRequested
//------------------------------------------------------------------
//
//

bool TAudioSettingsDialog::QuitRequested()
{
	Hide();
	return true;
}
