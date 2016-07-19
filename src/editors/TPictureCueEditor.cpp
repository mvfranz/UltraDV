//---------------------------------------------------------------------
//
//	File:	TPictureCueEditor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.26.98
//
//	Desc:	Picture Editor
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TCueView.h"
#include "TPictureCue.h"
#include "TPictureCueEditorToolbar.h"
#include "TPictureDurationDialog.h"

#include "TPictureCueEditor.h"

// Constants
const int16 kPictureCueEditorToolbarHeight = 23;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPictureCueEditor::TPictureCueEditor(BRect bounds, TPictureCue *theCue) : BWindow(bounds, "Picture Editor", B_DOCUMENT_WINDOW, 0,0)
{
	// Save cue
	fCue = theCue;

	// Default initialization
	bool retVal = Init();

}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPictureCueEditor::~TPictureCueEditor()
{

	// Inform cue that we have been closed
	fCue->SetEditorOpen(false);

	// CLose dialog if needed
	if (fDialog) {
		fDialog->Lock();
		fDialog->Quit();
		fDialog = NULL;
	}
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

bool TPictureCueEditor::Init()
{
	Lock();

	// Initialize member variables
	fDialog = NULL;

	const BRect bounds = Bounds();

	// Load in image data from file
	bool retVal = true;
	fBitmap = fCue->GetBitmap();

	// Create toolbar
	BRect toolbarBounds = bounds;
	toolbarBounds.bottom = toolbarBounds.top + kPictureCueEditorToolbarHeight;
	fToolbar = new TPictureCueEditorToolbar(toolbarBounds, fCue);
	AddChild(fToolbar);
	fToolbar->Show();

	// Create Background View.  It hold all the other views...
	BRect bgRect    = bounds;
	bgRect.top              = fToolbar->Frame().bottom+1;
	bgRect.right    -= (B_V_SCROLL_BAR_WIDTH+1);
	bgRect.bottom   -= (B_H_SCROLL_BAR_HEIGHT+1);
	fBackground = new BView(bgRect, "Container", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(fBackground);
	fBackground->SetViewColor(kWhite);
	fBackground->Show();

	// Create editor view.
	if (retVal == true) {
		fEditorView = new TPictureCueEditorView(fBitmap);
		fBackground->AddChild(fEditorView);
		fEditorView->Show();
	}

	// Vertical	Scroll Bar
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.top+kPictureCueEditorToolbarHeight+1, scrollRect.right, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT);
	fVScroll = new BScrollBar(scrollRect, "VScroll", fBackground, 0, 1000, B_VERTICAL);
	AddChild(fVScroll);

	// Horizontal	Scroll Bar
	scrollRect = Bounds();
	scrollRect.Set(scrollRect.left, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT, scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.bottom);
	fHScroll = new BScrollBar(scrollRect, "HScroll", fBackground, 0, 1000, B_HORIZONTAL);
	AddChild(fHScroll);

	// Now set the window to the size of the bitmap
	// Set minimum window size and then open it to max
	SetSizeLimits(50, fBitmap->Bounds().Width()+B_V_SCROLL_BAR_WIDTH, 50, fBitmap->Bounds().Height()+B_H_SCROLL_BAR_HEIGHT);
	Zoom();

	AdjustScrollBars();

	Unlock();

	return true;
}



//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//


void TPictureCueEditor::FrameResized( float newWidth, float newHeight)
{
	AdjustScrollBars();

	BWindow::FrameResized(newWidth, newHeight);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TPictureCueEditor::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	case PICTURE_DURATION_MSG:
		ShowDurationDialog();
		break;

	default:
		BWindow::MessageReceived(message);
		break;
	}
}


#pragma mark -
#pragma mark === Scrollbar Handling ===

//---------------------------------------------------------------------
//	AdjustScrollBars
//---------------------------------------------------------------------
//
//	Adjust scroll bars to proper dimensions
//

void TPictureCueEditor::AdjustScrollBars()
{
	BRect bounds    = Bounds();
	BRect area              = fBitmap->Bounds();

	AdjustScrollBar(fHScroll, bounds.Width(), bounds.Width() - 4.0, area.Width()+1, bounds.left);
	AdjustScrollBar(fVScroll, bounds.Height(), bounds.Height() - 4.0, area.Height()+1, bounds.top);
}


#pragma mark -
#pragma mark === Scrollbar Handling ===

//---------------------------------------------------------------------
//	ShowDurationDialog
//---------------------------------------------------------------------
//
//	Display picture duration dialog
//

void TPictureCueEditor::ShowDurationDialog()
{
	// Is the window already created?
	if (fDialog) {
		fDialog->Show();
		fDialog->Activate(true);
	}
	// If not, create it
	else{
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("PictureDurationWindow");
		fDialog = new TPictureDurationDialog(theMessage, fCue);
		ASSERT(fDialog);

		// Center it
		CenterWindow(fDialog);

		// Show the dialog
		fDialog->Show();
	}
}
