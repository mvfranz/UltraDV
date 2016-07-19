//---------------------------------------------------------------------
//
//	File:	TExportText.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.16.98
//
//	Desc:	This is an derived version of BStringView that is responsible
//			for the display of the In and Out name in the ExportTimeView.
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
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TExportTimeView.h"

#include "TExportText.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExportText::TExportText( TExportTimeView *parent, BRect bounds, const char *name, const char *text, uint32 resizeFlags, uint32 flags) :
							BStringView(bounds, name, text, resizeFlags, flags)
{
	fParent = parent;

	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TExportText::~TExportText()
{
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TExportText::Draw(BRect bounds)
{
	PushState();

	// Fill background
	SetHighColor(kKhaki);
	FillRect(bounds);

	// Draw text
	SetHighColor(kBlack);
	SetLowColor(kKhaki);
	BStringView::Draw(bounds);

	PopState();
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TExportText::MessageReceived(BMessage *message)
{

	BStringView::MessageReceived(message);

}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TExportText::MouseDown(BPoint pt)
{
	/*
	// Do nothing if we are playing
	if ( ((TCueSheetWindow *)fChannel->Window())->IsPlaying() )
		return;

	// If the option key is down, allow user to change the text
	if ( IsOptionKeyDown() )
	{
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("ChannelNameWindow");
		TChannelName *theDialog = new TChannelName(theMessage, fChannel);
		ASSERT(theDialog);

		// Move it under the mouse
		ConvertToScreen(&pt);
		theDialog->MoveTo(pt.x, pt.y);

		// Show the dialog
		theDialog->Show();
	}
	// Otherwise, select all the cues in the channel
	else
	{
		if ( IsShiftKeyDown() == false )
			fChannel->GetCueSheet()->DeselectAllCues();

		fChannel->SelectAllCues();
	}
	*/
}
