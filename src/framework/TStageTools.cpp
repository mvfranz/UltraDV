//---------------------------------------------------------------------
//
//	File:	TStageTools.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.13.98
//
//	Desc:	This palette contains the Stage Tools
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <stdio.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TStageWindow.h"
#include "TStageView.h"

#include "TStageToolsView.h"
#include "TStatusView.h"

#include "TStageTools.h"



// Constants
const int32 kStatusRectHeight = 14;

// Set the window size
const BRect kMediaCueBounds(0, 0, ( kStageToolWidth * 2) - 2, (kStageToolHeight * 4) - 4 + kStatusRectHeight);



//------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------
//
//

TStageTools::TStageTools():BWindow( kMediaCueBounds, "Tools", B_FLOATING_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
									  B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE)
{
	Init();
}

//------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------
//
//

TStageTools::~TStageTools()
{
}


//------------------------------------------------------------------
//	Init
//------------------------------------------------------------------
//
//

void TStageTools::Init()
{
	Lock();

	// Create status bar
	BRect bounds = Bounds();
	bounds.top = bounds.bottom - kStatusRectHeight;
	fToolsStatusView = new TStatusView(bounds);
	AddChild(fToolsStatusView);

	// Create StageToolsView and add it to the window
	BRect viewRect = Bounds();
	viewRect.bottom -= kStatusRectHeight;
	fToolsView = new TStageToolsView(this, viewRect);
	AddChild(fToolsView);

	Unlock();
}


#pragma mark -
#pragma mark === Message Handling ===

//------------------------------------------------------------------
//	MessageReceived
//------------------------------------------------------------------
//
//

void TStageTools::MessageReceived(BMessage* message)
{

	switch (message->what)
	{
		// 	Update the status bar to diplay name of media cue
		//	Inform stage that current tool has been changed
		case UPDATE_STATUS_TEXT_MSG:
			fToolsStatusView->MessageReceived(message);
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage()->PostMessage(message, static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage()->GetStageView());
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

bool TStageTools::QuitRequested()
{
	//be_app->PostMessage(MEDIA_PALETTE_CLOSE_MSG);
	//return true;
	Hide();
	return false;
}
