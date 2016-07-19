//-------------------------------------------------------------------
//
//	File:	TAddChannel.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.06.98
//
//	Desc:	Dialog for addition of channels to cue sheet
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <Application.h>
#include <StringView.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "MuseumApp.h"
#include "TCueSheetView.h"
#include "TAddChannel.h"
#include "TNumberTextControl.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TAddChannel::TAddChannel(BRect bounds, TCueSheetView *theCueSheet) : BWindow( bounds, "Insert Channel", B_TITLED_WINDOW,
																			  B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE, 0)
{

	// Save channel
	fCueSheet = theCueSheet;

	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TAddChannel::~TAddChannel()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TAddChannel::Init()
{
	// Get total number of channels
	int32 numChannels = fCueSheet->GetTotalChannels();
	char numStr[10];
	sprintf(numStr, "%d", numChannels);

	//
	// Create dialog elements
	//

	BRect bounds = Bounds();

	// Create background view
	bounds.OffsetTo(0,0);
	fBackView = new BView(bounds, "InsertChannelView", B_FOLLOW_ALL, B_WILL_DRAW);
	fBackView->SetViewColor(kGrey);
	AddChild(fBackView);

	// Insert Text Control
	bounds = fBackView->Bounds();
	BRect textBounds;
	textBounds.Set(bounds.left+10, bounds.top+10, bounds.left+75, bounds.top+29);
	fAddChannelText = new TNumberTextControl(textBounds, "Insert", "InsertChannel", "1", new BMessage(ADD_CHANNEL_MSG));
	fAddChannelText->SetDivider(29);
	fBackView->AddChild(fAddChannelText);

	// After Text Control
	bounds = fBackView->Bounds();
	textBounds.Set(bounds.left+81, bounds.top+10, bounds.left+231, bounds.top+29);
	fAfterChannelText = new TNumberTextControl(textBounds, "Channel(s) After Channel", "AfterChannel", numStr, new BMessage(AFTER_CHANNEL_MSG));
	fAfterChannelText->SetDivider(114);
	fBackView->AddChild(fAfterChannelText);

	// OK and Cancel buttons

	// Reset the bounds to dialog coordinates
	bounds = Bounds();

	// Set up dimensions of our buttons
	const BRect cancelFrame( bounds.left + 30, bounds.bottom - 35, bounds.left + 90, bounds.bottom - 55);
	const BRect okFrame( bounds.right - 90, bounds.bottom - 35, bounds.right - 30, bounds.bottom - 55);

	// Create the Cancel button
	fCancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fBackView->AddChild(fCancelButton);

	// Create the OK button
	fOKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fBackView->AddChild(fOKButton);
	fOKButton->MakeDefault(true);

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

void TAddChannel::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		// User pressed OK button.
		case OK_MSG:
			{
				// Get total number of channels
				int32 numChannels = fCueSheet->GetTotalChannels();
				char numStr[10];
				sprintf(numStr, "%d", numChannels);

				// Get total number of channels the user wants to add
				int32 newChannels = atoi(fAddChannelText->Text());

				// Get the channel after which the insertion should occur
				int32 afterChannel = atoi(fAfterChannelText->Text());

				// Send message to cue sheet view
				if (afterChannel > 0 && afterChannel <= numChannels)
				{
					BMessage *theMessage = new BMessage(INSERT_CHANNEL_MSG);
					theMessage->AddInt32("NumChannels", newChannels);
					theMessage->AddInt32("AfterChannel", afterChannel);
					(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(theMessage, (BHandler *)static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView(), NULL );
					delete theMessage;
				}

				// Close the dialog
				Lock();
				Quit();
			}
			break;

		// User has canceled the dialog
		case CANCEL_MSG:
			Lock();
			Quit();
			break;

		// Should we do a sanity limit here?
		case ADD_CHANNEL_MSG:
			break;

		// The text has been changed in the after channel text box
		// Contrain the number to the total number of channels
		case AFTER_CHANNEL_MSG:
			{
     			// Get total number of channels
				int32 numChannels = fCueSheet->GetTotalChannels();
				char numStr[10];
				sprintf(numStr, "%d", numChannels);

     			// 	If the after channel is greater than total channels,
     			//	correct the user input
				int32 theNum = atoi( fAfterChannelText->Text());

				if (theNum > numChannels)
     				fAfterChannelText->SetText(numStr);
     		}
			break;

		default:
			BWindow::MessageReceived(message);
			break;

	}

}