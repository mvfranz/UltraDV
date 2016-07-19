//-------------------------------------------------------------------
//
//	File:	TPasteCues.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.07.98
//
//	Desc:	Dialog for pasting of cue(s) into channels
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <Application.h>
#include <StringView.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TCueSheetView.h"
#include "TPasteCues.h"
#include "TCueTimeText.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TNumberTextControl.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TPasteCues::TPasteCues(TCueSheetView *cueSheet, BList *cueList, BMessage *theMessage) : BWindow(theMessage)
{
	fCueSheet 	= cueSheet;
	fCueList	= cueList;
		
	// Default initialization
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TPasteCues::~TPasteCues()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TPasteCues::Init()
{
	// Set window look and feel
	SetFeel(B_MODAL_APP_WINDOW_FEEL);
	
	// Get total number of channels
	int32 numChannels = fCueSheet->GetTotalChannels();
	char numStr[10];
	sprintf(numStr, "%d", numChannels);
	
	//
	// Create dialog elements
	//
	
	BRect bounds = Bounds();
	
	// Find main background view
	fBackView = (BView *)FindView("PasteCuesView");
	
	// Replace BTextView with our own beveled TCueTimeText
	BTextView *textView = (BTextView *)FindView("AtTimeText");
		
	fPasteTime = new TCueTimeText( (BHandler *) NULL, (int32) NULL, (BRect) textView->Bounds(), (const char *)textView->Name(), (uint32)textView->ResizingMode(), kTextBevel);
	fBackView->AddChild(fPasteTime);
	fPasteTime->MoveTo(textView->Frame().left, textView->Frame().top);
	
	fBackView->RemoveChild(textView);
	delete textView;
		
	// Replace BTextControl with our own TNumberTextControl
	BTextControl *numberText = (BTextControl *)FindView("InTrackText");
	fChannelText = new TNumberTextControl(numberText->Bounds(), "In Channel:", numberText->Name(), numberText->Text(), NULL);
	fBackView->AddChild(fChannelText);
	fChannelText->MoveTo(numberText->Frame().left, numberText->Frame().top);
	fChannelText->SetDivider(numberText->Divider());	
	
	BMessage *modMessage = new BMessage(IN_CHANNEL_MSG);
	fChannelText->SetModificationMessage(modMessage);
	
	alignment label, text;
	numberText->GetAlignment(&label, &text);
	fChannelText->SetAlignment(label, text);
	
	fBackView->RemoveChild(numberText);
	delete numberText;
	
	// Set text to current time
	char timeStr[256];
	TimeToString(fCueSheet->GetCurrentTime(), fCueSheet->GetTimeFormat(), timeStr, false);
	fPasteTime->SetText(timeStr);
	
	// Set focus to text
	fPasteTime->MakeFocus(true);
		
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

void TPasteCues::MessageReceived(BMessage* message)
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
		
				// 	Get the time the user wants to paste at
				int64 theTime = fPasteTime->GetTime();

				// 	Get the channel the user wants to paste into
				int32 theChannel = atoi(fChannelText->Text());
								
				// Send message to cue sheet view.  Add time, channel and cue list
				if (theChannel > 0 && theChannel <= numChannels)
				{
					BMessage *theMessage = new BMessage(CUE_PASTE_MSG);
					theMessage->AddPointer("CueList", fCueList);
					theMessage->AddInt32("PasteTime", theTime);
					theMessage->AddInt32("PasteChannel", theChannel);
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
			{
				// Free the cue list
				delete fCueList;
				
				Lock();			
				Quit();
			}
			break;
					
		// 	The text has been changed in the "In Channel" text box
		// 	Contrain the number to the total number of channels
		case IN_CHANNEL_MSG:
			{     			
     			// Get total number of channels
				int32 numChannels = fCueSheet->GetTotalChannels();
				char numStr[10];
				char oneStr[10];
				
				sprintf(oneStr, "%d", 1);
				sprintf(numStr, "%d", numChannels);
				
				// Get the channel number entered
				int32 theNum = atoi( fChannelText->Text());
				
				// 	If the after channel is less than zero, correct the user input
				if (theNum < 1)
     				fChannelText->SetText(oneStr);    

				// 	If the after channel is greater than total channels,
     			//	correct the user input
				if (theNum > numChannels)
     				fChannelText->SetText(numStr);    
     		}
			break;
			
		default:
			BWindow::MessageReceived(message);						
			break;

	}
	
}