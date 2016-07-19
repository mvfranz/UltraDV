//---------------------------------------------------------------------
//
//	File:	TSorterBar.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	This is the toolbar of a TSorterBar.  A text field decribing
//			the sorted column is diplayed here.  It responds to clicks,
//			drags and resizing.
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
#include "ResourceManager.h"

#include "TSorterContainer.h"
#include "TSorterList.h"
#include "TMouseWatcher.h"

#include "TSorterBar.h"


// Constants
const int16 kResizeZoneWidth    = 5;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSorterBar::TSorterBar(BRect bounds, char *title, TSorterContainer *parent, SorterType theType) : BView(bounds, "SorterBarView", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS)
{
	fParent         = parent;
	fTitle  = title;
	fType           = theType;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSorterBar::~TSorterBar()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TSorterBar::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Set up resize rect
	fResizeRect = Bounds();
	fResizeRect.left = fResizeRect.right - kResizeZoneWidth;

	// We are inactive
	fIsActive = false;
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Handle messages
//

void TSorterBar::MessageReceived(BMessage *theMessage)
{
	switch(theMessage->what)
	{
	// Mouse oved. Send resize message to parent
	case MW_MOUSE_MOVED:
	{
		BPoint mousePt;

		if ( theMessage->FindPoint("where", &mousePt) == B_OK) {
			BMessage *theMessage = new BMessage(SORTER_RESIZE_MSG);
			theMessage->AddPoint("ResizePoint", mousePt);
			Parent()->MessageReceived(theMessage);
		}
	}
	break;

	case SORTER_REFS_MSG:
		//refMessage->AddRef("FileRef", &theRef);
		//fIconSorter->MessageReceived(refMessage);
		break;

	default:
		BView::MessageReceived(theMessage);
		break;
	}
}

#pragma mark -
#pragma mark === Drawing Routines ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TSorterBar::Draw(BRect updateRect)
{

	PushState();

	// Fill background
	SetHighColor(kBeGrey);
	FillRect(Bounds());

	// Frame it
	BPoint endPt;
	SetHighColor(kWhite);
	MovePenTo(Bounds().left, Bounds().top+1);
	endPt.Set( Bounds().right, Bounds().top+1 );
	StrokeLine(endPt);

	SetHighColor(kMediumGrey);
	MovePenTo(Bounds().left, Bounds().bottom-1);
	endPt.Set( Bounds().right, Bounds().bottom-1 );
	StrokeLine(endPt);

	SetHighColor(kBlack);
	MovePenTo(Bounds().left, Bounds().top);
	endPt.Set( Bounds().right, Bounds().top );
	StrokeLine(endPt);
	MovePenTo(Bounds().left, Bounds().bottom);
	endPt.Set( Bounds().right, Bounds().bottom );
	StrokeLine(endPt);

	// Right side divider
	if ( fType != kIconSorter) {
		SetHighColor(kMediumGrey);
		MovePenTo(Bounds().right-1, Bounds().top+1);
		endPt.Set( Bounds().right-1, Bounds().bottom-1 );
		StrokeLine(endPt);
		SetHighColor(kDarkGrey);
		MovePenTo(Bounds().right, Bounds().top+1);
		endPt.Set( Bounds().right, Bounds().bottom-1);
		StrokeLine(endPt);
	}

	// Draw name if not an icon sorter
	if ( fType != kIconSorter) {
		// Draw title left justified and centered vertically
		SetHighColor(kBlack);
		BFont theFont(be_plain_font);
		SetFont(&theFont);
		SetFontSize(10);
		float strWidth = theFont.StringWidth(fTitle);
		BPoint textPt( Bounds().left + kSorterTextOffset, (Bounds().Height()/2) + 3 );
		DrawString(fTitle, textPt);

		// If we are active, underline title
		if (fIsActive) {
			SetHighColor(kBlack);
			BPoint startPt(textPt.x-1, textPt.y+1);
			BPoint endPt(startPt.x+strWidth, startPt.y);
			StrokeLine(startPt, endPt);
		}
	}

	// Restore
	PopState();
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TSorterBar::MouseDown(BPoint where)
{
	// Do nothing if IconSorter
	if (fType == kIconSorter)
		return;

	// Check to see if mouseDown is in our resize zone.  If so resize and inform
	// parent as we are doing so
	if ( fResizeRect.Contains(where)) {
		// Launch mouse tracking thread
		StartMouseWatcher(this);
	}
	// Invert the rect
	else{
		BRect invertRect = Bounds();
		invertRect.InsetBy(2,2);

		InvertRect(invertRect);
		Sync();
		snooze(100 * 1000);
		InvertRect(invertRect);

		// Tell parent we were clicked
		BMessage *theMessage = new BMessage(SORTER_CLICK_MSG);
		fParent->MessageReceived(theMessage);
		delete theMessage;
	}
}

//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TSorterBar::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TSorterBar::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TSorterBar::FrameMoved(BPoint new_position)
{
	//  Update the resize zone
	fResizeRect = Bounds();
	fResizeRect.left = fResizeRect.right - kResizeZoneWidth;
}

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TSorterBar::FrameResized(float new_width, float new_height)
{
	//  Update the resize zone
	fResizeRect = Bounds();
	fResizeRect.left = fResizeRect.right - kResizeZoneWidth;

	// Redraw
	Draw(Bounds());
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

void TSorterBar::MakeActive(bool theState)
{
	fIsActive = theState;

	// Force redraw
	Invalidate();
}
