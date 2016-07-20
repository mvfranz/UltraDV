//---------------------------------------------------------------------
//
//	File:	TCueTimeText.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.01.98
//
//	Desc:	This class implements the start and end time fields
//			that are displayed in the cue rect.  The user can select
//			individual fields and modify the time therein.
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

#include "AppConstants.h"
#include "AppUtils.h"

#include "TCueView.h"
#include "TCueChannel.h"
#include "TCueTimeText.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueTimeText::TCueTimeText(BHandler* target, int32 messageID, BRect bounds, const char* name, uint32 resizing, TextDrawModes drawMode) :
	BView(bounds, name, resizing, B_WILL_DRAW)
{

	fTarget                 = target;
	fMessageID      = messageID;
	fDrawMode               = drawMode;

	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create a new object based on one passed in
//


TCueTimeText::TCueTimeText(const TCueTimeText* theText) :
	BView(theText->Bounds(), theText->Name(), theText->ResizingMode(), B_WILL_DRAW)
{
	fTarget                 = theText->fTarget;
	fMessageID      = theText->fMessageID;

	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create from BMessage archive
//


TCueTimeText::TCueTimeText(BMessage* data) : BView(data)
{
	fTarget = NULL;

	// Get our member variables from the BMessage
	ssize_t numBytes;
	rgb_color* color;
	BFont* font;

	data->FindInt32("MessageID", &fMessageID);
	data->FindFloat("TextSize", &fTextSize);

	data->FindData("Font", B_ANY_TYPE, (const void**)&font, &numBytes);
	fFont = *font;

	data->FindData("Color", B_RGB_COLOR_TYPE, (const void**)&color, &numBytes);
	fColor = *color;

	data->FindInt16("DrawMode", &fDrawMode);

	// Set up our key stroke counter
	fKeyCount = 0;

	// Set up font
	SetFont(&fFont);

	// Perform default initialization
	sprintf(fText, "00:00:00:00");

	//      Set up time rects.  We divide the area up into four areas
	//	for hour, minutes, seconds and frames
	float numberWidth;
	float spaceWidth;

	// Get bound and adjust for border type
	BRect bounds = Bounds();
	switch(fDrawMode)
	{
	case kTextBevel:
		bounds.left+=2;
		bounds.right-=2;
		bounds.top+=2;
		bounds.bottom-=2;
		break;

	case kTextOutline:
		bounds.left++;
		bounds.right--;
		bounds.top++;
		bounds.bottom--;
		break;

	default:
		bounds.left++;
		bounds.right--;
		bounds.top++;
		bounds.bottom--;
		break;
	}

	// Get text drawing location
	float startX;
	float width = fFont.StringWidth(fText);
	font_height fh;
	fFont.GetHeight(&fh);
	startX = (bounds.right - width)/2;

	numberWidth = StringWidth("00");
	spaceWidth      = StringWidth(":");

	fHoursRect.left                 = bounds.left;
	fHoursRect.top          = bounds.top;
	fHoursRect.right                = fHoursRect.left + startX + numberWidth;
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

	// Set cell to none
	fCurrentCell = 0;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueTimeText::~TCueTimeText()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Default initialization
//

void TCueTimeText::Init()
{
	// Set up our key stroke counter
	fKeyCount = 0;

	// Set up font
	BFont font(be_plain_font);
	fFont = font;
	fFont.SetSize(10.0);
	SetFont(&fFont);
	fColor = kBlack;

	// Perform default initialization
	SetViewColor(kWhite);
	sprintf(fText, "00:00:00:00");

	//      Set up time rects.  We divide the area up into four areas
	//	for hour, minutes, seconds and frames
	float numberWidth;
	float spaceWidth;

	// Get bounds and adjust for border type
	BRect bounds = Bounds();
	switch(fDrawMode)
	{
	case kTextBevel:
		bounds.left+=2;
		bounds.right-=2;
		bounds.top+=2;
		bounds.bottom-=2;
		break;

	case kTextOutline:
		bounds.left++;
		bounds.right--;
		bounds.top++;
		bounds.bottom--;
		break;

	default:
		bounds.left++;
		bounds.right--;
		bounds.top++;
		bounds.bottom--;
		break;
	}

	// Get text drawing location
	float startX;
	float width = fFont.StringWidth(fText);
	font_height fh;
	fFont.GetHeight(&fh);
	startX = (bounds.right - width)/2;

	numberWidth = StringWidth("00");
	spaceWidth      = StringWidth(":");

	fHoursRect.left                 = bounds.left;
	fHoursRect.top          = bounds.top;
	fHoursRect.right                = fHoursRect.left + startX + numberWidth;
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

	// Set cell to none
	fCurrentCell = 0;
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TCueTimeText::Instantiate(BMessage* archive)
{
	if ( validate_instantiation(archive, "TCueTimeText") )
		return new TCueTimeText(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueTimeText::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueTimeText");

		// Add our member variables to the BMessage
		data->AddInt32("MessageID", fMessageID);
		data->AddFloat("TextSize", fTextSize);
		data->AddData("Font", B_ANY_TYPE, &fFont, sizeof(BFont) );
		data->AddData("Color", B_RGB_COLOR_TYPE, &fColor, sizeof(rgb_color) );
		data->AddInt16("DrawMode", fDrawMode);
	}

	Looper()->Unlock();

	return myErr;
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent.
//	This is intended for use when instantiating cues from an archive.
//

void TCueTimeText::AttachedToWindow()
{
	if (fTarget == NULL) {
		fTarget = (TCueView*)Parent();
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TCueTimeText::Draw(BRect updateRect)
{
	// Save state
	PushState();

	//	Fill background
	SetHighColor(kWhite);
	FillRect(updateRect);

	switch (fDrawMode)
	{
	case kTextBevel:
	{
		// Draw standard Be Style bevel
		BPoint startPt, endPt;
		BRect bounds = Bounds();
		bounds.InsetBy(1, 1);

		// Draw blue outline is text view has focus
		if (IsFocus())
			DrawHighliteRect();
		else{
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
	}
	break;

	// Draw outline
	case kTextOutline:
	{
		BRect bounds = Bounds();
		SetHighColor(kTextHilite);
		BPoint startPt, endPt;
		startPt.Set(bounds.right, bounds.top);
		endPt.Set(bounds.right, bounds.bottom);
		StrokeLine(startPt, endPt);
		startPt.Set(bounds.right, bounds.bottom);
		endPt.Set(bounds.left+1, bounds.bottom);
		StrokeLine(startPt, endPt);
		SetHighColor(kTextShadow);
		startPt.Set(bounds.left, bounds.bottom);
		endPt.Set(bounds.left, bounds.top);
		StrokeLine(startPt, endPt);
		startPt.Set(bounds.left, bounds.top);
		endPt.Set(bounds.right-1, bounds.top);
		StrokeLine(startPt, endPt);
	}
	break;

	default:
		break;

	}
	// Draw text
	SetHighColor(fColor);

	// Center text
	BPoint drawPt;
	BRect bounds = Bounds();
	float width = fFont.StringWidth(fText);
	font_height fh;
	fFont.GetHeight(&fh);
	drawPt.x = (bounds.right - width)/2;
	drawPt.y = (bounds.Height() / 2)+4;

	DrawString(fText, drawPt);

	// Invert selected cell
	InvertCurrentCell();

	// Restore
	PopState();
}


//---------------------------------------------------------------------
//	DrawHighliteRect
//---------------------------------------------------------------------
//
//	Draw Be style blue focus highlite
//

void TCueTimeText::DrawHighliteRect()
{
	BRect bounds = Bounds();
	bounds.InsetBy(1, 1);

	SetHighColor(kBeFocusBlue);
	StrokeRect(bounds);
}


//---------------------------------------------------------------------
//	ClearHighliteRect
//---------------------------------------------------------------------
//
//	Clear the blue focus highlite
//

void TCueTimeText::ClearHighliteRect()
{
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
	startPt.Set(bounds.right, bounds.top+1);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left+1, bounds.bottom);
	StrokeLine(startPt, endPt);
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TCueTimeText::MouseDown(BPoint where)
{
	// Do nothing if locked...
	if (fTarget) {
		if ( ((TCueView*)fTarget)->IsLocked() || ((TCueView*)fTarget)->GetChannel()->IsLocked() )
			return;
	}

	// Check internal lock flag
	if (fIsLocked)
		return;

	// We now have focus
	if ( IsFocus() == false)
		MakeFocus(true);

	// Reset key counter
	fKeyCount = 0;

	// Verify last entry
	CheckLastEdit();

	// Determine where the click is
	if ( fHoursRect.Contains(where) ) {
		if (fCurrentCell != 1) {
			InvertCurrentCell();
			InvertRect(fHoursRect);
			fCurrentCell = 1;
		}
	} else if ( fMinutesRect.Contains(where) ) {
		if (fCurrentCell != 2) {
			InvertCurrentCell();
			InvertRect(fMinutesRect);
			fCurrentCell = 2;
		}
	} else if ( fSecondsRect.Contains(where) ) {
		if (fCurrentCell != 3) {
			InvertCurrentCell();
			InvertRect(fSecondsRect);
			fCurrentCell = 3;
		}
	} else if ( fFramesRect.Contains(where) ) {
		if (fCurrentCell != 4) {
			InvertCurrentCell();
			InvertRect(fFramesRect);
			fCurrentCell = 4;
		}
	}
	// Default to first cell if all else fails
	else{
		InvertCurrentCell();
		InvertRect(fHoursRect);
		fCurrentCell = 1;
	}

	BView::MouseDown(where);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TCueTimeText::MouseUp(BPoint where)
{
	// Do nothing if locked...
	if (fTarget) {
		if ( ((TCueView*)fTarget)->IsLocked() || ((TCueView*)fTarget)->GetChannel()->IsLocked() )
			return;
	}

	BView::MouseUp(where);
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TCueTimeText::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	// Do nothing if locked...
	//if (fTarget)
	//{
	//	if ( ((TCueView *)fTarget)->IsLocked() || ((TCueView *)fTarget)->GetChannel()->IsLocked() )
	//	return;
	//}

	ConvertToParent(&where);
	Parent()->MouseMoved(where, code, message);
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TCueTimeText::KeyDown(const char* bytes, int32 numBytes)
{
	char theChar = *bytes;

	// Do nothing if locked...
	if (fTarget) {
		if ( ((TCueView*)fTarget)->IsLocked() || ((TCueView*)fTarget)->GetChannel()->IsLocked() )
			return;
	}

	// Increment our counter
	fKeyCount++;
	if (fKeyCount > 2)
		fKeyCount = 1;

	//	Check for a return or enter
	if (theChar == B_ENTER || theChar == B_RETURN || theChar == 'r') {
		MakeFocus(false);
	}
	// We have a number.  Update the cell...
	else if ( isdigit(theChar) ) {
		UpdateCurrentCell(theChar);
	}
	// Reset selected cell to zero
	else if ( IsDelete(theChar) || IsBackspace(theChar) ) {
		//ClearCell();
	}
	// Tab key or period '.' moves us through the time elements
	else if( IsTab(theChar) || theChar == '.' ) {
		// Check for shift key.  This move us back through the elements
		if ( IsShiftKeyDown() )
			DecrementCell();
		else
			IncrementCell();
	} else if ( IsArrows(theChar) ) {
		switch(theChar)
		{
		// Increment value
		case B_UP_ARROW:
			break;

		// Decrement value
		case B_DOWN_ARROW:
			break;

		// Select next cell
		case B_LEFT_ARROW:
			DecrementCell();
			break;

		// Select previous cell
		case B_RIGHT_ARROW:
			IncrementCell();
			break;
		}
	}
	// Illegal character
	else
		; //beep();
}


//---------------------------------------------------------------------
//	MakeFocus
//---------------------------------------------------------------------
//
//


void TCueTimeText::MakeFocus(bool focusState)
{
	// Do nothing if locked...
	if (fTarget) {
		if ( ((TCueView*)fTarget)->IsLocked() || ((TCueView*)fTarget)->GetChannel()->IsLocked() )
			return;
	}

	// If we are out of focus, deselect cells
	if (focusState == false) {
		if (fTarget) {
			BLooper* looper = fTarget->Looper();

			if ( looper->Lock() ) {
				// Check integrity of last edited value
				CheckLastEdit();

				for (int16 index = kHoursCell; index < kTotalCells; index++)
					ConvertToTime(index);

				BMessage* message = new BMessage(fMessageID);
				message->AddInt32("TheTime", fTime);
				fTarget->MessageReceived( message);
				looper->Unlock();
				delete message;
			}
		}

		InvertCurrentCell();
		fCurrentCell = 0;

		// If we are in bevel mode, clear highlite rect
		if (fDrawMode == kTextBevel)
			ClearHighliteRect();
	}
	// We are getting focus
	else{
		//	If we are in bevel mode, force a redraw to update Highlite
		if (fDrawMode == kTextBevel)
			DrawHighliteRect();
	}

	BView::MakeFocus(focusState);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TCueTimeText::MessageReceived(BMessage* message)
{
	// Do nothing if locked...
	if (fTarget) {
		if ( ((TCueView*)fTarget)->IsLocked() || ((TCueView*)fTarget)->GetChannel()->IsLocked() )
			return;
	}

	BView::MessageReceived(message);
}


#pragma mark -
#pragma mark === Text Routines ===

//---------------------------------------------------------------------
//	SetText
//---------------------------------------------------------------------
//
//	Set the vie BFont and Font Color
//

void TCueTimeText::SetFontAndColor(BFont* theFont, uint32 properties, rgb_color theColor)
{
	// Update our member variables
	fFont   = theFont;
	fColor = theColor;

	SetFont(&fFont);
}


//---------------------------------------------------------------------
//	SetText
//---------------------------------------------------------------------
//
//	Set the text to the Insert text
//

void TCueTimeText::SetText(char* theText)
{

	sprintf(fText, theText);

	// Force redraw
	if ( Looper()->Lock()) {
		Invalidate();
		Looper()->Unlock();
	}
}


//---------------------------------------------------------------------
//	UpdateCell
//---------------------------------------------------------------------
//
//	Set cell to character
//

void TCueTimeText::UpdateCurrentCell(char theChar)
{

	BRect updateRect;
	updateRect.left         = fHoursRect.left;
	updateRect.top          = fHoursRect.top;
	updateRect.right        = fFramesRect.right;
	updateRect.bottom       = fFramesRect.bottom;


	switch(fCurrentCell)
	{
	case kHoursCell:
	{
		if (fKeyCount == 1)
			strcpy( &fText[0], "0");
		else
			fText[0]        = fText[1];

		fText[1]        = theChar;

		//Invalidate(fHoursRect);
		Invalidate(updateRect);
	}
	break;

	case kMinutesCell:
	{
		if (fKeyCount == 1)
			strcpy( &fText[3], "0");
		else
			fText[3]        = fText[4];

		fText[4]        = theChar;

		//Invalidate(fMinutesRect);
		Invalidate(updateRect);
	}
	break;

	case kSecondsCell:
	{
		if (fKeyCount == 1)
			strcpy( &fText[6], "0");
		else
			fText[6]        = fText[7];

		fText[7]        = theChar;

		//Invalidate(fSecondsRect);
		Invalidate(updateRect);
	}
	break;

	// Set the frames to the entered value
	case kFramesCell:
	{
		if (fKeyCount == 1)
			strcpy( &fText[9], "0");
		else
			fText[9]        = fText[10];

		fText[10]       = theChar;

		//Invalidate(fFramesRect);
		Invalidate(updateRect);
	}
	break;

	default:
		break;
	}
}


//---------------------------------------------------------------------
//	CheckLastEdit
//---------------------------------------------------------------------
//
//	Check for valid data after a user edit
//

void TCueTimeText::CheckLastEdit()
{
	char textBuf[2];
	int32 tmpNum;

	// Check for bad characters and number overflow
	switch(fCurrentCell)
	{
	case kHoursCell:
		strncpy(textBuf, &fText[0], 2);
		break;

	case kMinutesCell:
	{
		strncpy(textBuf, &fText[3], 2);

		tmpNum = atoi(textBuf);

		if (tmpNum > 59)
			strncpy(&fText[3], "59", 2);

		Invalidate(fMinutesRect);
	}
	break;

	case kSecondsCell:
	{
		strncpy(textBuf, &fText[6], 2);

		tmpNum = atoi(textBuf);

		if (tmpNum > 59)
			strncpy(&fText[6], "59", 2);

		Invalidate(fSecondsRect);
	}
	break;

	case kFramesCell:
	{
		strncpy(textBuf, &fText[9], 2);

		tmpNum = atoi(textBuf);

		switch (GetCurrentTimeFormat())
		{
		case B_TIMECODE_DEFAULT:
			if (tmpNum > 100)
				strncpy(&fText[9], "99", 2);
			break;

		case B_TIMECODE_24:
			if (tmpNum > 24)
				strncpy(&fText[9], "24", 2);
			break;

		case B_TIMECODE_25:
			if (tmpNum > 25)
				strncpy(&fText[9], "25", 2);
			break;

		case B_TIMECODE_30_DROP_2:
		case B_TIMECODE_30:
			if (tmpNum > 30)
				strncpy(&fText[9], "30", 2);
			break;
		}
		Invalidate(fFramesRect);
	}
	break;

	default:
		break;
	}

	ConvertToTime(fCurrentCell);
}


#pragma mark -
#pragma mark === Input Routines ===


//---------------------------------------------------------------------
//	IsBackspace
//---------------------------------------------------------------------
//
//	Check to see if character is backspace
//

bool TCueTimeText::IsBackspace(char theChar)
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

bool TCueTimeText::IsArrows(char theChar)
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

bool TCueTimeText::IsDelete(char theChar)
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

bool TCueTimeText::IsTab(char theChar)
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
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	Lock
//---------------------------------------------------------------------
//
//	Disable input
//

void TCueTimeText::LockText()
{
	//MakeFocus(false);
	//Select(0,0);
	fIsLocked = true;
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//	Enable input
//

void TCueTimeText::UnlockText()
{
	fIsLocked = false;
}


//---------------------------------------------------------------------
//	InvertCurrentCell
//---------------------------------------------------------------------
//
//	Invert the currently selected text cell
//

void TCueTimeText::InvertCurrentCell()
{
	switch(fCurrentCell)
	{
	case 1:
		InvertRect(fHoursRect);
		break;

	case 2:
		InvertRect(fMinutesRect);
		break;

	case 3:
		InvertRect(fSecondsRect);
		break;

	case 4:
		InvertRect(fFramesRect);
		break;

	default:
		break;
	}
}


//---------------------------------------------------------------------
//	DecrementCell
//---------------------------------------------------------------------
//
//	Decrement the currently selected text cell
//

void TCueTimeText::DecrementCell()
{
	// Reset key counter
	fKeyCount = 0;

	// Verify last text entry as valid
	CheckLastEdit();

	// Clean up old cell
	InvertCurrentCell();

	// Select the previous cell
	fCurrentCell--;

	// Check for overflow
	if (fCurrentCell <= 0)
		fCurrentCell = 4;

	// Select new cell
	InvertCurrentCell();

}


//---------------------------------------------------------------------
//	IncrementCell
//---------------------------------------------------------------------
//
//	Increment the currently selected text cell
//

void TCueTimeText::IncrementCell()
{
	// Reset key counter
	fKeyCount = 0;

	// Verify last text entry as valid
	CheckLastEdit();

	// Clean up old cell
	InvertCurrentCell();

	// Select the next cell
	fCurrentCell++;

	// Check for overflow
	if (fCurrentCell > 4)
		fCurrentCell = 1;

	// Clean up old cell
	InvertCurrentCell();
}

#pragma mark -
#pragma mark === String Conversion Routines ===

//---------------------------------------------------------------------
//	ConvertToTime
//---------------------------------------------------------------------
//
//	Return time from displayed time string
//

void TCueTimeText::ConvertToTime(int16 theCell)
{
	int32 oldVal, delta;
	char textBuf[12];
	char tmpStr[3];
	int16 tmpNum;

	// Fill buffer with current text
	strncpy(textBuf, fText, 12);
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
	                        fTime += ((uint32)(minutes - 1) * 59993.326);
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

	                fTime += ((uint32)(frames + 0.5) * 33.3667);
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


//---------------------------------------------------------------------
//	GetTime
//---------------------------------------------------------------------
//
//	Return time as a string
//

int32 TCueTimeText::GetTime()
{
	// Convert all cells
	for (int16 index = kHoursCell; index < kTotalCells; index++) {
		ConvertToTime(index);
	}

	return fTime;
}
