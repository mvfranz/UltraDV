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

TExportZone::TExportZone(BRect bounds, TCueSheetWindow *parent) : BView(bounds, "ExportZoneView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	// Set CueSheet parent
	m_CueSheetWindow = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

TExportZone::TExportZone(BMessage *data) : BView(data)
{	
	m_CueSheetWindow = NULL;
	
	//	Load left and right slider tabs
	m_InMarker	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_ExportSliderLeft;
	m_OutMarker	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_ExportSliderRight;
	
	//	Initialize drag type to non
	m_DragType = kNoDrag;	
	
	//
	// Rehydrate the cue from message data
	//
	
	// Extract our member variables from the archive	
	data->FindRect("ExportChannel", &m_ExportChannel);
	data->FindRect("InRect", &m_InRect);
	data->FindRect("OutRect", &m_OutRect);
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
	m_InRect.Set( bounds.left, bounds.top+1, bounds.left+kExportSliderWidth, bounds.bottom-1);
	m_OutRect.Set( bounds.right - kExportSliderWidth, bounds.top+1, bounds.right, bounds.bottom-1);

	//	Set up channel zone
	m_ExportChannel.Set(m_InRect.right, m_InRect.top, m_OutRect.left, m_InRect.bottom-1);
		
	//	Load left and right slider tabs
	m_InMarker	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_ExportSliderLeft;
	m_OutMarker	= static_cast<MuseumApp *>(be_app)->m_MuseumIcons->m_ExportSliderRight;
	
	//	Initialize drag type to non
	m_DragType = kNoDrag;	
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TExportZone::Instantiate(BMessage *archive) 
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

status_t TExportZone::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TExportZone");
		
		// Add our member variables to the archive		
		data->AddRect("ExportChannel", m_ExportChannel);
		data->AddRect("InRect", m_InRect);
		data->AddRect("OutRect", m_OutRect);

		// Add attached views
		if (deep)
		{		
		
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
	static_cast<MuseumApp *>(be_app)->m_IsCueDrag = false;
	
	//	Reset drag type
	m_DragType = kNoDrag;
	
	// Check to see which button is down
	uint32 	buttons = 0;				
	Window()->CurrentMessage()->FindInt32("buttons", (int32 *)&buttons);
								
	uint32 	type;
	int32	count = 0;
	BMessage *message = Window()->CurrentMessage();	
	
	// Determine which button has been clicked
	switch(buttons)
	{
		case B_PRIMARY_MOUSE_BUTTON:
			if (B_OK == message->GetInfo("clicks", &type, &count) )
			{
				int32 clickCount = message->FindInt32("clicks", count-1);
				//	Handle double click
				if ( (clickCount == 2) )
				{
					HandleDoubleClick(where);
				}
				//	Determine where mouse click occured
				else
				{
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
	if ( m_CueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;

}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TExportZone::MouseMoved( BPoint where, uint32 code, const BMessage *message )
{

	// Do nothing if we are playing
	if ( m_CueSheetWindow->GetPlaybackEngine()->IsPlaying() )
		return;
		
	// Do nothing if window is inactive
	//if ( m_CueSheetWindow->IsActive() = =false )
	//	return;

	// Exit if panel is open
	if ( m_CueSheetWindow->IsPanelOpen())
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
	//uint32 	buttons = 0;	
	
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
	if (m_InRect.Contains(where))
	{
		m_DragType = kInDrag;		
	}
	//	m_OutRect?		     						 	
	else if (m_OutRect.Contains(where))
	{
		m_DragType = kOutDrag;		
	}		     						 	
	//	m_ExportChannel?
	else if (m_ExportChannel.Contains(where) )
	{
		//	Handle zone drag special
		TrackZone(where);
	}
		
	// Launch mouse tracking thread
	if (m_DragType != kNoDrag)
		m_MouseTracker = StartMouseWatcher(this);

}

//---------------------------------------------------------------------
//	HandleDoubleClick
//---------------------------------------------------------------------
//
//	Handle mouse double click.  If within m_ExportChannel, resize
//	m_ExportChannel to span visible timeline width
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
	m_InRect.left 	= bounds.left;
	m_InRect.right 	= m_InRect.left + kExportSliderWidth;

	//	Set up out rect.  Check for right side length bounds violation
	int32 outRight = (bounds.left + winBounds.Width()) - kHeaderWidth;
	if (outRight > pixelWidth)
		outRight = pixelWidth;
	
	m_OutRect.right	 = outRight;
	m_OutRect.left 	 = m_OutRect.right - kExportSliderWidth;
	
	//	Update m_ExportChannel
	m_ExportChannel.left  = m_InRect.right;
	m_ExportChannel.right = m_OutRect.left;
	
	//	Update cue sheet variables
	m_CueSheetWindow->GetCueSheetView()->SetExportStartTime( PixelsToTime(m_InRect.left, GetCurrentTimeFormat(), GetCurrentResolution()) );
	m_CueSheetWindow->GetCueSheetView()->SetExportStopTime( PixelsToTime(m_OutRect.right, GetCurrentTimeFormat(), GetCurrentResolution()) );
	
	//	Update text items
	m_CueSheetWindow->GetExportTimeView()->DrawInText();
	m_CueSheetWindow->GetExportTimeView()->DrawOutText();

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

void TExportZone::MessageReceived(BMessage *message)
{
		switch(message->what)
		{
				
			// Mouse Tracking Messages
			case MW_MOUSE_MOVED:
				{
					BPoint mousePt;
					message->FindPoint("where", &mousePt);
					
					switch(m_DragType)
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
	BRect channelRect = m_ExportChannel;
	channelRect.bottom--;

	// Draw export channel
	if (updateRect.Intersects(channelRect) )
	{		
		BRect fillRect = updateRect & channelRect;
		
		//	Fill
		SetHighColor(kKhaki);
		FillRect(fillRect);
		
		//	Highlight
		BPoint startPt, endPt;
		SetHighColor(kLightKhaki);
		startPt.Set(fillRect.left, m_ExportChannel.top);
		endPt.Set(fillRect.right, m_ExportChannel.top);
		StrokeLine(startPt, endPt);
		
		//	Shadow
		SetHighColor(kDarkKhaki);
		startPt.Set(fillRect.left, m_ExportChannel.bottom);
		endPt.Set(fillRect.right, m_ExportChannel.bottom);
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
	if (updateRect.Intersects(m_InRect) )
	{
		// Draw indicator in new location
		drawPt.Set(m_InRect.left, m_InRect.top);
		DrawBitmap(m_InMarker, drawPt);
	}

	// Draw right marker
	if (updateRect.Intersects(m_OutRect) )
	{
		// Draw indicator in new location
		drawPt.Set(m_OutRect.left, m_OutRect.top);
		DrawBitmap(m_OutMarker, drawPt);
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
	if (updateRect.Intersects(m_OutRect) )
	{
		// Draw indicator in new location
		drawPt.Set(m_OutRect.left, m_OutRect.top);
		DrawBitmap(m_OutMarker, drawPt);
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
	uint32 startTime = m_CueSheetWindow->GetCueSheetView()->GetExportStartTime();
	uint32 stopTime  = m_CueSheetWindow->GetCueSheetView()->GetExportStopTime();
		
	uint32 inPixels 	= TimeToPixels( startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	uint32 outPixels = TimeToPixels( stopTime, GetCurrentTimeFormat(), GetCurrentResolution());

	//	Update slider zones
	m_InRect.left 	= inPixels;
	m_InRect.right 	= m_InRect.left + kExportSliderWidth;

	m_OutRect.right	 = outPixels;
	m_OutRect.left 	 = m_OutRect.right - kExportSliderWidth;
	
	// Update channel zone
	m_ExportChannel.left  = m_InRect.right;
	m_ExportChannel.right = m_OutRect.left;

	// Force redraw
	Invalidate();
}


#pragma mark -
#pragma mark === Marker Tracking ===

//---------------------------------------------------------------------
//	TrackInMarker
//---------------------------------------------------------------------
//
//	Draw new position of m_InMarker
//

void TExportZone::TrackInMarker(BPoint mousePt)
{			
	
	const BRect bounds = Bounds();
		
	//	Constrain to left side view area
	if (mousePt.x < 0)
		mousePt.x = 0;

	//	Don't allow overlap with left side tracker
	if ( (mousePt.x + kExportSliderWidth) >= m_OutRect.left)
		mousePt.x = m_OutRect.left - kExportSliderWidth;
				
	// Save oldRect for redraw														
	BRect oldRect  = m_InRect;
	m_InRect.left  = mousePt.x;
	m_InRect.right = m_InRect.left + kExportSliderWidth;	
	
	// Exit if there is no change in position
	if (oldRect == m_InRect)
		return;

	//	Update m_ExportChannel
	m_ExportChannel.left = m_InRect.right;
	
	//	Clean up old position
	BRect updateRect = oldRect;
	
	if (oldRect.left <= m_InRect.left)
	{
		updateRect.right = m_InRect.right;
	}
	else
	{
		updateRect.left  = m_InRect.left;
	}
		
	Draw(updateRect);
	
	//	Update CueSheet variable
	uint32 newInTime = StartTime() + PixelsToTime(m_InRect.left, GetCurrentTimeFormat(), GetCurrentResolution());
	m_CueSheetWindow->GetCueSheetView()->SetExportStartTime(newInTime);

	//	Update text
	m_CueSheetWindow->GetExportTimeView()->DrawInText();
}


//---------------------------------------------------------------------
//	TrackOutMarker
//---------------------------------------------------------------------
//
//	Draw new position of m_OutMarker
//

void TExportZone::TrackOutMarker(BPoint mousePt)
{					
	//	Constrain to out time of cue sheet
	uint32 outPixels = TimeToPixels( Duration() - StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());
	
	if (mousePt.x > outPixels)
		mousePt.x = outPixels;

	//	Don't allow overlap with m_InRect
	if ( (mousePt.x-kExportSliderWidth) < m_InRect.right)
		mousePt.x = m_InRect.right + kExportSliderWidth;

	// Save oldRect for redraw														
	BRect oldRect 	= m_OutRect;	
	m_OutRect.right = mousePt.x;	
	m_OutRect.left = m_OutRect.right - kExportSliderWidth;	
	
	
	// Exit if there is no change in position
	if (oldRect == m_OutRect)
		return;

	//	Update m_ExportChannel
	m_ExportChannel.right = m_OutRect.left;
	
	//	Clean up old position
	BRect updateRect = oldRect;
	
	if (m_OutRect.left <= oldRect.left)
	{
		updateRect.left = m_OutRect.left;		
	}
	else
	{
		updateRect.right  = m_OutRect.right;
	}
	
	Draw(updateRect);
	
	//	Update CueSheet variable
	uint32 newOutTime = StartTime() + PixelsToTime(m_OutRect.right, GetCurrentTimeFormat(), GetCurrentResolution());
	m_CueSheetWindow->GetCueSheetView()->SetExportStopTime(newOutTime);

	//	Update text
	m_CueSheetWindow->GetExportTimeView()->DrawOutText();
	
}


//---------------------------------------------------------------------
//	TrackZone
//---------------------------------------------------------------------
//
//	Track movement of entire m_ExportChannel
//

void TExportZone::TrackZone(BPoint mousePt)
{		
	//	Set up variables	
	const float width 	= m_ExportChannel.Width();	
	const int32 delta 	= mousePt.x - m_ExportChannel.left;			
	
	//	Save mousePt
	BPoint oldPt = mousePt;

	// Check to see if button is down
	uint32 	buttons = 0;
	GetMouse(&mousePt, &buttons, true);	
	
	//	Track while mouse is down
	while(buttons)
	{						
		if (mousePt.x != oldPt.x)		
		{													
			// Save oldRect for redraw														
			BRect oldRect = m_ExportChannel;
			oldRect.left  -= kExportSliderWidth;
			oldRect.right += kExportSliderWidth;
			
			//	Update m_ExportChannel
			m_ExportChannel.left  += mousePt.x - oldPt.x;
			m_ExportChannel.right = m_ExportChannel.left + width;
			
			//	Correct left side bounds violation
			if ( m_ExportChannel.left - kExportSliderWidth < 0)
			{
				m_ExportChannel.left = 0 + kExportSliderWidth;
				m_ExportChannel.right = m_ExportChannel.left + width;
			}

			//	Correct right side bounds violation
			uint32 outPixels = TimeToPixels( Duration() - StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());
			if ( m_ExportChannel.right + kExportSliderWidth > outPixels)
			{
				m_ExportChannel.right = outPixels - kExportSliderWidth;
				m_ExportChannel.left  = m_ExportChannel.right - width;
			}
			
			//	Update m_InRect and m_InRect
			m_InRect.left 	= m_ExportChannel.left - kExportSliderWidth;
			m_InRect.right 	= m_InRect.left + kExportSliderWidth;
		
			m_OutRect.left 	 = m_ExportChannel.right;
			m_OutRect.right	 = m_OutRect.left + kExportSliderWidth;									
													
			//	Update CueSheet variables
			uint32 newInTime  = StartTime() + PixelsToTime(m_InRect.left, GetCurrentTimeFormat(), GetCurrentResolution());
			uint32 newOutTime = StartTime() + PixelsToTime(m_OutRect.right, GetCurrentTimeFormat(), GetCurrentResolution());			
			m_CueSheetWindow->GetCueSheetView()->SetExportStartTime(newInTime);
			m_CueSheetWindow->GetCueSheetView()->SetExportStopTime(newOutTime);
		
			//	Calculate updateRect
			BRect updateRect = oldRect;
			
			//	Did we move forward
			if (updateRect.left < m_InRect.left)
			{
				updateRect.right = m_OutRect.right;
			}
			//	We moved back
			else
			{
				updateRect.left = m_InRect.left;	
			}
			
			//	Force redraw
			if (oldRect != updateRect)
				Invalidate(updateRect);
			
			//	Save point for future compare
			oldPt = mousePt;
			
			//	Update text items
			m_CueSheetWindow->GetExportTimeView()->DrawInText();
			m_CueSheetWindow->GetExportTimeView()->DrawOutText();
		}
						
		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);
		
		//
		//	Clip mouse
		
		// Constrain mouse x to the m_ExportZone
		if (mousePt.x < 0)
			mousePt.x = 0;
				
		//if (mousePt.x >= m_ExportZone.right)
		//	mousePt.x = m_ExportZone.right - (kExportSliderWidth/2);

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
	if(m_CueSheetWindow == NULL)
	{
		m_CueSheetWindow = (TCueSheetWindow *)Window();		
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
void TExportZone::SetParent(TCueSheetWindow *parent)
{
 	m_CueSheetWindow = parent;
}