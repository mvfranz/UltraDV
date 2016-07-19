//---------------------------------------------------------------------
//
//	File:	TAudioScalerView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//	Desc:	View that handles the setting of the audio editor scale
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
#include "TBitmapView.h"
#include "TAudioScalerView.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"


// Constants
const short kScalerTickHeight = 5;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioScalerView::TAudioScalerView(BRect bounds, TAudioEditorView *parent) : BView(bounds, "AudioScalerView", B_FOLLOW_LEFT|B_FOLLOW_BOTTOM , B_WILL_DRAW)
{
	// Set CueSheet parent
	fEditor = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioScalerView::~TAudioScalerView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TAudioScalerView::Init()
{
	// Set up the bounding rectangles for the Text Display and Tick Indicator areas
	BRect area = Bounds();
	
	// The control rect is the left half of the bounding rect
	fControlRect = area;
	fControlRect.right = (fControlRect.right - fControlRect.left) / 2;
	
	// The text rect is the right half of the bounding rect
	fTextRect = area;
	fTextRect.left = (fTextRect.right - fTextRect.left) / 2;
		
	// Set up the array of rects for hitpoint detection.  We divide the area into 
	// a number of rects equal to the total number of ticks.		
	short tickWidth = fControlRect.Width();
	short spaceWidth = tickWidth / kTotalTicks;
	
	for (short index = 0; index < kTotalTicks; index++)
	{
		fClickArray[index].Set( fControlRect.left +( index * spaceWidth), fControlRect.top,
								 fControlRect.left + spaceWidth +( index * spaceWidth), fControlRect.bottom);						
	}
		
	// Load thumb bitmap
	BBitmap *bitmap = GetIcon8FromResource("TimeThumb");
	
	// Create Indicator View 
	BRect bounds( Bounds().left, Bounds().bottom - (kTimeIndicatorHeight-1), Bounds().left+(kTimeIndicatorWidth-1), Bounds().bottom);
	fIndicator = new TBitmapView( bounds, "Scaler", bitmap, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	
	// Add to view, move it to the proper resolution tick and show it
	AddChild(fIndicator);
	//fIndicator->MoveTo(fClickArray[fCueSheet->fResolution].left, Bounds().bottom - (kTimeIndicatorHeight-1));	
	fIndicator->Show();
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioScalerView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;
		
	// Save old colors
	PushState();
		
	// Fill text rect
	SetHighColor(kBeGrey);
	FillRect(fTextRect);	
		
	// Fill control rect
	SetHighColor(kMediumGrey);
	FillRect(fControlRect);	
	
	// Outline entire area
	SetHighColor(kBeShadow);
	StrokeRect(fTextRect);
	
	// Draw light shadow at top of control area
	SetHighColor(kDarkGrey);
	startPt.Set(fControlRect.left+1, fControlRect.top+1);
	endPt.Set(fControlRect.right-1, fControlRect.top+1);
	StrokeLine(startPt, endPt);
	
	// Draw highlight at top of text area
	SetHighColor(kBeHighlight);
	startPt.Set(fTextRect.left+1, fTextRect.top+1);
	endPt.Set(fTextRect.right-1, fTextRect.top+1);
	StrokeLine(startPt, endPt);
		
	// Draw tick indicator and ticks
	DrawTimeScaleTicks();
	DrawTickIndicator();
			
	// Draw seperator
	SetHighColor(kDarkGrey);
	startPt.x = (Bounds().right - Bounds().left) / 2;
	startPt.y = Bounds().top+1;
	endPt.x = (Bounds().right - Bounds().left) / 2;
	endPt.y = Bounds().bottom;
	StrokeLine(startPt, endPt);
	
	// Draw text
	// gzr: to do... Figure out how to center the text in the fTextRect
	//char timeStr[256];
	//GetTimeScaleString(fCueSheet->fResolution, (char *)&timeStr);
	SetFont(be_plain_font);   
	SetHighColor(kBlack);	
	endPt.x += 2;
	endPt.y -= 3;
	//DrawString(timeStr, endPt); 
	
	// Restore environment
	PopState();
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAudioScalerView::MouseDown(BPoint where)
{	
	
	// Make sure we are in the control rect
	if ( !fControlRect.Contains(where) )
		return;
	
	// Now, clip the mouse and keep it inside the fControlRect - the width
	// of the indicator
	if (where.x > fControlRect.right - kTimeIndicatorWidth)
		where.x = fControlRect.right - kTimeIndicatorWidth;
	
	// Now, locate which tick rect click is in
	short rectIndex = GetClickRect(where);

	// Move tick indicator view to the click rect closest to the mouse down location
	fIndicator->MoveTo(fClickArray[rectIndex].left, Bounds().bottom - (kTimeIndicatorHeight-1));
	//fCueSheet->fResolution = rectIndex;
	Invalidate(fTextRect);
	
	// If the user is holding down the mouse, move the indicator in reply
	uint32 	buttons = 0;
	BPoint 	savePt;
	
	GetMouse(&where, &buttons, true);		
	while (buttons)
	{
		// Clip the mouse point to be within our control rect
		if (where.x < fControlRect.left)
			where.x = fControlRect.left;
		
		if (where.x > fControlRect.right - kTimeIndicatorWidth)
			where.x = fControlRect.right - kTimeIndicatorWidth;
			
		where.y = fControlRect.Height()/2;
		
		if (where != savePt)
		{
			// Now, locate which tick rect click is in
			short rectIndex = GetClickRect(where);

			// Move tick indicator view to the click rect closest to the mouse down location
			fIndicator->MoveTo(fClickArray[rectIndex].left, Bounds().bottom - (kTimeIndicatorHeight-1));		
			//fCueSheet->fResolution = rectIndex;
			Invalidate(fTextRect);
			
			// Save the mouse location
			savePt = where;
		}		

		// Get new mouse location and button state
		GetMouse(&where, &buttons, true);							
					
		// Let other events through
		snooze(20 * 1000);
	}
	
	// Inform everyone that the resolution has changed.  We need to tell the timeline and all the cues
	//BMessage *message = new BMessage(RESOLUTION_CHANGE_MSG);		
	//((MuseumApp *)be_app)->GetCueSheet()->PostMessage(message, NULL);
	//delete message;	
	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioScalerView::MouseUp(BPoint where)
{
	
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioScalerView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioScalerView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioScalerView::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioScalerView::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioScalerView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioScalerView::FrameResized(float new_width, float new_height)
{
}


#pragma mark === TimeScale Indicator ===

	
//---------------------------------------------------------------------
//	DrawTimeScaleTicks
//---------------------------------------------------------------------
//
//	Draw the tick marks
//

void TAudioScalerView::DrawTimeScaleTicks()
{
	BPoint startPt, endPt;
	
	// Save current color
	rgb_color saveColor = HighColor();
	
	// Draw ticks.  There are ten, accounting for each of the 
	// ten time states that are represented on the Timeline
	SetHighColor(kBlack);
	short tickWidth = kTimeScalerWidth / 2;
	short spaceWidth = tickWidth / kTotalTicks;
	for (short i = 0; i < kTotalTicks; i++)
	{
		startPt.Set(fControlRect.left+5+(i * spaceWidth), fControlRect.top+1);
		endPt.Set(fControlRect.left+5+(i * spaceWidth), fControlRect.top+kScalerTickHeight);
		StrokeLine(startPt, endPt);	
	}
	
	// Restore color
	SetHighColor(saveColor);
}	
	
//---------------------------------------------------------------------
//	DrawTickIndicator
//---------------------------------------------------------------------
//
//	Draw the tick indicator icon at the proper tick mark
//

void TAudioScalerView::DrawTickIndicator()
{
	/*BPoint startPt, endPt;	
	
	// Save current color
	rgb_color saveColor = HighColor();
	
	BPoint tickOffset;	
	tickOffset.Set(Bounds().left+5, Bounds().bottom);
		
	short tickWidth = (kTimeScalerWidth / 2);
	short spaceWidth = tickWidth / kTotalTicks;
	
	// Set proper tick location
	tickOffset.x += (spaceWidth*fCueSheet->fResolution) - 4;
	
	// Move view	
	fIndicator->MoveTo(tickOffset.x, Bounds().bottom - (kTimeIndicatorHeight-1));
	
	// Restore color
	SetHighColor(saveColor);*/
}



//---------------------------------------------------------------------
//	GetClickRect
//---------------------------------------------------------------------
//
//	Return the rect in the fClickArray that the point resides in
//

short TAudioScalerView::GetClickRect(BPoint where)
{
	// Now, locate which tick rect click is in
	short rectIndex = 0;
	
	for (short index = 0; index < kTotalTicks; index++)
	{
		if ( fClickArray[index].Contains(where) )
		{
			rectIndex = index;
			return rectIndex;
		}
	}
	return 0;
}