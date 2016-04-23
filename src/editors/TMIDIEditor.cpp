//---------------------------------------------------------------------
//
//	File:	TMIDIEditor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	MIDI Cue editor.  Opens cue into a window that displays it's 
//			channels, instruments and other related data.
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

#include "TMIDIEditor.h"
#include "TMIDIEditorView.h"

#include "TCueView.h"
#include "TMIDICue.h"
#include "TMIDIFile.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEditor::TMIDIEditor(BRect bounds, TMIDICue *theCue): BWindow(bounds, "MIDI Mixer", B_DOCUMENT_WINDOW, 0,0)
{
	// Save cue
	m_MIDICue = theCue;

	// Default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEditor::~TMIDIEditor()
{
	
	// Inform cue that we have been closed
	m_MIDICue->SetEditorOpen(false);	
	
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMIDIEditor::Init()
{
		
	// Create Background View.  It hold all the other views...
	BRect bgRect   	= Bounds();
	bgRect.right   	-= (B_V_SCROLL_BAR_WIDTH+1);
	bgRect.bottom  	-= (B_H_SCROLL_BAR_HEIGHT+1);
	m_Background = new BView(bgRect, "Container", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(m_Background);
	m_Background->SetViewColor(kWhite);
	m_Background->Show();	
	
				
	// Create editor view.
	BRect audioBounds = Bounds();
	audioBounds.bottom -= -(B_H_SCROLL_BAR_HEIGHT+1); 
	m_EditorView = new TMIDIEditorView(this, audioBounds);
	m_Background->AddChild(m_EditorView);
	m_EditorView->Show();
	
	// Horizontal	Scroll Bar
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.left, scrollRect.bottom-B_H_SCROLL_BAR_HEIGHT, scrollRect.right-B_V_SCROLL_BAR_WIDTH, scrollRect.bottom);
	m_HScroll = new BScrollBar(scrollRect, "HMIDIScroll", m_Background, 0, 1000, B_HORIZONTAL);
	AddChild(m_HScroll);	
		
	// Set minimum window size and then open it to max	
	SetSizeLimits(100, 600, 500, 500);
	
	//
	// Set up midi editor parameters
	//
	
	// Get number of tracks in General MIDI file
	//m_TotalTracks = m_MIDICue->GetMidiFile()->GetTotalTracks();
	
	// Test: Get name of track 1...
	
}


//---------------------------------------------------------------------
//	Refresh
//---------------------------------------------------------------------
//
//

void TMIDIEditor::Refresh()
{
	Lock();
	Unlock();
}


#pragma mark -
#pragma mark === Message Handling === 

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TMIDIEditor::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{
				
		default:
			BWindow::MessageReceived(message);						
			break;
	}		
}	
