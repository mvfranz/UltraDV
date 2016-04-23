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

TChannelName::TChannelName(BMessage *theMessage, TCueChannel *theChannel) : BWindow(theMessage)
{
	
	// Save channel
	m_Channel = theChannel;

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
	m_TextParent = FindView("ChannelNameView");
	m_TextView = (BTextControl *)m_TextParent->FindView("ChannelNameText");
	m_TextView->SetText(m_Channel->GetName());
	m_TextView->MakeFocus(true);		
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
			m_Channel->SetChannelName( m_TextView->Text() );
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