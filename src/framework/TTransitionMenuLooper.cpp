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
	m_CueView = theCue;
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
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kTransitionNone);
				m_CueView->SetTransitionInDuration(0);
				buttonMessage->AddString("TransitionName", "TransitionIn");
			}
			else
			{
				m_CueView->SetTransitionOutID(kTransitionNone);
				m_CueView->SetTransitionOutDuration (0);
				buttonMessage->AddString("TransitionName", "TransitionOut");
			}
			break;
		
		
		//	Straight Wipes
		//
		
		case TRAN_WIPE_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeRight");			
			}
			break;
			
		case TRAN_WIPE_LEFT_MSG	:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeLeft");
			}
			break;
			
		case TRAN_WIPE_DOWN_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeDown);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeDown");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeDown + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeDown");
			}
			break;
			
		case TRAN_WIPE_UP_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeTop);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeUp");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeTop + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeUp");
			}
			break;
			
		
		//	Diagonal Wipes
		//
		
		case TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeTopLeftBottomRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeTopLeftBottomRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeTopLeftBottomRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeTopLeftBottomRight");
			}
			break;
			
		case TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeTopRightBottomLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeTopRightBottomLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeTopRightBottomLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeTopRightBottomLeft");
			}
			break;
			
		case TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeBottomLeftTopRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeBottomLeftTopRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeBottomLeftTopRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeBottomLeftTopRight");
			}
			break;
		
		case TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kWipeBottomRightTopLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeBottomRightTopLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kWipeBottomRightTopLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "WipeBottomRightTopLeft");
			}
			break;
					
		
	
		//	Reveals
		//
		
		case TRAN_REVEAL_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealRight");
			}
			break;
			
		case TRAN_REVEAL_LEFT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealLeft");
			}
			break;
			
		case TRAN_REVEAL_DOWN_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealDown);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealDown");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealDown + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealDown");
			}
			break;
			
		case TRAN_REVEAL_UP_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealUp);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealUp");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealUp + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealUp");
			}		
			break;

		case TRAN_REVEAL_TOP_LEFT_BOTTOM_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealTopLeftBottomRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealTopLeftBottomRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealTopLeftBottomRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealTopLeftBottomRight");
			}		
			break;
			
		case TRAN_REVEAL_TOP_RIGHT_BOTTON_LEFT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealTopRightBottomLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealTopRightBottomLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealTopRightBottomLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealTopRightBottomLeft");
			}				
			break;
			
		case TRAN_REVEAL_BOTTOM_LEFT_TOP_RIGHT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealBottomLeftTopRight);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealBottomLeftTopRight");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealBottomLeftTopRight + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealBottomLeftTopRight");
			}				
			break;
			
		case TRAN_REVEAL_BOTTOM_RIGHT_TOP_LEFT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kRevealBottomRightTopLeft);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealBottomRightTopLeft");
			}
			else
			{
				m_CueView->SetTransitionOutID(kRevealBottomRightTopLeft + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "RevealBottomRightTopLeft");
			}				
			break;

		
		//	Curtains
		//
		
		case TRAN_CURTAINS_IN_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kCurtainsIn);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "CurtainsIn");
			}
			else
			{
				m_CueView->SetTransitionOutID(kCurtainsIn + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "CurtainsIn");
			}
			break;
			
		case TRAN_CURTAINS_OUT_MSG:
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kCurtainsOut);
				m_CueView->SetTransitionInDuration(1000);
				buttonMessage->AddString("TransitionName", "CurtainsOut");
			}
			else
			{
				m_CueView->SetTransitionOutID(kCurtainsOut + (kTotalTransitions-1));
				m_CueView->SetTransitionOutDuration(1000);
				buttonMessage->AddString("TransitionName", "CurtainsOut");
			}		
			break;
			

		// 	Open up settings dialog...
		//	Don't change transition icon
		case TRAN_DURATION:
			{
				return; // ABH TTransitionRate not implemented
#ifdef ABH				
				// Create the transition rate dialog from a resource archive
				BMessage *theMessage = GetWindowFromResource("TransitionRateWindow");	
// ABH				TTransitionRate *theDialog = new TTransitionRate(theMessage, transitionIn, m_CueView);				
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
			if (transitionIn)
			{
				m_CueView->SetTransitionInID(kTransitionNone);
				m_CueView->SetTransitionInDuration(0);
				buttonMessage->AddString("TransitionName", "TransitionIn");			
			}
			else
			{
				m_CueView->SetTransitionOutID(kTransitionNone);
				m_CueView->SetTransitionOutDuration(0);
				buttonMessage->AddString("TransitionName", "TransitionIn");			
			}
			break;
	}
	
	// 	Check for message overflow.  The duration cannot be longer than the cue duration.
	//	A transition in duration cannot overwrite a transition our duration and
	//	vice-versa.
	if (m_CueView->GetTransitionInDuration() > m_CueView->GetDuration())
		m_CueView->SetTransitionInDuration(m_CueView->GetDuration());
		
	if (m_CueView->GetTransitionOutDuration() > m_CueView->GetDuration())
		m_CueView->SetTransitionOutDuration( m_CueView->GetDuration() );
		
	// Now check for out transition overwrites
	if ( m_CueView->HasTransitionIn() && m_CueView->HasTransitionOut() ) 
	{
		int32 durationIn = m_CueView->GetTransitionInDuration();
		int32 durationOut = m_CueView->GetTransitionOutDuration();
		
		if ( (durationIn + durationOut) >= m_CueView->GetDuration() )
		{
			m_CueView->SetTransitionInDuration(m_CueView->GetDuration() / 2 );
			m_CueView->SetTransitionOutDuration( m_CueView->GetDuration() / 2 );
		}	
	}

#ifdef ABH		
	// Post the message
	if (transitionIn)
		m_CueView->Window()->PostMessage(buttonMessage, m_CueView->GetCueTransitionInButton());
	else
		m_CueView->Window()->PostMessage(buttonMessage, m_CueView->GetCueTransitionOutButton());
#endif
	
	// Clean up
	delete buttonMessage;
}




/*SetTransitionInID(	= 0;
		m_TransitionInDuration	= 0;
		SetTransitionOutID(	= 0;
		m_TransitionOutTime	= 0;*/
