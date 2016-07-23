//---------------------------------------------------------------------
//
//	File:	TExportZone.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Control determining amount of cue sheet time to
//			export or preview duting playback
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
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"
#include "MuseumApp.h"

#include "TPlaybackEngine.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TExportTimeView.h"

#include "TMouseWatcher.h"

#include "TExportZone.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExportZone::TExportZone(BRect bounds, TCueSheetWindow* parent) : BView(bounds, "ExportZoneView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	// Set CueSheet parent
	fCueSheetWindow = parent;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

TExportZone::TExportZone(BMessage* data) : BView(data)
{
	fCueSheetWindow = NULL;

	//	Load left and right slider tabs
	fInMarker       = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fExportSliderLeft;
	fOutMarker      = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fExportSliderRight;

	//	Initialize drag type to non
	fDragType = kNoDrag;

	//
	// Rehydrate the cue from message data
	//

	// Extract our member variables from the archive
	data->FindRect("ExportChannel", &fExportChannel);
	data->FindRect("InRect", &fInRect);
	data->FindRect("OutRect", &fOutRect);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TExportZone::~TExportZone()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TExportZone::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Set up time rect
	const BRect bounds = Bounds();

	// Set up slider zones
	fInRect.Set( bounds.left, bounds.top+1, bounds.left+kExportSliderWidth, bounds.bottom-1);
	fOutRect.Set( bounds.right - kExportSliderWidth, bounds.top+1, bounds.right, bounds.bottom-1);

	//	Set up channel zone
	fExportChannel.Set(fInRect.right, fInRect.top, fOutRect.left, fInRect.bottom-1);

	//	Load left and right slider tabs
	fInMarker       = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fExportSliderLeft;
	fOutMarker      = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fExportSliderRight;

	//	Initialize drag type to non
	fDragType = kNoDrag;
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TExportZone::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TExportZone") )
		return new TExportZone(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TExportZone::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TExportZone");

		// Add our member variables to the archive
		data->AddRect("ExportChannel", fExportChannel);
		data->AddRect("InRect", fInRect);
		data->AddRect("OutRect", fOutRect);

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}

#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TExportZone::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( IsPlaying() )
		return;

	// Activate cue sheet
	Window()->Activate(true);

	// Reset cue drag flag
	static_cast<MuseumApp*>(be_app)->fIsCueDrag = false;

	//	Reset drag type
	fDragType = kNoDrag;

	// Check to see which button is down
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	uint32 type;
	int32 count = 0;
	BMessage* message = Window()->CurrentMessage();

	// Determine which button has been clicked
	switch(buttons)
	{
	case B_PRIMARY_MOUSE_BUTTON:
		if (B_OK == message->GetInfo("clicks", &type, &count) ) {
			int32 clickCount = message->FindInt32("clicks", count-1);
			//	Handle double click
			if ( (clickCount == 2) ) {
				HandleDoubleClick(where);
			}
			//	Determine where mouse click occured
			else{
				HandleClick(where);
			}
		}
		break;

	default:
		break;
	}
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TExportZone::MouseUp(BPoint where)
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

void TExportZone::MouseMoved( BPoint where, uint32 code, const BMessage* message )
{

	// Do nothing if we are playing
	if ( fCueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;

	// Do nothing if window is inactive
	//if ( fCueSheetWindow->IsActive() = =false )
	//	return;

	// Exit if panel is open
	if ( fCueSheetWindow->IsPanelOpen())
		return;

	// Determine type of movement
	switch(code)
	{
	case B_ENTERED_VIEW:
		break;

	//	Determie tpye of tracking to handle
	case B_INSIDE_VIEW:
	{


	}
	break;

	case B_EXITED_VIEW:
		// Send tick off the screen
		where.x = 0;
		break;

	default:
		break;
	}

	// Only update the tick if we aren't tracking position indicator
	//uint32        buttons = 0;

	//Window()->CurrentMessage()->FindInt32("buttons", (int32 *)&buttons);
	//if (!buttons)
	//	UpdateTimeTick(where);

}


//---------------------------------------------------------------------
//	HandleClick
//---------------------------------------------------------------------
//
//	Handle mouse click
//

void TExportZone::HandleClick(BPoint where)
{

	//
	//	Determine where mouse click occured
	//

	//	In InRect?
	if (fInRect.Contains(where)) {
		fDragType = kInDrag;
	}
	//	fOutRect?
	else if (fOutRect.Contains(where)) {
		fDragType = kOutDrag;
	}
	//	fExportChannel?
	else if (fExportChannel.Contains(where) ) {
		//	Handle zone drag special
		TrackZone(where);
	}

	// Launch mouse tracking thread
	if (fDragType != kNoDrag)
		fMouseTracker = StartMouseWatcher(this);

}

//---------------------------------------------------------------------
//	HandleDoubleClick
//---------------------------------------------------------------------
//
//	Handle mouse double click.  If within fExportChannel, resize
//	fExportChannel to span visible timeline width
//

void TExportZone::HandleDoubleClick(BPoint where)
{
	//	Get window bounds
	const BRect winBounds = Window()->Bounds();

	//	Get cue sheet bounds
	const BRect bounds = Bounds();

	//	Get total pixel length for bounds checking
	int32 pixelWidth = TimeToPixels( Duration(), GetCurrentTimeFormat(), GetCurrentResolution() );

	//	Update tracking rects
	fInRect.left    = bounds.left;
	fInRect.right   = fInRect.left + kExportSliderWidth;

	//	Set up out rect.  Check for right side length bounds violation
	int32 outRight = (bounds.left + winBounds.Width()) - kHeaderWidth;
	if (outRight > pixelWidth)
		outRight = pixelWidth;

	fOutRect.right   = outRight;
	fOutRect.left    = fOutRect.right - kExportSliderWidth;

	//	Update fExportChannel
	fExportChannel.left  = fInRect.right;
	fExportChannel.right = fOutRect.left;

	//	Update cue sheet variables
	fCueSheetWindow->GetCueSheetView()->SetExportStartTime( PixelsToTime(fInRect.left, GetCurrentTimeFormat(), GetCurrentResolution()) );
	fCueSheetWindow->GetCueSheetView()->SetExportStopTime( PixelsToTime(fOutRect.right, GetCurrentTimeFormat(), GetCurrentResolution()) );

	//	Update text items
	fCueSheetWindow->GetExportTimeView()->DrawInText();
	fCueSheetWindow->GetExportTimeView()->DrawOutText();

	//	Redraw
	Invalidate();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//

void TExportZone::MessageReceived(BMessage* message)
{
	switch(message->what)
	{

	// Mouse Tracking Messages
	case MW_MOUSE_MOVED:
	{
		BPoint mousePt;
		message->FindPoint("where", &mousePt);

		switch(fDragType)
		{
		case kInDrag:
			TrackInMarker(mousePt);
			break;

		case kOutDrag:
			TrackOutMarker(mousePt);
			break;

		default:
			break;
		}
	}
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

void TExportZone::Draw(BRect updateRect)
{
	BPoint startPt, endPt;

	const BRect bounds = Bounds();

	// Save environment
	PushState();

	//
	//	Draw zone and Start and End indicators
	//

	// Draw export zone
	SetHighColor(kTaupe);
	FillRect(updateRect);

	//	Draw top highlight
	SetHighColor(kLightTaupe);
	startPt.Set(updateRect.left, bounds.top);
	endPt.Set(updateRect.right, bounds.top);
	StrokeLine(startPt, endPt);

	//	Draw top shadow
	SetHighColor(kDarkTaupe);
	startPt.Set(updateRect.left, bounds.top-1);
	endPt.Set(updateRect.right, bounds.top-1);
	StrokeLine(startPt, endPt);

	//	Draw bottom highlight
	SetHighColor(kLightTaupe);
	startPt.Set(updateRect.left, bounds.bottom-1);
	endPt.Set(updateRect.right, bounds.bottom-1);
	StrokeLine(startPt, endPt);

	//	Draw bottom border line
	SetHighColor(kBlack);
	startPt.Set(updateRect.left, bounds.bottom);
	endPt.Set(updateRect.right, bounds.bottom);
	StrokeLine(startPt, endPt);

	//	Crop 1 pixel from bottom of channel height
	BRect channelRect = fExportChannel;
	channelRect.bottom--;

	// Draw export channel
	if (updateRect.Intersects(channelRect) ) {
		BRect fillRect = updateRect & channelRect;

		//	Fill
		SetHighColor(kKhaki);
		FillRect(fillRect);

		//	Highlight
		BPoint startPt, endPt;
		SetHighColor(kLightKhaki);
		startPt.Set(fillRect.left, fExportChannel.top);
		endPt.Set(fillRect.right, fExportChannel.top);
		StrokeLine(startPt, endPt);

		//	Shadow
		SetHighColor(kDarkKhaki);
		startPt.Set(fillRect.left, fExportChannel.bottom);
		endPt.Set(fillRect.right, fExportChannel.bottom);
		StrokeLine(startPt, endPt);
	}

	// Draw ExportStart and ExportEnd markers
	DrawInMarker(updateRect);
	DrawOutMarker(updateRect);

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	DrawInMarker
//---------------------------------------------------------------------
//
//	Draw the ExportTimeStart marker at proper location
//

void TExportZone::DrawInMarker(BRect updateRect)
{
	// Set up environment
	PushState();

	BPoint drawPt;

	// Draw left marker
	if (updateRect.Intersects(fInRect) ) {
		// Draw indicator in new location
		drawPt.Set(fInRect.left, fInRect.top);
		DrawBitmap(fInMarker, drawPt);
	}

	// Draw right marker
	if (updateRect.Intersects(fOutRect) ) {
		// Draw indicator in new location
		drawPt.Set(fOutRect.left, fOutRect.top);
		DrawBitmap(fOutMarker, drawPt);
	}

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	DrawOutMarker
//---------------------------------------------------------------------
//
//	Draw the ExportTimeEnd marker at proper location
//

void TExportZone::DrawOutMarker(BRect updateRect)
{
	// Set up environment
	PushState();

	BPoint drawPt;

	// Draw right marker
	if (updateRect.Intersects(fOutRect) ) {
		// Draw indicator in new location
		drawPt.Set(fOutRect.left, fOutRect.top);
		DrawBitmap(fOutMarker, drawPt);
	}

	// Restore environment
	PopState();
}


#pragma mark -
#pragma mark === Resolution Handling ===

//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//	Resize timeline based on new resolution
//

void TExportZone::ResolutionChanged(int32 resizePixels)
{
	const BRect bounds = Bounds();

	// Get start and duration in pixels
	uint32 startTime = fCueSheetWindow->GetCueSheetView()->GetExportStartTime();
	uint32 stopTime  = fCueSheetWindow->GetCueSheetView()->GetExportStopTime();

	uint32 inPixels         = TimeToPixels( startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	uint32 outPixels = TimeToPixels( stopTime, GetCurrentTimeFormat(), GetCurrentResolution());

	//	Update slider zones
	fInRect.left    = inPixels;
	fInRect.right   = fInRect.left + kExportSliderWidth;

	fOutRect.right   = outPixels;
	fOutRect.left    = fOutRect.right - kExportSliderWidth;

	// Update channel zone
	fExportChannel.left  = fInRect.right;
	fExportChannel.right = fOutRect.left;

	// Force redraw
	Invalidate();
}


#pragma mark -
#pragma mark === Marker Tracking ===

//---------------------------------------------------------------------
//	TrackInMarker
//---------------------------------------------------------------------
//
//	Draw new position of fInMarker
//

void TExportZone::TrackInMarker(BPoint mousePt)
{

	const BRect bounds = Bounds();

	//	Constrain to left side view area
	if (mousePt.x < 0)
		mousePt.x = 0;

	//	Don't allow overlap with left side tracker
	if ( (mousePt.x + kExportSliderWidth) >= fOutRect.left)
		mousePt.x = fOutRect.left - kExportSliderWidth;

	// Save oldRect for redraw
	BRect oldRect  = fInRect;
	fInRect.left  = mousePt.x;
	fInRect.right = fInRect.left + kExportSliderWidth;

	// Exit if there is no change in position
	if (oldRect == fInRect)
		return;

	//	Update fExportChannel
	fExportChannel.left = fInRect.right;

	//	Clean up old position
	BRect updateRect = oldRect;

	if (oldRect.left <= fInRect.left) {
		updateRect.right = fInRect.right;
	} else {
		updateRect.left  = fInRect.left;
	}

	Draw(updateRect);

	//	Update CueSheet variable
	uint32 newInTime = StartTime() + PixelsToTime(fInRect.left, GetCurrentTimeFormat(), GetCurrentResolution());
	fCueSheetWindow->GetCueSheetView()->SetExportStartTime(newInTime);

	//	Update text
	fCueSheetWindow->GetExportTimeView()->DrawInText();
}


//---------------------------------------------------------------------
//	TrackOutMarker
//---------------------------------------------------------------------
//
//	Draw new position of fOutMarker
//

void TExportZone::TrackOutMarker(BPoint mousePt)
{
	//	Constrain to out time of cue sheet
	uint32 outPixels = TimeToPixels( Duration() - StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());

	if (mousePt.x > outPixels)
		mousePt.x = outPixels;

	//	Don't allow overlap with fInRect
	if ( (mousePt.x-kExportSliderWidth) < fInRect.right)
		mousePt.x = fInRect.right + kExportSliderWidth;

	// Save oldRect for redraw
	BRect oldRect   = fOutRect;
	fOutRect.right = mousePt.x;
	fOutRect.left = fOutRect.right - kExportSliderWidth;


	// Exit if there is no change in position
	if (oldRect == fOutRect)
		return;

	//	Update fExportChannel
	fExportChannel.right = fOutRect.left;

	//	Clean up old position
	BRect updateRect = oldRect;

	if (fOutRect.left <= oldRect.left) {
		updateRect.left = fOutRect.left;
	} else {
		updateRect.right  = fOutRect.right;
	}

	Draw(updateRect);

	//	Update CueSheet variable
	uint32 newOutTime = StartTime() + PixelsToTime(fOutRect.right, GetCurrentTimeFormat(), GetCurrentResolution());
	fCueSheetWindow->GetCueSheetView()->SetExportStopTime(newOutTime);

	//	Update text
	fCueSheetWindow->GetExportTimeView()->DrawOutText();

}


//---------------------------------------------------------------------
//	TrackZone
//---------------------------------------------------------------------
//
//	Track movement of entire fExportChannel
//

void TExportZone::TrackZone(BPoint mousePt)
{
	//	Set up variables
	const float width       = fExportChannel.Width();
	const int32 delta       = mousePt.x - fExportChannel.left;

	//	Save mousePt
	BPoint oldPt = mousePt;

	// Check to see if button is down
	uint32 buttons = 0;
	GetMouse(&mousePt, &buttons, true);

	//	Track while mouse is down
	while(buttons) {
		if (mousePt.x != oldPt.x) {
			// Save oldRect for redraw
			BRect oldRect = fExportChannel;
			oldRect.left  -= kExportSliderWidth;
			oldRect.right += kExportSliderWidth;

			//	Update fExportChannel
			fExportChannel.left  += mousePt.x - oldPt.x;
			fExportChannel.right = fExportChannel.left + width;

			//	Correct left side bounds violation
			if ( fExportChannel.left - kExportSliderWidth < 0) {
				fExportChannel.left = 0 + kExportSliderWidth;
				fExportChannel.right = fExportChannel.left + width;
			}

			//	Correct right side bounds violation
			uint32 outPixels = TimeToPixels( Duration() - StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());
			if ( fExportChannel.right + kExportSliderWidth > outPixels) {
				fExportChannel.right = outPixels - kExportSliderWidth;
				fExportChannel.left  = fExportChannel.right - width;
			}

			//	Update fInRect and fInRect
			fInRect.left    = fExportChannel.left - kExportSliderWidth;
			fInRect.right   = fInRect.left + kExportSliderWidth;

			fOutRect.left    = fExportChannel.right;
			fOutRect.right   = fOutRect.left + kExportSliderWidth;

			//	Update CueSheet variables
			uint32 newInTime  = StartTime() + PixelsToTime(fInRect.left, GetCurrentTimeFormat(), GetCurrentResolution());
			uint32 newOutTime = StartTime() + PixelsToTime(fOutRect.right, GetCurrentTimeFormat(), GetCurrentResolution());
			fCueSheetWindow->GetCueSheetView()->SetExportStartTime(newInTime);
			fCueSheetWindow->GetCueSheetView()->SetExportStopTime(newOutTime);

			//	Calculate updateRect
			BRect updateRect = oldRect;

			//	Did we move forward
			if (updateRect.left < fInRect.left) {
				updateRect.right = fOutRect.right;
			}
			//	We moved back
			else{
				updateRect.left = fInRect.left;
			}

			//	Force redraw
			if (oldRect != updateRect)
				Invalidate(updateRect);

			//	Save point for future compare
			oldPt = mousePt;

			//	Update text items
			fCueSheetWindow->GetExportTimeView()->DrawInText();
			fCueSheetWindow->GetExportTimeView()->DrawOutText();
		}

		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);

		//
		//	Clip mouse

		// Constrain mouse x to the fExportZone
		if (mousePt.x < 0)
			mousePt.x = 0;

		//if (mousePt.x >= fExportZone.right)
		//	mousePt.x = fExportZone.right - (kExportSliderWidth/2);

	}
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

void TExportZone::AttachedToWindow()
{
	if(fCueSheetWindow == NULL) {
		fCueSheetWindow = (TCueSheetWindow*)Window();
	}

	//	PAss up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//
void TExportZone::SetParent(TCueSheetWindow* parent)
{
	fCueSheetWindow = parent;
}