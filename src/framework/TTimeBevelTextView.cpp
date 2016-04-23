//---------------------------------------------------------------------
//
//	File:	TTimeBevelTextView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.24.98
//
//	Desc:	Enhanced version of TTimeBevelTextView drawn with a beveled
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
#include "TTimeBevelTextView.h"
#include "TTimeTextView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimeBevelTextView::TTimeBevelTextView( BRect bounds, char *name, uint32 resizing) : BView(bounds, name, resizing, B_WILL_DRAW)					  
{		
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimeBevelTextView::~TTimeBevelTextView()
{	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization
//

void TTimeBevelTextView::Init()
{		
	// Create child TNumberTextView
	BRect bounds = Bounds();
	bounds.InsetBy(2, 2);
	m_TextView = new TTimeTextView( NULL, 0, bounds, "TimeTextView", B_FOLLOW_ALL);
	AddChild(m_TextView);
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TTimeBevelTextView::Draw(BRect inRect)
{
	// Save colors
	rgb_color saveColor = HighColor();
			
	// Draw text
	m_TextView->Draw(inRect);
	
	// Draw standard Be Style bevel
	BPoint startPt, endPt;
	BRect bounds = Bounds();
	bounds.InsetBy(1, 1);
	
	SetHighColor(kBeShadow);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right, bounds.top);
	StrokeLine(startPt, endPt);

	SetHighColor(kBeGrey);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(startPt, endPt);

	bounds = Bounds();
	SetHighColor(kBeShadow);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right, bounds.top);
	StrokeLine(startPt, endPt);

	SetHighColor(kWhite);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(startPt, endPt);

	// Restore color
	SetHighColor(saveColor);
	
	//BView::Draw(inRect);
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::MouseDown(BPoint where)
{
	BView::MouseDown(where);	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::MouseUp(BPoint where)
{	
	BView::MouseUp(where);	
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	BView::MouseMoved(where, code, message);	
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::KeyDown(const char *bytes, int32 numBytes)
{
	BView::KeyDown(bytes, numBytes);
}


//---------------------------------------------------------------------
//	MakeFocus
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::MakeFocus(bool focusState)
{
	BView::MakeFocus(focusState);
}


#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TTimeBevelTextView::MessageReceived(BMessage *message)
{		
	
	switch(message->what)
	{
			
		default:
			BView::MessageReceived(message);			
			break;
	}
	
}

