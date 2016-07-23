//-------------------------------------------------------------------
//
//	File:	TChannelName.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.06.98
//
//	Desc:	Dialog for the entry and renaming of cue channels
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/StringView.h>

#include "AppMessages.h"

#include "MuseumApp.h"
#include "TCueChannel.h"
#include "TChannelName.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TChannelName::TChannelName(BMessage* theMessage, TCueChannel* theChannel) : BWindow(theMessage)
{

	// Save channel
	fChannel = theChannel;

	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TChannelName::~TChannelName()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TChannelName::Init()
{
	// Set edit text to display name ChannelNameText
	fTextParent = FindView("ChannelNameView");
	fTextView = (BTextControl*)fTextParent->FindView("ChannelNameText");
	fTextView->SetText(fChannel->GetName());
	fTextView->MakeFocus(true);
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

void TChannelName::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
	// User pressed OK button.  Update channel name with text in control field
	case OK_MSG:
		fChannel->SetChannelName( fTextView->Text() );
		Lock();
		Quit();
		break;

	case CANCEL_MSG:
		Lock();
		Quit();
		break;

	default:
		BWindow::MessageReceived(message);
		break;

	}

}