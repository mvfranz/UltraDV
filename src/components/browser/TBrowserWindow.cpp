//---------------------------------------------------------------------
//
//	File:	TBrowserWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Browser Window
//
//				The browser window consists of tabbed views handling the
//				following items:
//
//				-- Media Elements
//				-- Transitions
//				-- Workspaces
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

#include "TBrowserTabView.h"
#include "TSorterContainer.h"

#include "TBrowserWindow.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TBrowserWindow::TBrowserWindow(BRect bounds) : BWindow( bounds, "Browser", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE)
{
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TBrowserWindow::~TBrowserWindow()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TBrowserWindow::Init()
{

	//
	// Add tabs to the window
	//

	// MediaTabView
	BRect bounds = Bounds();

	fBrowserTabView = new TBrowserTabView(bounds, "MediaTabView");
	ASSERT(fBrowserTabView);
	fBrowserTabView->SetViewColor(kBeGrey);

	AddChild(fBrowserTabView);

	// Show window
	//Show();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TBrowserWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
	case SORTER_SELECT_MSG:
	case SORTER_INVOKE_MSG:
		fBrowserTabView->MessageReceived(message);
		break;

	// Some cue is adding an entry_ref to our browser
	case ADD_REF_MSG:
		fBrowserTabView->MessageReceived(message);
		break;

	default:
		BWindow::MessageReceived(message);
		break;
	}
}


#pragma mark -
#pragma mark === Window Handling ===


//------------------------------------------------------------------
//	QuitRequested
//------------------------------------------------------------------
//
//

bool TBrowserWindow::QuitRequested()
{
	Hide();
	return false;
}
