//---------------------------------------------------------------------
//
//	File:	TTimeTextView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.09.98
//
//	Desc:	Enhanced version of BText View that only allows the entry
//			of numbers.  Does not allow a paste, text drag, etc. that
//			contains non-numeric data.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <Application.h>
#include <Debug.h>
#include <TimeCode.h>

#include "AppConstants.h"
#include "AppMessages.h"

#include "AppUtils.h"
#include "TTimeTextView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimeTextView::TTimeTextView( BHandler* target, int32 messageID, BRect bounds, char* name, uint32 resizing) :
	BTextView(bounds, name, bounds, resizing, B_FRAME_EVENTS | B_PULSE_NEEDED)

{
	fTarget                 = target;
	fMessageID      = messageID;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimeTextView::~TTimeTextView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization
//

void TTimeTextView::Init()
{
	fIsLocked = false;

	SetTextRect(Bounds());
	SetAlignment(B_ALIGN_CENTER);

	// Set up max number of character
	SetMaxBytes(11);

	// Set font and color
	BFont theFont(be_plain_font);
	theFont.SetSize(10);
	SetViewColor(kWhite);
	SetFontAndColor(&theFont, B_FONT_ALL, &kBlack);

	//      Set up time rects.  We divide the area up into four areas
	//	for hour, minutes, seconds and frames
	float numberWidth;
	float spaceWidth;

	BRect bounds = Bounds();

	numberWidth = StringWidth("00");
	spaceWidth = StringWidth(":");

	fHoursRect.left                 = bounds.left;
	fHoursRect.top          = bounds.top;
	fHoursRect.right                = fHoursRect.left + numberWidth;
	fHoursRect.bottom               = bounds.bottom;

	fMinutesRect.left               = fHoursRect.right + spaceWidth;
	fMinutesRect.top                = bounds.top;
	fMinutesRect.right      = fMinutesRect.left + numberWidth;
	fMinutesRect.bottom     = bounds.bottom;

	fSecondsRect.left               = fMinutesRect.right + spaceWidth;
	fSecondsRect.top                = bounds.top;
	fSecondsRect.right      = fSecondsRect.left + numberWidth;
	fSecondsRect.bottom     = bounds.bottom;

	fFramesRect.left                = fSecondsRect.right + spaceWidth;
	fFramesRect.top                 = bounds.top;
	fFramesRect.right               = bounds.right;
	fFramesRect.bottom      = bounds.bottom;

	// Select first cell
	//Select(0,2);
	//ScrollToSelection();
	fCurrentCell = 1;

}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TTimeTextView::Draw(BRect inRect)
{
	// Save colors
	PushState();

	// Deselect current selection
	Select(0,0);

	// Pass up to draw
	BTextView::Draw(inRect);

	// Reselect
	Reselect();

	// Restore color
	PopState();
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TTimeTextView::MouseDown(BPoint where)
{

	// Do nothing if locked
	if (fIsLocked)
		return;

	// Deselect all
	MakeFocus(true);
	Select(0,0);

	// Make sure last edit is valid
	CheckLastEdit();

	// Determine where the click is
	if ( fHoursRect.Contains(where) ) {
		Select(0,2);
		ScrollToSelection();
		fCurrentCell = 1;
	} else if ( fMinutesRect.Contains(where) ) {
		Select(3,5);
		ScrollToSelection();
		fCurrentCell = 2;
	} else if ( fSecondsRect.Contains(where) ) {
		Select(6,8);
		ScrollToSelection();
		fCurrentCell = 3;
	} else if ( fFramesRect.Contains(where) ) {
		Select(9,11);
		ScrollToSelection();
		fCurrentCell = 4;
	}
	// Default to first cell if all else fails
	else{
		Select(0,2);
		ScrollToSelection();
		fCurrentCell = 1;
	}

	//BTextView::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TTimeTextView::MouseUp(BPoint where)
{
	BTextView::MouseUp(where);
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TTimeTextView::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	//BTextView::MouseMoved(where, code, message);
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TTimeTextView::KeyDown(const char* bytes, int32 numBytes)
{
	char theChar = *bytes;

	//	Check for a return or enter
	if (theChar == B_ENTER || theChar == B_RETURN || theChar == 'r') {
		// Deselect all
		Select(0,0);

		// Check integrity of last edited value
		CheckLastEdit();

		MakeFocus(false);
	}
	// Accept numbers
	else if( isdigit(theChar) ) {
		/*
		   short	tmpNum;
		   bool	addIt = false;

		   switch (fCurrentCell)
		   {
		        case 1:
		                if (inputNum[0] < 2 || inputNum[0] == 2 && theChar < '4')
		                        addIt = true;
		                        break;

		        case 2:
		                if (inputNum[1] < 6)
		                        addIt = true;
		                        break;

		        case 3:
		                if (inputNum[2] < 6)
		                        addIt = true;
		                        break;

		        case 4:
		                if (GetCurrentTimeFOrmat() == kMIDI && inputNum[3] < 10)
		                        addIt = true;
		                else if (inputNum[3] < 2)
		                        addIt = true;
		                else if (inputNum[3] == 2)
		                {
		                        switch (GetCurrentTimeFormat())
		                        {

		                                case kSMPTE24:
		                                        if (theChar < '4')
		                                                addIt = true;
		                                        break;

		                                case kSMPTE25:
		                                        if (theChar < '5')
		                                                addIt = true;
		                                        break;

		                                case kSMPTE2997:
		                                case kSMPTE30:
		                                        addIt = true;
		                                        break;
		                        }
		                }
		                break;
		   }

		   if (addIt)
		   {
		        inputNum[itsCurrentField - 1] *= 10;
		        inputNum[itsCurrentField - 1] += theChar - '0';
		        //DrawEditField();
		   }
		 */

		BTextView::KeyDown(bytes, numBytes);
	}
	// Tab key moves us through the time elements
	else if( IsTab(theChar) || theChar == '.' ) {
		int16 lastCell = fCurrentCell;

		// Deselect all
		Select(0,0);

		// Check integrity of last edited value
		CheckLastEdit();

		// Check for shift key
		if ( IsShiftKeyDown() ) {
			// Select the previous cell
			fCurrentCell--;

			if (fCurrentCell <= 0)
				fCurrentCell = 4;
		} else {
			// Select the next cell
			fCurrentCell++;

			if (fCurrentCell > 4)
				fCurrentCell = 1;
		}

		switch (fCurrentCell)
		{
		case 1:
			Select(0,2);
			ScrollToSelection();
			break;

		case 2:
			Select(3,5);
			ScrollToSelection();
			break;

		case 3:
			Select(6,8);
			ScrollToSelection();
			break;

		case 4:
			Select(9,11);
			ScrollToSelection();
			break;

		default:
			break;
		}
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

void TTimeTextView::MakeFocus(bool focusState)
{
	BTextView::MakeFocus(focusState);

	// Inform target
	if ( focusState == false) {
		if (fTarget) {
			BLooper* looper = fTarget->Looper();

			if ( looper->Lock() ) {
				CheckLastEdit();

				for (int16 index = 1; index < 5; index++)
					ConvertToTime(index);

				if (fMessageID) {
					BMessage* message = new BMessage(fMessageID);
					message->AddInt32("TheTime", fTime);
					fTarget->MessageReceived( message);
					looper->Unlock();
					delete message;
				}
			}
		}
	}
}


#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TTimeTextView::MessageReceived(BMessage* message)
{
	//BLooper *looper = Looper();
	//looper->Lock();

	switch(message->what)
	{

	// Subvert these messages
	case B_CUT:
	case B_PASTE:
	case B_SELECT_ALL:
		break;

	default:
		BTextView::MessageReceived(message);
		break;
	}

	//looper->Unlock();
}



#pragma mark -
#pragma mark === Input Routines ===


//---------------------------------------------------------------------
//	AcceptsDrop
//---------------------------------------------------------------------
//
//

bool TTimeTextView::AcceptsDrop(const BMessage* message)
{
	return false;
}

//---------------------------------------------------------------------
//	AcceptsPaste
//---------------------------------------------------------------------
//
//

bool TTimeTextView::AcceptsPaste(BClipboard* clipboard)
{
	return false;
}

//---------------------------------------------------------------------
//	InsertText
//---------------------------------------------------------------------
//
//

void TTimeTextView::InsertText(const char* text, int32 length, int32 offset, const text_run_array* runs)
{
	BTextView::InsertText(text, length, offset, runs);
}




//---------------------------------------------------------------------
//	IsBackspace
//---------------------------------------------------------------------
//
//	Check to see if character is backspace
//

bool TTimeTextView::IsBackspace(char theChar)
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

bool TTimeTextView::IsArrows(char theChar)
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

bool TTimeTextView::IsDelete(char theChar)
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

bool TTimeTextView::IsTab(char theChar)
{
	switch(theChar)
	{
	case B_TAB:
		return true;

	default:
		return false;

	}
}


#pragma mark -
#pragma mark === Text Entry Routines ===

//---------------------------------------------------------------------
//	CheckLastEdit
//---------------------------------------------------------------------
//
//	Check for valid data after a user edit
//

void TTimeTextView::CheckLastEdit()
{
	char textBuf[2];
	int32 tmpNum;

	// Check for bad characters and number overflow
	switch(fCurrentCell)
	{
	// Hours
	case 1:
		GetText(0, 2, textBuf);
		if (textBuf[1] == ':')
			Insert(0, "0", 1);
		break;

	// Minutes
	case 2:
	{
		GetText(3, 2, textBuf);
		if (textBuf[1] == ':')
			Insert(3, "0", 1);

		// Get adjusted text
		GetText(3, 2, textBuf);
		tmpNum = atoi(textBuf);

		if (tmpNum > 59) {
			Delete(3, 5);
			Insert(3, "59", 2);
		}
	}
	break;

	// Seconds
	case 3:
	{
		GetText(6, 2, textBuf);
		if (textBuf[1] == ':')
			Insert(6, "0", 1);

		// Get adjusted text
		GetText(6, 2, textBuf);
		tmpNum = atoi(textBuf);

		if (tmpNum > 59) {
			Delete(6, 8);
			Insert(6, "59", 2);
		}
	}
	break;

	case 4:
	{
		GetText(8, 2, textBuf);
		if (textBuf[0] == ':')
			Insert(9, "0", 1);

		// Get adjusted text
		GetText(9, 2, textBuf);
		tmpNum = atoi(textBuf);

		switch (GetCurrentTimeFormat())
		{
		case B_TIMECODE_DEFAULT:
			if (tmpNum > 100) {
				Delete(9, 12);
				Insert(9, "99", 2);
			}
			break;

		case B_TIMECODE_24:
			if (tmpNum > 24) {
				Delete(9, 12);
				Insert(9, "24", 2);
			}
			break;

		case B_TIMECODE_25:
			if (tmpNum > 25) {
				Delete(9, 12);
				Insert(9, "25", 2);
			}
			break;

		case B_TIMECODE_30_DROP_2:
		case B_TIMECODE_30:
			if (tmpNum > 30) {
				Delete(9, 12);
				Insert(9, "30", 2);
			}
			break;
		}
	}
	break;

	default:
		break;
	}

	ConvertToTime(fCurrentCell);
}


#pragma mark -
#pragma mark === String Conversion Routines ===

//---------------------------------------------------------------------
//	ConvertToTime
//---------------------------------------------------------------------
//
//	Return time from displayed time string
//

void TTimeTextView::ConvertToTime(int16 theCell)
{
	int32 oldVal, delta;
	char textBuf[12];
	char tmpStr[3];
	int16 tmpNum;

	// Fill buffer with current text
	GetText(0, 11, textBuf);
	tmpStr[2] = '\n';

	/*
	   // Determine current time format and convert text to time
	   if ( GetCurrentTimeFormat() == kSMPTE30)
	   {
	        char	text[256];
	        bool	changed = false;
	        int16	hours, minutes, seconds, frames;

	        TimeToString(fTime, GetCurrentTimeFormat(), text, false);

	        // Stick the new value (if it changed) into the string and reverse calc the time
	        switch (theCell)
	        {
	                // Hours
	                case 1:
	                        oldVal = ((text[1] - '0') * 10) + (text[2] - '0');
	                        if (textBuf[0] != oldVal)
	                        {
	                                text[1] = textBuf[0] / 10 + '0';
	                                textBuf[0] %= 10;
	                                text[2] = textBuf[0] + '0';
	                                changed = true;
	                        }
	                        break;

	                // Minutes
	                case 2:
	                        oldVal = ((text[4] - '0') * 10) + (text[5] - '0');
	                        if (textBuf[1] != oldVal)
	                        {
	                                text[4] = textBuf[1] / 10 + '0';
	                                textBuf[1] %= 10;
	                                text[5] = textBuf[1] + '0';
	                                changed = true;
	                        }
	                        break;

	                // Seconds
	                case 3:
	                        oldVal = ((text[7] - '0') * 10) + (text[8] - '0');
	                        if (textBuf[2] != oldVal)
	                        {
	                                text[7] = textBuf[2] / 10 + '0';
	                                textBuf[2] %= 10;
	                                text[8] = textBuf[2] + '0';
	                                changed = true;
	                        }
	                        break;

	                // Frames
	                case 4:
	                        oldVal = ((text[10] - '0') * 10) + (text[11] - '0');

	                        if (textBuf[3] < 2 && text[7] == '0' && text[8] == '0' && text[5] != '0')
	                                textBuf[3] = 2;

	                        if (textBuf[3] != oldVal)
	                        {
	                                text[10] = textBuf[3] / 10 + '0';
	                                textBuf[3] %= 10;
	                                text[11] = textBuf[3] + '0';
	                                changed = true;
	                        }
	                        break;
	        }

	        if (changed)
	        {
	                hours = ((text[1] - '0') * 10) + (text[2] - '0');
	                minutes = ((text[4] - '0') * 10) + (text[5] - '0');
	                seconds = ((text[7] - '0') * 10) + (text[8] - '0');
	                frames = ((text[10] - '0') * 10) + (text[11] - '0');

	                fTime = (long)hours * 3600000L;				// Hours
	                fTime += (long)(minutes / 10) * 600000;		// Ten minutes
	                minutes %= 10;

	                if (minutes)
	                {
	                        // First minute
	                        fTime += 60060.066;
	                        fTime += ((double)(minutes - 1) * 59993.326);
	                }

	                if (seconds)
	                {
	                        // First second
	                        if (minutes)
	                                fTime += 934;
	                        else
	                                fTime += 1001;
	                        fTime += (long)(seconds - 1) * 1001;
	                }

	                if (frames >= 2 && !seconds && minutes)
	                        frames -= 2;

	                fTime += ((double)(frames + 0.5) * 33.3667);
	        }

	   }
	   else switch (theCell)
	 */
	switch (theCell)
	{
	// Hours
	case 1:
		oldVal = fTime / 3600000;

		strncpy(tmpStr, &textBuf[0], 2);
		tmpNum = atoi(tmpStr);

		if (tmpNum != oldVal) {
			delta = tmpNum - oldVal;
			fTime += (delta * 3600000);
		}
		break;

	// Minutes
	case 2:
		// Convert time to minutes
		oldVal = fTime % 3600000;
		oldVal /= 60000;

		strncpy(tmpStr, &textBuf[3], 2);
		tmpNum = atoi(tmpStr);

		if (tmpNum != oldVal) {
			delta = tmpNum - oldVal;
			fTime += (delta * 60000);
		}
		break;

	// Seconds
	case 3:
		oldVal = fTime % 3600000;
		oldVal %= 60000;
		oldVal /= 1000;

		strncpy(tmpStr, &textBuf[6], 2);
		tmpNum = atoi(tmpStr);

		if (tmpNum != oldVal) {
			delta = tmpNum - oldVal;
			fTime += (delta * 1000);
		}
		break;

	// Frames/100ths
	case 4:
		fTime /= 1000;
		fTime *= 1000;

		strncpy(tmpStr, &textBuf[9], 2);
		tmpNum = atoi(tmpStr);

		switch (GetCurrentTimeFormat())
		{

		case B_TIMECODE_DEFAULT:
			fTime += (tmpNum * 10);
			break;

		case B_TIMECODE_24:
			fTime += ((tmpNum * 1000 + 12) / 24);
			break;

		case B_TIMECODE_25:
			fTime += ((tmpNum * 1000 + 13) / 25);
			break;

		case B_TIMECODE_30_DROP_2:
		case B_TIMECODE_30:
			fTime += ((tmpNum * 1000 + 15) / 30);
			break;
		}
		break;
	}
}



#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	Reselect
//---------------------------------------------------------------------
//
//	Reselect text based on current cell
//

void TTimeTextView::Reselect()
{
	switch (fCurrentCell)
	{
	case 1:
		Select(0,2);
		break;

	case 2:
		Select(3,5);
		break;

	case 3:
		Select(6,8);
		break;

	case 4:
		Select(9,11);
		break;

	default:
		break;
	}
}


//---------------------------------------------------------------------
//	Lock
//---------------------------------------------------------------------
//
//	Disable input
//

void TTimeTextView::LockText()
{
	MakeFocus(false);
	Select(0,0);
	fIsLocked = true;
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//	Enable input
//

void TTimeTextView::UnlockText()
{
	fIsLocked = false;

}