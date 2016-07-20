//---------------------------------------------------------------------
//
//	File:	TRecordPrefsDialog.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.09.98
//
//	Desc:	Record Preferences Dialog
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
#include "MuseumApp.h"
#include "AppUtils.h"
#include "DebugUtils.h"

#include "TRecordPrefsDialog.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TRecordPrefsDialog::TRecordPrefsDialog(BMessage* theMessage) : BWindow(theMessage)
{
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TRecordPrefsDialog::~TRecordPrefsDialog()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TRecordPrefsDialog::Init()
{
	// Find background view
	fPrefsView = (BView*)FindView("RecordPrefsView");

	BBox* settingsBox = (BBox*)FindView("SettingsBox");

	// Find radio buttons
	fCaptureToDiskRadio = (BRadioButton*)FindView("CaptureToDiskRadio");
	fCaptureToRAMRadio  = (BRadioButton*)FindView("CaptureToRAMRadio");
	fCaptureToDiskRadio->SetMessage( new BMessage(RECORD_DISK_MSG));
	fCaptureToDiskRadio->SetTarget(this);
	fCaptureToRAMRadio->SetMessage( new BMessage(RECORD_RAM_MSG));
	fCaptureToRAMRadio->SetTarget(this);

	//
	// Set up swap menu
	//

	// Get list of mounted BeOS disks
	BVolumeRoster volRoster;
	BVolume bootVolume;

	// Locate boot volume
	volRoster.GetBootVolume(&bootVolume);
	char bootStr[256];
	bootVolume.GetName(bootStr);

	fSwapDiskMenu   = (BMenuField*)FindView("SwapDiskMenu");

	BMenu* theMenu = fSwapDiskMenu->Menu();
	if (theMenu) {
		// Boot Volume
		BMenuItem* swapItem01 = new BMenuItem( bootStr, new BMessage(SWAP_BOOT_MSG) );
		swapItem01->SetTarget(this);
		theMenu->AddItem(swapItem01);

		// Select the first item
		swapItem01->SetMarked(true);
	}

	// Find checkbox
	fPostCompressCheckbox = (BCheckBox*)FindView("PostCompressCheckbox");
	fPostCompressCheckbox->SetMessage( new BMessage(RECORD_POST_MSG));
	fPostCompressCheckbox->SetTarget(this);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TRecordPrefsDialog::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
	case OK_MSG:
	case CANCEL_MSG:
		Lock();
		Quit();
		break;

	case SWAP_BOOT_MSG:
		beep();
		break;

	case RECORD_DISK_MSG:
	case RECORD_RAM_MSG:
	case RECORD_POST_MSG:
		break;

	default:
		BWindow::MessageReceived(message);
		break;
	}
}