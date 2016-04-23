//---------------------------------------------------------------------
//
//	File:	TTimeScalerView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	View that handles the setting of the time scale
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
#include "TTimeScalerView.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"


// Constants
const short kScalerTickHeight = 5;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimeScalerView::TTimeScalerView(BRect bounds, TCueSheetView *parent) : BView(bounds, "TimeScalerView", B_FOLLOW_LEFT|B_FOLLOW_BOTTOM , B_WILL_DRAW)
{
	// Set CueSheet parent
	//m_CueSheet = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimeScalerView::TTimeScalerView(BMessage *data) : BView(data)
{
	// Perform default initialization
	Init();		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimeScalerView::~TTimeScalerView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TTimeScalerView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	// Set up the bounding rectangles for the Text Display and Tick Indicator areas
	BRect area = Bounds();
	
	// The control rect is the left half of the bounding rect
	m_ControlRect = area;
	m_ControlRect.right = (m_ControlRect.right - m_ControlRect.left) / 2;
	
	// The text rect is the right half of the bounding rect
	m_TextRect = area;
	m_TextRect.left = (m_TextRect.right - m_TextRect.left) / 2;
		
	// Set up the array of rects for hitpoint detection.  We divide the area into 
	// a number of rects equal to the total number of ticks.		
	short tickWidth = m_ControlRect.Width();
	short spaceWidth = tickWidth / kTotalTicks;
	
	for (short index = 0; index < kTotalTicks; index++)
	{
		m_ClickArray[index].Set( m_ControlRect.left +( index * spaceWidth), m_ControlRect.top,
								 m_ControlRect.left + spaceWidth +( index * spaceWidth), m_ControlRect.bottom);						
	}
		
	// Load thumb bitmap
	BBitmap *bitmap = GetIcon8FromResource("TimeThumb");
	
	// Create Indicator View 
	BRect bounds( Bounds().left, Bounds().bottom - (kTimeIndicatorHeight-1), Bounds().left+(kTimeIndicatorWidth-1), Bounds().bottom);
	m_Indicator = new TBitmapView( bounds, "TimeScaler", bitmap, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	
	// Add to view, move it to the proper resolution tick and show it
	AddChild(m_Indicator);
	m_Indicator->MoveTo(m_ClickArray[GetCurrentResolution()].left, Bounds().bottom - (kTimeIndicatorHeight-1));	
	m_Indicator->Show();
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TTimeScalerView::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TTimeScalerView") ) 
		return new TTimeScalerView(archive); 
		
	return NULL; 
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TTimeScalerView::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TTimeScalerView");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{		
		
		}		
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
//

void TTimeScalerView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;
		
	// Save old colors
	PushState();
		
	// Fill text rect
	SetHighColor(kBeGrey);
	FillRect(m_TextRect);	
		
	// Fill control rect
	SetHighColor(kMediumGrey);
	FillRect(m_ControlRect);	
	
	// Outline entire area
	SetHighColor(kBeShadow);
	StrokeRect(m_TextRect);
	
	// Draw light shadow at top of control area
	SetHighColor(kBlack);
	startPt.Set(m_ControlRect.left+1, m_ControlRect.top+1);
	endPt.Set(m_ControlRect.right-1, m_ControlRect.top+1);
	StrokeLine(startPt, endPt);
	
	// Draw highlight at top of text area
	SetHighColor(kBeHighlight);
	startPt.Set(m_TextRect.left+1, m_TextRect.top+1);
	endPt.Set(m_TextRect.right-1, m_TextRect.top+1);
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
	// gzr: to do... Figure out how to center the text in the m_TextRect
	char timeStr[256];
	GetTimeScaleString(GetCurrentResolution(), (char *)&timeStr);
	SetFont(be_plain_font);   
	SetHighColor(kBlack);	
	endPt.x += 2;
	endPt.y -= 3;
	DrawString(timeStr, endPt); 
	
	// Restore environment
	PopState();
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TTimeScalerView::MouseDown(BPoint where)
{	
	
	// Make sure we are in the control rect
	if ( !m_ControlRect.Contains(where) )
		return;
	
	// Activate cue sheet
	Window()->Activate(true);
	
	// Now, clip the mouse and keep it inside the m_ControlRect - the width
	// of the indicator
	if (where.x > m_ControlRect.right - kTimeIndicatorWidth)
		where.x = m_ControlRect.right - kTimeIndicatorWidth;
	
	// Now, locate which tick rect click is in
	short rectIndex = GetClickRect(where);

	// Move tick indicator view to the click rect closest to the mouse down location
	m_Indicator->MoveTo(m_ClickArray[rectIndex].left, Bounds().bottom - (kTimeIndicatorHeight-1));
	TCueSheetView *cueSheet = static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView();	
	cueSheet->SetResolution(rectIndex);
	Invalidate(m_TextRect);
	
	// If the user is holding down the mouse, move the indicator in reply
	uint32 	buttons = 0;
	BPoint 	savePt;
	
	GetMouse(&where, &buttons, true);		
	while (buttons)
	{
		// Clip the mouse point to be within our control rect
		if (where.x < m_ControlRect.left)
			where.x = m_ControlRect.left;
		
		if (where.x > m_ControlRect.right - kTimeIndicatorWidth)
			where.x = m_ControlRect.right - kTimeIndicatorWidth;
			
		where.y = m_ControlRect.Height()/2;
		
		if (where != savePt)
		{
			// Now, locate which tick rect click is in
			short rectIndex = GetClickRect(where);

			// Move tick indicator view to the click rect closest to the mouse down location
			m_Indicator->MoveTo(m_ClickArray[rectIndex].left, Bounds().bottom - (kTimeIndicatorHeight-1));		
			cueSheet->SetResolution(rectIndex);
			Invalidate(m_TextRect);
			
			// Save the mouse location
			savePt = where;
		}		

		// Get new mouse location and button state
		GetMouse(&where, &buttons, true);							
					
		// Let other events through
		snooze(20 * 1000);
	}
	
	// Inform everyone that the resolution has changed.  We need to tell the timeline and all the cues
	BMessage *message = new BMessage(RESOLUTION_CHANGE_MSG);		
	cueSheet->GetParent()->PostMessage(message, NULL);
	delete message;	
	
}

#pragma mark === TimeScale Indicator ===

	
//---------------------------------------------------------------------
//	DrawTimeScaleTicks
//---------------------------------------------------------------------
//
//	Draw the tick marks
//

void TTimeScalerView::DrawTimeScaleTicks()
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
		startPt.Set(m_ControlRect.left+5+(i * spaceWidth), m_ControlRect.top+1);
		endPt.Set(m_ControlRect.left+5+(i * spaceWidth), m_ControlRect.top+kScalerTickHeight);
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

void TTimeScalerView::DrawTickIndicator()
{
	BPoint startPt, endPt;	
	
	// Save current color
	PushState();
	
	TCueSheetView *cueSheet = static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView();	
	
	BPoint tickOffset;	
	tickOffset.Set(Bounds().left+5, Bounds().bottom);
		
	short tickWidth = (kTimeScalerWidth / 2);
	short spaceWidth = tickWidth / kTotalTicks;
	
	// Set proper tick location
	tickOffset.x += (spaceWidth*cueSheet->GetResolution()) - 4;
	
	// Move view	
	m_Indicator->MoveTo(tickOffset.x, Bounds().bottom - (kTimeIndicatorHeight-1));
	
	// Restore color
	PopState();
}



//---------------------------------------------------------------------
//	GetClickRect
//---------------------------------------------------------------------
//
//	Return the rect in the m_ClickArray that the point resides in
//

short TTimeScalerView::GetClickRect(BPoint where)
{
	// Now, locate which tick rect click is in
	short rectIndex = 0;
	
	for (short index = 0; index < kTotalTicks; index++)
	{
		if ( m_ClickArray[index].Contains(where) )
		{
			rectIndex = index;
			return rectIndex;
		}
	}
	return 0;
}