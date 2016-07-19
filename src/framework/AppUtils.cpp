//---------------------------------------------------------------------
//
//	File:	AppUtils.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Misc utility routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <InterfaceDefs.h>
#include <Debug.h>
#include <TimeCode.h>

#include "AppConstants.h"
#include "AppTypes.h"
#include "MuseumApp.h"

#include "TCueView.h"
#include "TCueChannel.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"

#include "TStageWindow.h"
#include "TStageView.h"
#include "TPlaybackEngine.h"

#include "AppUtils.h"

#pragma mark -
#pragma mark === Unit Handling ===

//---------------------------------------------------------------------
//	GetUnitMSec
//---------------------------------------------------------------------
//
//	Return current unit base
//

int32 GetUnitMSec(short format, short resolution)
{
	switch (resolution)
	{
	case 0:                                                 // Frames/10ths
		switch (format)
		{
		case B_TIMECODE_DEFAULT:
			return 100.0;                           // 10th second
			break;

		case B_TIMECODE_24:
			return 1000.0 / 24;
			break;

		case B_TIMECODE_25:
			return 1000.0 / 25;
			break;

		case B_TIMECODE_30_DROP_2:
			return 1000.0 / 29.97;
			break;

		case B_TIMECODE_30:
			return 1000.0 / 30;
			break;
		}
		break;

	case 1:
		return 250.0;                           // 1/4 Second
		break;

	case 2:                                                 // Half Second
		return 500.0;
		break;

	case 3:                                                 // One Second
		return 1000.0;
		break;

	case 4:                                                 // 2 Seconds
		return 2000.0;
		break;

	case 5:                                                 // 5 Seconds
		return 5000.0;
		break;

	case 6:                                                 // 10 Seconds
		return 10000.0;
		break;

	case 7:                                                 // 30 Seconds
		return 30000.0;
		break;

	case 8:                                                 // 1 Minute
		return 60000.0;
		break;

	case 9:                                                 // 5 Minutes
		return 300000.0;
		break;

//		case 10:					// 30 Minutes
//			return 1800000.0;
//			break;

	}
	return 0;
}



//---------------------------------------------------------------------
//	TimeToString
//---------------------------------------------------------------------
//
//	Converts a millisecond time into a formatted time string.
//

#define         k30_DROP_MSEC_PER_NON_TENTH_MINUTE      59993.326
#define         k30_DROP_MSEC_PER_TENTH_MINUTE          60060.066
#define         k30_DROP_MSEC_PER_FRAME                         33.3667

void TimeToString(uint32 mSecTime, short format, char str[256], bool stripLeadingZeros)
{
	short hours, minutes, seconds, fraction;
	uint32 mSecCopy = mSecTime;
	bool isNegative = mSecTime < 0;

	if (isNegative)
		mSecTime = -mSecTime;

	fraction = mSecTime % 1000;

	// Convert to seconds
	mSecTime /= 1000;

	hours = mSecTime / 3600;

	// Convert to minutes
	mSecTime %= 3600;

	minutes = mSecTime / 60;
	seconds = mSecTime % 60;

	switch (format)
	{
	case B_TIMECODE_DEFAULT:
		fraction /= 10;                 // convert to 1/100 sec.
		break;

	case B_TIMECODE_24:
		fraction = (fraction * 24 + 12) / 1000;
		break;

	case B_TIMECODE_25:
		fraction = (fraction * 25 + 13) / 1000;
		break;

	case B_TIMECODE_30_DROP_2:
		hours           = (int32)mSecCopy / 3600000L;
		mSecCopy        = (int32)mSecCopy % 3600000L;                                   // take out hours
		minutes         = (int32)mSecCopy / 600000L * 10;                               // Ten minute marks
		mSecCopy        -= (minutes * 60000L);

		// At this point, mSecCopy contains the number of milliseconds since
		// the last ten minute (or 0 minute) time

		fraction = 0;
		// First minute is longer
		if (mSecCopy > k30_DROP_MSEC_PER_TENTH_MINUTE) {
			minutes++;
			mSecCopy -= k30_DROP_MSEC_PER_TENTH_MINUTE;
			while (mSecCopy > k30_DROP_MSEC_PER_NON_TENTH_MINUTE) {
				minutes++;
				mSecCopy -= k30_DROP_MSEC_PER_NON_TENTH_MINUTE;
			}
			// We start at frame 2
			fraction = 2;
		}

		// At this point, mSecCopy contains the number of milliseconds since the
		// last minute time
		fraction += (mSecCopy / k30_DROP_MSEC_PER_FRAME);

		// fraction is the number of frames...
		seconds = fraction / 30;
		fraction = fraction % 30;
		break;

	case B_TIMECODE_30:
		fraction = (fraction * 30 + 15) / 1000;
		break;
	}

	if (isNegative) {
		if (format == B_TIMECODE_30_DROP_2 || format == B_TIMECODE_30)
			sprintf((char*)str, "-%02d:%02d:%02d.%02d\0", hours, minutes, seconds, fraction);
		else
			sprintf((char*)str, "-%02d:%02d:%02d:%02d\0", hours, minutes, seconds, fraction);
	} else   {
		if (format == B_TIMECODE_30_DROP_2 || format == B_TIMECODE_30)
			sprintf((char*)str, "%02d:%02d:%02d.%02d\0", hours, minutes, seconds, fraction);
		else
			sprintf((char*)str, "%02d:%02d:%02d:%02d\0", hours, minutes, seconds, fraction);
	}
}


//---------------------------------------------------------------------
//	GetTimeScaleString
//---------------------------------------------------------------------
//
//	Return string indicating current time scale
//

void GetTimeScaleString(short resolution, char *timeStr)
{
	switch (resolution)
	{
	// One Frame
	case 0:
		sprintf(timeStr, "One Frame");
		break;

	// 1/4 Second
	case 1:
		sprintf(timeStr, "1/4 Second");
		break;

	// Half Second
	case 2:
		sprintf(timeStr, "1/2 Second");
		break;

	// One Second
	case 3:
		sprintf(timeStr, "1 Second");
		break;

	// 2 Seconds
	case 4:
		sprintf(timeStr, "2 Seconds");
		break;

	// 5 Seconds
	case 5:
		sprintf(timeStr, "5 Seconds");
		break;

	// 10 Seconds
	case 6:
		sprintf(timeStr, "10 Seconds");
		break;

	// 30 Seconds
	case 7:
		sprintf(timeStr, "30 Seconds");
		break;

	// 1 Minute
	case 8:
		sprintf(timeStr, "1 Minute");
		break;

	// 5 Minutes
	case 9:
		sprintf(timeStr, "5 Minutes");
		break;
	}
	// 30 Minutes
//		case 10:
//			return 1800000.0;
//			break;
}


//---------------------------------------------------------------------
//	TimeToPixels
//---------------------------------------------------------------------
//
//	Converts a millisecond time into screen pixels.
//

uint32 TimeToPixels(uint32 duration, int16 format, int16 resolution)
{
	//	Get milliseconds per pixels
	int32 mSecPerPixel = GetUnitMSec(format, resolution) / kTickSpacing;

	//	Calculate screen pixels
	if (duration == 0)
		return 0;

	uint32 length = duration / mSecPerPixel;

	//printf("length: %.4f\n", (double)length/1000000.0);

	return length;
}

//---------------------------------------------------------------------
//	PixelsToTime
//---------------------------------------------------------------------
//
//	Converts pixel value into millisecond time.
//

uint32 PixelsToTime(uint32 pixels, short format, short resolution)
{

	int32 mSecsPerPixel = GetUnitMSec(format, resolution) / kTickSpacing;

	// Get time
	int32 theTime = mSecsPerPixel * pixels;

	int32 frames;

	// Clip time to current resolution
	switch (format)
	{
	case B_TIMECODE_DEFAULT:
		frames  = theTime / 100;
		theTime = (frames * 1000 + 12) / 24;
		break;

	case B_TIMECODE_24:
		frames  = theTime / (1000.0 / 24);
		theTime = (frames * 1000 + 12) / 24;
		break;

	case B_TIMECODE_25:
		frames  = theTime / (1000.0 / 25);
		theTime = (frames * 1000 + 13) / 25;
		break;

	case B_TIMECODE_30_DROP_2:
		frames   =  theTime / (1000.0 / 29.97);
		theTime = (frames * 1000 + 15) / 30;
		break;

	case B_TIMECODE_30:
		frames = theTime / (1000.0 / 30);
		theTime = (frames * 1000 + 15) / 30;
		break;
	}

	return theTime;
}



//---------------------------------------------------------------------
//	ClipTime
//---------------------------------------------------------------------
//
//	Converts time value to proper time for current format and resolution.
//

uint32 ClipTime(uint32 theTime, short format, short resolution)
{

	int32 frames;

	// Clip time to current resolution
	switch (format)
	{
	case B_TIMECODE_DEFAULT:
		frames  = theTime / 100;
		theTime = (frames * 1000 + 12) / 24;
		break;

	case B_TIMECODE_24:
		frames  = theTime / (1000.0 / 24);
		theTime = (frames * 1000 + 12) / 24;
		break;

	case B_TIMECODE_25:
		frames  = theTime / (1000.0 / 25);
		theTime = (frames * 1000 + 13) / 25;
		break;

	case B_TIMECODE_30_DROP_2:
		frames   =  theTime / (1000.0 / 29.97);
		theTime = (frames * 1000 + 15) / 30;
		break;

	case B_TIMECODE_30:
		frames = theTime / (1000.0 / 30);
		theTime = (frames * 1000 + 15) / 30;
		break;
	}

	return theTime;
}


//---------------------------------------------------------------------
//	GetFPSValue
//---------------------------------------------------------------------
//
//	Get float value of time format
//

float GetFPSValue(timecode_type theType)
{
	// Clip time to current resolution
	switch (theType)
	{
	case B_TIMECODE_DEFAULT:
		return 30;

	case B_TIMECODE_24:
		return 24;

	case B_TIMECODE_25:
		return 25;

	case B_TIMECODE_30_DROP_2:
		return 29.97;

	case B_TIMECODE_30:
		return 30;
	}

	return 30;
}

#pragma mark -
#pragma mark === Messaging Utilities ===


#pragma mark -
#pragma mark === Keyboard Routines ===


//---------------------------------------------------------------------
//	IsKeyDown
//---------------------------------------------------------------------
//
//	Check for down state of indicated key
//

bool IsKeyDown( char theKey)
{
	key_info keyInfo;

	get_key_info(&keyInfo);

	if ( keyInfo.key_states[theKey >> 3] & (1 << (7 - (theKey % 8))) )
		return true;
	else
		return false;
}


//---------------------------------------------------------------------
//	IsShiftKeyDown
//---------------------------------------------------------------------
//
//	Check for down state of shift key
//

bool IsShiftKeyDown()
{

	key_info keyInfo;

	get_key_info(&keyInfo);

	if (keyInfo.modifiers & B_SHIFT_KEY)
		return true;
	else
		return false;
}


//---------------------------------------------------------------------
//	IsCommandKeyDown
//---------------------------------------------------------------------
//
//	Check for down state of command key
//

bool IsCommandKeyDown()
{
	key_info keyInfo;

	get_key_info(&keyInfo);

	if (keyInfo.modifiers & B_COMMAND_KEY)
		return true;
	else
		return false;
}

//---------------------------------------------------------------------
//	IsOptionKeyDown
//---------------------------------------------------------------------
//
//	Check for down state of option key
//

bool IsOptionKeyDown()
{
	key_info keyInfo;

	get_key_info(&keyInfo);

	if (keyInfo.modifiers & B_OPTION_KEY)
		return true;
	else
		return false;
}

//---------------------------------------------------------------------
//	IsControlKeyDown
//---------------------------------------------------------------------
//
//	Check for down state of control key
//

bool IsControlKeyDown()
{
	key_info keyInfo;

	get_key_info(&keyInfo);

	if (keyInfo.modifiers & B_CONTROL_KEY)
		return true;
	else
		return false;
}



//---------------------------------------------------------------------
//	AdjustScrollBar
//---------------------------------------------------------------------
//
//	Check for down state of control key
//

void AdjustScrollBar( BScrollBar *scrollBar, float page, float pageStep, float total, float start)
{
	if (total <= page) {
		scrollBar->SetRange(start, start);
	} else   {
		scrollBar->SetRange(start, start+total-page);
	}

	//float pgStep = page-4.0;

	//if (pgStep<16.0)
	//	pgStep = 16.0;

	scrollBar->SetSteps(pageStep, pageStep*4);

	//fVScroll->SetSteps(kTickSpacing, kTickSpacing*4);
}


#pragma mark -
#pragma mark === Time Utilities ===

//---------------------------------------------------------------------
//	Duration
//---------------------------------------------------------------------
//
//	Return fDuration
//

uint32 Duration()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() ) {
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView() )
			return static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->Duration();
		else
			return 0;
	} else
		return 0;
}


//---------------------------------------------------------------------
//	GetCurrentTime
//---------------------------------------------------------------------
//
//	Return fCurrentTime
//

uint32 GetCurrentTime()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() ) {
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView() )
			return static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->GetCurrentTime();
		else
			return 0;
	} else
		return 0;
}


//---------------------------------------------------------------------
//	StartTime
//---------------------------------------------------------------------
//
//	Return fStartTime
//

uint32 StartTime()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() ) {
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView() )
			return static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->StartTime();
		else
			return 0;
	} else
		return 0;
}


//---------------------------------------------------------------------
//	GetCurrentResolution
//---------------------------------------------------------------------
//
//	Return fResolution
//

short GetCurrentResolution()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() ) {
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView() ) {
			return static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->GetResolution();
		} else
			return 0;
	} else
		return 0;
}


//---------------------------------------------------------------------
//	GetCurrentTimeFormat
//---------------------------------------------------------------------
//
//	Return fTimeFormat
//

timecode_type GetCurrentTimeFormat()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() ) {
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView() )
			return static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->GetTimeFormat();
		else
			return B_TIMECODE_DEFAULT;
	} else
		return B_TIMECODE_DEFAULT;
}


//---------------------------------------------------------------------
//	IsPlaying
//---------------------------------------------------------------------
//
//

bool IsPlaying()
{
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetPlaybackEngine()->IsPlaying() )
		return true;
	else
		return false;
}


#pragma mark -
#pragma mark === Cue Utilities ===

//---------------------------------------------------------------------
//	SortCuesByChannelID
//---------------------------------------------------------------------
//
//	Sort a cue by channel ID with low numbers placed at start of list
//


int SortCuesByChannelID( const void *itemOne, const void *itemTwo)
{
	//TStageCue **firstCue =  (TStageCue **)itemOne;
	//TStageCue **secondCue = (TStageCue **)itemTwo;

	//int32 firstID         = (*firstCue)->GetChannelCue()->GetChannel()->GetID();
	//int32 secondID        = (*secondCue)->GetChannelCue()->GetChannel()->GetID();

	//if ( firstID < secondID)
	//	return -1;
	//else
	return 1;
}


#pragma mark -
#pragma mark === Window Utilities ===

//---------------------------------------------------------------------
//	FindWindow
//---------------------------------------------------------------------
//
//	Find the window whose title matches the string
//


BWindow  *FindWindow(const char *title)
{
	BWindow        *theWindow = NULL;
	const char *tmpTitle;

	for ( int32 index = 0; index < be_app->CountWindows(); index++) {
		theWindow = be_app->WindowAt(index);

		if (theWindow) {
			theWindow->Lock();

			tmpTitle = theWindow->Title();

			if( tmpTitle != NULL && strcmp( tmpTitle, title) == 0) {
				theWindow->Unlock();
				return theWindow;
			}
			theWindow->Unlock();
		}
	}

	return NULL;
}


//---------------------------------------------------------------------
//	CenterWindow
//---------------------------------------------------------------------
//
//	Center window on current screen
//

void CenterWindow(BWindow *theWindow)
{
	// Get screen settings
	BScreen theScreen(B_MAIN_SCREEN_ID);
	BRect screenBounds = theScreen.Frame();
	BRect windBounds = theWindow->Frame();

	theWindow->Lock();
	theWindow->Hide();
	theWindow->MoveTo( (screenBounds.Width() - windBounds.Width())/2, (screenBounds.Height() - windBounds.Height())/2);
	theWindow->Show();
	theWindow->Unlock();
}



#pragma mark -
#pragma mark === Stage Utilities ===

//---------------------------------------------------------------------
//	UpdateStage
//---------------------------------------------------------------------
//
//	Force a stage redraw
//

void UpdateStage()
{
	//	Force an update of the stage
	TCueSheetWindow *cueSheet = static_cast<MuseumApp *>(be_app)->GetCueSheet();
	TStageWindow *stageWindow = cueSheet->GetStage();
	stageWindow->Lock();
	TStageView *theView = cueSheet->GetStage()->GetStageView();
	if (theView) {
		theView->StageDraw(theView->Bounds(), GetCurrentTime());
		theView->Invalidate();
	}
	stageWindow->Unlock();
}

#pragma mark -
#pragma mark === Unit Utilities ===

//---------------------------------------------------------------------
//	DiffRect
//---------------------------------------------------------------------
//
//	Calculate the difference between two rects
//

bool DiffRect( BRect *rectOne, BRect *rectTwo, BRect *dstRect)
{
	BRect diffRect;

	if (rectOne->Intersects( *rectTwo ) ) {
		if (rectOne->Width() > rectTwo->Width() ) {
			// Determine shared side of rect
			//

			// Left sides match
			if (rectOne->left == rectTwo->left) {
				dstRect->left   = rectTwo->right;
				dstRect->top    = rectTwo->top;
				dstRect->right  = rectOne->right;
				dstRect->bottom = rectTwo->bottom;
			}
			// right sides match
			else if ( rectOne->right == rectTwo->right) {
				dstRect->left   = rectOne->left;
				dstRect->top    = rectOne->top;
				dstRect->right  = rectTwo->left;
				dstRect->bottom = rectOne->bottom;
			}
			// no shared side...
			else
				return false;
		} else   {
			// Determine shared side of rect
			//

			// Left sides match
			if (rectOne->left == rectTwo->left) {
				dstRect->left   = rectOne->right;
				dstRect->top    = rectOne->top;
				dstRect->right  = rectTwo->right;
				dstRect->bottom = rectOne->bottom;
			}
			// right sides match
			else if ( rectOne->right == rectTwo->right) {
				dstRect->left   = rectTwo->left;
				dstRect->top    = rectTwo->top;
				dstRect->right  = rectOne->left;
				dstRect->bottom = rectTwo->bottom;
			}
			// no shared side...
			else
				return false;
		}
		return true;
	}

	return false;
}

#pragma mark -
#pragma mark === Bitmap Utilities ===

//---------------------------------------------------------------------
//	FillBitmap
//---------------------------------------------------------------------
//
//	Fill bitmap with color
//

void FillBitmap(BBitmap * map, uint32 color)
{
	ASSERT(map->ColorSpace() == B_RGB_32_BIT);
	ASSERT(sizeof(double) == 8);

	uchar *dst = (uchar *)map->Bits();
	uchar *end = dst+map->BitsLength();

	if (((ulong)dst)&4) {
		*(uint32*)dst = color;
		dst += 4;
	}

	double d;
	uint32 a[2] = { color, color };

	d = *(double *)a;
	while (end-dst > 7) {
		*(double *)dst = d;
		dst += 8;
	}

	if (end-dst > 3)
		*(uint32*)dst = color;
}


//---------------------------------------------------------------------
//	CloneBitmap
//---------------------------------------------------------------------
//
//	Clone bitmap
//

BBitmap *CloneBitmap(const BBitmap &input)
{
	BBitmap *clone = new BBitmap(input.Bounds(), input.ColorSpace());
	ASSERT(input.BytesPerRow() == clone->BytesPerRow());
	ASSERT(input.BitsLength() <= clone->BitsLength());
	memcpy(clone->Bits(), input.Bits(), input.BitsLength());
	return clone;
}

#pragma mark -
#pragma mark === File Utilities ===


//---------------------------------------------------------------------
//	IsSupportedType
//---------------------------------------------------------------------
//
//	Return true if this is a file type that we can handle
//

bool IsSupportedType(BNodeInfo &nodeInfo)
{
	//	We support the following supertypes:
	//
	//	-- Image, Audio, Video
	//

	if ( IsImage(nodeInfo) )
		return true;

	if ( IsAudio(nodeInfo))
		return true;

	if (IsVideo(nodeInfo))
		return true;

	// We don't support it
	return false;
}


//---------------------------------------------------------------------
//	IsImage
//---------------------------------------------------------------------
//
//	Return true if this is an "image" file.
//

bool IsImage(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr, "image") == 0)
				return true;
		}
		return false;
	}

	return false;
}


//---------------------------------------------------------------------
//	IsAudio
//---------------------------------------------------------------------
//
//	Return true if this is an "audio" file.
//

bool IsAudio(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr, "audio") == 0) {
				//	Make sure it is not a MIDI file
				if ( strcmp(mimeType.Type(), "audio/midi") != 0)
					return true;
			}
			return false;
		}
		return false;
	}

	return false;
}


//---------------------------------------------------------------------
//	IsMIDI
//---------------------------------------------------------------------
//
//	Return true if this is a "midi" file.
//

bool IsMIDI(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr, "audio") == 0) {
				//	Make sure it is not a MIDI file
				if ( strcmp( mimeType.Type(), "audio/midi") == 0)
					return true;
			}
			return false;
		}
		return false;
	}

	return false;
}

//---------------------------------------------------------------------
//	IsVideo
//---------------------------------------------------------------------
//
//	Return true if this is an "video" file.
//

bool IsVideo(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr, "video") == 0)
				return true;
		}
		return false;
	}

	return false;
}


//---------------------------------------------------------------------
//	IsText
//---------------------------------------------------------------------
//
//	Return true if this is a "text" file.
//

bool IsText(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr,  "text") == 0)
				return true;
		}
		return false;
	}

	return false;
}


//---------------------------------------------------------------------
//	IsCueSheet
//---------------------------------------------------------------------
//
//	Return true if this is an "application/x-mediapede-cuesheet" file.
//

bool IsCueSheet(BNodeInfo &nodeInfo)
{
	char theType[B_MIME_TYPE_LENGTH];

	if (nodeInfo.GetType(theType) == B_NO_ERROR) {
		BMimeType mimeType(theType);
		if (mimeType.InitCheck() != B_NO_ERROR)
			return false;

		BMimeType superType;
		if ( mimeType.GetSupertype(&superType) == B_NO_ERROR) {
			const char *superTypeStr= superType.Type();

			if ( strcmp(superTypeStr, "application") == 0) {
				//	Make sure it is not a MIDI file
				if ( strcmp( mimeType.Type(), "application/x-mediapede-cuesheet") == 0)
					return true;
			}
			return false;
		}
		return false;
	}

	return false;
}


#pragma mark -
#pragma mark === Thread Utilities ===


//---------------------------------------------------------------------
//	ValidThread
//---------------------------------------------------------------------
//
//	Return true if this is a valid thread_id
//

bool ValidThread( thread_id theThread)
{
	if( theThread != B_NO_MORE_THREADS && theThread != B_NO_MEMORY)
		return true;
	else
		return false;
}