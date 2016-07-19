//---------------------------------------------------------------------
//
//	File:	TMediaTabView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Browser Tab VIew
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

#include "TMediaTabView.h"
#include "TSettingsTabView.h"
#include "TTransitionsTabView.h"
#include "TSorterContainer.h"           // For SORTER MSG decls

#include "TBrowserTabView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TBrowserTabView::TBrowserTabView( BRect frame, const char *name) :
//				BTabView(frame, name, B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_NAVIGABLE_JUMP|B_FRAME_EVENTS|B_NAVIGABLE)
	BTabView(frame, name)
{
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from BMesage
//

TBrowserTabView::TBrowserTabView( BMessage *data) : BTabView(data)
{
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TBrowserTabView::~TBrowserTabView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TBrowserTabView::Init()
{
	BRect bounds = Bounds();

	bounds.InsetBy(5,5);
	bounds.bottom -= TabHeight();

	// Create all the tab views

	// Create Elements tab. By default, the media tab is up
	// and active.
	AddTab(new TMediaTabView(bounds));

	// Create Transitions tab
	AddTab(new TTransitionsTabView(bounds));

	// Create Settings tab
	AddTab(new TSettingsTabView(bounds));
}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TBrowserTabView::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TBrowserTabView") )
		return new TBrowserTabView(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TBrowserTabView::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BTabView::Archive(data, deep);

	if (myErr == B_OK) {

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

void TBrowserTabView::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
	case SORTER_SELECT_MSG:
	case SORTER_INVOKE_MSG:
	{
		BTab* tab = TabAt(Selection());
		BView* tabView = tab->View();
		tabView->MessageReceived(message);
		break;
	}

	// Some cue is adding an entry_ref to our browser
	case ADD_REF_MSG:
	{
		BTab* tab = TabAt(Selection());
		BView* tabView = tab->View();
		tabView->MessageReceived(message);
		break;
	}

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

void TBrowserTabView::WindowActivated(bool state)
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

void TBrowserTabView::Draw(BRect updateRect)
{
	// Save environment
	//PushState();

	// Restore
	//PopState();

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


//void TBrowserTabView::FrameResized(float width, float height)
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

void TBrowserTabView::MouseDown(BPoint where)
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

void TBrowserTabView::MouseUp(BPoint where)
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

void TBrowserTabView::MouseMoved( BPoint where, uint32 code, const BMessage *message )
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




