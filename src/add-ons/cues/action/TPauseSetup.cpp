//
// TPauseSetup.cpp
//
// ABH original source file does not exist

#include <be/app/Message.h>
#include <stdio.h>
#include "AppMessages.h"
#include "TPauseSetup.h"

//
// Constructor
//


TPauseSetup::TPauseSetup(BWindow *parent, BMessage *theMessage) :
		BWindow(theMessage){

	fEditor = parent;
}

TPauseSetup::~TPauseSetup(){
}

//
// Destructor
//

void TPauseSetup::Init(){
	
}

void TPauseSetup::MessageReceived(BMessage* message){

	switch(message->what){
		// User pressed OK button.  
		case PAUSE_SETUP_MSG:
		{
			printf("PAUSE_SETUP_MSG unimplemented in TPauseSetup.cpp\n");
		}
		break;
				
		case OK_MSG:
			{
							
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
