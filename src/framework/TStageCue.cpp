//---------------------------------------------------------------------
//
//	File:	TStageCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.25.98
//
//	Desc:	A stage cue is the abstract class of view representing
//			a cues visual data on the stage.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <algorithm>
// ABH was #include <algobase.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "DebugUtils.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TStageView.h"
#include "TStageCueMenu.h"
#include "TVisualCue.h"
#include "TMouseWatcher.h"

#include "TRotateDialog.h"

#include "TStageCue.h"

// Constants
const short kBorder = 6;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageCue::TStageCue(TStageView *theStage, TVisualCue *theCue)
{
	// Save the stage
	fStage = theStage;

	// Save cue pointer
	fChannelCue = theCue;

	//	Do defualt initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageCue::~TStageCue()
{
	Deselect();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TStageCue::Init()
{
	// We start life hidden and unselected
	fIsSelected = false;

	fAntThread = NULL;

	// Set up resize zone rects
	SetResizeZones();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TStageCue::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
	//	Mouse watching messages.  Pass to selected cues...
	case MW_MOUSE_DOWN:
	case MW_MOUSE_UP:
	case MW_MOUSE_MOVED:
		HandleMouseMessage(message);
		break;

	default:
		break;
	}
}


//---------------------------------------------------------------------
//	HandleMouseMessage
//---------------------------------------------------------------------
//
//	Handle mouse operation on stage cue.  This is usually in repsonse to
//	some sort of tool manipulation of the cue
//

void TStageCue::HandleMouseMessage(BMessage *message)
{
	//	What tool mode are we in?

}



#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw data directly to stage
//

void TStageCue::Draw(BRect updateRect)
{
	// Only draw if selected
	if (fIsSelected) {
		//	Draw data selection and resizing rect directly to stage
		DrawSelectionRect(false);
	}
}


//---------------------------------------------------------------------
//	DrawData
//---------------------------------------------------------------------
//
//	Buffer drawing to offscreens
//

void TStageCue::DrawData()
{
	if (fIsSelected) {
		//	Update offscreen drawing our selection rect
		DrawSelectionRect(true);

		//	Copy offscreen to the stage
		fStage->Looper()->Lock();
		fStage->BlitOffscreen();
		fStage->Looper()->Unlock();
	}
}


//---------------------------------------------------------------------
//	DrawSelectionRect
//---------------------------------------------------------------------
//
//	Draw the selection rect and resize handles
//

void TStageCue::DrawSelectionRect(bool toOffscreen)
{
	// Only draw if selected
	if (fIsSelected) {
		// Draw to offscreen
		if (toOffscreen) {
			BView *offView = fStage->GetOffscreenView();

			offView->Looper()->Lock();
			offView->PushState();

			// Draw main selection rect
			offView->SetHighColor(kWhite);
			offView->SetDrawingMode(B_OP_INVERT);
			offView->SetPenSize(1.0);
			offView->StrokeRect(fChannelCue->CuePosition()->Enclosure());

			//	Draw cross hatch
			BPoint topLeft          = fChannelCue->CuePosition()->Corner1();
			BPoint topRight         = fChannelCue->CuePosition()->Corner2();
			BPoint bottomRight      = fChannelCue->CuePosition()->Corner3();
			BPoint bottomLeft       = fChannelCue->CuePosition()->Corner4();
			offView->StrokeLine(topLeft,  bottomRight);
			offView->StrokeLine(topRight, bottomLeft);

			// Draw resizing handles
			DrawResizingHandles(offView);

			//	Update offscreen
			offView->Sync();
			offView->PopState();
			offView->Looper()->Unlock();
		}
		//	Directly to view
		else{
			fStage->Looper()->Lock();
			fStage->PushState();

			// Draw main selection rect
			fStage->SetHighColor(kWhite);
			fStage->SetDrawingMode(B_OP_INVERT);
			fStage->SetPenSize(1.0);
			fStage->StrokeRect(fChannelCue->CuePosition()->Enclosure());

			//	Draw cross hatch
			BPoint topLeft          = fChannelCue->CuePosition()->Corner1();
			BPoint topRight         = fChannelCue->CuePosition()->Corner2();
			BPoint bottomRight      = fChannelCue->CuePosition()->Corner3();
			BPoint bottomLeft       = fChannelCue->CuePosition()->Corner4();
			fStage->StrokeLine(topLeft,  bottomRight);
			fStage->StrokeLine(topRight, bottomLeft);

			// Draw resizing handles
			DrawResizingHandles(fStage);

			//	Restore
			fStage->PopState();
			fStage->Looper()->Unlock();
		}
	}
}

//---------------------------------------------------------------------
//	DrawResizingHandle
//---------------------------------------------------------------------
//
//	Draw handles based on stage tool mode
//

void TStageCue::DrawResizingHandles(BView *theView)
{
	//	Save state
	theView->Looper()->Lock();
	theView->PushState();

	//	Determine whcih handles to draw based on stage tool mode
	switch(fStage->GetToolMode())
	{
	//	Draw none
	case kMoveMode:
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners
	case kRotateMode:
		DrawResizeCorners(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners and middles
	case kCropMode:
		DrawResizeCorners(theView);
		DrawResizeMiddles(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners and middles
	case kScaleMode:
		DrawResizeCorners(theView);
		DrawResizeMiddles(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners
	case kShearMode:
		DrawResizeCorners(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners
	case kSkewMode:
		DrawResizeCorners(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw corners
	case kPerspectiveMode:
		DrawResizeCorners(theView);
		DrawRegistrationPoint(theView);
		break;

	//	Draw middles
	case kMirrorMode:
		DrawResizeMiddles(theView);
		DrawRegistrationPoint(theView);
		break;

	default:
		break;
	}

	//	Restore
	theView->PopState();
	theView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	DrawResizeCorners
//---------------------------------------------------------------------
//
//	Draw corner resize handles
//

void TStageCue::DrawResizeCorners(BView *theView)
{
	//	Save state
	theView->Looper()->Lock();
	theView->PushState();

	theView->SetHighColor(kWhite);
	theView->SetDrawingMode(B_OP_INVERT);
	theView->SetPenSize(1.0);

	// Stroke handles
	BPolygon *handlePoly;

	handlePoly = fTopLeftResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fTopRightResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fBottomRightResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fBottomLeftResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	//	Restore
	theView->PopState();
	theView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	DrawResizeMiddles
//---------------------------------------------------------------------
//
//	Draw middle resize handles
//

void TStageCue::DrawResizeMiddles(BView *theView)
{
	//	Save state
	theView->Looper()->Lock();
	theView->PushState();

	theView->SetHighColor(kWhite);
	theView->SetDrawingMode(B_OP_INVERT);
	theView->SetPenSize(1.0);

	//	Stroke handles
	//theView->StrokePolygon(&fTopMiddleResize, true);
	//theView->StrokePolygon(&fRightMiddleResize, true);
	//theView->StrokePolygon(&fBottomMiddleResize, true);
	//theView->StrokePolygon(&fLeftMiddleResize, true);

	BPolygon *handlePoly;

	handlePoly = fTopMiddleResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fRightMiddleResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fBottomMiddleResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;

	handlePoly = fLeftMiddleResize.GetBPolygon();
	theView->StrokePolygon(handlePoly, true);
	delete handlePoly;


	//	Restore
	theView->PopState();
	theView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	DrawRegistrationPoint
//---------------------------------------------------------------------
//
//	Draw cue registration point
//

void TStageCue::DrawRegistrationPoint(BView *theView)
{
	//	Save state
	theView->Looper()->Lock();
	theView->PushState();

	const int32 kRegPointWidth = 12;
	const int32 kRegPointHalf  = 6;
	const int32 kRegLineLength = 2;

	// Calculate point position
	BPoint regPt = fChannelCue->CuePosition()->Registration();

	BRect ellipse;
	ellipse.left    = regPt.x - kRegPointHalf;
	ellipse.right   = regPt.x + kRegPointHalf;
	ellipse.top             = regPt.y - kRegPointHalf;
	ellipse.bottom  = regPt.y + kRegPointHalf;

	//	Draw in XOR mode
	theView->SetDrawingMode(B_OP_INVERT);
	theView->SetPenSize(1.0);

	//	Stroke ellipse
	theView->SetHighColor(kWhite);
	theView->StrokeEllipse(ellipse);

	//
	// Draw Cross
	//

	BPoint startPt, endPt;

	// Left
	startPt.Set( ellipse.left+kRegLineLength, ellipse.top+kRegLineLength);
	endPt.Set( ellipse.right-kRegLineLength, ellipse.bottom-kRegLineLength);
	theView->StrokeLine(startPt, endPt);

	// Right
	startPt.Set( ellipse.right-kRegLineLength, ellipse.top+kRegLineLength);
	endPt.Set( ellipse.left+kRegLineLength, ellipse.bottom-kRegLineLength);
	theView->StrokeLine(startPt, endPt);

	//	Restore
	theView->PopState();
	theView->Looper()->Unlock();
}



#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TStageCue::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( IsPlaying() )
		return;

	// Make sure point falls within our bounds or resize zones
	if ( fChannelCue->CuePosition()->Contains(where) || PointInResizeZones(where) ) {

		uint32 type;
		int32 count = 0;
		BMessage *message = fStage->Window()->CurrentMessage();
		if (B_OK == message->GetInfo("clicks", &type, &count) ) {
			int32 clickCount = message->FindInt32("clicks", count-1);

			// Is this a double click?  If so, open editor
			if (clickCount == 2) {
				//if(fChannelCue->HasEditor())
				//	fChannelCue->OpenEditor();
			}
			//	Open transform dialog for current tool
			else if ( IsOptionKeyDown() ) {
				OpenCurrentToolDialog();
			}
			// else, begin MouseDown handling of StageCue
			else{
				// Wait a short while before dragging
				snooze(60 * 1000);

				// Determine which button has been clicked
				uint32 buttons = 0;
				BMessage *message = fStage->Window()->CurrentMessage();
				message->FindInt32("buttons", (long *)&buttons);

				switch(buttons)
				{
				case B_PRIMARY_MOUSE_BUTTON:
				{
					//	Find location of click.  If it is the main body of the picture, they are moving the picture.
					//	Otherwise, they are using a stage tool
					if ( PointInResizeZones(where) ) {
						switch(fStage->GetToolMode())
						{
						case kMoveMode:
							DragPicture(where);
							break;

						case kRotateMode:
							//Rotate(where);
							break;

						case kCropMode:
							//Crop(where);
							break;

						case kScaleMode:
							//Scale(where);
							break;

						case kShearMode:
							break;

						case kSkewMode:
							break;

						case kPerspectiveMode:
							break;

						case kMirrorMode:
							//Mirror(where);
							break;

						default:
							break;
						}
					}
					// They are dragging the picture...
					else{
						DragPicture(where);
					}
				}
				break;

				// Show stage cue menu
				case B_SECONDARY_MOUSE_BUTTON:
					OpenStageCueMenu(where);
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TStageCue::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
	//	Do nothing if window is not active
	if (fStage->Window()->IsActive() == false)
		return;

	//	Determine tool mode and set proper cursor
	switch(fStage->GetToolMode())
	{
	case kMoveMode:
		SetMoveCursor(where);
		break;

	case kRotateMode:
		SetRotateCursor(where);
		break;

	case kCropMode:
		SetCropCursor(where);
		break;

	case kScaleMode:
		SetScaleCursor(where);
		break;

	case kShearMode:
		SetShearCursor(where);
		break;

	case kSkewMode:
		SetSkewCursor(where);
		break;

	case kPerspectiveMode:
		SetPerspectiveCursor(where);
		break;

	case kMirrorMode:
		SetMirrorCursor(where);
		break;

	default:
		be_app->SetCursor(B_HAND_CURSOR);
		break;
	}
}


#pragma mark -
#pragma mark === Dragging Routines ===

//---------------------------------------------------------------------
//	DragPicture
//---------------------------------------------------------------------
//
//	While the mouse is down, move the picture
//

void TStageCue::DragPicture(BPoint thePoint)
{
	BRect outline;
	BPoint savePt;
	uint32 buttons = 0;

	fStage->GetMouse(&thePoint, &buttons, true);

	// Save point for future compare
	savePt = thePoint;

	while (buttons) {
		if (thePoint != savePt) {
			// Move the stage cue
			//

			// Offset mousePt
			float diffX = thePoint.x - savePt.x;
			float diffY = thePoint.y - savePt.y;

			//	Clean up old selection rect
			DrawSelectionRect(false);

			// Adjust cue's stage location
			outline = fChannelCue->CuePosition()->Enclosure();
			outline.left    += diffX;
			outline.top     += diffY;
			outline.right   += diffX;
			outline.bottom  += diffY;
			fChannelCue->CuePosition()->Outline(outline);

			// Update resize zones
			SetResizeZones();

			// Save mouse location for next compare
			savePt = thePoint;

			//	Draw new selection rect
			DrawSelectionRect(false);

			//	Draw new bitmap position if control key is down
			if ( IsControlKeyDown()) {
				if (fStage->LockLooper()) {
					fStage->StageDraw(fChannelCue->CuePosition()->Enclosure(), GetCurrentTime());
					fStage->Draw(fStage->Bounds());
					fStage->UnlockLooper();
				}
			}
		}
		fStage->GetMouse(&thePoint, &buttons, true);
	}

	//	Draw new position
	if ( fStage->LockLooper()) {
		fStage->StageDraw(fChannelCue->CuePosition()->Enclosure(), GetCurrentTime());
		fStage->Draw(fStage->Bounds());
		fStage->UnlockLooper();
	}
}


#pragma mark -
#pragma mark === Selection Routines ===


//---------------------------------------------------------------------
//	Select
//---------------------------------------------------------------------
//
//	Select and draw selection gird
//

void TStageCue::Select()
{
	if (fIsSelected == false) {
		fIsSelected = true;

		//	Draw selection rect directly to stage
		DrawSelectionRect(false);
	}
}

//---------------------------------------------------------------------
//	Deselect
//---------------------------------------------------------------------
//
//	Deselect and stop marching ants

void TStageCue::Deselect()
{
	if (fIsSelected) {
		fIsSelected = false;
	}
}

#pragma mark -
#pragma mark === Cue Selection and Resizing ===


//---------------------------------------------------------------------
//	InvalidateSelectionRect
//---------------------------------------------------------------------
//
//	Invalidate just the selection rect and force a redraw
//


void TStageCue::InvalidateSelectionRect()
{
	// Create a region containing selection rect
	BRegion theRegion;
	BRect selectRect;
	/*
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
	 */
}

//---------------------------------------------------------------------
//	SetResizeZones
//---------------------------------------------------------------------
//
//	Set up the rects defining the picture resize zones

void TStageCue::SetResizeZones()
{
	//	Clear out old polygon positions
	ClearResizeZones();

	//	Do we need to adjust for rotation?
	//if ( fChannelCue->GetRotation() > 0)
	//{
	//	TranformResizeZones();
	//}
	//	Standard display
	//else
	{
		const BRect bounds              = fChannelCue->CuePosition()->Enclosure();
		const int32 halfBorder      = kBorder / 2;
		const int32 halfWidth       = bounds.IntegerWidth() / 2;
		const int32 halfHeight      = bounds.IntegerHeight() / 2;
		BPoint thePoint;

		//	Top Left
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopLeftResize.AddPoint(thePoint);

		//	Top Middle
		thePoint.x = (bounds.left+halfWidth)-halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopMiddleResize.AddPoint(thePoint);
		thePoint.x = (bounds.left+halfWidth)+halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopMiddleResize.AddPoint(thePoint);
		thePoint.x = (bounds.left+halfWidth)+halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopMiddleResize.AddPoint(thePoint);
		thePoint.x = (bounds.left+halfWidth)-halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopMiddleResize.AddPoint(thePoint);

		//	Top Right
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = bounds.top-halfBorder;
		fTopRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = bounds.top+halfBorder;
		fTopRightResize.AddPoint(thePoint);

		//	Right Middle
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = (halfHeight+bounds.top)-halfBorder;
		fRightMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = (halfHeight+bounds.top)-halfBorder;
		fRightMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = (halfHeight+bounds.top)+halfBorder;
		fRightMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = (halfHeight+bounds.top)+halfBorder;
		fRightMiddleResize.AddPoint(thePoint);

		//	Right Bottom
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right+halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomRightResize.AddPoint(thePoint);
		thePoint.x = bounds.right-halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomRightResize.AddPoint(thePoint);

		//	Bottom Middle
		thePoint.x = (halfWidth+bounds.left)-halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomMiddleResize.AddPoint(thePoint);
		thePoint.x = (halfWidth+bounds.left)+halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomMiddleResize.AddPoint(thePoint);
		thePoint.x = (halfWidth+bounds.left)+halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomMiddleResize.AddPoint(thePoint);
		thePoint.x = (halfWidth+bounds.left)-halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomMiddleResize.AddPoint(thePoint);

		//	Bottom Left
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = bounds.bottom-halfBorder;
		fBottomLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomLeftResize.AddPoint(thePoint);
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = bounds.bottom+halfBorder;
		fBottomLeftResize.AddPoint(thePoint);

		//	Left Middle
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = (halfHeight+bounds.top)-halfBorder;
		fLeftMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = (halfHeight+bounds.top)-halfBorder;
		fLeftMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.left+halfBorder;
		thePoint.y = (halfHeight+bounds.top)+halfBorder;
		fLeftMiddleResize.AddPoint(thePoint);
		thePoint.x = bounds.left-halfBorder;
		thePoint.y = (halfHeight+bounds.top)+halfBorder;
		fLeftMiddleResize.AddPoint(thePoint);
	}
}


//---------------------------------------------------------------------
//	TranformResizeZones
//---------------------------------------------------------------------
//
//	Adjust zones for rotation
//

void TStageCue::TranformResizeZones()
{
	/*
	   const BRect bounds           = fChannelCue->GetDrawArea();
	   const int8	halfBorder      = kBorder / 2;
	   const int32	halfWidth	= bounds.Width() / 2;
	   const int32	halfHeight	= bounds.Height() / 2;
	   BPoint		thePoint;

	   //	Variables used for point precalculation
	   float cosLeft, cosRight, cosTop, cosBottom;
	   float sinLeft, sinRight, sinTop, sinBottom;

	   //	Get point of rotation
	   BPoint center = fChannelCue->GetRegistrationOffset();

	   //	Calculate rotation angle in radians
	   double	angle   = ((double) 3.14159265 / (double) 180.0) * (double) fChannelCue->GetRotation();

	   //	Calculate sine and cosine of rotation angle
	   double       cosine  = cos(angle);
	   double       sine    = sin(angle);

	   //	Precalculate center point
	   float centerXVal = center.x * (1-cosine) + center.y * sine;
	   float centerYVal = (-center.x * sine + center.y * (1-cosine));

	   //
	   //	Top Left
	   //

	   // Calculate base rectangle
	   BRect topLeft;

	   //	Top Left
	   topLeft.left         = bounds.left-halfBorder;
	   topLeft.top  = bounds.top-halfBorder;
	   topLeft.right        = bounds.left+halfBorder;
	   topLeft.bottom       = bounds.top+halfBorder;

	   //	Precalculate points
	   cosLeft      = topLeft.left   * cosine;
	   sinLeft      = topLeft.left   * sine;
	   cosRight	= topLeft.right  * cosine;
	   sinRight	= topLeft.right  * sine;
	   cosTop		= topLeft.top    * cosine;
	   sinTop		= topLeft.top    * sine;
	   cosBottom	= topLeft.bottom * cosine;
	   sinBottom	= topLeft.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fTopLeftResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fTopLeftResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fTopLeftResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fTopLeftResize.AddPoint(thePoint);


	   //
	   //	Top Middle
	   //

	   // Calculate base rectangle
	   BRect topMiddle;

	   //	Top Middle
	   topMiddle.left        = (bounds.left+halfWidth)-halfBorder;
	   topMiddle.top         = bounds.top-halfBorder;
	   topMiddle.right  = (bounds.left+halfWidth)+halfBorder;
	   topMiddle.bottom = bounds.top+halfBorder;

	   //	Precalculate points
	   cosLeft      = topMiddle.left   * cosine;
	   sinLeft      = topMiddle.left   * sine;
	   cosRight	= topMiddle.right  * cosine;
	   sinRight	= topMiddle.right  * sine;
	   cosTop		= topMiddle.top    * cosine;
	   sinTop		= topMiddle.top    * sine;
	   cosBottom	= topMiddle.bottom * cosine;
	   sinBottom	= topMiddle.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fTopMiddleResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fTopMiddleResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fTopMiddleResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fTopMiddleResize.AddPoint(thePoint);



	   //
	   //	Top Right
	   //

	   // Calculate base rectangle
	   BRect topRight;

	   //	Top Right
	   topRight.left        = bounds.right-halfBorder;
	   topRight.top         = bounds.top-halfBorder;
	   topRight.right  = bounds.right+halfBorder;
	   topRight.bottom	= bounds.top+halfBorder;

	   //	Precalculate points
	   cosLeft      = topRight.left   * cosine;
	   sinLeft      = topRight.left   * sine;
	   cosRight	= topRight.right  * cosine;
	   sinRight	= topRight.right  * sine;
	   cosTop		= topRight.top    * cosine;
	   sinTop		= topRight.top    * sine;
	   cosBottom	= topRight.bottom * cosine;
	   sinBottom	= topRight.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fTopRightResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fTopRightResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fTopRightResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fTopRightResize.AddPoint(thePoint);


	   //
	   //	Right Middle
	   //

	   // Calculate base rectangle
	   BRect rightMiddle;

	   //	Right Middle
	   rightMiddle.left     = bounds.right-halfBorder;
	   rightMiddle.top      = (halfHeight+bounds.top)-halfBorder;
	   rightMiddle.right    = bounds.right+halfBorder;
	   rightMiddle.bottom	= (halfHeight+bounds.top)+halfBorder;

	   //	Precalculate points
	   cosLeft      = rightMiddle.left   * cosine;
	   sinLeft      = rightMiddle.left   * sine;
	   cosRight	= rightMiddle.right  * cosine;
	   sinRight	= rightMiddle.right  * sine;
	   cosTop		= rightMiddle.top    * cosine;
	   sinTop		= rightMiddle.top    * sine;
	   cosBottom	= rightMiddle.bottom * cosine;
	   sinBottom	= rightMiddle.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fRightMiddleResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fRightMiddleResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fRightMiddleResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fRightMiddleResize.AddPoint(thePoint);


	   //
	   //	Right Bottom
	   //

	   // Calculate base rectangle
	   BRect rightBottom;

	   //	Right Bottom
	   rightBottom.left     = bounds.right-halfBorder;
	   rightBottom.top      = bounds.bottom-halfBorder;
	   rightBottom.right    = bounds.right+halfBorder;
	   rightBottom.bottom	= bounds.bottom+halfBorder;

	   //	Precalculate points
	   cosLeft      = rightBottom.left   * cosine;
	   sinLeft      = rightBottom.left   * sine;
	   cosRight	= rightBottom.right  * cosine;
	   sinRight	= rightBottom.right  * sine;
	   cosTop		= rightBottom.top    * cosine;
	   sinTop		= rightBottom.top    * sine;
	   cosBottom	= rightBottom.bottom * cosine;
	   sinBottom	= rightBottom.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fBottomRightResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fBottomRightResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fBottomRightResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fBottomRightResize.AddPoint(thePoint);



	   //
	   //	Bottom Middle
	   //

	   // Calculate base rectangle
	   BRect bottomMiddle;

	   //	Bottom Middle
	   bottomMiddle.left    = (halfWidth+bounds.left)-halfBorder;
	   bottomMiddle.top     = bounds.bottom-halfBorder;
	   bottomMiddle.right  = (halfWidth+bounds.left)+halfBorder;
	   bottomMiddle.bottom	= bounds.bottom+halfBorder;

	   //	Precalculate points
	   cosLeft      = bottomMiddle.left   * cosine;
	   sinLeft      = bottomMiddle.left   * sine;
	   cosRight	= bottomMiddle.right  * cosine;
	   sinRight	= bottomMiddle.right  * sine;
	   cosTop		= bottomMiddle.top    * cosine;
	   sinTop		= bottomMiddle.top    * sine;
	   cosBottom	= bottomMiddle.bottom * cosine;
	   sinBottom	= bottomMiddle.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fBottomMiddleResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fBottomMiddleResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fBottomMiddleResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fBottomMiddleResize.AddPoint(thePoint);



	   //
	   //	Bottom Left
	   //

	   // Calculate base rectangle
	   BRect bottomLeft;

	   //	Bottom Left
	   bottomLeft.left      = bounds.left-halfBorder;
	   bottomLeft.top               = bounds.bottom-halfBorder;
	   bottomLeft.right     = bounds.left+halfBorder;
	   bottomLeft.bottom	= bounds.bottom+halfBorder;

	   //	Precalculate points
	   cosLeft      = bottomLeft.left   * cosine;
	   sinLeft      = bottomLeft.left   * sine;
	   cosRight	= bottomLeft.right  * cosine;
	   sinRight	= bottomLeft.right  * sine;
	   cosTop		= bottomLeft.top    * cosine;
	   sinTop		= bottomLeft.top    * sine;
	   cosBottom	= bottomLeft.bottom * cosine;
	   sinBottom	= bottomLeft.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fBottomLeftResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fBottomLeftResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fBottomLeftResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fBottomLeftResize.AddPoint(thePoint);


	   //
	   //	Left Middle
	   //

	   // Calculate base rectangle
	   BRect leftMiddle;

	   //	Bottom Left
	   leftMiddle.left      = bounds.left-halfBorder;
	   leftMiddle.top               = (halfHeight+bounds.top)-halfBorder;
	   leftMiddle.right     = bounds.left+halfBorder;
	   leftMiddle.bottom	= (halfHeight+bounds.top)+halfBorder;

	   //	Precalculate points
	   cosLeft      = leftMiddle.left   * cosine;
	   sinLeft      = leftMiddle.left   * sine;
	   cosRight	= leftMiddle.right  * cosine;
	   sinRight	= leftMiddle.right  * sine;
	   cosTop		= leftMiddle.top    * cosine;
	   sinTop		= leftMiddle.top    * sine;
	   cosBottom	= leftMiddle.bottom * cosine;
	   sinBottom	= leftMiddle.bottom * sine;

	   //
	   //	Set up polygon

	   //	Top left
	   thePoint.x = cosLeft - sinTop + centerXVal;
	   thePoint.y = sinLeft + cosTop + centerYVal;
	   fLeftMiddleResize.AddPoint(thePoint);

	   //	Top right
	   thePoint.x = cosRight - sinTop + centerXVal;
	   thePoint.y = sinRight + cosTop + centerYVal;
	   fLeftMiddleResize.AddPoint(thePoint);

	   //	Bottom right
	   thePoint.x = cosRight - sinBottom + centerXVal;
	   thePoint.y = sinRight + cosBottom + centerYVal;
	   fLeftMiddleResize.AddPoint(thePoint);

	   //	Bottom left
	   thePoint.x = cosLeft - sinBottom + centerXVal;
	   thePoint.y = sinLeft + cosBottom + centerYVal;
	   fLeftMiddleResize.AddPoint(thePoint);
	 */
}


//---------------------------------------------------------------------
//	ClearResizeZones
//---------------------------------------------------------------------
//
//	Empty and clear resize polygons
//

void TStageCue::ClearResizeZones()
{
	TPolygon emptyPoly;

	fTopLeftResize          = emptyPoly;
	fTopMiddleResize                = emptyPoly;
	fTopRightResize                 = emptyPoly;
	fRightMiddleResize      = emptyPoly;
	fBottomRightResize      = emptyPoly;
	fBottomMiddleResize     = emptyPoly;
	fBottomLeftResize               = emptyPoly;
	fLeftMiddleResize               = emptyPoly;

}

//---------------------------------------------------------------------
//	PointInResizeZones
//---------------------------------------------------------------------
//
//	If the point is within one of the resize zones, return true
//

bool TStageCue::PointInResizeZones(BPoint thePoint)
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
//	GetResizeZoneID
//---------------------------------------------------------------------
//
//	Return resize ID from point
//


short TStageCue::GetResizeZoneID(BPoint thePoint)
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

#pragma mark -
#pragma mark === Scaling Routines ===
/*
   //---------------------------------------------------------------------
   //	Scale
   //---------------------------------------------------------------------
   //
   //	Scale the picture
   //

   void TStageCue::Scale(BPoint thePoint)
   {
        short zoneID = GetResizeZoneID(thePoint);

        switch(zoneID)
        {
                case kTopLeftResize:
                        ScaleTopLeft(thePoint);
                        break;

                case kTopMiddleResize:
                        ScaleTop(thePoint);
                        break;

                case kTopRightResize:
                        ScaleTopRight(thePoint);
                        break;

                case kRightMiddleResize:
                        ScaleRight(thePoint);
                        break;

                case kBottomRightResize:
                        ScaleBottomRight(thePoint);
                        break;

                case kBottomMiddleResize:
                        ScaleBottom(thePoint);
                        break;

                case kBottomLeftResize:
                        ScaleBottomLeft(thePoint);
                        break;

                case kLeftMiddleResize:
                        ScaleLeft(thePoint);
                        break;

                // Bad ID value
                default:
                        break;
        }

        //	Update stage
        fStage->Looper()->Lock();
        //fStage->StageDraw(fChannelCue->GetCroppedArea(), GetCurrentTime());
        fStage->Invalidate();
        fStage->Looper()->Unlock();

        //	Update bounds polygon
        //fChannelCue->GetTransformedDrawPolygon(fBoundsPolygon);

        // Update resize zones
        SetResizeZones();
   }


   //---------------------------------------------------------------------
   //	ResizeTopLeft
   //---------------------------------------------------------------------
   //
   //	Resize the top left of the view.
   //

   void TStageCue::ScaleTopLeft( BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->CuePosition()->Enclosure();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 bottomSide = cropArea.bottom;
        const int32 rightSide  = cropArea.right;

        //	Get registration percentage
        float	regPercX = oldReg.x / cropArea.Width();
        float	regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origWidth  = origRect.Width();
        const float origHeight = origRect.Height();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        if (thePoint.x != savePt.x)
                        {
                                // Clip if needed
                                if ( thePoint.x > rightSide - (kBorder*2))
                                        thePoint.x  = rightSide - (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetDrawPointX(thePoint.x);
                                fChannelCue->SetScaleX((rightSide - thePoint.x) / origWidth);
                        }

                        if (thePoint.y != savePt.y)
                        {
                                // Clip if needed
                                if ( thePoint.y > bottomSide - (kBorder*2))
                                        thePoint.y  = bottomSide - (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetDrawPointY(thePoint.y);
                                fChannelCue->SetScaleY((bottomSide - thePoint.y) / origHeight);
                        }

                        //	Normalize registration point
                        oldReg.x = fChannelCue->GetDrawArea().Width()  * regPercX;
                        oldReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(oldReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;

                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x > rightSide - (kBorder*2))
                        thePoint.x  = rightSide - (kBorder*2);

                if ( thePoint.y > bottomSide - (kBorder*2))
                        thePoint.y  = bottomSide - (kBorder*2);

        }
   }

   //---------------------------------------------------------------------
   //	ScaleTop
   //---------------------------------------------------------------------
   //
   //	Scale the top side of the view. Don't allow top to be less
   //	than bottom
   //

   void TStageCue::ScaleTop(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 bottomSide = cropArea.bottom;

        //	Get registration percentage
        float regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origHeight = origRect.Height();

        // Resize the cue to the left while the mouse button is down
        BPoint	savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.y != savePt.y)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Clip if needed
                        if ( thePoint.y > bottomSide - (kBorder*2))
                                thePoint.y  = bottomSide - (kBorder*2);

                        //	Update the cue varibles to new settings
                        fChannelCue->SetDrawPointY(thePoint.y);
                        fChannelCue->SetScaleY((bottomSide - thePoint.y) / origHeight);

                        //	Normalize registration point
                        BPoint newReg = oldReg;
                        newReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.y > bottomSide - (kBorder*2))
                        thePoint.y  = bottomSide - (kBorder*2);
        }
   }


   //---------------------------------------------------------------------
   //	ScaleTopRight
   //---------------------------------------------------------------------
   //
   //	Scale the top and right side of the view.
   //

   void TStageCue::ScaleTopRight(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 bottomSide = cropArea.bottom;
        const int32 leftSide   = cropArea.left;

        //	Get registration percentage
        float	regPercX = oldReg.x / cropArea.Width();
        float	regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origWidth  = origRect.Width();
        const float origHeight = origRect.Height();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        if (thePoint.x != savePt.x)
                        {
                                // Clip if needed
                                if ( thePoint.x < leftSide + (kBorder*2))
                                        thePoint.x  = leftSide + (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetScaleX((thePoint.x-leftSide) / origWidth);
                        }

                        if (thePoint.y != savePt.y)
                        {
                                // Clip if needed
                                if ( thePoint.y > bottomSide - (kBorder*2))
                                        thePoint.y  = bottomSide - (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetDrawPointY(thePoint.y);
                                fChannelCue->SetScaleY((bottomSide - thePoint.y) / origHeight);
                        }

                        //	Normalize registration point
                        oldReg.x = fChannelCue->GetDrawArea().Width()  * regPercX;
                        oldReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(oldReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;

                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x < leftSide + (kBorder*2))
                        thePoint.x  = leftSide + (kBorder*2);

                if ( thePoint.y > bottomSide - (kBorder*2))
                        thePoint.y  = bottomSide - (kBorder*2);
        }
   }

   //---------------------------------------------------------------------
   //	ScaleRight
   //---------------------------------------------------------------------
   //
   //	Scale the right side of the view. Don't allow right to be
   //	less than left
   //

   void TStageCue::ScaleRight(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 leftSide = cropArea.left;

        //	Get registration percentage
        float regPercX = oldReg.x / cropArea.Width();

        //	Save bitmap original size
        const BRect origRect  = fChannelCue->GetOriginalArea();
        const float origWidth = origRect.Width();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.x != savePt.x)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Clip if needed
                        if ( thePoint.x < leftSide + (kBorder*2))
                                thePoint.x  = leftSide + (kBorder*2);

                        //	Update the cue varibles to new settings
                        fChannelCue->SetScaleX((thePoint.x-leftSide) / origWidth);

                        //	Normalize registration point
                        BPoint newReg = oldReg;
                        newReg.x = fChannelCue->GetDrawArea().Width() * regPercX;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x < leftSide + (kBorder*2))
                        thePoint.x  = leftSide + (kBorder*2);
        }
   }

   //---------------------------------------------------------------------
   //	ScaleBottomRight
   //---------------------------------------------------------------------
   //
   //	Scale the bottom and right side of the view.
   //

   void TStageCue::ScaleBottomRight(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 topSide  = cropArea.top;
        const int32 leftSide = cropArea.left;

        //	Get registration percentage
        float	regPercX = oldReg.x / cropArea.Width();
        float	regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origWidth  = origRect.Width();
        const float origHeight = origRect.Height();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        if (thePoint.x != savePt.x)
                        {
                                // Clip if needed
                                if ( thePoint.x < leftSide + (kBorder*2))
                                        thePoint.x  = leftSide + (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetScaleX((thePoint.x-leftSide) / origWidth);
                        }

                        if (thePoint.y != savePt.y)
                        {
                                // Clip if needed
                                if ( thePoint.y < topSide + (kBorder*2))
                                        thePoint.y  = topSide + (kBorder*2);

                                //	Update the cue varibles to new settings
                                fChannelCue->SetScaleY((thePoint.y - topSide) / origHeight);
                        }

                        //	Normalize registration point
                        oldReg.x = fChannelCue->GetDrawArea().Width()  * regPercX;
                        oldReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(oldReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x < leftSide + (kBorder*2))
                        thePoint.x  = leftSide + (kBorder*2);

                if ( thePoint.y < topSide + (kBorder*2))
                        thePoint.y  = topSide + (kBorder*2);
        }
   }


   //---------------------------------------------------------------------
   //	ScaleBottom
   //---------------------------------------------------------------------
   //
   //	Scale the bottom side of the view.  Constrain to the height of
   //	the bitmap.
   //

   void TStageCue::ScaleBottom(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        int32 topSide  = cropArea.top;

        //	Get registration percentage
        float regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origHeight = origRect.Height();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.y != savePt.y)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Clip if needed
                        if ( thePoint.y < topSide + (kBorder*2))
                                thePoint.y  = topSide + (kBorder*2);

                        //	Update the cue variables to new settings
                        fChannelCue->SetScaleY((thePoint.y - topSide) / origHeight);

                        //	Normalize registration point
                        BPoint newReg = oldReg;
                        newReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.y < topSide + (kBorder*2))
                        thePoint.y  = topSide + (kBorder*2);
        }
   }

   //---------------------------------------------------------------------
   //	ScaleBottomLeft
   //---------------------------------------------------------------------
   //
   //	Scale the bottom and left side of the view.
   //

   void TStageCue::ScaleBottomLeft(BPoint thePoint)
   {
        // Setup variables
        BRect cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        const int32 topSide     = cropArea.top;
        const int32 rightSide	= cropArea.right;

        //	Get registration percentage
        float	regPercX = oldReg.x / cropArea.Width();
        float	regPercY = oldReg.y / cropArea.Height();

        //	Save bitmap original size
        const BRect origRect   = fChannelCue->GetOriginalArea();
        const float origWidth  = origRect.Width();
        const float origHeight = origRect.Height();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        if (thePoint.x != savePt.x)
                        {
                                // Clip if needed
                                if ( thePoint.x > rightSide - (kBorder*2))
                                        thePoint.x  = rightSide - (kBorder*2);

                                //	Update the cue variables to new settings
                                fChannelCue->SetScaleX((rightSide - thePoint.x) / origWidth);
                                fChannelCue->SetDrawPointX(thePoint.x);
                        }

                        if (thePoint.y != savePt.y)
                        {
                                // Clip if needed
                                if ( thePoint.y < topSide + (kBorder*2))
                                        thePoint.y  = topSide + (kBorder*2);

                                //	Update the cue variables to new settings
                                fChannelCue->SetScaleY((thePoint.y - topSide) / origHeight);
                        }

                        //	Normalize registration point
                        oldReg.x = fChannelCue->GetDrawArea().Width()  * regPercX;
                        oldReg.y = fChannelCue->GetDrawArea().Height() * regPercY;
                        fChannelCue->SetRegistration(oldReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;

                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x > rightSide - (kBorder*2))
                        thePoint.x  = rightSide - (kBorder*2);

                if ( thePoint.y < topSide + (kBorder*2))
                        thePoint.y  = topSide + (kBorder*2);
        }
   }


   //---------------------------------------------------------------------
   //	ScaleLeft
   //---------------------------------------------------------------------
   //
   //	Scale the left side of the view.  Constrain to the width of
   //	the bitmap.  Also, don't allow right to be less than left
   //

   void TStageCue::ScaleLeft(BPoint thePoint)
   {
        // Setup variables
        BRect  cropArea;
        BPoint oldReg, drawPt, scalePt;

        cropArea = fChannelCue->GetDrawArea();
        oldReg   = fChannelCue->CuePosition()->Registration();
        drawPt	 = fChannelCue->CuePosition()->Corner1();
        scalePt  = fChannelCue->GetScale();

        int32 rightSide = cropArea.right;

        //	Get registration percentage
        float	regPercX = oldReg.x / cropArea.Width();

        //	Save bitmap original size
        const BRect origRect  = fChannelCue->GetOriginalArea();
        const float origWidth = origRect.Width();

        // Resize the cue to the left while the mouse button is down
        BPoint	savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.x != savePt.x)
                {
                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Clip if needed
                        if ( thePoint.x > rightSide - (kBorder*2))
                                thePoint.x  = rightSide - (kBorder*2);

                        //	Update the cue varibles to new settings
                        fChannelCue->SetScaleX((rightSide - thePoint.x) / origWidth);
                        fChannelCue->SetDrawPointX(thePoint.x);

                        //	Normalize registration point
                        BPoint newReg = oldReg;
                        newReg.x = fChannelCue->GetDrawArea().Width() * regPercX;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if ( thePoint.x > rightSide - (kBorder*2))
                        thePoint.x  = rightSide - (kBorder*2);
        }
   }


 #pragma mark -
 #pragma mark === Cropping Routines ===


   //---------------------------------------------------------------------
   //	Crop
   //---------------------------------------------------------------------
   //
   //	Crop the picture
   //

   void TStageCue::Crop(BPoint thePoint)
   {
        short zoneID = GetResizeZoneID(thePoint);

        switch(zoneID)
        {
                case kTopLeftResize:
                        CropTopLeft(thePoint);
                        break;

                case kTopMiddleResize:
                        CropTop(thePoint);
                        break;

                case kTopRightResize:
                        CropTopRight(thePoint);
                        break;

                case kRightMiddleResize:
                        CropRight(thePoint);
                        break;

                case kBottomRightResize:
                        CropBottomRight(thePoint);
                        break;

                case kBottomMiddleResize:
                        CropBottom(thePoint);
                        break;

                case kBottomLeftResize:
                        CropBottomLeft(thePoint);
                        break;

                case kLeftMiddleResize:
                        CropLeft(thePoint);
                        break;

                // Bad ID value
                default:
                        break;
        }

        //	Update stage
        fStage->Looper()->Lock();
        //fStage->StageDraw(fChannelCue->GetCroppedArea(), GetCurrentTime());
        fStage->Invalidate();
        fStage->Looper()->Unlock();

        //	Update bounds polygon
        fChannelCue->GetTransformedDrawPolygon(fBoundsPolygon);

        // Update resize zones
        SetResizeZones();
   }


   //---------------------------------------------------------------------
   //	CropTopLeft
   //---------------------------------------------------------------------
   //
   //	Resize the top left of the view.
   //

   void TStageCue::CropTopLeft(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap             = fChannelCue->GetBitmap();
        const BRect	 bmapBounds             = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        //	Calculate constraint parameters.
        bool	constrainX;

        BRect constraintRect;
        constraintRect.left   = bmapBounds.left;
        constraintRect.top    = bmapBounds.top;
        constraintRect.right  = constraintRect.left + croppedAreaPtr->right;
        constraintRect.bottom = constraintRect.top + croppedAreaPtr->bottom;

        float constraintVal;

        if ( constraintRect.Width() >= constraintRect.Height() )
        {
                constrainX        = true;
                constraintVal = constraintRect.Height();
        }
        else
        {
                constrainX    = false;
                constraintVal = constraintRect.Width();
        }

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        float resizeX = thePoint.x - savePt.x;
                        float resizeY = thePoint.y - savePt.y;

                        //	Check for shift key constraint.  If shift is down, maintain aspect
                        //	ratio of item.	Use X span as guide to adjust Y span
                        if (IsShiftKeyDown())
                        {
                                if (thePoint != savePt)
                                {
                                        //	Erase old selection rect
                                        DrawSelectionRect(false);

                                        // Do resize
                                        if (constrainX)
                                        {
                                                croppedAreaPtr->left += resizeX;
                                                croppedAreaPtr->top  = croppedAreaPtr->bottom - croppedAreaPtr->Width();

                                                // Clip if needed
                                                croppedAreaPtr->top  = max(croppedAreaPtr->top, bmapBounds.top);
                                                croppedAreaPtr->top  = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));
                                                croppedAreaPtr->left = max(croppedAreaPtr->left, croppedAreaPtr->right - constraintVal);
                                                croppedAreaPtr->left = min(croppedAreaPtr->left, bmapBounds.right - (kBorder*2));
                                        }
                                        else
                                        {
                                                croppedAreaPtr->top  += resizeY;
                                                croppedAreaPtr->left = croppedAreaPtr->right - croppedAreaPtr->Height();

                                                // Clip if needed
                                                croppedAreaPtr->left = max(croppedAreaPtr->left, bmapBounds.left);
                                                croppedAreaPtr->left = min(croppedAreaPtr->left, bmapBounds.right - (kBorder*2));
                                                croppedAreaPtr->top  = max(croppedAreaPtr->top, croppedAreaPtr->bottom - constraintVal);
                                                croppedAreaPtr->top  = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));
                                        }

                                        // Adjust resize zones
                                        SetResizeZones();

                                        //	Draw new selection rect
                                        DrawSelectionRect(false);
                                }
                        }
                        else
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Resize top
                                if (thePoint.y != savePt.y)
                                {
                                        // Don't allow resize past bitmap height
                                        if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                                        {
                                                // Do resize
                                                croppedAreaPtr->top += resizeY;

                                                // Clip if needed
                                                croppedAreaPtr->top = max(croppedAreaPtr->top, bmapBounds.top);
                                                croppedAreaPtr->top = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));
                                        }
                                }

                                // Resize left
                                if (thePoint.x != savePt.x)
                                {
                                        // Don't allow resize past bitmap width
                                        if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                                        {
                                                // Do resize
                                                croppedAreaPtr->left += resizeX;

                                                // Clip if needed
                                                croppedAreaPtr->left = max(croppedAreaPtr->left, bmapBounds.left);
                                                croppedAreaPtr->left = min(croppedAreaPtr->left, bmapBounds.right - (kBorder*2));
                                        }
                                }

                                //	Normalize registration point
                                BPoint newReg;
                                newReg.x                = croppedAreaPtr->Width() / 2;
                                newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                                newReg.y                = croppedAreaPtr->Height() / 2;
                                newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Draw new selection rect
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if (IsShiftKeyDown())
                {
                        const BRect drawArea = fChannelCue->GetDrawArea();

                        if (constrainX)
                        {
                                thePoint.y = max(thePoint.y, bmapBounds.top + drawPt.y);
                                thePoint.y = min(thePoint.y, drawArea.bottom - (kBorder*2));

                                thePoint.x = max(thePoint.x, drawArea.right - constraintVal);
                                thePoint.x = min(thePoint.x, drawArea.right - (kBorder*2));
                        }
                        else
                        {
                                thePoint.y = max(thePoint.y, drawArea.bottom - constraintVal);
                                thePoint.y = min(thePoint.y, drawArea.bottom - (kBorder*2));

                                thePoint.x = max(thePoint.x, bmapBounds.left + drawPt.x);
                                thePoint.x = min(thePoint.x, drawArea.right - (kBorder*2));
                        }
                }
                else
                {
                        thePoint.x = max(thePoint.x, bmapBounds.left + drawPt.x);
                        thePoint.y = max(thePoint.y, bmapBounds.top + drawPt.y);

                        thePoint.x = min(thePoint.x, fChannelCue->GetDrawArea().right - (kBorder*2));
                        thePoint.y = min(thePoint.y, fChannelCue->GetDrawArea().bottom - (kBorder*2));
                }
        }
   }

   //---------------------------------------------------------------------
   //	CropTop
   //---------------------------------------------------------------------
   //
   //	Crop the top side of the view.  Constrain to the height of
   //	the bitmap.  Also, don't allow top to be less than bottom
   //

   void TStageCue::CropTop( BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.y != savePt.y)
                {
                        float resize = thePoint.y - savePt.y;

                        // Don't allow resize past bitmap width
                        if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Do resize
                                croppedAreaPtr->top += resize;

                                // Clip if needed
                                croppedAreaPtr->top = max(croppedAreaPtr->top, bmapBounds.top);
                                croppedAreaPtr->top = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));

                                //	Normalize registration point
                                BPoint newReg   = fChannelCue->CuePosition()->Registration();
                                newReg.y                = croppedAreaPtr->Height() / 2;
                                newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Draw new selection rect
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.y = max(thePoint.y, bmapBounds.top + drawPt.y);
                thePoint.y = min(thePoint.y, fChannelCue->GetDrawArea().bottom - (kBorder*2));
        }
   }


   //---------------------------------------------------------------------
   //	CropTopRight
   //---------------------------------------------------------------------
   //
   //	CropTopRight the top and right side of the view.
   //

   void TStageCue::CropTopRight(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        //	Calculate constraint parameters.  Use cropped area left and bottom side
        //	and bitmap bounds right and top side
        BRect constrainRect;

        constrainRect.right  = bmapBounds.right;
        constrainRect.top        = bmapBounds.top;
        constrainRect.left   = constrainRect.right - croppedAreaPtr->Width();
        constrainRect.bottom = constrainRect.top + croppedAreaPtr->Height();

        int32 constrainPix;
        if ( constrainRect.Width() > constrainRect.Height() )
                constrainPix = constrainRect.Width() - constrainRect.Height();
        else
                constrainPix = constrainRect.Height() - constrainRect.Width();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        float resizeX = thePoint.x - savePt.x;
                        float resizeY = thePoint.y - savePt.y;

                        //	Check for shift key constraint.  If shift is down, maintain aspect
                        //	ratio of item.	Use X span as guide to adjust Y span
                        if (IsShiftKeyDown())
                        {
                                if (thePoint.x != savePt.x)
                                {
                                        //	Erase old selection rect
                                        DrawSelectionRect(false);

                                        // Do resize
                                        croppedAreaPtr->right += resizeX;
                                        croppedAreaPtr->top   = croppedAreaPtr->bottom - croppedAreaPtr->Width();

                                        // Clip if needed
                                        croppedAreaPtr->top	  = max(croppedAreaPtr->top, bmapBounds.top);
                                        croppedAreaPtr->top   = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));
                                        croppedAreaPtr->right = min( croppedAreaPtr->right, bmapBounds.Width());
                                        croppedAreaPtr->right = max( croppedAreaPtr->right, croppedAreaPtr->left + (kBorder*2));

                                        // Adjust resize zones
                                        SetResizeZones();

                                        //	Draw new selection rect
                                        DrawSelectionRect(false);
                                }
                        }
                        else
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Resize top
                                if (thePoint.y != savePt.y)
                                {
                                        // Don't allow resize past bitmap height
                                        if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                                        {
                                                // Do resize
                                                croppedAreaPtr->top += resizeY;

                                                // Clip if needed
                                                croppedAreaPtr->top = max(croppedAreaPtr->top, bmapBounds.top);
                                                croppedAreaPtr->top = min(croppedAreaPtr->top, bmapBounds.bottom - (kBorder*2));
                                        }
                                }

                                // Resize right
                                if (thePoint.x != savePt.x)
                                {
                                        // Don't allow resize past bitmap width
                                        if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                                        {
                                                // Do resize
                                                croppedAreaPtr->right += resizeX;

                                                // Clip if needed
                                                croppedAreaPtr->right = min( croppedAreaPtr->right, bmapBounds.Width());
                                                croppedAreaPtr->right = max( croppedAreaPtr->right, croppedAreaPtr->left + (kBorder*2));
                                        }
                                }

                                //	Normalize registration point
                                BPoint newReg;
                                newReg.x                = croppedAreaPtr->Width() / 2;
                                newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                                newReg.y                = croppedAreaPtr->Height() / 2;
                                newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Draw new selection rect
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                if (IsShiftKeyDown())
                {
                        thePoint.y = max(thePoint.y, bmapBounds.top + drawPt.y);
                        thePoint.y = min(thePoint.y, fChannelCue->GetDrawArea().bottom - (kBorder*2));

                        thePoint.x = min(thePoint.x, (bmapBounds.right + drawPt.x) - constrainPix );
                        thePoint.x = max(thePoint.x, fChannelCue->GetDrawArea().left+(kBorder*2));
                }
                else
                {
                        thePoint.x = min(thePoint.x, bmapBounds.right + drawPt.x);
                        thePoint.y = max(thePoint.y, bmapBounds.top + drawPt.y);

                        thePoint.x = max(thePoint.x, fChannelCue->GetDrawArea().left+(kBorder*2));
                        thePoint.y = min(thePoint.y, fChannelCue->GetDrawArea().bottom - (kBorder*2));
                }
        }
   }

   //---------------------------------------------------------------------
   //	CropRight
   //---------------------------------------------------------------------
   //
   //	Resize the right side of the view.  Constrain to the width of
   //	the bitmap.  Also, don't allow right to be less than left
   //

   void TStageCue::CropRight(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap      = fChannelCue->GetBitmap();
        const BRect	 bmapBounds      = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.x != savePt.x)
                {
                        float resize = thePoint.x - savePt.x;

                        // Don't allow resize past bitmap width
                        if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Do resize
                                croppedAreaPtr->right += resize;

                                // Clip if needed
                                croppedAreaPtr->right = min( croppedAreaPtr->right, bmapBounds.Width());
                                croppedAreaPtr->right = max( croppedAreaPtr->right, croppedAreaPtr->left + (kBorder*2));

                                //	Normalize registration point
                                BPoint newReg   = fChannelCue->CuePosition()->Registration();
                                newReg.x                = croppedAreaPtr->Width() / 2;
                                newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Draw new selection rect
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.x = min(thePoint.x, bmapBounds.right + drawPt.x);
                thePoint.x = max(thePoint.x, fChannelCue->GetDrawArea().left+(kBorder*2));
        }
   }

   //---------------------------------------------------------------------
   //	CropBottomRight
   //---------------------------------------------------------------------
   //
   //	Resize the bottom and right side of the view.
   //

   void TStageCue::CropBottomRight(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        float resizeX = thePoint.x - savePt.x;
                        float resizeY = thePoint.y - savePt.y;

                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Resize bottom
                        if (thePoint.y != savePt.y)
                        {
                                // Don't allow resize past bitmap width
                                if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                                {
                                        // Do resize
                                        croppedAreaPtr->bottom += resizeY;

                                        // Clip if needed
                                        croppedAreaPtr->bottom = min(croppedAreaPtr->bottom, bmapBounds.bottom);
                                        croppedAreaPtr->bottom = max(croppedAreaPtr->bottom, croppedAreaPtr->top + (kBorder*2));
                                }
                        }

                        // Resize right
                        if (thePoint.x != savePt.x)
                        {
                                // Don't allow resize past bitmap width
                                if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                                {
                                        // Do resize
                                        croppedAreaPtr->right += resizeX;

                                        // Clip if needed
                                        croppedAreaPtr->right = min( croppedAreaPtr->right, bmapBounds.Width());
                                        croppedAreaPtr->right = max( croppedAreaPtr->right, croppedAreaPtr->left + (kBorder*2));
                                }
                        }

                        //	Normalize registration point
                        BPoint newReg;
                        newReg.x                = croppedAreaPtr->Width() / 2;
                        newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                        newReg.y                = croppedAreaPtr->Height() / 2;
                        newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.x = min(thePoint.x, bmapBounds.right + drawPt.x);
                thePoint.y = min(thePoint.y, bmapBounds.bottom + drawPt.y);

                thePoint.x = max(thePoint.x, fChannelCue->GetDrawArea().left + (kBorder*2));
                thePoint.y = max(thePoint.y, fChannelCue->GetDrawArea().top + (kBorder*2));
        }
   }


   //---------------------------------------------------------------------
   //	CropBottom
   //---------------------------------------------------------------------
   //
   //	Resize the bottom side of the view.  Constrain to the height of
   //	the bitmap.
   //

   void TStageCue::CropBottom(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.y != savePt.y)
                {
                        float resize = thePoint.y - savePt.y;

                        // Don't allow resize past bitmap width
                        if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Do resize
                                croppedAreaPtr->bottom += resize;

                                // Clip if needed
                                croppedAreaPtr->bottom = min(croppedAreaPtr->bottom, bmapBounds.bottom);
                                croppedAreaPtr->bottom = max(croppedAreaPtr->bottom, croppedAreaPtr->top + (kBorder*2));

                                BPoint newReg   = fChannelCue->CuePosition()->Registration();
                                newReg.y                = croppedAreaPtr->Height() / 2;
                                newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Draw new selection rect
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.y = min(thePoint.y, bmapBounds.bottom + drawPt.y);

                thePoint.y = max(thePoint.y, fChannelCue->GetDrawArea().top + (kBorder*2));
        }
   }

   //---------------------------------------------------------------------
   //	CropBottomLeft
   //---------------------------------------------------------------------
   //
   //	Resize the bottom and left side of the view.
   //

   void TStageCue::CropBottomLeft(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr = fChannelCue->GetCroppedAreaPtr();
        drawPt = fChannelCue->CuePosition()->Corner1();

        // Resize while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint != savePt)
                {
                        float resizeX = thePoint.x - savePt.x;
                        float resizeY = thePoint.y - savePt.y;

                        //	Erase old selection rect
                        DrawSelectionRect(false);

                        // Resize bottom
                        if (thePoint.y != savePt.y)
                        {
                                // Don't allow resize past bitmap width
                                if ( croppedAreaPtr->Height() <= bmapBounds.Height() )
                                {
                                        // Do resize
                                        croppedAreaPtr->bottom += resizeY;

                                        // Clip if needed
                                        croppedAreaPtr->bottom = min(croppedAreaPtr->bottom, bmapBounds.bottom);
                                        croppedAreaPtr->bottom = max(croppedAreaPtr->bottom, croppedAreaPtr->top + (kBorder*2));
                                }
                        }

                        // Resize left
                        if (thePoint.x != savePt.x)
                        {
                                // Don't allow resize past bitmap width
                                if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                                {
                                        // Do resize
                                        croppedAreaPtr->left += resizeX;

                                        // Clip if needed
                                        croppedAreaPtr->left = max(croppedAreaPtr->left, bmapBounds.left);
                                        croppedAreaPtr->left = min(croppedAreaPtr->left, bmapBounds.right - (kBorder*2));
                                }
                        }

                        //	Normalize registration point
                        BPoint newReg;
                        newReg.x                = croppedAreaPtr->Width() / 2;
                        newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                        newReg.y                = croppedAreaPtr->Height() / 2;
                        newReg.y                += croppedAreaPtr->top - fChannelCue->GetOriginalArea().top;
                        fChannelCue->SetRegistration(newReg);

                        // Adjust resize zones
                        SetResizeZones();

                        //	Draw new selection rect
                        DrawSelectionRect(false);

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.x = max(thePoint.x, bmapBounds.left + drawPt.x);
                thePoint.y = min(thePoint.y, bmapBounds.bottom + drawPt.y);

                thePoint.x = min(thePoint.x, fChannelCue->GetDrawArea().right - (kBorder*2));
                thePoint.y = max(thePoint.y, fChannelCue->GetDrawArea().top + (kBorder*2));
        }
   }


   //---------------------------------------------------------------------
   //	CropLeft
   //---------------------------------------------------------------------
   //
   //	Resize the left side of the view.  Constrain to the width of
   //	the bitmap.  Also, don't allow right to be less than left
   //

   void TStageCue::CropLeft(BPoint thePoint)
   {
        // Setup variables
        BBitmap          *theBitmap = fChannelCue->GetBitmap();
        const BRect	 bmapBounds = theBitmap->Bounds();

        BRect	*croppedAreaPtr;
        BPoint  drawPt;

        croppedAreaPtr  = fChannelCue->GetCroppedAreaPtr();
        drawPt                  = fChannelCue->CuePosition()->Corner1();



        // Resize the cue to the right while the mouse button is down
        BPoint  savePt;
        uint32	buttons = 0;

        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        while (buttons)
        {
                if (thePoint.x != savePt.x)
                {
                        float resize = thePoint.x - savePt.x;

                        // Don't allow resize past bitmap width
                        if ( croppedAreaPtr->Width() <= bmapBounds.Width() )
                        {
                                //	Erase old selection rect
                                DrawSelectionRect(false);

                                // Do resize
                                croppedAreaPtr->left += resize;

                                // Clip if needed
                                croppedAreaPtr->left = max(croppedAreaPtr->left, bmapBounds.left);
                                croppedAreaPtr->left = min(croppedAreaPtr->left, bmapBounds.right - (kBorder*2));

                                //	Normalize registration point
                                BPoint newReg   = fChannelCue->CuePosition()->Registration();
                                newReg.x                = croppedAreaPtr->Width() / 2;
                                newReg.x                += croppedAreaPtr->left - fChannelCue->GetOriginalArea().left;
                                fChannelCue->SetRegistration(newReg);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Update stage
                                DrawSelectionRect(false);
                        }

                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);

                // Clip the point back within the proper bounds
                thePoint.x = max(thePoint.x, bmapBounds.left + drawPt.x);

                thePoint.x = min(thePoint.x, fChannelCue->GetDrawArea().right - (kBorder*2));
        }
   }

 #pragma mark -
 #pragma mark === Rotation Routines ===


   //---------------------------------------------------------------------
   //	Rotate
   //---------------------------------------------------------------------
   //
   //	Rotate the picture
   //

 #define ACCEL_FACTOR ((double)1.3)

   void TStageCue::Rotate(BPoint thePoint)
   {
        // Rotate the cue while the mouse button is down
        BPoint  savePt = thePoint;
        float	rotation, degrees, saveDegrees, diffDegrees;
        double	x21, y21, x23, y23, theta;
        uint32	buttons  = 0;

        //	Get pointer to stage
        BView *offView = fStage->GetOffscreenView();

        //	Get cues visual center point
        const BPoint centerPt = fChannelCue->GetRegistrationOffset();

        //	Get mouse position
        fStage->GetMouse(&thePoint, &buttons, true);
        savePt = thePoint;

        //	Save our start point
        const BPoint startPt = thePoint;

        //	Calculate starting degress
        x21 = startPt.x  - centerPt.x;
        y21 = startPt.y  - centerPt.y;
        x23 = thePoint.x - centerPt.x;
        y23 = thePoint.y - centerPt.y;

        theta           = acos((x21*x23 + y21*y23) / sqrt((x21*x21 + y21*y21) * (x23*x23 + y23*y23)));
        saveDegrees = theta * (180 / M_PI);

        //	Grab a pointer to the cue's rotate dialog
        TRotateDialog *theDialog = fChannelCue->GetRotateDialog();

        while (buttons)
        {
                //	Rotate
                if (thePoint != savePt)
                {
                        //	Get current rotation degrees
                        rotation = fChannelCue->GetRotation();

                        //	Calculate new angle
                        x21 = startPt.x  - centerPt.x;
                        y21 = startPt.y  - centerPt.y;
                        x23 = thePoint.x - centerPt.x;
                        y23 = thePoint.y - centerPt.y;

                        theta = acos((x21*x23 + y21*y23) / sqrt((x21*x21 + y21*y21) * (x23*x23 + y23*y23)));

                        //	Convert from radian to degrees
                        degrees = theta * (180 / M_PI);

                        if (degrees != saveDegrees)
                        {
                                if ( saveDegrees > degrees)
                                        diffDegrees = saveDegrees - degrees;
                                else
                                        diffDegrees = degrees - saveDegrees;

                                //	Handle X-axis.  Rotation direction is determined by position of mouse
                                //	above or below axis point

                                //	Rotate clockwise
                                if (thePoint.y < centerPt.y)
                                {
                                        if (thePoint.x > savePt.x)
                                        {
                                                rotation += diffDegrees;
                                                if (rotation > 359)
                                                        rotation = 0;
                                        }
                                        else
                                        {
                                                rotation -= diffDegrees;
                                                if (rotation < 0)
                                                        rotation = 359;
                                        }
                                }
                                //	Counter-clockwise
                                else
                                {
                                        if (thePoint.x > savePt.x)
                                        {
                                                rotation -= diffDegrees;
                                                if (rotation < 0)
                                                        rotation = 359;
                                        }
                                        else
                                        {
                                                rotation += diffDegrees;

                                                if (rotation > 359)
                                                        rotation = 0;
                                        }
                                }

                                //	Erase old selction rect
                                DrawSelectionRect(false);

                                //	Update degrees of rotation
                                fChannelCue->SetRotation(rotation);

                                // Adjust resize zones
                                SetResizeZones();

                                //	Update stage
                                DrawSelectionRect(false);

                                //	Save for next compare
                                saveDegrees = degrees;

                                //	Inform dialog box if open
                                if (theDialog)
                                {
                                        BMessage *message = new BMessage(ROTATE_TOOL_MSG);
                                        message->AddInt32("RotateDegrees", (int32)rotation);
                                        theDialog->Lock();
                                        theDialog->MessageReceived(message);
                                        theDialog->Unlock();
                                }
                        }
                        // Save mouse location for next compare
                        savePt = thePoint;
                }

                fStage->GetMouse(&thePoint, &buttons, true);
        }

        //	Draw updated bitmap
        fStage->Looper()->Lock();
        //fStage->StageDraw(fChannelCue->GetCroppedArea(), GetCurrentTime());
        fStage->Invalidate();
        fStage->Looper()->Unlock();

        //	Update bounds polygon
        fChannelCue->GetTransformedDrawPolygon(fBoundsPolygon);
   }

 #pragma mark -
 #pragma mark === Mirroring Routines ===


   //---------------------------------------------------------------------
   //	Mirror
   //---------------------------------------------------------------------
   //
   //	Mirror the picture
   //

   void TStageCue::Mirror(BPoint thePoint)
   {
        short zoneID = GetResizeZoneID(thePoint);

        switch(zoneID)
        {
                case kTopMiddleResize:
                case kBottomMiddleResize:
                        MirrorVertical(thePoint);
                        break;

                case kRightMiddleResize:
                case kLeftMiddleResize:
                        MirrorHorizontal(thePoint);
                        break;

                // Bad ID value
                default:
                        break;
        }

        // Update resize zones
        SetResizeZones();
   }

   //---------------------------------------------------------------------
   //	MirrorHorizontal
   //---------------------------------------------------------------------
   //
   //	Mirror the picture horizontally
   //

   void TStageCue::MirrorHorizontal(BPoint thePoint)
   {
        BRect            *croppedAreaPtr        = fChannelCue->GetCroppedAreaPtr();

        //      Get current miror value
        bool theVal = fChannelCue->GetMirrorH();

        //      Set new mirror value
        if (theVal)
                fChannelCue->SetMirrorH(false);
        else
                fChannelCue->SetMirrorH(true);

        // Adjust resize zones
        SetResizeZones();

        //	Update fChannelCue's internal bitmap
        fChannelCue->Unmute();
        fChannelCue->RenderData(GetCurrentTime(), fChannelCue->GetDrawArea());
        fChannelCue->Mute();

        // Force a stage draw
        fStage->Looper()->Lock();
        fStage->StageDraw(*croppedAreaPtr, GetCurrentTime());
        fStage->Invalidate();
        fStage->Looper()->Unlock();
   }

   //---------------------------------------------------------------------
   //	MirrorVertical
   //---------------------------------------------------------------------
   //
   //	Mirror the picture vertically
   //

   void TStageCue::MirrorVertical(BPoint thePoint)
   {
        BRect            *croppedAreaPtr        = fChannelCue->GetCroppedAreaPtr();

        //      Get current miror value
        bool theVal = fChannelCue->GetMirrorV();

        //      Set new mirror value
        if (theVal)
                fChannelCue->SetMirrorV(false);
        else
                fChannelCue->SetMirrorV(true);

        // Adjust resize zones
        SetResizeZones();

        //	Update fChannelCue's internal bitmap
        fChannelCue->Unmute();
        fChannelCue->RenderData(GetCurrentTime(), fChannelCue->GetDrawArea());
        fChannelCue->Mute();

        // Force a stage draw
        fStage->Looper()->Lock();
        fStage->StageDraw(*croppedAreaPtr, GetCurrentTime());
        fStage->Invalidate();
        fStage->Looper()->Unlock();
   }
 */


#pragma mark -
#pragma mark === Cursor Routines ===

//---------------------------------------------------------------------
//	SetMoveCursor
//---------------------------------------------------------------------
//
//	Set the proper move cursor based on cursor position
//

void TStageCue::SetMoveCursor(BPoint thePoint)
{
	be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
}


//---------------------------------------------------------------------
//	SetRotateCursor
//---------------------------------------------------------------------
//
//	Set the proper rotate cursor based on cursor position
//

void TStageCue::SetRotateCursor(BPoint thePoint)
{
	be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
}


//---------------------------------------------------------------------
//	SetCropCursor
//---------------------------------------------------------------------
//
//	Set the proper crop cursor based on cursor position
//

void TStageCue::SetCropCursor(BPoint thePoint)
{

	//	Do we need to set a special size cursor?
	if ( PointInResizeZones(thePoint) ) {
		switch( GetResizeZoneID(thePoint) )
		{
		case kTopLeftResize:
		case kBottomRightResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fResizeDiagRightCursor );
			break;

		case kTopRightResize:
		case kBottomLeftResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fResizeDiagLeftCursor );
			break;

		case kTopMiddleResize:
		case kBottomMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeVertCursor );
			break;

		case kLeftMiddleResize:
		case kRightMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeHorzCursor );
			break;

		default:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
			break;
		}
	} else   {
		be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
	}
}


//---------------------------------------------------------------------
//	SetScaleCursor
//---------------------------------------------------------------------
//
//	Set the proper scale cursor based on cursor position
//

void TStageCue::SetScaleCursor(BPoint thePoint)
{
	//	Do we need to set a special size cursor?
	if ( PointInResizeZones(thePoint) ) {
		switch( GetResizeZoneID(thePoint) )
		{
		case kTopLeftResize:
		case kBottomRightResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fResizeDiagRightCursor );
			break;

		case kTopRightResize:
		case kBottomLeftResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fResizeDiagLeftCursor );
			break;

		case kTopMiddleResize:
		case kBottomMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeVertCursor );
			break;

		case kLeftMiddleResize:
		case kRightMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeHorzCursor );
			break;

		default:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
			break;
		}
	} else   {
		be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
	}
}


//---------------------------------------------------------------------
//	SetShearCursor
//---------------------------------------------------------------------
//
//	Set the proper shear cursor based on cursor position
//

void TStageCue::SetShearCursor(BPoint thePoint)
{

	//	Do we need to set a special size cursor?
	if ( PointInResizeZones(thePoint) ) {
		switch( GetResizeZoneID(thePoint) )
		{
		case kTopLeftResize:
		case kTopRightResize:
		case kBottomRightResize:
		case kBottomLeftResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeHorzCursor );
			break;

		default:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
			break;
		}
	} else   {
		be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
	}
}


//---------------------------------------------------------------------
//	SetSkewCursor
//---------------------------------------------------------------------
//
//	Set the proper skew cursor based on cursor position
//

void TStageCue::SetSkewCursor(BPoint thePoint)
{
	be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
}


//---------------------------------------------------------------------
//	SetPerspectiveCursor
//---------------------------------------------------------------------
//
//	Set the proper perspective cursor based on cursor position
//

void TStageCue::SetPerspectiveCursor(BPoint thePoint)
{
	be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
}


//---------------------------------------------------------------------
//	SetMirrorCursor
//---------------------------------------------------------------------
//
//	Set the proper mirror cursor based on cursor position
//

void TStageCue::SetMirrorCursor(BPoint thePoint)
{
	//	Do we need to set a special size cursor?
	if ( PointInResizeZones(thePoint) ) {
		switch( GetResizeZoneID(thePoint) )
		{
		case kTopMiddleResize:
		case kBottomMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeVertCursor );
			break;

		case kLeftMiddleResize:
		case kRightMiddleResize:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fSizeHorzCursor );
			break;

		default:
			be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
			break;
		}
	} else   {
		be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->fMoveCursor );
	}
}


#pragma mark -
#pragma mark === Member Variable Routines ===

//---------------------------------------------------------------------
//	SetChannelCue
//---------------------------------------------------------------------
//
//	Set fChannelCue
//

void TStageCue::SetChannelCue(TVisualCue *theCue)
{
	fChannelCue = theCue;
}


#pragma mark -
#pragma mark === Menu Routines ===

//---------------------------------------------------------------------
//	OpenStageCueMenu
//---------------------------------------------------------------------
//
//	Open stage cue pop up menu
//

void TStageCue::OpenStageCueMenu(BPoint menuPt)
{
	BMenuItem       *selected;

	// Create the menu and mark the current transition
	TStageCueMenu *theMenu = new TStageCueMenu(this->fChannelCue);

	if (theMenu) {
		// Set menu location point
		fStage->ConvertToScreen(&menuPt);
		selected = theMenu->Go(menuPt);

		// Check and see if we have a menu message
		//int32 drawingMode;
		if (selected) {
			switch( selected->Message()->what)
			{
			//case OPACITY_MSG:
			//	break;

			default:
				break;
			}

			//if ( selected->Message()->FindInt32("DrawingMode", &drawingMode) == B_OK)
			//{
			// Lock offscreen
			//fOffscreenView->Looper()->Lock();

			// Save drawing mode
			//fChannelCue->SetDrawingMode( (drawing_mode)drawingMode );

			// Only redraw if mode has changed
			//if ( fOffscreenView->DrawingMode() != fChannelCue->DrawingMode() )
			//{
			//	Draw the bitmap into the offscreen using the new mode.
			//	fOffscreenView->SetDrawingMode(fChannelCue->DrawingMode());
			//	fOffscreenView->DrawBitmap(fBitmap);
			//	fOffscreenView->Sync();
			//	Invalidate();
			//}
			//fOffscreenView->Looper()->Unlock();
			//}
		}

		// Clean up
		delete theMenu;
	}
}


//---------------------------------------------------------------------
//	OpenCurrentToolDialog
//---------------------------------------------------------------------
//
//	Open dialog based on current tool
//

void TStageCue::OpenCurrentToolDialog()
{
	switch(fStage->GetToolMode())
	{
	case kMoveMode:
		break;

	case kRotateMode:
	{
		BMessage *theMessage = new BMessage(STAGE_CUE_ROTATE_MSG);
		fChannelCue->MessageReceived(theMessage);
		delete theMessage;
	}
	break;

	case kCropMode:
		break;

	case kScaleMode:
	{
		BMessage *theMessage = new BMessage(STAGE_CUE_SCALE_MSG);
		fChannelCue->MessageReceived(theMessage);
		delete theMessage;
	}
	break;

	case kShearMode:
		break;

	case kSkewMode:
		break;

	case kPerspectiveMode:
		break;

	case kMirrorMode:
		break;

	default:
		break;
	}
}

