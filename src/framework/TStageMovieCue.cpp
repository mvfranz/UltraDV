//---------------------------------------------------------------------
//
//	File:	TStageMovieCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.12.98
//
//	Desc:	Visual representation of a movie cue on the stage
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppAlerts.h"
#include "AppMessages.h"
#include "MuseumApp.h"

// ABH these 2 do not exist
//#include "MuseumUtils.h"
//#include "MediapedeVideo.h"

#include "TStageView.h"
#include "TStageMovieCue.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"

#include "TTransition.h"
#include "TStageCueMenu.h"

// Constants
const short kBorder = 6;
	 
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageMovieCue::TStageMovieCue(BRect bounds, char *name, TCueView *theCue) : TStageCue(bounds, name, theCue)
{
	// Do default initialization
	Init();
		
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageMovieCue::TStageMovieCue(BMessage *theMessage) : TStageCue(theMessage)
{
	// Do default initialization
	Init();
		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageMovieCue::~TStageMovieCue()
{
	// Stop and clean up transitions
	if (m_Transition)
	{
		delete m_Transition;
		m_Transition = NULL;
	}
	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TStageMovieCue::Init()
{
	// Set up resize zone rects
	SetResizeZones();
	
	m_Transition = NULL;
	
}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TStageMovieCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TStageMovieCue") ) 
		return new TStageMovieCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TStageMovieCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TStageCue::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TStageMovieCue");						
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

void TStageMovieCue::Draw(BRect updateRect)
{	
	if ( (m_Bitmap) && IsCueHidden() == true &&!IsHidden() )
	{
		rgb_color saveColor = HighColor();
				
		// Draw bitmap 
		DrawBitmap(m_Offscreen, B_ORIGIN);
		
		// Data selection and resizing rect
		DrawSelectionRect();
								
		SetHighColor(saveColor);		
	}
}


//---------------------------------------------------------------------
//	DrawData
//---------------------------------------------------------------------
//
//	Draw cues internal data
//

void TStageMovieCue::DrawData(BRect updateRect, long theTime)
{	
	// Do not draw if cue or channel is muted
	if ( m_ChannelCue->GetChannel()->GetMute() )
		return;
	
	if ( m_Bitmap && IsCueHidden() == false && IsHidden() == true)
	{
		BRect area = m_ChannelCue->GetCroppedArea();
		
		// Set up environment
		BRegion clipRegion, saveRegion;
		Parent()->GetClippingRegion(&saveRegion);
		clipRegion.Set(m_ChannelCue->GetCroppedArea());
		Parent()->ConstrainClippingRegion( &clipRegion );
	
		// Draw bitmap
		BPoint thePt(area.left, area.top); 
		Parent()->DrawBitmap(m_Offscreen, thePt);
		
		// Restore
		Parent()->ConstrainClippingRegion(&saveRegion);
	}
}


//---------------------------------------------------------------------
//	CompositeData
//---------------------------------------------------------------------
//
//	Draw cues internal data into composite view
//

void TStageMovieCue::CompositeData(BRect updateRect, BView *offscreen)
{	
	// Do not draw if cue or channel is muted
//	if ( m_ChannelCue->GetChannel()->GetMute() )
//		return;
	
//	if ( m_Bitmap && IsCueHidden() == false && IsHidden() == true)
	{
		BRect area = m_ChannelCue->GetCroppedArea();
		
		// Set up environment
		BRegion clipRegion, saveRegion;
		offscreen->GetClippingRegion(&saveRegion);
		clipRegion.Set(m_ChannelCue->GetCroppedArea());
		offscreen->ConstrainClippingRegion( &clipRegion );
	
		// Draw bitmap
		BPoint thePt(area.left, area.top); 
		offscreen->DrawBitmap(m_Offscreen, thePt);
		
		// Restore
		offscreen->ConstrainClippingRegion(&saveRegion);
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

void TStageMovieCue::MouseDown(BPoint where)
{				
	ChunkHeader ckHeader;
	VideoChunk	vidChunk;
	
	const int32 frameSize = 320*240*2;
	
	BBitmap *displayBitmap = new BBitmap(BRect(0, 0, 319, 239), B_RGB15, false, false);
	
	// Test:  play movie
	BFile *theFile = m_ChannelCue->GetFile();
	
	// Load headers
	theFile->Seek(0, SEEK_SET);	
	theFile->Read(&ckHeader, sizeof(ChunkHeader));	
	theFile->Read(&vidChunk, sizeof(VideoChunk));	
		
	// Load frames and display
	for (int32 frame = 0; frame < vidChunk.numFrames; frame++)
	{
		ssize_t numRead = theFile->Read(displayBitmap->Bits(), frameSize);
		Looper()->Lock();
		DrawBitmap(displayBitmap);
		Sync();
		Looper()->Unlock();
	}
		
	/*
	// Do nothing if we are playing
	if ( static_cast<MuseumApp *>(be_app)->GetCueSheet()->IsPlaying() )
		return;
		
	// Do nothing if view is hidden
	if (IsHidden())
		return;

	// Check for double click
	TStageCue::MouseDown(where);
	
	// Determine which button has been clicked
	uint32 	buttons = 0;
	BMessage *message = Window()->CurrentMessage();
	message->FindInt32("buttons", (long *)&buttons);

	switch(buttons)
	{
		case B_PRIMARY_MOUSE_BUTTON:
			// Wait a short while before dragging
			snooze(60 * 1000);
			
			//	Find location of click.  If it is the main body of the picture, they are moving the picture.
			//	Otherwise, they are resizing or cropping the image
			if ( PointInResizeZones(where) )
			{
				ResizeOrCrop(where);
			}
			// They are dragging the picture...
			else
			{
				DragPicture(where);				
			}
				
			// Update the picture rects
			m_ChannelCue->SetArea( Frame() );
			m_ChannelCue->SetCroppedArea( Frame() );
			break;
			
		// Show stage cue menu
		case B_SECONDARY_MOUSE_BUTTON:
			OpenStageCueMenu(where);
			break;
			
	}
	*/
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TStageMovieCue::MouseUp(BPoint where)
{
	// Do nothing if view is hidden
	if (IsHidden())
		return;

}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TStageMovieCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	// Do nothing if view is hidden
	if (IsHidden())
		return;
		
	// Set proper cursor for resize zones
	if ( m_TopLeftResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_ResizeDiagRightCursor);
	else if ( m_TopMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_SizeVertCursor);
	else if ( m_TopRightResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_ResizeDiagLeftCursor);
	else if ( m_RightMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_SizeHorzCursor);		
	else if ( m_RightMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_SizeHorzCursor);		
	else if ( m_BottomRightResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_ResizeDiagRightCursor);
	else if ( m_BottomMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_SizeVertCursor);
	else if ( m_BottomLeftResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_ResizeDiagLeftCursor);
	else if ( m_LeftMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->m_SizeHorzCursor);		
	else
		be_app->SetCursor(B_HAND_CURSOR);		
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TStageMovieCue::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TStageMovieCue::KeyDown(const char *bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TStageMovieCue::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TStageMovieCue::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TStageMovieCue::FrameResized(float new_width, float new_height)
{
	
	// Resize sizing zones
	SetResizeZones();	
}


#pragma mark -
#pragma mark === Bitmap Handling ===

//---------------------------------------------------------------------
//	SetBitmap
//---------------------------------------------------------------------
//
//

void TStageMovieCue::SetBitmap(BBitmap *bitmap)
{
	if (bitmap)
	{
		m_Bitmap = bitmap;	
		
		// Create offscreen bitmap and view		
		m_Offscreen = new BBitmap( m_Bitmap->Bounds(), m_Bitmap->ColorSpace(), true, false);
		
		m_OffscreenView = new BView( m_Bitmap->Bounds(), "OffscreenView", B_FOLLOW_NONE, NULL);
		m_Offscreen->AddChild(m_OffscreenView);
		m_OffscreenView->Looper()->Lock();
		m_OffscreenView->DrawBitmap(m_Bitmap);
		m_OffscreenView->Sync();
		m_OffscreenView->Looper()->Unlock();
	}
}



#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TStageMovieCue::MessageReceived(BMessage *message)
{
	double theTime;
	
	switch (message->what)
	{
		// Get updated bitmap
		case UPDATE_TIMELINE_MSG:
			{
				// Inform channel cue
				m_ChannelCue->MessageReceived(message);
				theTime = message->FindDouble("TheTime");				
				SetVisibility(theTime);
			}
			break;	
			
		default:
			TStageCue::MessageReceived(message);						
			break;
	}		
}	


#pragma mark -
#pragma mark === Cue Visibility ===


//---------------------------------------------------------------------
//	SetVisibility
//---------------------------------------------------------------------
//
//	Determines the visibility of the stage cue data.  The view itself is not shown,
//	rather the view data is copied onto the stage view
		
void TStageMovieCue::SetVisibility(double theTime)
{
	double cueStartTime = m_ChannelCue->GetStartTime();
	double cueEndTime 	= m_ChannelCue->GetStartTime() + m_ChannelCue->GetDuration();
					
	// Is the current time before the cue's start time or after the cue's end time?
	if (theTime < cueStartTime || theTime >= cueEndTime )
	{
		if ( IsCueHidden() == false)
		{
			m_IsHidden = true;
			UpdatePictureCueRegion(theTime);
		}
	}
	// We need to show the cue if it is not already visible
	else 
	{
		if ( IsCueHidden() == true)
		{
			// Set it's hidden flag to false
			m_IsHidden = false;			
			UpdatePictureCueRegion(theTime);			
		}
	}
}


//---------------------------------------------------------------------
//	UpdatePictureCueRegion
//---------------------------------------------------------------------
//
//	We are showing or hiding the cue.  Create a region that will
//	be used to invalidate the stage and force a proper redraw.  It
//	handles the invalidation of layered cues properly.
//
		
void TStageMovieCue::UpdatePictureCueRegion(double theTime)
{
	// Determine channelID
	int32 cueChannelID = m_ChannelCue->GetChannelID();
	
	// Get total channels
	int32 totalChannels = static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->GetTotalChannels();
	
	// 	Determine cue layer.  Iterate through all higher layers and determine 
	//	the area to be invalidated.  Construct a region to do this.  Exlude all 
	//	other area rects in higher channels than us.  We wil then break 
	//	the region down into rects and invalidate them.		
	BRegion invalRegion;
	invalRegion.Include(m_ChannelCue->GetArea());
	
	for(int32 index = cueChannelID+1; index <= totalChannels; index++)
	{
		TStageCue *stageCue = ((TStageView *)Parent())->GetStageCueAtTimeandChannel(theTime, index);
		
		if (stageCue)
			invalRegion.Exclude( stageCue->GetChannelCue()->GetArea());							
	} 
	
	// Now call our custom invalidation routine	
	for(int32 index = 0; index < invalRegion.CountRects(); index++)
	{		
		Parent()->Invalidate( invalRegion.RectAt(index));	
		//((TStageView *)Parent())->StageDraw( invalRegion.RectAt(index), theTime);				
	}	
}


#pragma mark -
#pragma mark === Cue Selection and Resizing ===


//---------------------------------------------------------------------
//	DrawSelectionRect
//---------------------------------------------------------------------
//
//	Draw the selection rect and resize handles
//

void TStageMovieCue::DrawSelectionRect()
{
	rgb_color saveColor;
	saveColor = HighColor();
	
	SetHighColor(kRed);
	
	// Draw main selection rect 
	//BRect frame = Bounds();
	//frame.InsetBy(kBorder/2, kBorder/2);
	//StrokeRect(frame);	
	
	// Draw resizing handles
	//
	
	// Fill them first...
	SetHighColor(kWhite);	
	FillRect(m_TopLeftResize);
	FillRect(m_TopMiddleResize);
	FillRect(m_TopRightResize);
	FillRect(m_RightMiddleResize);
	FillRect(m_BottomRightResize);
	FillRect(m_BottomMiddleResize);
	FillRect(m_BottomLeftResize);
	FillRect(m_LeftMiddleResize);
	
	// Now stroke...	
	SetHighColor(kBlack);
	StrokeRect(m_TopLeftResize);
	StrokeRect(m_TopMiddleResize);
	StrokeRect(m_TopRightResize);
	StrokeRect(m_RightMiddleResize);
	StrokeRect(m_BottomRightResize);
	StrokeRect(m_BottomMiddleResize);
	StrokeRect(m_BottomLeftResize);
	StrokeRect(m_LeftMiddleResize);
				
}		



//---------------------------------------------------------------------
//	InvalidateSelectionRect
//---------------------------------------------------------------------
//
//	Invalidate just the selection rect and force a redraw
//


void TStageMovieCue::InvalidateSelectionRect()
{
	// Create a region containing selection rect
	BRegion theRegion;
	BRect	selectRect;
	
	// Top
	selectRect.Set(Bounds().left, Bounds().top, Bounds().right, Bounds().top +kBorder);
	theRegion.Include(selectRect);
	
	// Right
	selectRect.Set(Bounds().right-kBorder, Bounds().top, Bounds().right, Bounds().bottom);
	theRegion.Include(selectRect);

	// Bottom
	selectRect.Set(Bounds().left, Bounds().bottom-kBorder, Bounds().right, Bounds().bottom);
	theRegion.Include(selectRect);
	
	// Left
	selectRect.Set(Bounds().left, Bounds().top, Bounds().left+kBorder, Bounds().bottom);
	theRegion.Include(selectRect);
	
	// Now invalidate
	for(int32 index = 0; index < theRegion.CountRects(); index++)
	{		
		Invalidate( theRegion.RectAt(index));				
	}	

}


			
#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	SetResizeZones
//---------------------------------------------------------------------
//
//	Set up the rects defining the picture resize zones

void TStageMovieCue::SetResizeZones()
{	
	BRect bounds = Bounds();
	
	m_TopLeftResize.Set( bounds.left, bounds.top, bounds.left+kBorder, bounds.top+kBorder);
	m_TopMiddleResize.Set( (bounds.Width()/2) - (kBorder/2), bounds.top, (bounds.Width()/2) + (kBorder/2), bounds.top+kBorder);
	m_TopRightResize.Set( bounds.right-kBorder, bounds.top, bounds.right, bounds.top+kBorder);	
	m_RightMiddleResize.Set( bounds.right-kBorder, (bounds.Height()/2) - (kBorder/2), bounds.right, (bounds.Height()/2) + (kBorder/2));
	m_BottomRightResize.Set( bounds.right-kBorder, bounds.bottom-kBorder, bounds.right, bounds.bottom);	
	m_BottomMiddleResize.Set( (bounds.Width()/2) - (kBorder/2), bounds.bottom-kBorder, (bounds.Width()/2) + (kBorder/2), bounds.bottom);
	m_BottomLeftResize.Set( bounds.left, bounds.bottom-kBorder, bounds.left+kBorder, bounds.bottom);
	m_LeftMiddleResize.Set( bounds.left, (bounds.Height()/2) - (kBorder/2), bounds.left+kBorder, (bounds.Height()/2) + (kBorder/2));
		
}


//---------------------------------------------------------------------
//	PointInResizeZones
//---------------------------------------------------------------------
//
//	If the point is within one of the resize zones, return true
//

bool TStageMovieCue::PointInResizeZones(BPoint thePoint)
{
	if (m_TopLeftResize.Contains(thePoint))
		return true;
		
	if (m_TopMiddleResize.Contains(thePoint))
		return true;

	if (m_TopRightResize.Contains(thePoint))
		return true;

	if (m_RightMiddleResize.Contains(thePoint))
		return true;

	if (m_BottomRightResize.Contains(thePoint))
		return true;

	if (m_BottomMiddleResize.Contains(thePoint))
		return true;

	if (m_BottomLeftResize.Contains(thePoint))
		return true;

	if (m_LeftMiddleResize.Contains(thePoint))
		return true;
		
	return false;
}


//---------------------------------------------------------------------
//	DragPicture
//---------------------------------------------------------------------
//
//	While the mouse is down, move the picture
//

void TStageMovieCue::DragPicture(BPoint thePoint)
{
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);
	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint != savePt)
		{
			// Convert to parents coordinate system
			ConvertToParent(&thePoint);
			
			MoveBy( ( thePoint.x - savePt.x) , (thePoint.y - savePt.y) );
			
			// Save mouse location for next compare
			savePt = thePoint;
		}
		
		GetMouse(&thePoint, &buttons, true);
	}
}



//---------------------------------------------------------------------
//	ResizeOrCrop
//---------------------------------------------------------------------
//
//	Resize of crop the picture depending upon current tool
//

void TStageMovieCue::ResizeOrCrop(BPoint thePoint)
{
	short zoneID = GetResizeZoneID(thePoint);
	
	switch(zoneID)
	{
		case kTopLeftResize:
			ResizeTopLeft(thePoint);
			break;
			
		case kTopMiddleResize:
			break;
			
		case kTopRightResize:			
			break;
			
		case kRightMiddleResize:
			ResizeRight(thePoint);
			break;
			
		case kBottomRightResize:
			ResizeBottomRight(thePoint);
			break;
			
		case kBottomMiddleResize:
			ResizeBottom(thePoint);
			break;
			
		case kBottomLeftResize:
			break;
			
		case kLeftMiddleResize:
			ResizeLeft(thePoint);
			break;	
			
		// Bad ID value
		default:
			break;			
	}
		
	// Update resize zones
	SetResizeZones();	
}


//---------------------------------------------------------------------
//	GetResizeZoneID
//---------------------------------------------------------------------
//
//	Return resize ID from point
//


short TStageMovieCue::GetResizeZoneID(BPoint thePoint)
{
	if (m_TopLeftResize.Contains(thePoint))
		return kTopLeftResize;
		
	if (m_TopMiddleResize.Contains(thePoint))
		return kTopMiddleResize;

	if (m_TopRightResize.Contains(thePoint))
		return kTopRightResize;

	if (m_RightMiddleResize.Contains(thePoint))
		return kRightMiddleResize;

	if (m_BottomRightResize.Contains(thePoint))
		return kBottomRightResize;

	if (m_BottomMiddleResize.Contains(thePoint))
		return kBottomMiddleResize;

	if (m_BottomLeftResize.Contains(thePoint))
		return kBottomLeftResize;

	if (m_LeftMiddleResize.Contains(thePoint))
		return kLeftMiddleResize;

	// No zone found
	return -1;
}


//---------------------------------------------------------------------
//	ResizeTopLeft
//---------------------------------------------------------------------
//
//	Resize the top left of the view.  
//

void TStageMovieCue::ResizeTopLeft( BPoint thePoint)
{
	
	
		
}

//---------------------------------------------------------------------
//	ResizeTopRight
//---------------------------------------------------------------------
//
//	Resize the top and right side of the view. 
//

void TStageMovieCue::ResizeTopRight( BPoint thePoint)
{
	// Resize the cue to the right will the mouse button is down	
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint.x != savePt.x)
		{								
			float resize = thePoint.x - savePt.x;
									
			// Don't allow resize past bitmap width 
			if ( ( Bounds().Width() + resize <= m_Bitmap->Bounds().Width()) || ( Bounds().right - resize >=  kBorder*2 ) ) 
			{
				// Redraw the selection rect and resize points
				InvalidateSelectionRect();
				
				// Do resize
				ResizeBy( resize, 0 );
				SetResizeZones();
			}
																		
			// Save mouse location for next compare
			savePt = thePoint;
		}
		
		GetMouse(&thePoint, &buttons, true);
		
		// Clip the point back within the proper bounds
		if (thePoint.x > m_Bitmap->Bounds().right)
			thePoint.x = m_Bitmap->Bounds().right;	
			
		if (thePoint.x < m_Bitmap->Bounds().left + (kBorder*2) )
			thePoint.x = m_Bitmap->Bounds().left + (kBorder*2);		
			
		// Convert to parents coordinate system
		ConvertToParent(&thePoint);													
	}		
}

//---------------------------------------------------------------------
//	ResizeRight
//---------------------------------------------------------------------
//
//	Resize the right side of the view.  Constrain to the width of 
//	the bitmap.  Also, don't allow right to be less than left
//

void TStageMovieCue::ResizeRight( BPoint thePoint)
{
	// Resize the cue to the right will the mouse button is down	
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint.x != savePt.x)
		{								
			float resize = thePoint.x - savePt.x;
									
			// Don't allow resize past bitmap width 
			if ( ( Bounds().Width() + resize <= m_Bitmap->Bounds().Width()) || ( Bounds().right - resize >=  kBorder*2 ) ) 
			{
				// Redraw the selection rect and resize points
				InvalidateSelectionRect();
				
				// Do resize
				ResizeBy( resize, 0 );
				SetResizeZones();
			}
																		
			// Save mouse location for next compare
			savePt = thePoint;
		}
		
		GetMouse(&thePoint, &buttons, true);
		
		// Clip the point back within the proper bounds
		if (thePoint.x > m_Bitmap->Bounds().right)
			thePoint.x = m_Bitmap->Bounds().right;	
			
		if (thePoint.x < m_Bitmap->Bounds().left + (kBorder*2) )
			thePoint.x = m_Bitmap->Bounds().left + (kBorder*2);		
			
		// Convert to parents coordinate system
		ConvertToParent(&thePoint);													
	}		
}

//---------------------------------------------------------------------
//	ResizeBottomRight
//---------------------------------------------------------------------
//
//	Resize the bottom and right side of the view. 
//

void TStageMovieCue::ResizeBottomRight( BPoint thePoint)
{
	// Resize the cue to the right will the mouse button is down	
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint != savePt)
		{								
			float resizeX = thePoint.x - savePt.x;
			float resizeY = thePoint.y - savePt.y;
									
			// Don't allow resize past bitmap width and height 			
			if ( ( Bounds().Width() + resizeX <= m_Bitmap->Bounds().Width()) || Bounds().Height() + resizeY <= m_Bitmap->Bounds().Height() ) 
			{
				// Constrian to minimum bounds
				if ( ( Bounds().right - resizeX >=  kBorder*2 ) || Bounds().bottom - resizeY >= m_Bitmap->Bounds().top + kBorder*2 ) 
				{
					// Redraw the selection rect and resize points
					InvalidateSelectionRect();
				
					// Do resize
					ResizeBy( resizeX, resizeY );
					SetResizeZones();
				}
			}
																		
			// Save mouse location for next compare
			savePt = thePoint;
		}
		
		GetMouse(&thePoint, &buttons, true);
		
		// Clip the point back within the proper bounds
		if (thePoint.x > m_Bitmap->Bounds().right)
			thePoint.x = m_Bitmap->Bounds().right;	
			
		if (thePoint.y > m_Bitmap->Bounds().bottom)
			thePoint.y = m_Bitmap->Bounds().bottom;	
			
		if (thePoint.x < m_Bitmap->Bounds().left + (kBorder*2) )
			thePoint.x = m_Bitmap->Bounds().left + (kBorder*2);	
			
		if (thePoint.y < m_Bitmap->Bounds().top + (kBorder*2))
			thePoint.y = m_Bitmap->Bounds().top + (kBorder*2);		
			
		// Convert to parents coordinate system
		ConvertToParent(&thePoint);													
	}		
}


//---------------------------------------------------------------------
//	ResizeBottom
//---------------------------------------------------------------------
//
//	Resize the bottom side of the view.  Constrain to the height of 
//	the bitmap.
//

void TStageMovieCue::ResizeBottom( BPoint thePoint)
{
	// Resize the cue to the right will the mouse button is down	
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint.y != savePt.y)
		{								
			float resize = thePoint.y - savePt.y;
															
			// Don't allow resize past bitmap height or mimimum height
			if ( Bounds().Height() + resize <= m_Bitmap->Bounds().Height() || Bounds().bottom - resize >= m_Bitmap->Bounds().top + kBorder*2 )
			{
				// Redraw the selection rect and resize points
				InvalidateSelectionRect();
				
				// Do resize
				ResizeBy( 0, resize );
				SetResizeZones();
			}
																		
			// Save mouse location for next compare
			savePt = thePoint;
		}
		
		GetMouse(&thePoint, &buttons, true);
		
		// Clip the point back within the proper bounds
		if (thePoint.y > m_Bitmap->Bounds().bottom)
			thePoint.y = m_Bitmap->Bounds().bottom;	
						
		if (thePoint.y < m_Bitmap->Bounds().top + (kBorder*2))
			thePoint.y = m_Bitmap->Bounds().top + (kBorder*2);		
			
		// Convert to parents coordinate system
		ConvertToParent(&thePoint);													
	}		
}


//---------------------------------------------------------------------
//	ResizeLeft
//---------------------------------------------------------------------
//
//	Resize the left side of the view.  Constrain to the width of 
//	the bitmap.  Also, don't allow right to be less than left
//

void TStageMovieCue::ResizeLeft( BPoint thePoint)
{
	// Resize the cue to the right will the mouse button is down	
	BPoint 	savePt;
	uint32	buttons = 0;
	
	GetMouse(&thePoint, &buttons, true);	
	ConvertToParent(&thePoint);
	savePt = thePoint;
			
	while (buttons)
	{				
		if (thePoint != savePt)
		{								
			float resize = thePoint.x - savePt.x;
									
			// Don't allow resize past bitmap width 
			if ( Bounds().Width() <= m_Bitmap->Bounds().Width() ) 
			{
				// Redraw the selection rect and resize points
				InvalidateSelectionRect();
				
				// Do resize.  Resize the view and move it to the proper location
				//float curRight = Bounds().right;
												
				BPoint movePt = thePoint;
				ConvertToParent(movePt);
				MoveTo( movePt.x, Frame().top );
				
				if ( Bounds().Width() + resize > m_Bitmap->Bounds().Width() )				
					resize = Bounds().Width() - m_Bitmap->Bounds().Width();
				
				ResizeBy( -resize, 0 );
				
				SetResizeZones();
			}
																		
			// Save mouse location for next compare
			savePt = thePoint;			
		}
		
		GetMouse(&thePoint, &buttons, true);
		
		// Clip the point back within the proper bounds
			
		// Fix bounds violation
		//if ( Bounds().Width() > m_Bitmap->Bounds().Width() )
		//	ResizeBy( (Bounds().Width() - m_Bitmap->Bounds().Width()), 0 );		
					
		// Convert to parents coordinate system
		ConvertToParent(&thePoint);													
	}		
}


#pragma mark -
#pragma mark === Playback Routines ===


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Playback has been stopped.  Stop any transitions.		
//
		
void TStageMovieCue::Stop()
{
	if (m_Transition)
	{
		delete m_Transition;
		m_Transition = NULL;
	}	
}


#pragma mark -
#pragma mark === Transition Routines ===


//---------------------------------------------------------------------
//	DoTransition
//---------------------------------------------------------------------
//
//	Fire off a transition at the time
//
		
void TStageMovieCue::DoTransition(bool transitionIn)
{
	// Clean up last transition if neccessary
	if (m_Transition)
	{
		delete m_Transition;	
		m_Transition = NULL;
	}
	
	// We are now visible
	m_IsHidden = false;
	
	// Create new one and fire it off...
	if (transitionIn)
		m_Transition = new TTransition( this, (TStageView *)Parent(), m_ChannelCue->GetTransitionInID(), m_ChannelCue->GetTransitionInDuration()); 
	else
		m_Transition = new TTransition( this, (TStageView *)Parent(), m_ChannelCue->GetTransitionOutID(), m_ChannelCue->GetTransitionOutDuration()); 
	
	// Start it
	m_Transition->Start();

}



#pragma mark -
#pragma mark === Menu Routines ===

//---------------------------------------------------------------------
//	OpenStageCueMenu
//---------------------------------------------------------------------
//
//	Open stage cue pop up menu
//

void TStageMovieCue::OpenStageCueMenu(BPoint menuPt)
{
	BMenuItem 	*selected;
	
	// Create the menu and mark the current transition
	TStageCueMenu *theMenu = new TStageCueMenu(this->m_ChannelCue);
	
	if (theMenu)
	{	
		// Set menu location point		
		ConvertToScreen(&menuPt);
		selected = theMenu->Go(menuPt);
		
		// Check and see if we have a menu message
		int32 drawingMode;
		if (selected)
		{
			if ( selected->Message()->FindInt32("DrawingMode", &drawingMode) == B_OK)
			{			
				// Lock offscreen 								
				m_OffscreenView->Looper()->Lock();
				
				// Save drawing mode
				m_ChannelCue->SetDrawingMode( (drawing_mode)drawingMode );
				
				// Only redraw if mode has changed		
				if ( m_OffscreenView->DrawingMode() != m_ChannelCue->GetDrawingMode() )
				{
					//	Draw the bitmap into the offscreen using the new mode.
					m_OffscreenView->SetDrawingMode(m_ChannelCue->GetDrawingMode());			
					m_OffscreenView->DrawBitmap(m_Bitmap);
					m_OffscreenView->Sync();			
					Invalidate();
				}
				m_OffscreenView->Looper()->Unlock();
			}
		}
			
		// Clean up
		delete theMenu;
	}
}
