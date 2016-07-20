//---------------------------------------------------------------------
//
//	File:	TTimelineView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Cue sheet view
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>
#include <Debug.h>

#include <TimeSource.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "MuseumApp.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"
#include "TTimelineView.h"
#include "TTimePalette.h"
#include "TTimePaletteView.h"
#include "TTransportPalette.h"
#include "TTransportPaletteView.h"
#include "TTimeText.h"
#include "TProjectSettings.h"

#include "TCueSheetScrollBarH.h"
#include "TCueSheetScrollBarV.h"

#include "TMouseWatcher.h"
#include "TPlaybackEngine.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimelineView::TTimelineView(BRect bounds, TCueSheetWindow* parent) :
	BView(bounds, "TimelineView", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	// Set CueSheet parent
	fCueSheetWindow = parent;

	// Perform default initialization
	Init();

	BMessage message(RUN_MESSAGE_RUNNER_MSG);
 	fRunner = new BMessageRunner(BMessenger(this), &message, 50000);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

TTimelineView::TTimelineView(BMessage* message) : BView(message)
{
	fCueSheetWindow = NULL;

	// Set up time rect
	const BRect bounds = Bounds();

	// Load position indicator
	fIndicator = GetIcon16FromResource("Position");

	// Set white bits to B_TRANSPARENT_8_BIT
	// gzr: to do... Why does 63 == white?
	int32 bitsLength = fIndicator->BitsLength();
	for ( int32 index = 0; index < bitsLength; index++) {
		unsigned char color = ((unsigned char*)fIndicator->Bits())[index];
		if (color == 63) {
			((unsigned char*)fIndicator->Bits())[index] = B_TRANSPARENT_8_BIT;
		}
	}
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimelineView::~TTimelineView()
{
	// Free Indicator
	delete fIndicator;

	delete fRunner;
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TTimelineView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TTimelineView") )
		return new TTimelineView(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TTimelineView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TTimelineView");

		// Add our member variables to the archive

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TTimelineView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	//	Set up member variables
	fIsPlaying      = false;
	fIsStopping     = false;

	// Set up time rect
	const BRect bounds = Bounds();

	// Set up indicator rect
	fIndicatorRect.Set( bounds.left, bounds.top, bounds.left+kIndicatorWidth, bounds.top+kIndicatorHeight);

	// Load position indicator
	fIndicator = GetIcon16FromResource("Position");

	// Set white bits to B_TRANSPARENT_8_BIT
	// gzr: to do... Why does 63 == white?
	int32 bitsLength = fIndicator->BitsLength();
	for ( int32 index = 0; index < bitsLength; index++) {
		unsigned char color = ((unsigned char*)fIndicator->Bits())[index];
		if (color == 63) {
			((unsigned char*)fIndicator->Bits())[index] = B_TRANSPARENT_8_BIT;
		}
	}
}

#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TTimelineView::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( fCueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;

	// Activate cue sheet
	Window()->Activate(true);

	const BRect bounds = Bounds();

	// Reset cue drag flag
	static_cast<MuseumApp*>(be_app)->fIsCueDrag = false;

	// Clip out Header and right side of indicator zone
	PushState();

	// Set up redraw color
	SetHighColor(kBeShadow);

	// Draw new position.
	TrackIndicator(where);

	// Launch mouse tracking thread
	fMouseTracker = StartMouseWatcher(this);

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TTimelineView::MouseUp(BPoint where)
{
	// Do nothing if we are playing
	if ( fCueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;

}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TTimelineView::MouseMoved( BPoint where, uint32 code, const BMessage* message )
{

	// Do nothing if we are playing
	if ( fCueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;

	// Exit if panel is open
	if ( fCueSheetWindow->IsPanelOpen())
		return;

	// Determine type of movement
	switch(code)
	{
	case B_ENTERED_VIEW:
		break;

	case B_INSIDE_VIEW:
		break;

	case B_EXITED_VIEW:
		// Send tick off the screen
		where.x = 0;
		break;

	default:
		break;
	}

	// Only update the tick if we aren't tracking position indicator
	uint32 buttons = 0;

	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);
	if (!buttons)
		UpdateTimeTick(where);

}

#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//

void TTimelineView::MessageReceived(BMessage* message)
{
	switch(message->what)
	{

	case RUN_MESSAGE_RUNNER_MSG:
	{
		//	Draw playback head
		TrackPlayback();
		break;	
	}

	//	Update position of playback indicator.  Called by Transport button click
	case TIMELINE_DRAG_MSG:
	{
		//	Update position
		TrackPlayback();
	}
	break;

	// We need to redraw the tick
	case UPDATE_TIMELINE_MSG:
	{
		BPoint where;
		if (message->FindPoint("Where", &where) == B_OK)
			UpdateTimeTick(where);
	}
	break;

	// Mouse Tracking Messages
	case MW_MOUSE_MOVED:
	{
		BPoint mousePt;
		message->FindPoint("where", &mousePt);

		// Track the position indicator
		TrackIndicator(mousePt);

		// If the mouse location is outside of the window bounds, scroll the view
		BPoint scrollPt = ConvertToParent(mousePt);
		if (scrollPt.x > fCueSheetWindow->Bounds().right) {
			TCueSheetScrollBarH* theScroll = fCueSheetWindow->GetScrollH();
			float theVal = theScroll->Value();
			theScroll->SetValue( theVal + kTickSpacing);
		} else if (scrollPt.x < (fCueSheetWindow->Bounds().left + kHeaderWidth) ) {
			TCueSheetScrollBarH* theScroll = fCueSheetWindow->GetScrollH();
			float theVal = theScroll->Value();
			theScroll->SetValue( theVal - kTickSpacing);
		}
	}
	break;

	//	Update stage
	case MW_MOUSE_UP:
		break;

	default:
		BView::MessageReceived(message);
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

void TTimelineView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;

	// Save environment
	PushState();

	drawing_mode saveMode = DrawingMode();
	SetDrawingMode(B_OP_COPY);

	//
	// Draw Ticks and Time Indicators
	//

	//	Calculate indicator zone
	const BRect bounds = Bounds();
	const BRect indicatorZone(bounds.left, bounds.top, bounds.right, bounds.top+kIndicatorHeight);

	// Draw indicator zone
	if (updateRect.Intersects(indicatorZone) ) {
		SetHighColor(kBeShadow);
		BRect fillRect = updateRect & indicatorZone;
		FillRect(fillRect);
		SetHighColor(kBlack);
		startPt.Set(fillRect.left, indicatorZone.bottom);
		endPt.Set(fillRect.right, indicatorZone.bottom);
		StrokeLine(startPt, endPt);
	}

	// Draw timeline
	BPoint textPt;
	char timeStr[256];

	// The update rect should always be sized to the visible area of the timeline to avoid drawing glitchs
	BRect frame             = Window()->Bounds();
	BRect timeBounds        = Bounds();
	updateRect.left         = frame.left + timeBounds.left;
	updateRect.right        = frame.right + timeBounds.left;
	updateRect.top          = indicatorZone.bottom+1;

	// Do background fill
	SetHighColor(kWhite);
	FillRect(updateRect);

	// Draw black line at bottom of timeline
	SetHighColor(kBlack);
	startPt.Set(updateRect.right, Bounds().bottom);
	endPt.Set(updateRect.left, Bounds().bottom);
	StrokeLine(startPt, endPt);

	// updateRect.left should be divisible by kTickSpacing.  Force it to be...
	int32 left = updateRect.left;
	int32 mod = left % kTickSpacing;
	updateRect.left -= mod;

	// Set up variables here to increase efficiency in loop
	//

	BFont font;
	GetFont(&font);
	SetFont(be_plain_font);
	SetHighColor(kBlack);

	// Text drawing point
	int16 textPtY = ( (timeBounds.Height() - indicatorZone.Height()) / 2);
	textPtY += indicatorZone.bottom+4;

	// Get time variables
	uint32 unitMSec       = GetUnitMSec( GetCurrentTimeFormat(), GetCurrentResolution() );
	uint32 roundVar   = (StartTime() + Duration()) + 9;
	timecode_type timeFormat = GetCurrentTimeFormat();

	for (int32 index = updateRect.left / kTickSpacing * kTickSpacing; index < updateRect.right + kTickSpacing; index += kTickSpacing) {
		// Move pen to proper area.  We are fudging here to center the text over the tick
		startPt.Set( index - 26, timeBounds.bottom - 2);

		// Get time
		int32 time = index / kTickSpacing * unitMSec;

		// If we are in frames/10ths resolution, round time up to nearest value.
		// 9 turns out to work for all time formats.
		if ( GetCurrentResolution() == 0)
			time += 9;

		// Now draw time text.
		TimeToString(time, timeFormat, timeStr, FALSE);

		// Skip 00:00:00:00
		if (time > 9) {
			// Add in rounding factor...
			if (time <= roundVar) {
				BPoint textPt;
				textPt.x = startPt.x;
				textPt.y = textPtY;
				DrawString(timeStr, textPt);
			}
		}

		// Draw main ticks...
		//

		// Top
		startPt.Set( index, indicatorZone.bottom);
		endPt.Set( startPt.x, indicatorZone.bottom+kTickHeight);
		StrokeLine(startPt, endPt);

		// Bottom
		startPt.Set( index, timeBounds.bottom);
		endPt.Set( startPt.x, timeBounds.bottom-kTickHeight);
		StrokeLine(startPt, endPt);

		// Draw intermediate ticks from last main tick
		DrawSubTicks(index);
	}

	// Draw position indicator
	DrawIndicator();

	// Restore environment
	SetDrawingMode(saveMode);
	PopState();
}


//---------------------------------------------------------------------
//	DrawSubTicks
//---------------------------------------------------------------------
//
//	Draw intermediate ticks between main tick indicators
//

void TTimelineView::DrawSubTicks(int32 index)
{
	BPoint startPt, endPt;

	const BRect timeBounds = Bounds();

	//	Calculate indicator zone
	const BRect indicatorZone(timeBounds.left, timeBounds.top, timeBounds.right, timeBounds.top+kIndicatorHeight);

	int16 fpsValue  = GetFPSValue( GetCurrentTimeFormat() );

	int16 numTicks;
	switch( GetCurrentResolution() )
	{

	// One Frame
	case 0:
		numTicks = 0;
		break;

	// 1/4 Second
	case 1:
		numTicks = kTickSpacing/ ( (fpsValue-1) / 4);
		break;

	// Half Second
	case 2:
		numTicks = kTickSpacing/ ( (fpsValue-1)/2);
		break;

	// One Second
	case 3:
		numTicks = kTickSpacing/(fpsValue-1);
		break;

	// 2 Seconds
	case 4:
		numTicks = 2;
		break;

	// 5 Seconds
	case 5:
		numTicks = 5;
		break;

	// 10 Seconds
	case 6:
		numTicks = 10;
		break;

	// 30 Seconds
	case 7:
		numTicks = 30;
		break;

	// 1 Minute
	case 8:
		numTicks = 10;
		break;

	// 5 Minutes
	case 9:
		numTicks = 10;
		break;
	}

	if (numTicks > 0) {
		for (int32 lastIndex = index; lastIndex < (index+kTickSpacing); lastIndex+=numTicks) {
			// Top tick
			startPt.Set( lastIndex, indicatorZone.bottom);
			endPt.Set( startPt.x, indicatorZone.bottom+(kTickHeight/2));
			StrokeLine(startPt, endPt);

			// Bottom tick
			startPt.Set( lastIndex, timeBounds.bottom);
			endPt.Set( startPt.x, timeBounds.bottom-(kTickHeight/2));
			StrokeLine(startPt, endPt);
		}
	}
}


//---------------------------------------------------------------------
//	UpdateTimeTick
//---------------------------------------------------------------------
//
//	Update the location of the time indicator tick
//

void TTimelineView::UpdateTimeTick(BPoint where)
{

	//
	// Set up the environment
	//

	Looper()->Lock();

	// Clip out Indicator Zone
	BRect bounds    = Bounds();
	bounds.top              += kIndicatorZoneHeight+1;
	bounds.bottom   -= 1;

	// Set up environment
	PushState();

	//      Only proceed if we have moved in the X coordinate space.  We don't care about Y.
	//	This avoids flicker in the time indicator palette
	if (where.x == fLastTick.x) {
		PopState();
		Looper()->Unlock();
		return;
	}

	//      Cleanup and draw indicator and save position for next pass through
	BPoint startPt, endPt;
	SetHighColor(kBlack);
	SetDrawingMode(B_OP_INVERT);

	// Erase last indicator tick
	startPt.Set(fLastTick.x, bounds.top);
	endPt.Set(fLastTick.x, bounds.bottom);
	StrokeLine(startPt, endPt);

	// Draw the new indicator tick
	startPt.Set(where.x, bounds.top);
	endPt.Set(where.x, bounds.bottom);
	StrokeLine(startPt, endPt);

	// Save position
	fLastTick = where;

	//      Send a message to the Locator Palette telling it about the new tick time
	//      We don't need to do this during playback.  The playback engines handles
	//	it at that time.
	if (!IsPlaying()) {
		BMessage* message = new BMessage(UPDATE_TIMELINE_MSG);
		uint32 theTime = PixelsToTime((where.x), GetCurrentTimeFormat(), GetCurrentResolution());
		message->AddInt32("TheTime", theTime);

		// Inform Locator
		TTimePalette* timePalette = static_cast<MuseumApp*>(be_app)->GetTimePalette();
		if (timePalette) {
			timePalette->Lock();
			timePalette->GetTimePaletteView()->MessageReceived(message);
			timePalette->Unlock();
		}

		// Clean up message
		delete message;
	}

	// Restore environment
	PopState();

	Looper()->Unlock();
}


//---------------------------------------------------------------------
//	DrawIndicator
//---------------------------------------------------------------------
//
//	Draw the position indicator at the proper location
//

void TTimelineView::DrawIndicator()
{
	// Set up environment
	PushState();

	drawing_mode saveMode = DrawingMode();
	SetDrawingMode(B_OP_OVER);

	SetHighColor(kBeShadow);
	BPoint drawPt(fIndicatorRect.left, fIndicatorRect.top);

	// Make sure it isn't outside the timeline bounds
	ClipIndicatorRect();

	// Draw indicator in new location
	DrawBitmap(fIndicator, drawPt);

	// Draw the indicator line down all views
	// We do this by sending a message out to all of the cue channels
	// and they will draw the line where indicated
	BMessage* message = new BMessage(UPDATE_INDICATOR_MSG);
	drawPt.Set( fIndicatorRect.left + (fIndicatorRect.Width() / 2), fIndicatorRect.top);
	message->AddPoint("IndicatorPoint", drawPt);

	// Force message into the application
	fCueSheetWindow->Lock();
	fCueSheetWindow->GetCueSheetView()->MessageReceived(message);
	fCueSheetWindow->Unlock();
	delete message;

	// Restore environment
	SetDrawingMode(saveMode);

	PopState();
}


//---------------------------------------------------------------------
//	DrawIndicator
//---------------------------------------------------------------------
//
//	Draw the position indicator at the proper location and clean
//	up last position
//

void TTimelineView::DrawIndicator(BRect oldPosition)
{

	// Set up environment
	PushState();

	drawing_mode saveMode = DrawingMode();
	SetDrawingMode(B_OP_OVER);

	SetHighColor(kBeShadow);

	BPoint drawPt(fIndicatorRect.left, fIndicatorRect.top);

	// Clean up last indicator position.  We clip two pixels off the bottom
	// so as not to overdraw border line.
	oldPosition.bottom -= 2;
	FillRect(oldPosition);

	// Make sure it isn't outside the timeline bounds
	ClipIndicatorRect();

	// Draw indicator in new location
	DrawBitmap(fIndicator, drawPt);

	// Draw the indicator line down all views
	// We do this by sending a message out to all of the cue channels
	// and they will draw the line where indicated
	BMessage* message = new BMessage(UPDATE_INDICATOR_MSG);
	drawPt.Set( fIndicatorRect.left + (fIndicatorRect.Width() / 2), fIndicatorRect.top);
	message->AddPoint("IndicatorPoint", drawPt);

	// Force message into the application
	fCueSheetWindow->GetCueSheetView()->MessageReceived(message);
	delete message;

	// Restore environment
	SetDrawingMode(saveMode);
	PopState();

}

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TTimelineView::FrameResized(float new_width, float new_height)
{
	//	Adjust Indicator zone
	fIndicatorRect.right = fIndicatorRect.left + new_width;
}


#pragma mark -
#pragma mark === Resolution Handling ===

//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//	Resize timeline based on new resolution
//

void TTimelineView::ResolutionChanged(int32 resizePixels)
{
	// Force redraw
	Invalidate();
}


//---------------------------------------------------------------------
//	TrackIndicator
//---------------------------------------------------------------------
//
//
//

void TTimelineView::TrackIndicator(BPoint mousePt)
{

	//	Don't allow scroll out of bounds
	//if (mousePt.x > indicatorZone.right)
	//	mousePt.x = indicatorZone.right;

	//	Left side
	if (mousePt.x < 0)
		mousePt.x = 0;

	// Move indicator to mouse position and track it while mouse button is down
	BRect oldRect;
	uint32 buttons = 0;

	// Save oldRect for redraw
	oldRect = fIndicatorRect;
	fIndicatorRect.left = mousePt.x - (kIndicatorWidth/2);
	fIndicatorRect.right = fIndicatorRect.left + kIndicatorWidth;

	// Exit if there is no change in position
	if (oldRect == fIndicatorRect)
		return;

	// Draw new position.
	DrawIndicator(oldRect);

	// Update current time
	uint32 theTime = PixelsToTime((mousePt.x), GetCurrentTimeFormat(), GetCurrentResolution());
	fCueSheetWindow->GetCueSheetView()->SetCurrentTime(theTime);

	//	Tell cue sheet about new time as well.  This will inform all cues and will
	//	update their visibility on the stage
	BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
	BPoint drawPt( fIndicatorRect.left + (fIndicatorRect.Width() / 2)+1, fIndicatorRect.top);
	message->AddPoint("Where", drawPt);
	message->AddInt32("TheTime", theTime);
	fCueSheetWindow->GetCueSheetView()->MessageReceived(message);

	// Inform Transport as well
	TTransportPalette* theTransport = static_cast<MuseumApp*>(be_app)->GetTransport();
	if (theTransport)
		theTransport->PostMessage(message, theTransport->GetTransportPaletteView());

	// Update indicator tick
	UpdateTimeTick(drawPt);

	// Clean up
	delete message;
}


#pragma mark -
#pragma mark === Playback Routines ===

//---------------------------------------------------------------------
//	TrackPlayback
//---------------------------------------------------------------------
//
//	Called in response to a transport button click.
//

void TTimelineView::TrackPlayback()
{
	// Move indicator to current time position
	BRect oldRect;
	uint32 buttons = 0;

	// Get current time position in pixels
	int32 position = TimeToPixels( GetCurrentTime() - StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());

	// Save oldRect for redraw
	oldRect = fIndicatorRect;
	fIndicatorRect.left  = position - (kIndicatorWidth/2);
	fIndicatorRect.right = fIndicatorRect.left + kIndicatorWidth;

	// Draw new position and update indicator tick
	BPoint drawPt( fIndicatorRect.left + (fIndicatorRect.Width() / 2)+1, fIndicatorRect.top);
	fCueSheetWindow->Lock();
	UpdateTimeTick(drawPt);
	DrawIndicator(oldRect);
	fCueSheetWindow->Unlock();
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	ClipIndicatorRect
//---------------------------------------------------------------------
//
//	Clip indicator rect within the bounds of the timeline

void TTimelineView::ClipIndicatorRect()
{
	/*
	   BRect bounds = Frame();

	   if (fIndicatorRect.left <= bounds.left)
	        fIndicatorRect.left = bounds.left +10 ;

	   if (fIndicatorRect.right > bounds.right)
	        fIndicatorRect.right = bounds.right;
	 */
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TTimelineView::AttachedToWindow()
{
	if(fCueSheetWindow == NULL) {
		fCueSheetWindow = (TCueSheetWindow*)Window();
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//
void TTimelineView::SetParent(TCueSheetWindow* parent)
{
	fCueSheetWindow = parent;
}
