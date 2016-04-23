//---------------------------------------------------------------------
//
//	File:	TTimeText.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	Implimentation of the view that displays the time in the text
//			palette.  The playback time is displayed here as well as feedback
//			during the movement of cues.
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
#include "AppMessages.h"
#include "AppUtils.h"

#include "TCueView.h"
#include "TCueChannel.h"

#include "TTimeText.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimeText::TTimeText(BHandler *target, int32 messageID, BRect bounds, char *name, uint32 resizing) : 
			  BView(bounds, name, resizing, B_WILL_DRAW)
{	

	m_Target 		= target;
	m_MessageID  	= messageID;	
	
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create a new object based on one passed in
//


TTimeText::TTimeText(const TTimeText *theText) : 
			  BView(theText->Bounds(), theText->Name(), theText->ResizingMode(), B_WILL_DRAW)
{	

	m_Target 		= theText->m_Target;
	m_MessageID  	= theText->m_MessageID;	
	
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create from BMessage archive
//


TTimeText::TTimeText(BMessage *data) : BView(data)
{	
	// Get our member variables from the BMessage
	ssize_t 	numBytes;
	rgb_color	*color;
	BFont 		*font;

	data->FindInt32("MessageID", &m_MessageID);		
	data->FindFloat("TextSize", &m_TextSize);		

	data->FindData("Font", B_ANY_TYPE, (const void **)&font, &numBytes);
	m_Font = *font;
	
	data->FindData("Color", B_RGB_COLOR_TYPE, (const void **)&color, &numBytes);
	m_Color = *color;
	
	// Set up our key stroke counter
	m_KeyCount = 0;
	
	// Set up font
   	SetFont(&m_Font);
   	
   	// Perform default initialization
	sprintf(m_Text, "00:00:00:00");
	
	// 	Set up time rects.  We divide the area up into four areas
	//	for hour, minutes, seconds and frames
	float 	numberWidth;
	float 	spaceWidth;

	// Get bound and adjust for border type
	BRect bounds = Bounds();
	bounds.left++;
	bounds.right--;
	bounds.top++;
	bounds.bottom--;
				
	// Get text drawing location
	float startX;
	float width = m_Font.StringWidth(m_Text);
	font_height fh;
	m_Font.GetHeight(&fh);	
	startX = (bounds.right - width)/2;
	
	numberWidth = StringWidth("00");
	spaceWidth 	= StringWidth(":");
	
	m_HoursRect.left 		= bounds.left;
	m_HoursRect.top 		= bounds.top;
	m_HoursRect.right 		= m_HoursRect.left + startX + numberWidth;
	m_HoursRect.bottom 		= bounds.bottom;
	
	m_MinutesRect.left 		= m_HoursRect.right + spaceWidth;
	m_MinutesRect.top 		= bounds.top;
	m_MinutesRect.right 	= m_MinutesRect.left + numberWidth;
	m_MinutesRect.bottom 	= bounds.bottom;
	
	m_SecondsRect.left 		= m_MinutesRect.right + spaceWidth;
	m_SecondsRect.top 		= bounds.top;
	m_SecondsRect.right 	= m_SecondsRect.left + numberWidth;
	m_SecondsRect.bottom 	= bounds.bottom;
	
	m_FramesRect.left 		= m_SecondsRect.right + spaceWidth;
	m_FramesRect.top 		= bounds.top;
	m_FramesRect.right 		= bounds.right;
	m_FramesRect.bottom 	= bounds.bottom;
	
	// Set cell to none
	m_CurrentCell = 0;
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimeText::~TTimeText()
{
 
	//	Free offscreen and view
	if (m_OffscreenBitmap)
 	{
 		m_OffscreenBitmap->Lock();	
 		m_OffscreenBitmap->RemoveChild(m_OffscreenView);
 		delete m_OffscreenView;	
 		delete m_OffscreenBitmap;	
 	}
	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Default initialization
//

void TTimeText::Init()
{
	// Set up our key stroke counter
	m_KeyCount = 0;
	
	//	Create offscreen bitmap and view
	m_OffscreenBitmap 	= new BBitmap(Bounds(), B_CMAP8, true);
	m_OffscreenView 	= new BView( m_OffscreenBitmap->Bounds(), "OffscreenView", B_FOLLOW_ALL, B_WILL_DRAW);
	
	// Add child and fill with background color
	if ( m_OffscreenBitmap->Lock() )
	{
		m_OffscreenBitmap->AddChild(m_OffscreenView);
		m_OffscreenView->SetViewColor(kBlack);
		m_OffscreenView->SetLowColor(ViewColor());		
		m_OffscreenView->FillRect(m_OffscreenView->Bounds());
		m_OffscreenView->SetFont(&m_Font);
		m_OffscreenView->Sync();
		m_OffscreenBitmap->Unlock();
	}
			
	// Perform default initialization
	sprintf(m_Text, "00:00:00:00");
		
	// 	Set up time rects.  We divide the area up into four areas
	//	for hour, minutes, seconds and frames
	float 	numberWidth;
	float 	spaceWidth;

	// Get text drawing location
	BRect bounds = Bounds();
	float startX;
	float width = m_Font.StringWidth(m_Text);
	startX = (bounds.right - width)/2;
	m_DrawPt.x = bounds.left + 2;
	m_DrawPt.y = (bounds.Height() / 2) + 8;
	
	numberWidth = StringWidth("00");
	spaceWidth 	= StringWidth(":");
	
	m_HoursRect.left 		= bounds.left;
	m_HoursRect.top 		= bounds.top;
	m_HoursRect.right 		= m_HoursRect.left + startX + numberWidth;
	m_HoursRect.bottom 		= bounds.bottom;
	
	m_MinutesRect.left 		= m_HoursRect.right + spaceWidth;
	m_MinutesRect.top 		= bounds.top;
	m_MinutesRect.right 	= m_MinutesRect.left + numberWidth;
	m_MinutesRect.bottom 	= bounds.bottom;
	
	m_SecondsRect.left 		= m_MinutesRect.right + spaceWidth;
	m_SecondsRect.top 		= bounds.top;
	m_SecondsRect.right 	= m_SecondsRect.left + numberWidth;
	m_SecondsRect.bottom 	= bounds.bottom;
	
	m_FramesRect.left 		= m_SecondsRect.right + spaceWidth;
	m_FramesRect.top 		= bounds.top;
	m_FramesRect.right 		= bounds.right;
	m_FramesRect.bottom 	= bounds.bottom;
	
	// Set cell to none
	m_CurrentCell = 0;
	
	// Set time to 0
	m_Time = 0;
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TTimeText::Instantiate(BMessage *archive) 
{ 
	if ( validate_instantiation(archive, "TTimeText") ) 
		return new TTimeText(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TTimeText::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{				
		// Add our class name to the archive
		data->AddString("class", "TTimeText");
		
		// Add our member variables to the BMessage
		data->AddInt32("MessageID", m_MessageID);		
		data->AddFloat("TextSize", m_TextSize);		
		data->AddData("Font", B_ANY_TYPE, &m_Font, sizeof(BFont) );
		data->AddData("Color", B_RGB_COLOR_TYPE, &m_Color, sizeof(rgb_color) );
	}
	
	Looper()->Unlock();
		
	return myErr;
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TTimeText::Draw(BRect updateRect)
{
	if (LockLooper())
	{	
		if ( m_OffscreenView->LockLooper())
		{
			m_OffscreenView->PushState();
			
			// Clear out old text
			m_OffscreenView->SetHighColor(m_OffscreenView->ViewColor());
			m_OffscreenView->FillRect( m_OffscreenView->Bounds() );
			
			// Draw Text
			m_OffscreenView->SetHighColor(m_Color);	
			m_OffscreenView->DrawString(m_Text, m_DrawPt);
			m_OffscreenView->Sync();
			
			m_OffscreenView->PopState();
			
			m_OffscreenView->UnlockLooper();
		}
		
		//	Copy offscreen bitmap to view
		DrawBitmap(m_OffscreenBitmap);
		
		UnlockLooper();
	}
}

#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TTimeText::MouseDown(BPoint where)
{
	/*
	// We now have focus
	if ( IsFocus() == false)
		MakeFocus(true);
	
	// Reset key counter
	m_KeyCount = 0;
	
	// Verify last entry
	CheckLastEdit();

	// Determine where the click is
	if ( m_HoursRect.Contains(where) ) 
	{
		if (m_CurrentCell != 1)
		{
			InvertCurrentCell();
			InvertRect(m_HoursRect);
			m_CurrentCell = 1;
		}
	}
	else if ( m_MinutesRect.Contains(where) ) 
	{
		if (m_CurrentCell != 2)
		{
			InvertCurrentCell();
			InvertRect(m_MinutesRect);
			m_CurrentCell = 2;
		}
	}

	else if ( m_SecondsRect.Contains(where) ) 
	{
		if (m_CurrentCell != 3)
		{
			InvertCurrentCell();
			InvertRect(m_SecondsRect);
			m_CurrentCell = 3;
		}
	}

	else if ( m_FramesRect.Contains(where) ) 
	{
		if (m_CurrentCell != 4)
		{
			InvertCurrentCell();
			InvertRect(m_FramesRect);
			m_CurrentCell = 4;
		}
	}
	// Default to first cell if all else fails
	else
	{
		InvertCurrentCell();
		InvertRect(m_HoursRect);
		m_CurrentCell = 1;
	}
	*/		
	BView::MouseDown(where);	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TTimeText::MouseUp(BPoint where)
{	
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TTimeText::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//

void TTimeText::KeyDown(const char *bytes, int32 numBytes)
{
	char theChar = *bytes;
			
	// Increment our counter
	m_KeyCount++;
	if (m_KeyCount > 2)
		m_KeyCount = 1;
		
	//	Check for a return or enter 
	if (theChar == B_ENTER || theChar == B_RETURN || theChar == 'r')
	{
		MakeFocus(false);
	}		
	// We have a number.  Update the cell...
	else if ( isdigit(theChar) )
	{
		UpdateCurrentCell(theChar);	
	}
	// Reset selected cell to zero
	else if ( IsDelete(theChar) || IsBackspace(theChar) )
	{
		//ClearCell();
	}
	// Tab key or period '.' moves us through the time elements
	else if( IsTab(theChar) || theChar == '.' )
	{	
		// Check for shift key.  This move us back through the elements
		if ( IsShiftKeyDown() )		
			DecrementCell();
		else
			IncrementCell();
	}
	else if ( IsArrows(theChar) )
	{
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
		;//beep();
}


//---------------------------------------------------------------------
//	MakeFocus
//---------------------------------------------------------------------
//
//

	
void TTimeText::MakeFocus(bool focusState)
{
	/*
	// If we are out of focus, deselect cells
	if (focusState == false)
	{		
		if (m_Target)
		{
			BLooper *looper = m_Target->Looper();
			
			if ( looper->Lock() )
			{
				// Check integrity of last edited value
				CheckLastEdit();
				
				for (int16 index = kHoursCell; index < kTotalCells; index++)
					ConvertToTime(index);
				
				BMessage *message = new BMessage(m_MessageID);
				message->AddInt32("TheTime", m_Time);
				m_Target->MessageReceived( message);
				looper->Unlock();
				delete message;
			}			
		}
		
		InvertCurrentCell();
		m_CurrentCell = 0;
		
	}
	// We are getting focus	
	else
	{
	}
	*/	
	BView::MakeFocus(focusState);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TTimeText::MessageReceived(BMessage *theMessage)
{
	switch( theMessage->what)
	{			
		// 	Time is inferred from point.  This is used during a mouse over the cue sheet
		case UPDATE_TIMELINE_MSG:
			{ 
				uint32 theTime = theMessage->FindInt32("TheTime");

				// Don't allow negative times to slip through
				if (theTime >= 0)
				{
					TimeToString(theTime, GetCurrentTimeFormat(), m_Text, false);
					Draw(Bounds());										
				}
			}
			break;
			
		default:
			break;
		
	}						
}


#pragma mark -
#pragma mark === Text Routines ===

//---------------------------------------------------------------------
//	SetFontAndColor
//---------------------------------------------------------------------
//
//	Set the view BFont and Font Color
//
		
void TTimeText::SetFontAndColor(BFont *theFont, uint32 properties, rgb_color theColor)
{
	// Update our member variables
	m_Font 	= theFont;
	m_Color = theColor;
	
	if ( m_OffscreenView->LockLooper())
	{
		m_OffscreenView->SetFont(&m_Font);
   		m_OffscreenView->SetHighColor(m_Color);
   		m_OffscreenView->SetLowColor(m_OffscreenView->ViewColor());
   		m_OffscreenView->UnlockLooper();
   	}	   	
}


//---------------------------------------------------------------------
//	SetText
//---------------------------------------------------------------------
//
//	Set the text to the Insert text
//

void TTimeText::SetText(char *theText)
{
	sprintf(m_Text, theText);
	
	if (LockLooper())
	{
		Draw(Bounds()); 
		UnlockLooper();
	}
}


//---------------------------------------------------------------------
//	UpdateCell
//---------------------------------------------------------------------
//
//	Set cell to character
//

void TTimeText::UpdateCurrentCell(char theChar)
{
		
	BRect updateRect;
	updateRect.left 	= m_HoursRect.left;
	updateRect.top 		= m_HoursRect.top;
	updateRect.right 	= m_FramesRect.right;
	updateRect.bottom 	= m_FramesRect.bottom;
	
	
	switch(m_CurrentCell)
	{
		case kHoursCell:
			{
				if (m_KeyCount == 1)
					strcpy( &m_Text[0], "0");
				else
					m_Text[0] 	= m_Text[1];
					
				m_Text[1] 	= theChar;
			
				//Invalidate(m_HoursRect);
				Invalidate(updateRect);
			}
			break;
			
		case kMinutesCell:
			{
				if (m_KeyCount == 1)
					strcpy( &m_Text[3], "0");
				else
					m_Text[3] 	= m_Text[4];
					
				m_Text[4] 	= theChar;
			
				//Invalidate(m_MinutesRect);
				Invalidate(updateRect);
			}
			break;
			
		case kSecondsCell:
			{
				if (m_KeyCount == 1)
					strcpy( &m_Text[6], "0");
				else
					m_Text[6] 	= m_Text[7];
					
				m_Text[7] 	= theChar;
			
				//Invalidate(m_SecondsRect);
				Invalidate(updateRect);
			}
			break;
			
		// Set the frames to the entered value
		case kFramesCell:
			{				
				if (m_KeyCount == 1)
					strcpy( &m_Text[9], "0");
				else
					m_Text[9] 	= m_Text[10];
					
				m_Text[10] 	= theChar;
					
				//Invalidate(m_FramesRect);
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

void TTimeText::CheckLastEdit()
{
	char 	textBuf[2];
	int32	tmpNum;
	
	// Check for bad characters and number overflow
	switch(m_CurrentCell)
	{
		case kHoursCell:
			strncpy(textBuf, &m_Text[0], 2);
			break;
			
		case kMinutesCell:
			{
				strncpy(textBuf, &m_Text[3], 2);

				tmpNum = atoi(textBuf);
				
				if (tmpNum > 59)
					strncpy(&m_Text[3], "59", 2);
			
				Invalidate(m_MinutesRect);
			}
			break;
			
		case kSecondsCell:
			{
				strncpy(textBuf, &m_Text[6], 2);
				
				tmpNum = atoi(textBuf);
				
				if (tmpNum > 59)
					strncpy(&m_Text[6], "59", 2);

				Invalidate(m_SecondsRect);
			}			
			break;
					
		case kFramesCell:
			{
				strncpy(textBuf, &m_Text[9], 2);
				
				tmpNum = atoi(textBuf);

				switch (GetCurrentTimeFormat())
				{
					case B_TIMECODE_DEFAULT:
						if (tmpNum > 100)
							strncpy(&m_Text[9], "99", 2);
						break;
					
					case B_TIMECODE_24:
						if (tmpNum > 24)
							strncpy(&m_Text[9], "24", 2);
						break;
						
					case B_TIMECODE_25:
						if (tmpNum > 25)
							strncpy(&m_Text[9], "25", 2);
						break;
						
					case B_TIMECODE_30_DROP_2:
					case B_TIMECODE_30:						
						if (tmpNum > 30)
							strncpy(&m_Text[9], "30", 2);
						break;																
				}					
				Invalidate(m_FramesRect);							
			}
			break;
			
		default:
			break;
	}
		
	ConvertToTime(m_CurrentCell);
}


#pragma mark -
#pragma mark === Input Routines ===


//---------------------------------------------------------------------
//	IsBackspace
//---------------------------------------------------------------------
//
//	Check to see if character is backspace
//

bool  TTimeText::IsBackspace(char theChar)
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

bool  TTimeText::IsArrows(char theChar)
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

bool  TTimeText::IsDelete(char theChar)
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

bool  TTimeText::IsTab(char theChar)
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

void TTimeText::LockText()
{
	//MakeFocus(false);
	//Select(0,0);
	m_IsLocked = true;
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//	Enable input
//

void TTimeText::UnlockText()
{
	m_IsLocked = false;	
}


//---------------------------------------------------------------------
//	InvertCurrentCell
//---------------------------------------------------------------------
//
//	Invert the currently selected text cell
//

void TTimeText::InvertCurrentCell()
{
	switch(m_CurrentCell)
	{
		case 1:
			InvertRect(m_HoursRect);
			break;

		case 2:
			InvertRect(m_MinutesRect);
			break;

		case 3:
			InvertRect(m_SecondsRect);
			break;

		case 4:
			InvertRect(m_FramesRect);
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

void TTimeText::DecrementCell()
{
	// Reset key counter
	m_KeyCount = 0;
	
	// Verify last text entry as valid
	CheckLastEdit();
	
	// Clean up old cell
	InvertCurrentCell();

	// Select the previous cell			
	m_CurrentCell--;
			
	// Check for overflow
	if (m_CurrentCell <= 0)
		m_CurrentCell = 4;			

	// Select new cell
	InvertCurrentCell();
	
}


//---------------------------------------------------------------------
//	IncrementCell
//---------------------------------------------------------------------
//
//	Increment the currently selected text cell
//

void TTimeText::IncrementCell()
{
	// Reset key counter
	m_KeyCount = 0;

	// Verify last text entry as valid
	CheckLastEdit();

	// Clean up old cell
	InvertCurrentCell();

	// Select the next cell
	m_CurrentCell++;

	// Check for overflow
	if (m_CurrentCell > 4)
		m_CurrentCell = 1;						
	
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

void TTimeText::ConvertToTime(int16 theCell)
{
	int32 	oldVal, delta;	
	char 	textBuf[12];
	char	tmpStr[3];
	int16 	tmpNum;
	
	// Fill buffer with current text
	strncpy(textBuf, m_Text, 12);
	tmpStr[2] = '\n';
			
	/*
	// Determine current time format and convert text to time
	if ( GetCurrentTimeFormat() == kSMPTE30) 
	{
		char	text[256];
		bool	changed = false;
		int16	hours, minutes, seconds, frames;
				
		TimeToString(m_Time, GetCurrentTimeFormat(), text, false);
		
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
			
			m_Time = (long)hours * 3600000L;				// Hours
			m_Time += (long)(minutes / 10) * 600000;		// Ten minutes
			minutes %= 10;
			
			if (minutes) 
			{
				// First minute
				m_Time += 60060.066;			
				m_Time += ((double)(minutes - 1) * 59993.326);
			}
			
			if (seconds) 
			{
				// First second
				if (minutes)
					m_Time += 934;					
				else
					m_Time += 1001;
				m_Time += (long)(seconds - 1) * 1001;
			}
			
			if (frames >= 2 && !seconds && minutes)
				frames -= 2;
				
			m_Time += ((double)(frames + 0.5) * 33.3667);
		}
		
	} 
	else switch (theCell) 
	*/
	switch (theCell) 
	{
		// Hours
		case 1:				
			oldVal = m_Time / 3600000;			
			
			strncpy(tmpStr, &textBuf[0], 2);		
			tmpNum = atoi(tmpStr);
			
			if (tmpNum != oldVal) 
			{
				delta = tmpNum - oldVal;
				m_Time += (delta * 3600000);
			}
			break;
			
		// Minutes
		case 2:				
			// Convert time to minutes
			oldVal = m_Time % 3600000;			
			oldVal /= 60000;
									
			strncpy(tmpStr, &textBuf[3], 2);		
			tmpNum = atoi(tmpStr);

			if (tmpNum != oldVal) 
			{
				delta = tmpNum - oldVal;
				m_Time += (delta * 60000);
			}
			break;
			
		// Seconds
		case 3:				
			oldVal = m_Time % 3600000;
			oldVal %= 60000;
			oldVal /= 1000;
			
			strncpy(tmpStr, &textBuf[6], 2);		
			tmpNum = atoi(tmpStr);
			
			if (tmpNum != oldVal) 
			{
				delta = tmpNum - oldVal;
				m_Time += (delta * 1000);
			}
			break;
			
		// Frames/100ths
		case 4:				
			m_Time /= 1000;
			m_Time *= 1000;
			
			strncpy(tmpStr, &textBuf[9], 2);		
			tmpNum = atoi(tmpStr);
			
			switch (GetCurrentTimeFormat()) 
			{
			
				case B_TIMECODE_DEFAULT:
					m_Time += (tmpNum * 10);
					break;
					
				case B_TIMECODE_24:
					m_Time += ((tmpNum * 1000 + 12) / 24);
					break;
					
				case B_TIMECODE_25:
					m_Time += ((tmpNum * 1000 + 13) / 25);
					break;
					
				case B_TIMECODE_30_DROP_2:					
				case B_TIMECODE_30:
					m_Time += ((tmpNum * 1000 + 15) / 30);
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

int32 TTimeText::GetTime()
{
	// Convert all cells
	for (int16 index = kHoursCell; index < kTotalCells; index++)
	{
		ConvertToTime(index);
	}
		
	return m_Time;
}
