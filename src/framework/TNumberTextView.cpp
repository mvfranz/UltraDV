//---------------------------------------------------------------------
//
//	File:	TNumberTextView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.08.98
//
//	Desc:	General purpose text object that only allow entry
//			of numbers
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

#include "TNumberTextView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TNumberTextView::TNumberTextView(BRect bounds, char *name, uint32 resizing) : 
			  BTextView(bounds, name, bounds, resizing, B_FRAME_EVENTS | B_PULSE_NEEDED)
{	
	// Perform default initialization
	//bounds.OffsetTo(0, 0);
	//SetTextRect(bounds);
	SetAlignment(B_ALIGN_CENTER);	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TNumberTextView::~TNumberTextView()
{	
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TNumberTextView::Draw(BRect inRect)
{
	BTextView::Draw(inRect);
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TNumberTextView::MouseDown(BPoint where)
{
	BTextView::MouseDown(where);	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TNumberTextView::MouseUp(BPoint where)
{	
	BTextView::MouseUp(where);	
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TNumberTextView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	BTextView::MouseMoved(where, code, message);	
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TNumberTextView::KeyDown(const char *bytes, int32 numBytes)
{
	char theChar = *bytes;
		
	// Accept numbers, delete and arrows
	if( isdigit(theChar) || IsBackspace(theChar) || IsArrows(theChar) || IsDelete(theChar) )
	{
		BTextView::KeyDown(bytes, numBytes);
	}
	// Illegal character
	else
		beep();
}


//---------------------------------------------------------------------
//	MakeFocus
//---------------------------------------------------------------------
//
//

void TNumberTextView::MakeFocus(bool focusState)
{
	BTextView::MakeFocus(focusState);
}



//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TNumberTextView::MessageReceived(BMessage *message)
{
	BTextView::MessageReceived(message);
}

#pragma mark === Input Routines ===


//---------------------------------------------------------------------
//	AcceptsDrop
//---------------------------------------------------------------------
//
//

bool TNumberTextView::AcceptsDrop(const BMessage *message)
{
	return false;
}

//---------------------------------------------------------------------
//	AcceptsPaste
//---------------------------------------------------------------------
//
//

bool TNumberTextView::AcceptsPaste(BClipboard *clipboard)
{
	return false;
}

//---------------------------------------------------------------------
//	InsertText
//---------------------------------------------------------------------
//
//	We only allow the user to enter numbers
//

void  TNumberTextView::InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs)
{	
	BTextView::InsertText(text, length, offset, runs);
}



//---------------------------------------------------------------------
//	LockOutAlphabet
//---------------------------------------------------------------------
//
//	Disallow entry of alphabetic characters
//

void  TNumberTextView::LockOutAlphabet()
{
	DisallowChar('A');
	DisallowChar('B');
	DisallowChar('C');
	DisallowChar('D');
	DisallowChar('E');
	DisallowChar('F');
	DisallowChar('G');
	DisallowChar('H');
	DisallowChar('I');
	DisallowChar('J');
	DisallowChar('K');
	DisallowChar('L');
	DisallowChar('M');
	DisallowChar('N');
	DisallowChar('O');
	DisallowChar('P');
	DisallowChar('Q');
	DisallowChar('R');
	DisallowChar('S');
	DisallowChar('T');
	DisallowChar('U');
	DisallowChar('V');
	DisallowChar('W');
	DisallowChar('X');
	DisallowChar('Y');
	DisallowChar('Z');
}


//---------------------------------------------------------------------
//	IsBackspace
//---------------------------------------------------------------------
//
//	Check to see if character is backspace
//

bool  TNumberTextView::IsBackspace(char theChar)
{
	switch(theChar)
	{
		case B_BACKSPACE:
			return true;
			
		default:
			return false;
	
	}
}


//---------------------------------------------------------------------
//	IsArrows
//---------------------------------------------------------------------
//
//	Check to see if character is one of the arrow keys
//

bool  TNumberTextView::IsArrows(char theChar)
{
	switch(theChar)
	{
		case B_UP_ARROW:
		case B_LEFT_ARROW:
		case B_DOWN_ARROW:
		case B_RIGHT_ARROW:
			return true;
			
		default:
			return false;
	
	}
}

//---------------------------------------------------------------------
//	IsDelete
//---------------------------------------------------------------------
//
//	Check to see if character is the delete key
//

bool  TNumberTextView::IsDelete(char theChar)
{
	switch(theChar)
	{
		case B_DELETE:
			return true;
			
		default:
			return false;
	
	}
}


//---------------------------------------------------------------------
//	IsTab
//---------------------------------------------------------------------
//
//	Check to see if character is the tab key
//

bool  TNumberTextView::IsTab(char theChar)
{
	switch(theChar)
	{
		case B_TAB:
			return true;
			
		default:
			return false;
	
	}
}


