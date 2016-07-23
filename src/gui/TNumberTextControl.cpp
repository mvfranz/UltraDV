//---------------------------------------------------------------------
//
//	File:	TNumberTextControl.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.09.98
//
//	Desc:	Enhanced version of BTextControl that only allows the entry
//			of numbers.  Does not allow a paste, text drag, etc. that
//			contains non-numeric data.
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
#include "TNumberTextControl.h"
#include "TNumberTextView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TNumberTextControl::TNumberTextControl( BRect bounds, const char* label, const char* name,  const char* text, BMessage* message) :
	BTextControl(bounds, name, label, text, NULL)
{
	// Set up modification message
	SetModificationMessage(message);

	// Default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TNumberTextControl::~TNumberTextControl()
{
}




//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TNumberTextControl::Init()
{

	// Get text view
	//BTextView *theView = TextView();

	// Swap out text view for our own
	//fTextView = new TNumberTextView( theView->Frame(), "NumberText", theView->ResizingMode() );
	//fTextView->SetText( theView->Text() );
	//theView = fTextView;

	// Disallow all characters except numbers
	BTextView* theView = TextView();

	if (theView) {
		// All characters up to "1"
		int32 index;

		for (index = 0; index < 48; index++) {
			theView->DisallowChar(index);
		}

		// Skip over numbers
		for (index = 58; index < 2000; index++) {
			theView->DisallowChar(index);
		}
	}
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TNumberTextControl::KeyDown(const char* bytes, int32 numBytes)
{

	char theChar = *bytes;

	// Accept numbers, delete and arrows
	if( isdigit(theChar) || IsBackspace(theChar) || IsArrows(theChar) || IsDelete(theChar) ) {
		BTextControl::KeyDown(bytes, numBytes);
	}
	// Tab key moves us through the time elements
	else if( IsTab(theChar) ) {
		beep();
	}
	// Illegal character
	else
		beep();
}




#pragma mark === Input Routines ===


//---------------------------------------------------------------------
//	AcceptsDrop
//---------------------------------------------------------------------
//
//

bool TNumberTextControl::AcceptsDrop(const BMessage* message)
{
	return false;
}

//---------------------------------------------------------------------
//	AcceptsPaste
//---------------------------------------------------------------------
//
//

bool TNumberTextControl::AcceptsPaste(BClipboard* clipboard)
{
	return false;
}


//---------------------------------------------------------------------
//	IsBackspace
//---------------------------------------------------------------------
//
//	Check to see if character is backspace
//

bool TNumberTextControl::IsBackspace(char theChar)
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

bool TNumberTextControl::IsArrows(char theChar)
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

bool TNumberTextControl::IsDelete(char theChar)
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

bool TNumberTextControl::IsTab(char theChar)
{
	switch(theChar)
	{
	case B_TAB:
		return true;

	default:
		return false;

	}
}
