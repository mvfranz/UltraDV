//---------------------------------------------------------------------
//
//	File:	TNumberBevelTextView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.24.98
//
//	Desc:	Enhanced version of TNumberTextView drawn with a beveled
//			indentation and a focus highlight
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <stdio.h>
#include <ctype.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "AppUtils.h"
#include "TNumberBevelTextView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TNumberBevelTextView::TNumberBevelTextView( BRect bounds, char *name, uint32 resizing, BMessage *theMessage) : BView(bounds, name, resizing, B_WILL_DRAW)
{		
	fMessage = theMessage;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TNumberBevelTextView::~TNumberBevelTextView()
{	
	if (fMessage)
		delete fMessage;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization
//

void TNumberBevelTextView::Init()
{		
	// Create child TNumberTextView
	BRect bounds = Bounds();
	//bounds.InsetBy(2, 2);
	fTextView = new TNumberTextView( bounds, "TimeTextView", B_FOLLOW_ALL);
	AddChild(fTextView);
	
	fTextView->SetTextRect(Bounds());
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TNumberBevelTextView::Draw(BRect inRect)
{
	// Save colors
	PopState();
			
	// Draw text
	fTextView->Draw(inRect);
	
	// Draw standard Be Style bevel
	BPoint startPt, endPt;
	BRect bounds = Bounds();
	bounds.InsetBy(1, 1);
	
	// Draw blue outline is text view has focus
	if (fTextView->IsFocus())
	{
		SetHighColor(kBeFocusBlue);		
		StrokeRect(bounds);	
	}	
	else
	{
		SetHighColor(kBeShadow);
		startPt.Set(bounds.left, bounds.bottom);
		endPt.Set(bounds.left, bounds.top);
		StrokeLine(startPt, endPt);
		startPt.Set(bounds.left, bounds.top);
		endPt.Set(bounds.right, bounds.top);
		StrokeLine(startPt, endPt);
		
		SetHighColor(kBeGrey);
		startPt.Set(bounds.right, bounds.top+1);
		endPt.Set(bounds.right, bounds.bottom);
		StrokeLine(startPt, endPt);
		startPt.Set(bounds.right, bounds.bottom);
		endPt.Set(bounds.left+1, bounds.bottom);
		StrokeLine(startPt, endPt);
	}
	
	bounds = Bounds();
	SetHighColor(kBeDarkGrey);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right, bounds.top);
	StrokeLine(startPt, endPt);

	SetHighColor(kWhite);
	startPt.Set(bounds.right, bounds.top+1);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left+1, bounds.bottom);
	StrokeLine(startPt, endPt);

	// Restore color
	PushState();
	
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::MouseDown(BPoint where)
{
	fTextView->MakeFocus(true);
	
	BView::MouseDown(where);	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::MouseUp(BPoint where)
{	
	BView::MouseUp(where);	
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	BView::MouseMoved(where, code, message);	
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::KeyDown(const char *bytes, int32 numBytes)
{
	BView::KeyDown(bytes, numBytes);
}


//---------------------------------------------------------------------
//	MakeFocus
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::MakeFocus(bool focusState)
{
	// Send message to parent that we have focus
	if (focusState)
		Window()->PostMessage(fMessage, NULL);
		
	BView::MakeFocus(focusState);
}


#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TNumberBevelTextView::MessageReceived(BMessage *message)
{		
	
	switch(message->what)
	{
			
		default:
			BView::MessageReceived(message);			
			break;
	}
	
}

