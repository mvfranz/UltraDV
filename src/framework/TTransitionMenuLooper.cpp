//---------------------------------------------------------------------
//
//	File:	TTransitionMenuLooper.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.16.98
//
//	Desc:	Transition menu looper.  Handles use selction of transition
//			menu items.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <support/Debug.h>

#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TTransitionMenu.h"
//#include "TTransitionRate.h"
#include "TVisualCue.h"
#include "TCueTransitionButton.h"

#include "TTransitionMenuLooper.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTransitionMenuLooper::TTransitionMenuLooper( TVisualCue *theCue) : BLooper("TransitionMenuLoop", B_NORMAL_PRIORITY)
{
	fCueView = theCue;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransitionMenuLooper::~TTransitionMenuLooper()
{

}



#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Main message loop
//

void TTransitionMenuLooper::MessageReceived( BMessage *message)
{
	// Create message to notify transtion button to change icon
	BMessage *buttonMessage = new BMessage(CHANGE_ICON_MSG);
	bool transitionIn = message->FindBool("TransitionIn");

	switch (message->what)
	{
	case TRAN_NONE_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kTransitionNone);
			fCueView->SetTransitionInDuration(0);
			buttonMessage->AddString("TransitionName", "TransitionIn");
		} else   {
			fCueView->SetTransitionOutID(kTransitionNone);
			fCueView->SetTransitionOutDuration (0);
			buttonMessage->AddString("TransitionName", "TransitionOut");
		}
		break;


	//	Straight Wipes
	//

	case TRAN_WIPE_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeRight");
		} else   {
			fCueView->SetTransitionOutID(kWipeRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeRight");
		}
		break;

	case TRAN_WIPE_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeLeft");
		} else   {
			fCueView->SetTransitionOutID(kWipeLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeLeft");
		}
		break;

	case TRAN_WIPE_DOWN_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeDown);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeDown");
		} else   {
			fCueView->SetTransitionOutID(kWipeDown + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeDown");
		}
		break;

	case TRAN_WIPE_UP_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeTop);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeUp");
		} else   {
			fCueView->SetTransitionOutID(kWipeTop + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeUp");
		}
		break;


	//	Diagonal Wipes
	//

	case TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeTopLeftBottomRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeTopLeftBottomRight");
		} else   {
			fCueView->SetTransitionOutID(kWipeTopLeftBottomRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeTopLeftBottomRight");
		}
		break;

	case TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeTopRightBottomLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeTopRightBottomLeft");
		} else   {
			fCueView->SetTransitionOutID(kWipeTopRightBottomLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeTopRightBottomLeft");
		}
		break;

	case TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeBottomLeftTopRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeBottomLeftTopRight");
		} else   {
			fCueView->SetTransitionOutID(kWipeBottomLeftTopRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeBottomLeftTopRight");
		}
		break;

	case TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kWipeBottomRightTopLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeBottomRightTopLeft");
		} else   {
			fCueView->SetTransitionOutID(kWipeBottomRightTopLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "WipeBottomRightTopLeft");
		}
		break;



	//	Reveals
	//

	case TRAN_REVEAL_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealRight");
		} else   {
			fCueView->SetTransitionOutID(kRevealRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealRight");
		}
		break;

	case TRAN_REVEAL_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealLeft");
		} else   {
			fCueView->SetTransitionOutID(kRevealLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealLeft");
		}
		break;

	case TRAN_REVEAL_DOWN_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealDown);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealDown");
		} else   {
			fCueView->SetTransitionOutID(kRevealDown + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealDown");
		}
		break;

	case TRAN_REVEAL_UP_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealUp);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealUp");
		} else   {
			fCueView->SetTransitionOutID(kRevealUp + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealUp");
		}
		break;

	case TRAN_REVEAL_TOP_LEFT_BOTTOM_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealTopLeftBottomRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealTopLeftBottomRight");
		} else   {
			fCueView->SetTransitionOutID(kRevealTopLeftBottomRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealTopLeftBottomRight");
		}
		break;

	case TRAN_REVEAL_TOP_RIGHT_BOTTON_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealTopRightBottomLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealTopRightBottomLeft");
		} else   {
			fCueView->SetTransitionOutID(kRevealTopRightBottomLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealTopRightBottomLeft");
		}
		break;

	case TRAN_REVEAL_BOTTOM_LEFT_TOP_RIGHT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealBottomLeftTopRight);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealBottomLeftTopRight");
		} else   {
			fCueView->SetTransitionOutID(kRevealBottomLeftTopRight + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealBottomLeftTopRight");
		}
		break;

	case TRAN_REVEAL_BOTTOM_RIGHT_TOP_LEFT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kRevealBottomRightTopLeft);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealBottomRightTopLeft");
		} else   {
			fCueView->SetTransitionOutID(kRevealBottomRightTopLeft + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "RevealBottomRightTopLeft");
		}
		break;


	//	Curtains
	//

	case TRAN_CURTAINS_IN_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kCurtainsIn);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "CurtainsIn");
		} else   {
			fCueView->SetTransitionOutID(kCurtainsIn + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "CurtainsIn");
		}
		break;

	case TRAN_CURTAINS_OUT_MSG:
		if (transitionIn) {
			fCueView->SetTransitionInID(kCurtainsOut);
			fCueView->SetTransitionInDuration(1000);
			buttonMessage->AddString("TransitionName", "CurtainsOut");
		} else   {
			fCueView->SetTransitionOutID(kCurtainsOut + (kTotalTransitions-1));
			fCueView->SetTransitionOutDuration(1000);
			buttonMessage->AddString("TransitionName", "CurtainsOut");
		}
		break;


	//      Open up settings dialog...
	//	Don't change transition icon
	case TRAN_DURATION:
	{
		return;                 // ABH TTransitionRate not implemented
#ifdef ABH
		// Create the transition rate dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("TransitionRateWindow");
// ABH				TTransitionRate *theDialog = new TTransitionRate(theMessage, transitionIn, fCueView);
		ASSERT(theDialog);

		// Center under the mouse
		//uint32 buttons;
		//BPoint dialogPt;
		//theDialog->ChildAt(0)->GetMouse(&dialogPt, &buttons, true);
		//theDialog->ConvertToScreen(&dialogPt);
		//theDialog->MoveTo( dialogPt.x - theDialog->Bounds().Width() /2 , dialogPt.y - theDialog->Bounds().Height()/2);
		CenterWindow(theDialog);

		// Show the dialog
		theDialog->Show();
#endif
	}
		return;

	default:
		if (transitionIn) {
			fCueView->SetTransitionInID(kTransitionNone);
			fCueView->SetTransitionInDuration(0);
			buttonMessage->AddString("TransitionName", "TransitionIn");
		} else   {
			fCueView->SetTransitionOutID(kTransitionNone);
			fCueView->SetTransitionOutDuration(0);
			buttonMessage->AddString("TransitionName", "TransitionIn");
		}
		break;
	}

	//      Check for message overflow.  The duration cannot be longer than the cue duration.
	//	A transition in duration cannot overwrite a transition our duration and
	//	vice-versa.
	if (fCueView->GetTransitionInDuration() > fCueView->GetDuration())
		fCueView->SetTransitionInDuration(fCueView->GetDuration());

	if (fCueView->GetTransitionOutDuration() > fCueView->GetDuration())
		fCueView->SetTransitionOutDuration( fCueView->GetDuration() );

	// Now check for out transition overwrites
	if ( fCueView->HasTransitionIn() && fCueView->HasTransitionOut() ) {
		int32 durationIn = fCueView->GetTransitionInDuration();
		int32 durationOut = fCueView->GetTransitionOutDuration();

		if ( (durationIn + durationOut) >= fCueView->GetDuration() ) {
			fCueView->SetTransitionInDuration(fCueView->GetDuration() / 2 );
			fCueView->SetTransitionOutDuration( fCueView->GetDuration() / 2 );
		}
	}

#ifdef ABH
	// Post the message
	if (transitionIn)
		fCueView->Window()->PostMessage(buttonMessage, fCueView->GetCueTransitionInButton());
	else
		fCueView->Window()->PostMessage(buttonMessage, fCueView->GetCueTransitionOutButton());
#endif

	// Clean up
	delete buttonMessage;
}




/*SetTransitionInID(	= 0;
                fTransitionInDuration	= 0;
                SetTransitionOutID(	= 0;
                fTransitionOutTime	= 0;*/
