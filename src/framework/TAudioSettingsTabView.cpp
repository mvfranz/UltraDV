//---------------------------------------------------------------------
//
//	File:	TAudioSettingsTabView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Video Settings Tab VIew
//
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
#include "ResourceManager.h"

#include "TAudioCompressionView.h"
#include "TAudioSampleView.h"
#include "TAudioSourceView.h"
#include "TAudioSettingsView.h"

#include "TAudioSettingsTabView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioSettingsTabView::TAudioSettingsTabView(TAudioSettingsView* parent, BRect frame, const char* name) :
//				BTabView(frame, name, B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_NAVIGABLE_JUMP|B_FRAME_EVENTS|B_NAVIGABLE)
	BTabView(frame, name)
{
	fParent = parent,

	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from BMesage
//

TAudioSettingsTabView::TAudioSettingsTabView( BMessage* data) : BTabView(data)
{
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioSettingsTabView::~TAudioSettingsTabView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TAudioSettingsTabView::Init()
{
	BMessage* theMessage = NULL;

	// Create all the tab views

	// Get SoundCompressionView from resource
	BTab* compressionTab = new BTab();
	theMessage = GetWindowFromResource("SoundCompressionView");
	ASSERT(theMessage);
	fCompressionView = new TAudioCompressionView(theMessage);
	AddTab(fCompressionView, compressionTab);
	compressionTab->SetLabel("Compression");

	// Get SoundSampleView from resource
	BTab* sampleTab = new BTab();
	theMessage = GetWindowFromResource("SoundSampleView");
	ASSERT(theMessage);
	fSampleView = new TAudioSampleView(this, theMessage);
	AddTab(fSampleView, sampleTab);
	sampleTab->SetLabel("Sample");

	// Get SoundSourceView from resource
	BTab* sourceTab = new BTab();
	theMessage = GetWindowFromResource("SoundSourceView");
	ASSERT(theMessage);
	fSourceView = new TAudioSourceView(theMessage);
	AddTab(fSourceView, sourceTab);
	sourceTab->SetLabel("Source");

}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TAudioSettingsTabView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TAudioSettingsTabView") )
		return new TAudioSettingsTabView(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TAudioSettingsTabView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BTabView::Archive(data, deep);

	if (myErr == B_OK) {

	}

	return myErr;
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioSettingsTabView::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	default:
		BTabView::MessageReceived(message);
		break;
	}
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Inform our attached views
//

void TAudioSettingsTabView::WindowActivated(bool state)
{

	BTabView::WindowActivated(state);
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioSettingsTabView::Draw(BRect updateRect)
{
	// Inform parent
	BTabView::Draw(updateRect);
}


#pragma mark -
#pragma mark === Frame Routines ===

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//


//void TAudioSettingsTabView::FrameResized(float width, float height)
//{
//	BTabView::FrameResized(width, height);
//}



#pragma mark -
#pragma mark === Mouse Routines ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAudioSettingsTabView::MouseDown(BPoint where)
{

	// Inform parent
	BTabView::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioSettingsTabView::MouseUp(BPoint where)
{

	// Inform parent
	BTabView::MouseUp(where);
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioSettingsTabView::MouseMoved( BPoint where, uint32 code, const BMessage* message )
{
	switch(code)
	{

	case B_ENTERED_VIEW:
		break;

	case B_INSIDE_VIEW:
		break;

	case B_EXITED_VIEW:
		break;

	default:
		break;
	}

	BTabView::MouseMoved(where, code, message);
}




