//---------------------------------------------------------------------
//
//	File:	TTransportPaletteView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	Transport Palette View
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"

#include "TBitmapButton.h"

#include "TTransportPalette.h"
#include "TTimeText.h"
#include "TTimelineView.h"
#include "TVideoEngine.h"

#include "TTransportPaletteView.h"

// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTransportPaletteView::TTransportPaletteView(BRect bounds, BBitmap* bitmap) : TBitmapView(bounds, "TransportPaletteView", bitmap, true,  B_FOLLOW_ALL | B_PULSE_NEEDED)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransportPaletteView::~TTransportPaletteView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TTransportPaletteView::Init()
{
	BBitmap* offBitmap;
	BBitmap* onBitmap;

	// Set view background color
	SetViewColor(kBlack);
	BFont saveFont;
	GetFont(&saveFont);

	//
	// Create the transport button subviews
	//

	BRect buttonRect = Bounds();

	//
	//	First Row
	//

	// Rewind to beginning
	//

	// Set button location
	buttonRect.Set( buttonRect.left + kTransportButtonOffset, buttonRect.top, buttonRect.left+kTransportButtonWidth+kTransportButtonOffset, buttonRect.top+kTransportButtonHeight);

	// Load Off and On bitmaps
	offBitmap = GetTransportButton("RewindEndDown");
	onBitmap = GetTransportButton("RewindEndUp");

	// Create button
	fStartButton = new TBitmapButton( buttonRect, "StartButton", offBitmap, onBitmap, this, new BMessage(START_BUTTON_MSG), true);
	AddChild(fStartButton);


	// Play Button
	//

	// Set button location
	buttonRect.Set( buttonRect.left+kTransportButtonOffset+kTransportButtonWidth, buttonRect.top,
	                buttonRect.left+(kTransportButtonWidth*2)+kTransportButtonOffset, buttonRect.bottom);


	// Load Off and On bitmaps
	offBitmap = GetTransportButton("PlayUp");
	onBitmap  = GetTransportButton("PlayDown");

	// Create button
	fPlayButton = new TBitmapButton(buttonRect, "PlayButton", offBitmap, onBitmap, this, new BMessage(PLAY_BUTTON_MSG), true);
	AddChild(fPlayButton);


	// Pause Button
	//

	// Set button location
	buttonRect.Set( buttonRect.left+kTransportButtonOffset+kTransportButtonWidth, buttonRect.top,
	                buttonRect.left+(kTransportButtonWidth*2)+kTransportButtonOffset, buttonRect.bottom);

	// Load Off and On bitmaps
	offBitmap = GetTransportButton("PauseDown");
	onBitmap = GetTransportButton("PauseUp");

	// Create button
	fPauseButton = new TBitmapButton(buttonRect, "PauseButton", offBitmap, onBitmap, this, new BMessage(PAUSE_BUTTON_MSG), true);
	AddChild(fPauseButton);

	// FF To End Button
	//

	// Set button location
	buttonRect.right        = Bounds().right - kTransportButtonOffset;
	buttonRect.left         = buttonRect.right - kTransportButtonWidth;

	// Load Off and On bitmaps
	offBitmap = GetTransportButton("FFEndDown");
	onBitmap = GetTransportButton("FFEndUp");

	// Create button
	fEndButton = new TBitmapButton(buttonRect, "EndButton", offBitmap, onBitmap, this, new BMessage(END_BUTTON_MSG), true);
	AddChild(fEndButton);


	//
	//	Second Row
	//

	//
	//	Rewind Button

	// Set button location
	buttonRect.left         = Bounds().left + kTransportButtonOffset;
	buttonRect.right        = buttonRect.left + kTransportButtonWidth;
	buttonRect.bottom       = Bounds().bottom - kTransportButtonOffset;
	buttonRect.top          = buttonRect.bottom - kTransportButtonHeight;


	// Load Off and On bitmaps
	offBitmap       = GetTransportButton("RewindDown");
	onBitmap        = GetTransportButton("RewindUp");

	// Create button
	fRewindButton = new TBitmapButton(buttonRect, "RewindButton", offBitmap, onBitmap, this, new BMessage(REWIND_BUTTON_MSG), true);
	AddChild(fRewindButton);

	// FF Button
	//

	// Set button location
	buttonRect.right        = Bounds().right - kTransportButtonOffset;
	buttonRect.left         = buttonRect.right - kTransportButtonWidth;

	// Load Off and On bitmaps
	offBitmap       = GetTransportButton("FFDown");
	onBitmap        = GetTransportButton("FFUp");

	// Create button
	fFFButton = new TBitmapButton(buttonRect, "FFButton", offBitmap, onBitmap, this, new BMessage(FF_BUTTON_MSG), true);
	AddChild(fFFButton);


	//
	// Create text display
	//

	// Set text size temporarily to the text view size to get proper
	// string length of default start time "00:00:00:00"
	SetFont(be_fixed_font);
	SetFontSize(kTransportFontSize);
	float strWidth = StringWidth("00:00:00:00");

	// Create text display
	BRect textBounds        = Bounds();
	textBounds.right        = textBounds.left + strWidth;
	textBounds.bottom       = textBounds.top + kTransportFontSize;

	// Move to proper location
	textBounds.OffsetBy(52, 42);

	//	Create text
	fTransportText = new TTimeText(this, 0, textBounds, "TransportTextView", B_FOLLOW_LEFT);
	AddChild(fTransportText);

	BFont theFont(be_fixed_font);
	theFont.SetSize(kTransportFontSize);
	fTransportText->SetFontAndColor(&theFont, B_FONT_ALL, kGreen);

	//	Fine tune text draw pt
	BPoint drawPt;
	fTransportText->GetDrawPoint(&drawPt);
	drawPt.y -= 4;
	fTransportText->SetDrawPoint(&drawPt);

	fTransportText->Show();
	fTransportText->Invalidate();

	// Restore settings
	SetFont(&saveFont);
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TTransportPaletteView::Draw(BRect updateRect)
{
	TBitmapView::Draw(updateRect);

	if (fTransportText) {
		fTransportText->Draw(updateRect);
	}
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TTransportPaletteView::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
	//	Update text
	case TIMELINE_DRAG_MSG:
	case UPDATE_TIMELINE_MSG:
	{
		char text[12];
		uint32 theTime = message->FindInt32("TheTime");
		TimeToString(theTime, GetCurrentTimeFormat(), text, false);
		fTransportText->SetText(text);
	}
	break;

	//	Go to beginning of cue sheet
	case START_BUTTON_MSG:
	{
		//	Stop playback
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		TVideoEngine* engine = cueSheet->GetVideoEngine();
		engine->Stop(0);

		//	Reset current time
		cueSheet->GetCueSheetView()->SetCurrentTime(StartTime());

		//	Inform timeline which updates everyone
		TTimelineView* timeLine = cueSheet->GetTimeline();
		BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		timeLine->MessageReceived(message);
		cueSheet->GetCueSheetView()->MessageReceived(message);
	}
	break;

	//	Step back a time unit
	case REWIND_BUTTON_MSG:
	{
		//	Stop playback
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		TVideoEngine* engine = cueSheet->GetVideoEngine();
		engine->Stop(0);

		//	Set current time
		uint32 time = GetCurrentTime();
		time -= GetUnitMSec( GetCurrentTimeFormat(), GetCurrentResolution() ) / GetFPSValue(GetCurrentTimeFormat());

		if (time >= StartTime())
			cueSheet->GetCueSheetView()->SetCurrentTime(time);

		//	Inform timeline which updates everyone
		TTimelineView* timeLine = cueSheet->GetTimeline();
		BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		timeLine->MessageReceived(message);
		cueSheet->GetCueSheetView()->MessageReceived(message);
	}
	break;

	//	Start VideoEngine time source
	case PLAY_BUTTON_MSG:
	{
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		if (cueSheet) {
			TVideoEngine* engine = cueSheet->GetVideoEngine();
			if (engine)
				engine->Start(0);
		}
	}
	break;

	case PAUSE_BUTTON_MSG:
	case STOP_BUTTON_MSG:
	{
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		if (cueSheet) {
			TVideoEngine* engine = cueSheet->GetVideoEngine();
			if (engine)
				engine->Stop(0);
		}
	}
	break;

	case FF_BUTTON_MSG:
	{
		//	Stop playback
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		TVideoEngine* engine = cueSheet->GetVideoEngine();
		engine->Stop(0);

		//	Set current time
		uint32 time = GetCurrentTime();
		time += GetUnitMSec( GetCurrentTimeFormat(), GetCurrentResolution() ) / GetFPSValue(GetCurrentTimeFormat());

		if (time <= StartTime() + Duration())
			cueSheet->GetCueSheetView()->SetCurrentTime(time);

		//	Inform timeline which updates everyone
		TTimelineView* timeLine = cueSheet->GetTimeline();
		BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		timeLine->MessageReceived(message);
		cueSheet->GetCueSheetView()->MessageReceived(message);
	}
	break;

	case END_BUTTON_MSG:
	{
		//	Stop playback
		TCueSheetWindow* cueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet();
		TVideoEngine* engine = cueSheet->GetVideoEngine();
		engine->Stop(0);

		//	Reset current time
		cueSheet->GetCueSheetView()->SetCurrentTime( StartTime() + Duration());

		//	Inform timeline which updates everyone
		TTimelineView* timeLine = cueSheet->GetTimeline();
		BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		timeLine->MessageReceived(message);
		cueSheet->GetCueSheetView()->MessageReceived(message);
	}
	break;

	case RECORD_BUTTON_MSG:
		break;

	default:
		break;
	}
}


void
TTransportPaletteView::Pulse()
{
	char text[12];
	//	Update text
	TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), text, false);
	GetTransportText()->SetText(text);
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	GetTransportButton
//---------------------------------------------------------------------
//
//	Find and return 28x20 icon by string
//

BBitmap* GetTransportButton(char* theResource)
{
	// Get application info
	app_info info;

	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return NULL;

	size_t size;
	BBitmap* data;

	BResources res(&file);
	data = (BBitmap*)res.FindResource('bits', theResource, &size);
	if (!data)
		return NULL;

	// Load icon
	BRect bounds;
	bounds.Set(0, 0, 36, 29);
	BBitmap* bitmap = new BBitmap(bounds, B_COLOR_8_BIT);
	ASSERT(bitmap);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);

	return (bitmap);
}
