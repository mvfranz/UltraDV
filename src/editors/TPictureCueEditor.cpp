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

TPictureCueEditor::TPictureCueEditor(BRect bounds, TPictureCue *theCue): BWindow(bounds, "Picture Editor", B_DOCUMENT_WINDOW, 0,0)
{
	// Save cue
	m_Cue = theCue;

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
	m_Cue->SetEditorOpen(false);	
	
	// CLose dialog if needed
	if (m_Dialog)
	{
		m_Dialog->Lock();
		m_Dialog->Quit();
		m_Dialog = NULL;
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
	m_Dialog = NULL;
	
	const BRect bounds = Bounds();
	
	// Load in image data from file
	bool retVal = true;
	m_Bitmap = m_Cue->GetBitmap();
	
	// Create toolbar
	BRect toolbarBounds = bounds;		
	toolbarBounds.bottom = toolbarBounds.top + kPictureCueEditorToolbarHeight;
	m_Toolbar = new TPictureCueEditorToolbar(toolbarBounds, m_Cue);
	AddChild(m_Toolbar);
	m_Toolbar->Show();
	
	// Create Background View.  It hold all the other views...
	BRect bgRect   	= bounds;
	bgRect.top 		= m_Toolbar->Frame().bottom+1;
	bgRect.right   	-= (B_V_SCROLL_BAR_WIDTH+1);
	bgRect.bottom  	-= (B_H_SCROLL_BAR_HEIGHT+1);
	m_Background = new BView(bgRect, "Container", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(m_Background);
	m_Background->SetViewColor(kWhite);
	m_Background->Show();	
			
	// Create editor view.
	if (retVal == true)
	{
		m_EditorView = new TPictureCueEditorView(m_Bitmap);
		m_Background->AddChild(m_EditorView);
		m_EditorView->Show();
	}
	
	// Vertical	Scroll Bar
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.right-B_V_SCROLL_BAR_WIDTH , scrollRect.top+kPictureCueEditorToolbarHeight+1, scrollRect.right, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT);
	m_VScroll = new BScrollBar(scrollRect, "VScroll", m_Background, 0, 1000, B_VERTICAL);
	AddChild(m_VScroll);	
	
	// Horizontal	Scroll Bar
	scrollRect = Bounds();
	scrollRect.Set(scrollRect.left, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT, scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.bottom);
	m_HScroll = new BScrollBar(scrollRect, "HScroll", m_Background, 0, 1000, B_HORIZONTAL);
	AddChild(m_HScroll);	
		
	// Now set the window to the size of the bitmap
	// Set minimum window size and then open it to max	
	SetSizeLimits(50, m_Bitmap->Bounds().Width()+B_V_SCROLL_BAR_WIDTH, 50, m_Bitmap->Bounds().Height()+B_H_SCROLL_BAR_HEIGHT);
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
	BRect bounds 	= Bounds();
	BRect area 		= m_Bitmap->Bounds();
	
	AdjustScrollBar(m_HScroll, bounds.Width(), bounds.Width() - 4.0, area.Width()+1, bounds.left);
	AdjustScrollBar(m_VScroll, bounds.Height(), bounds.Height() - 4.0, area.Height()+1, bounds.top);
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
	if (m_Dialog)
	{
		m_Dialog->Show();
		m_Dialog->Activate(true);		
	}
	// If not, create it
	else
	{
		// Create the channel name dialog from a resource archive
		BMessage *theMessage = GetWindowFromResource("PictureDurationWindow");
		m_Dialog = new TPictureDurationDialog(theMessage, m_Cue);
		ASSERT(m_Dialog);
		
		// Center it
		CenterWindow(m_Dialog);
		
		// Show the dialog
		m_Dialog->Show();		
	}
}