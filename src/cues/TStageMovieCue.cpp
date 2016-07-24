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

TStageMovieCue::TStageMovieCue(BRect bounds, char* name, TCueView* theCue) : TStageCue(bounds, name, theCue)
{
	// Do default initialization
	Init();

}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageMovieCue::TStageMovieCue(BMessage* theMessage) : TStageCue(theMessage)
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
	if (fTransition) {
		delete fTransition;
		fTransition = NULL;
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

	fTransition = NULL;

}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TStageMovieCue::Instantiate(BMessage* archive)
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

status_t TStageMovieCue::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TStageCue::Archive(data, deep);

	if (myErr == B_OK) {
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
	if ( (fBitmap) && IsCueHidden() == true &&!IsHidden() ) {
		rgb_color saveColor = HighColor();

		// Draw bitmap
		DrawBitmap(fOffscreen, B_ORIGIN);

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
	if ( fChannelCue->GetChannel()->GetMute() )
		return;

	if ( fBitmap && IsCueHidden() == false && IsHidden() == true) {
		BRect area = fChannelCue->GetCroppedArea();

		// Set up environment
		BRegion clipRegion, saveRegion;
		Parent()->GetClippingRegion(&saveRegion);
		clipRegion.Set(fChannelCue->GetCroppedArea());
		Parent()->ConstrainClippingRegion( &clipRegion );

		// Draw bitmap
		BPoint thePt(area.left, area.top);
		Parent()->DrawBitmap(fOffscreen, thePt);

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

void TStageMovieCue::CompositeData(BRect updateRect, BView* offscreen)
{
	// Do not draw if cue or channel is muted
//	if ( fChannelCue->GetChannel()->GetMute() )
//		return;

//	if ( fBitmap && IsCueHidden() == false && IsHidden() == true)
	{
		BRect area = fChannelCue->GetCroppedArea();

		// Set up environment
		BRegion clipRegion, saveRegion;
		offscreen->GetClippingRegion(&saveRegion);
		clipRegion.Set(fChannelCue->GetCroppedArea());
		offscreen->ConstrainClippingRegion( &clipRegion );

		// Draw bitmap
		BPoint thePt(area.left, area.top);
		offscreen->DrawBitmap(fOffscreen, thePt);

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
	VideoChunk vidChunk;

	const int32 frameSize = 320*240*2;

	BBitmap* displayBitmap = new BBitmap(BRect(0, 0, 319, 239), B_RGB15, false, false);

	// Test:  play movie
	BFile* theFile = fChannelCue->GetFile();

	// Load headers
	theFile->Seek(0, SEEK_SET);
	theFile->Read(&ckHeader, sizeof(ChunkHeader));
	theFile->Read(&vidChunk, sizeof(VideoChunk));

	// Load frames and display
	for (int32 frame = 0; frame < vidChunk.numFrames; frame++) {
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
	   uint32       buttons = 0;
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
	                fChannelCue->SetArea( Frame() );
	                fChannelCue->SetCroppedArea( Frame() );
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

void TStageMovieCue::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	// Do nothing if view is hidden
	if (IsHidden())
		return;

	// Set proper cursor for resize zones
	if ( fTopLeftResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fResizeDiagRightCursor);
	else if ( fTopMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fSizeVertCursor);
	else if ( fTopRightResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fResizeDiagLeftCursor);
	else if ( fRightMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fSizeHorzCursor);
	else if ( fRightMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fSizeHorzCursor);
	else if ( fBottomRightResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fResizeDiagRightCursor);
	else if ( fBottomMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fSizeVertCursor);
	else if ( fBottomLeftResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fResizeDiagLeftCursor);
	else if ( fLeftMiddleResize.Contains(where) )
		be_app->SetCursor( &((MuseumApp*)be_app)->fSizeHorzCursor);
	else
		be_app->SetCursor(B_HAND_CURSOR);
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

void TStageMovieCue::SetBitmap(BBitmap* bitmap)
{
	if (bitmap) {
		fBitmap = bitmap;

		// Create offscreen bitmap and view
		fOffscreen = new BBitmap( fBitmap->Bounds(), fBitmap->ColorSpace(), true, false);

		fOffscreenView = new BView( fBitmap->Bounds(), "OffscreenView", B_FOLLOW_NONE, NULL);
		fOffscreen->AddChild(fOffscreenView);
		fOffscreenView->Looper()->Lock();
		fOffscreenView->DrawBitmap(fBitmap);
		fOffscreenView->Sync();
		fOffscreenView->Looper()->Unlock();
	}
}



#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TStageMovieCue::MessageReceived(BMessage* message)
{
	double theTime;

	switch (message->what)
	{
	// Get updated bitmap
	case UPDATE_TIMELINE_MSG:
	{
		// Inform channel cue
		fChannelCue->MessageReceived(message);
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
	double cueStartTime = fChannelCue->GetStartTime();
	double cueEndTime       = fChannelCue->GetStartTime() + fChannelCue->GetDuration();

	// Is the current time before the cue's start time or after the cue's end time?
	if (theTime < cueStartTime || theTime >= cueEndTime ) {
		if ( IsCueHidden() == false) {
			fIsHidden = true;
			UpdatePictureCueRegion(theTime);
		}
	}
	// We need to show the cue if it is not already visible
	else{
		if ( IsCueHidden() == true) {
			// Set it's hidden flag to false
			fIsHidden = false;
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
	int32 cueChannelID = fChannelCue->GetChannelID();

	// Get total channels
	int32 totalChannels = static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetCueSheetView()->GetTotalChannels();

	//      Determine cue layer.  Iterate through all higher layers and determine
	//	the area to be invalidated.  Construct a region to do this.  Exlude all
	//	other area rects in higher channels than us.  We wil then break
	//	the region down into rects and invalidate them.
	BRegion invalRegion;
	invalRegion.Include(fChannelCue->GetArea());

	for(int32 index = cueChannelID+1; index <= totalChannels; index++) {
		TStageCue* stageCue = ((TStageView*)Parent())->GetStageCueAtTimeandChannel(theTime, index);

		if (stageCue)
			invalRegion.Exclude( stageCue->GetChannelCue()->GetArea());
	}

	// Now call our custom invalidation routine
	for(int32 index = 0; index < invalRegion.CountRects(); index++) {
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
	FillRect(fTopLeftResize);
	FillRect(fTopMiddleResize);
	FillRect(fTopRightResize);
	FillRect(fRightMiddleResize);
	FillRect(fBottomRightResize);
	FillRect(fBottomMiddleResize);
	FillRect(fBottomLeftResize);
	FillRect(fLeftMiddleResize);

	// Now stroke...
	SetHighColor(kBlack);
	StrokeRect(fTopLeftResize);
	StrokeRect(fTopMiddleResize);
	StrokeRect(fTopRightResize);
	StrokeRect(fRightMiddleResize);
	StrokeRect(fBottomRightResize);
	StrokeRect(fBottomMiddleResize);
	StrokeRect(fBottomLeftResize);
	StrokeRect(fLeftMiddleResize);

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
	BRect selectRect;

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
	for(int32 index = 0; index < theRegion.CountRects(); index++) {
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

	fTopLeftResize.Set( bounds.left, bounds.top, bounds.left+kBorder, bounds.top+kBorder);
	fTopMiddleResize.Set( (bounds.Width()/2) - (kBorder/2), bounds.top, (bounds.Width()/2) + (kBorder/2), bounds.top+kBorder);
	fTopRightResize.Set( bounds.right-kBorder, bounds.top, bounds.right, bounds.top+kBorder);
	fRightMiddleResize.Set( bounds.right-kBorder, (bounds.Height()/2) - (kBorder/2), bounds.right, (bounds.Height()/2) + (kBorder/2));
	fBottomRightResize.Set( bounds.right-kBorder, bounds.bottom-kBorder, bounds.right, bounds.bottom);
	fBottomMiddleResize.Set( (bounds.Width()/2) - (kBorder/2), bounds.bottom-kBorder, (bounds.Width()/2) + (kBorder/2), bounds.bottom);
	fBottomLeftResize.Set( bounds.left, bounds.bottom-kBorder, bounds.left+kBorder, bounds.bottom);
	fLeftMiddleResize.Set( bounds.left, (bounds.Height()/2) - (kBorder/2), bounds.left+kBorder, (bounds.Height()/2) + (kBorder/2));

}


//---------------------------------------------------------------------
//	PointInResizeZones
//---------------------------------------------------------------------
//
//	If the point is within one of the resize zones, return true
//

bool TStageMovieCue::PointInResizeZones(BPoint thePoint)
{
	if (fTopLeftResize.Contains(thePoint))
		return true;

	if (fTopMiddleResize.Contains(thePoint))
		return true;

	if (fTopRightResize.Contains(thePoint))
		return true;

	if (fRightMiddleResize.Contains(thePoint))
		return true;

	if (fBottomRightResize.Contains(thePoint))
		return true;

	if (fBottomMiddleResize.Contains(thePoint))
		return true;

	if (fBottomLeftResize.Contains(thePoint))
		return true;

	if (fLeftMiddleResize.Contains(thePoint))
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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);

	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint != savePt) {
			// Convert to parents coordinate system
			ConvertToParent(&thePoint);

			MoveBy( (thePoint.x - savePt.x), (thePoint.y - savePt.y) );

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
	if (fTopLeftResize.Contains(thePoint))
		return kTopLeftResize;

	if (fTopMiddleResize.Contains(thePoint))
		return kTopMiddleResize;

	if (fTopRightResize.Contains(thePoint))
		return kTopRightResize;

	if (fRightMiddleResize.Contains(thePoint))
		return kRightMiddleResize;

	if (fBottomRightResize.Contains(thePoint))
		return kBottomRightResize;

	if (fBottomMiddleResize.Contains(thePoint))
		return kBottomMiddleResize;

	if (fBottomLeftResize.Contains(thePoint))
		return kBottomLeftResize;

	if (fLeftMiddleResize.Contains(thePoint))
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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);
	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint.x != savePt.x) {
			float resize = thePoint.x - savePt.x;

			// Don't allow resize past bitmap width
			if ( (Bounds().Width() + resize <= fBitmap->Bounds().Width()) || (Bounds().right - resize >=  kBorder*2) ) {
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
		if (thePoint.x > fBitmap->Bounds().right)
			thePoint.x = fBitmap->Bounds().right;

		if (thePoint.x < fBitmap->Bounds().left + (kBorder*2) )
			thePoint.x = fBitmap->Bounds().left + (kBorder*2);

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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);
	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint.x != savePt.x) {
			float resize = thePoint.x - savePt.x;

			// Don't allow resize past bitmap width
			if ( (Bounds().Width() + resize <= fBitmap->Bounds().Width()) || (Bounds().right - resize >=  kBorder*2) ) {
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
		if (thePoint.x > fBitmap->Bounds().right)
			thePoint.x = fBitmap->Bounds().right;

		if (thePoint.x < fBitmap->Bounds().left + (kBorder*2) )
			thePoint.x = fBitmap->Bounds().left + (kBorder*2);

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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);
	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint != savePt) {
			float resizeX = thePoint.x - savePt.x;
			float resizeY = thePoint.y - savePt.y;

			// Don't allow resize past bitmap width and height
			if ( (Bounds().Width() + resizeX <= fBitmap->Bounds().Width()) || Bounds().Height() + resizeY <= fBitmap->Bounds().Height() ) {
				// Constrian to minimum bounds
				if ( (Bounds().right - resizeX >=  kBorder*2) || Bounds().bottom - resizeY >= fBitmap->Bounds().top + kBorder*2 ) {
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
		if (thePoint.x > fBitmap->Bounds().right)
			thePoint.x = fBitmap->Bounds().right;

		if (thePoint.y > fBitmap->Bounds().bottom)
			thePoint.y = fBitmap->Bounds().bottom;

		if (thePoint.x < fBitmap->Bounds().left + (kBorder*2) )
			thePoint.x = fBitmap->Bounds().left + (kBorder*2);

		if (thePoint.y < fBitmap->Bounds().top + (kBorder*2))
			thePoint.y = fBitmap->Bounds().top + (kBorder*2);

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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);
	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint.y != savePt.y) {
			float resize = thePoint.y - savePt.y;

			// Don't allow resize past bitmap height or mimimum height
			if ( Bounds().Height() + resize <= fBitmap->Bounds().Height() || Bounds().bottom - resize >= fBitmap->Bounds().top + kBorder*2 ) {
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
		if (thePoint.y > fBitmap->Bounds().bottom)
			thePoint.y = fBitmap->Bounds().bottom;

		if (thePoint.y < fBitmap->Bounds().top + (kBorder*2))
			thePoint.y = fBitmap->Bounds().top + (kBorder*2);

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
	BPoint savePt;
	uint32 buttons = 0;

	GetMouse(&thePoint, &buttons, true);
	ConvertToParent(&thePoint);
	savePt = thePoint;

	while (buttons) {
		if (thePoint != savePt) {
			float resize = thePoint.x - savePt.x;

			// Don't allow resize past bitmap width
			if ( Bounds().Width() <= fBitmap->Bounds().Width() ) {
				// Redraw the selection rect and resize points
				InvalidateSelectionRect();

				// Do resize.  Resize the view and move it to the proper location
				//float curRight = Bounds().right;

				BPoint movePt = thePoint;
				ConvertToParent(movePt);
				MoveTo( movePt.x, Frame().top );

				if ( Bounds().Width() + resize > fBitmap->Bounds().Width() )
					resize = Bounds().Width() - fBitmap->Bounds().Width();

				ResizeBy( -resize, 0 );

				SetResizeZones();
			}

			// Save mouse location for next compare
			savePt = thePoint;
		}

		GetMouse(&thePoint, &buttons, true);

		// Clip the point back within the proper bounds

		// Fix bounds violation
		//if ( Bounds().Width() > fBitmap->Bounds().Width() )
		//	ResizeBy( (Bounds().Width() - fBitmap->Bounds().Width()), 0 );

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
	if (fTransition) {
		delete fTransition;
		fTransition = NULL;
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
	if (fTransition) {
		delete fTransition;
		fTransition = NULL;
	}

	// We are now visible
	fIsHidden = false;

	// Create new one and fire it off...
	if (transitionIn)
		fTransition = new TTransition( this, (TStageView*)Parent(), fChannelCue->GetTransitionInID(), fChannelCue->GetTransitionInDuration());
	else
		fTransition = new TTransition( this, (TStageView*)Parent(), fChannelCue->GetTransitionOutID(), fChannelCue->GetTransitionOutDuration());

	// Start it
	fTransition->Start();

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
	BMenuItem* selected;

	// Create the menu and mark the current transition
	TStageCueMenu* theMenu = new TStageCueMenu(this->fChannelCue);

	if (theMenu) {
		// Set menu location point
		ConvertToScreen(&menuPt);
		selected = theMenu->Go(menuPt);

		// Check and see if we have a menu message
		int32 drawingMode;
		if (selected) {
			if ( selected->Message()->FindInt32("DrawingMode", &drawingMode) == B_OK) {
				// Lock offscreen
				fOffscreenView->Looper()->Lock();

				// Save drawing mode
				fChannelCue->SetDrawingMode( (drawing_mode)drawingMode );

				// Only redraw if mode has changed
				if ( fOffscreenView->DrawingMode() != fChannelCue->GetDrawingMode() ) {
					//	Draw the bitmap into the offscreen using the new mode.
					fOffscreenView->SetDrawingMode(fChannelCue->GetDrawingMode());
					fOffscreenView->DrawBitmap(fBitmap);
					fOffscreenView->Sync();
					Invalidate();
				}
				fOffscreenView->Looper()->Unlock();
			}
		}

		// Clean up
		delete theMenu;
	}
}
