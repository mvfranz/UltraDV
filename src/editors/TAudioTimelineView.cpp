//---------------------------------------------------------------------
//
//	File:	TAudioTimelineView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.21.98
//
//	Desc:	Audio Timeline
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
#include "ResourceManager.h"

#include "MuseumApp.h"
#include "TAudioEditor.h"
#include "TAudioTimelineView.h"
#include "TAudioEditorView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioTimelineView::TAudioTimelineView(BRect bounds, TAudioEditor* parent) : BView(bounds, "AudioTimelineView", B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Set CueSheet parent
	fAudioEditor = parent;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioTimelineView::~TAudioTimelineView()
{
	// Free Indicator
	delete fIndicator;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TAudioTimelineView::Init()
{

	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Set up time rect
	BRect bounds = Bounds();

	// Set up indicator zone
	fIndicatorZone.Set( bounds.left, bounds.top, bounds.right, bounds.top + kIndicatorZoneHeight);

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


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAudioTimelineView::MouseDown(BPoint where)
{
	BRect bounds = Bounds();

	PushState();

	// Clip out Header and right side of indicator zone
	BRect clipRect = Bounds();
	clipRect.right = fIndicatorZone.right;

	// Set up redraw color
	rgb_color saveColor = HighColor();
	SetHighColor(kBeShadow);

	// Constrain mouse x to the fIndicatorZone
	if (where.x > fIndicatorZone.right)
		where.x = fIndicatorZone.right;

	// Trap the mouse down and move the indicator

	BPoint mousePt, savePt;
	BRect sectRect;
	uint32 buttons = 0;
	GetMouse(&mousePt, &buttons, true);

	while(buttons) {
		if (mousePt.x != savePt.x) {
			// Stash point for future compares
			savePt = mousePt;

			// Update tracking rect
			BRect oldRect = fIndicatorRect;
			fIndicatorRect.left = mousePt.x - (kIndicatorWidth/2);
			fIndicatorRect.right = fIndicatorRect.left + kIndicatorWidth;

			// Erase old indicator position and draw new position.
			oldRect.bottom-=2;
			//FillRect(oldRect);
			Invalidate(oldRect);
			DrawIndicator();

			// Tell time display where we are
			BPoint drawPt( fIndicatorRect.left + (fIndicatorRect.Width() / 2)+1, fIndicatorRect.top);
			UpdateTimeTick(drawPt);

		}

		// Get new mouse state
		GetMouse(&mousePt, &buttons, true);

		// Let other events pass
		snooze(20 * 1000);

		// Clip mouse from going into header or past fIndicatorZone
		if ( mousePt.x  <  fIndicatorZone.left )
			mousePt.x = fIndicatorZone.left;

		if ( mousePt.x  >  fIndicatorZone.right )
			mousePt.x = fIndicatorZone.right;

	}

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioTimelineView::MouseUp(BPoint where)
{


}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioTimelineView::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	// Draw new tick position
	UpdateTimeTick(where);
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioTimelineView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioTimelineView::KeyDown(const char* bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioTimelineView::KeyUp(const char* bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioTimelineView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioTimelineView::FrameResized(float new_width, float new_height)
{
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//

void TAudioTimelineView::MessageReceived(BMessage* message)
{
	BLooper* looper = Looper();
	looper->Lock();

	switch(message->what)
	{
	// We need to redraw the tick
	case UPDATE_AUDIOTIMELINE_MSG:
	{
		BPoint where;
		message->FindPoint("Where", &where);
		UpdateTimeTick(where);
	}
	break;


	default:
		BView::MessageReceived(message);
		break;
	}

	looper->Unlock();
}


#pragma mark -
#pragma mark === Drawing Routines ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioTimelineView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;

	// Save old colors
	PushState();

	// Draw indentation
	SetHighColor(kWhite);
	FillRect(updateRect);

	// Draw black line at bottom of timeline
	SetHighColor(kBlack);
	startPt.Set(Bounds().right, Bounds().bottom);
	endPt.Set(Bounds().left, Bounds().bottom);
	StrokeLine(startPt, endPt);

	//
	// Draw Ticks and Time Indicators
	//

	// Draw indicator zone
	if ( updateRect.Intersects( fIndicatorZone) ) {
		BRect indicatorRect(updateRect.left, fIndicatorZone.top, updateRect.right, fIndicatorZone.bottom);
		SetHighColor(kBeShadow);
		FillRect(indicatorRect);

		SetHighColor(kBlack);
		startPt.Set(updateRect.left, fIndicatorZone.bottom);
		endPt.Set(updateRect.right, fIndicatorZone.bottom);
		StrokeLine(startPt, endPt);
	}

	// Set up drawing coordinates.
	BRect frame = Frame();

	// Draw timeline
	BPoint textPt;
	//char  timeStr[256];

	BFont font;
	GetFont(&font);
	SetFont(be_plain_font);
	SetHighColor(kBlack);

	// Calculate the number of ticks that will fit in the timeline
	long numTicks = frame.Width() / kTickSpacing;

	// gzr: to do... Should this loop be put into a thread?
	for ( long index = 0; index <= numTicks; index++) {
		// Draw ticks
		startPt.Set( kTickSpacing * index, fIndicatorZone.bottom);
		endPt.Set( startPt.x, fIndicatorZone.bottom+kTickHeight);
		StrokeLine(startPt, endPt);
	}

	// Draw position indicator
	DrawIndicator();

	// Draw border line on left
	SetHighColor(kBlack);
	startPt.Set(fIndicatorZone.left, Bounds().top);
	endPt.Set(fIndicatorZone.left, Bounds().bottom);
	StrokeLine(startPt, endPt);

	// Draw border line on right
	startPt.Set(Bounds().right, Bounds().top);
	endPt.Set(Bounds().right, Bounds().bottom);
	StrokeLine(startPt, endPt);

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	UpdateTimeTick
//---------------------------------------------------------------------
//
//	Update the location of the time indicator tick
//

void TAudioTimelineView::UpdateTimeTick(BPoint where)
{
	//
	// Set up the environment
	//

	PushState();

	// Clip out Indicator Zone
	BRect bounds = Bounds();
	bounds.top += kIndicatorZoneHeight;

	// Set up environment
	PushState();

	// Force mouseX to be within the indicator zone bounds
	if (where.x > fIndicatorZone.right)
		where.x = fIndicatorZone.right;

	//      Only proceed if we have moved in the X coordinate space.  We don't care about Y.
	//	This avoids flicker in the time indicator palette
	if (where.x == fLastTick.x)
		return;

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

	// Send a message to the Time Palette telling it about the new tick time
	BMessage* message = new BMessage(NEW_TIME_MSG);

	// Get insert time based on point.x
	/*double theTime = PixelsToTime((where.x), fCueSheetWindow->GetCueSheetView()->fTimeFormat, fCueSheetWindow->GetCueSheetView()->fResolution);

	   // We are going to pass all the arguments needed for TimeToString
	   message->AddInt32("TheTime", theTime);
	   message->AddInt16("TimeFormat", fCueSheetWindow->GetCueSheetView()->fTimeFormat);
	   ((MuseumApp *)be_app)->GetTimePalette()->PostMessage(message, NULL);*/

	// Restore environment
	PopState();

	// Clean up message
	delete message;
}


//---------------------------------------------------------------------
//	DrawIndicator
//---------------------------------------------------------------------
//
//	Draw the position indicator at the proper location
//

void TAudioTimelineView::DrawIndicator()
{

	// Set up enironment
	PushState();

	SetDrawingMode(B_OP_OVER);
	BPoint drawPt(fIndicatorRect.left, fIndicatorRect.top);
	DrawBitmap(fIndicator, drawPt);

	// Make sure it isn't outside the timeline bounds
	ClipIndicatorRect();

	// Draw the indicator line down all views
	// We do this by sending a message out to the audio editor view
	// and they will draw the line where indicated
	BMessage* message = new BMessage(UPDATE_AUDIOINDICATOR_MSG);
	drawPt.Set( fIndicatorRect.left + (fIndicatorRect.Width() / 2), fIndicatorRect.top);
	message->AddPoint("IndicatorPoint", drawPt);

	// Force message into the application
	fAudioEditor->GetEditorView()->MessageReceived(message);
	delete message;

	// Restore environment
	PopState();

}



//---------------------------------------------------------------------
//	DrawIndicator
//---------------------------------------------------------------------
//
//	Draw the position indicator at the proper location
//

void TAudioTimelineView::SetTimelineViewBounds(BRect bounds)
{
	// Resize view
	ResizeTo( bounds.Width(), Frame().Height());

	// Resize indicator zone
	fIndicatorZone.Set( Bounds().left, Bounds().top, bounds.right, bounds.top + kIndicatorZoneHeight);

	// Redraw
	Invalidate();
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	ClipIndicatorRect
//---------------------------------------------------------------------
//
//	Clip indicator rect within the bounds of the timeline

void TAudioTimelineView::ClipIndicatorRect()
{
	/*
	   BRect bounds = Frame();

	   if (fIndicatorRect.left <= bounds.left)
	        fIndicatorRect.left = bounds.left +10 ;

	   if (fIndicatorRect.right > bounds.right)
	        fIndicatorRect.right = bounds.right;
	 */
}