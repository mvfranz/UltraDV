//---------------------------------------------------------------------
//
//	File:	TSorterContainer.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	A SorterContainer owns and diplays a SorterBar and a SorterList.
//			This arrangement simulates the funcionality given by the Tracker
//			windows.  The SorterBar displays the description of the items
//			in the vertical list.  The SorterList contains the actual items
//			to be displayed.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "ResourceManager.h"

#include "TSorterList.h"
#include "TSorterBar.h"
#include "TElementsSorter.h"

#include "TSorterContainer.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSorterContainer::TSorterContainer(BRect bounds, char* title, SorterType theType, int16 theID) : BView(bounds, "SorterContainer", B_FOLLOW_LEFT|B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW)
{
	fTitle = title;
	fType   = theType;
	fID     = theID;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSorterContainer::~TSorterContainer()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TSorterContainer::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Create the SorterBar
	BRect sortBarRect = Bounds();
	sortBarRect.bottom = sortBarRect.top + kSortbarHeight;
	fSorterBar = new TSorterBar(sortBarRect, fTitle, this, fType);
	AddChild(fSorterBar);

	// Create the SorterList
	BRect sorterRect = Bounds();
	sorterRect.top = sortBarRect.bottom+1;
	fSorterList = new TSorterList(sorterRect, this, fType);
	AddChild(fSorterList);

	// Set up messages
	BMessage* selectMsg = new BMessage(SORTER_SELECT_MSG);
	fSorterList->SetSelectionMessage(selectMsg);

	BMessage* invokeMsg = new BMessage(SORTER_INVOKE_MSG);
	fSorterList->SetInvocationMessage(invokeMsg);

	// We start life inactive
	fIsActive = false;

}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TSorterContainer::Draw(BRect updateRect)
{

	//PushState();

	//PopState();

	BView::Draw(updateRect);
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Handle messages
//

void TSorterContainer::MessageReceived(BMessage* theMessage)
{
	switch(theMessage->what)
	{
	// Pass refs up to parent
	case B_SIMPLE_DATA:
		Parent()->MessageReceived(theMessage);
		break;

	// Add our fID to the message and inform parent
	case SORTER_CLICK_MSG:
		theMessage->AddInt16("ID", fID);
		fSorterList->Sort();
		Parent()->MessageReceived(theMessage);
		break;

	case SORTER_RESIZE_MSG:
		theMessage->AddInt16("ID", fID);
		Parent()->MessageReceived(theMessage);
		break;

	case SORTER_SCROLL_V_MSG:
		HandleScrollVMessage(theMessage);
		break;

	// Pass this to our child views
	case SORTER_REFS_MSG:
		fSorterBar->MessageReceived(theMessage);
		fSorterList->MessageReceived(theMessage);
		break;

	default:
		BView::MessageReceived(theMessage);
		break;
	}

}

#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TSorterContainer::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TSorterContainer::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TSorterContainer::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
}


#pragma mark -
#pragma mark === Keyboard Handling ===

//---------------------------------------------------------------------
//	HandleKeyDown
//---------------------------------------------------------------------
//
//	Inform parent of key down
//

void TSorterContainer::DeleteItem( int32 theItem )
{
	static_cast<TElementsSorter*>(Parent())->DeleteItem(theItem);
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	MakeActive
//---------------------------------------------------------------------
//
//	Activate the sorted based on theState.  If we are being activated,
//	draw a line under out title and perform the sorting action the
//	user wants us to do.
//

void TSorterContainer::MakeActive(bool theState)
{
	fIsActive = theState;

	// Inform children
	fSorterBar->MakeActive(theState);
	fSorterList->MakeActive(theState);
}


//---------------------------------------------------------------------
//	HandleScrollMessage
//---------------------------------------------------------------------
//
//	A scrollbar has asked us to scroll vertically
//

void TSorterContainer::HandleScrollVMessage(BMessage* theMessage)
{
	bool negative;

	if ( theMessage->FindBool("Negative", &negative) == B_OK ) {
		float newValue, lastValue;
		theMessage->FindFloat( "NewValue", &newValue);
		theMessage->FindFloat( "LastValue", &lastValue);

		if (negative)
			fSorterList->ScrollBy(0, -(lastValue - newValue));
		else
			fSorterList->ScrollBy(0, newValue - lastValue);
	}
}


//---------------------------------------------------------------------
//	Sort
//---------------------------------------------------------------------
//
//	Sort our list according to its type
//

void TSorterContainer::Sort()
{
	fSorterList->Sort();
}


//---------------------------------------------------------------------
//	Synchronize
//---------------------------------------------------------------------
//
//	Reorder list based on id's of syncSource list
//

void TSorterContainer::Synchronize(TSorterContainer* syncSource)
{
	fSorterList->Synchronize(syncSource->GetSorterList() );
}