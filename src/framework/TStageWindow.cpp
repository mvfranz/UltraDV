//---------------------------------------------------------------------
//
//	File:	TStageWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Stage window creation and handling routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <stdio.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "TStageWindow.h"
#include "TStageView.h"

#include "TCueView.h"
#include "TVisualCue.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

// Lazlo is out video editor.  It's stage window is the size of the project video output size
TStageWindow::TStageWindow(BRect bounds, TCueSheetWindow *cueSheet) : BWindow( bounds, "Stage", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
													B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	//	Save parent cue sheet
	fCueSheet = cueSheet;

	//	Defualt initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

TStageWindow::TStageWindow(BMessage *data) : BWindow(data)
{

	//
	// Rehydrate the cue from message data
	//

	// Add archived child views
	BMessage 	childArchive;
	BRect		bounds;

	//	StageView
	if ( data->FindMessage("StageView", &childArchive) == B_OK)
	{
		if ( validate_instantiation(&childArchive, "TStageView") )
		{
			fStageView = new TStageView(&childArchive);
			ASSERT(fStageView);
			AddChild(fStageView);
		}
	}
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageWindow::~TStageWindow()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TStageWindow::Init()
{
	BFont font(be_fixed_font);

	fStageView = new TStageView(Bounds(), this);

	// Limit the window size
	SetSizeLimits( 100, 1500, 100, 1500);

	// Add view to frame
	AddChild(fStageView);
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TStageWindow::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TStageWindow") )
		return new TStageWindow(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TStageWindow::Archive(BMessage *data, bool deep) const
{
	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BWindow::Archive(data, deep);

	if (myErr == B_OK)
	{
		// Add our class name to the archive
		data->AddString("class", "TStageWindow");

		// Add our member variables to the archive

		// Add attached views
		if (deep)
		{
			//	StageView
			if (fStageView)
			{
				BMessage childArchive;
				if ( fStageView->Archive(&childArchive, deep) == B_OK )
					data->AddMessage("StageView", &childArchive);
			}
		}
	}

	Looper()->Unlock();

	return myErr;

}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TStageWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
