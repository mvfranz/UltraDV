//---------------------------------------------------------------------
//
//	File:	TCueEffectView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.12.98
//
//	Desc:	Visual appearence class of a TCueEffect.  This is not based
//			on a BView.  We are using our own mechanism.
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"

#include "TCueEffectView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueEffectView::TCueEffectView(BRect bounds, TCueView *theCue, TCueEffect *theEffect)
{
	//	Save bounds
	fBounds = bounds;

	// Save cue pointer
	fCue = theCue;

	// Save effect pointer
	fEffect = theEffect;

	//	Do default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueEffectView::~TCueEffectView()
{
	Deselect();

	//	Free effect
	delete fEffect;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TCueEffectView::Init()
{
	// We start life unselected
	fIsSelected = false;

	//	Set up resize zones
	UpdateResizeZones();
}




#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueEffectView::MouseDown(BPoint where, bool doubleClick)
{
	// Do nothing if we are playing
	if ( IsPlaying() )
		return;

	//	Double click?
	if (doubleClick)
	{
		//	Display effect dialog
	}
	else
	{
		//	Deselect everyone
		if (fIsSelected == false)
			fCue->DeselectAllEffects();

		//	Select
		Select();
	}

	/*
	// Make sure point falls within our bounds or resize zones
	if ( fChannelCue->CuePosition()->Contains(where) || PointInResizeZones(where) )
	{

		uint32 	type;
		int32	count = 0;
		BMessage *message = fStage->Window()->CurrentMessage();
		if (B_OK == message->GetInfo("clicks", &type, &count) )
		{
			int32 clickCount = message->FindInt32("clicks", count-1);

			// Is this a double click?  If so, open editor
			if (clickCount == 2)
			{
				//if(fChannelCue->HasEditor())
				//	fChannelCue->OpenEditor();
			}
			//	Open transform dialog for current tool
			else if ( IsOptionKeyDown() )
			{
				OpenCurrentToolDialog();
			}
			// else, begin MouseDown handling of StageCue
			else
			{
				// Wait a short while before dragging
				snooze(60 * 1000);

				// Determine which button has been clicked
				uint32 	buttons = 0;
				BMessage *message = fStage->Window()->CurrentMessage();
				message->FindInt32("buttons", (long *)&buttons);

				switch(buttons)
				{
					case B_PRIMARY_MOUSE_BUTTON:
						{
							//	Find location of click.  If it is the main body of the picture, they are moving the picture.
							//	Otherwise, they are using a stage tool
							if ( PointInResizeZones(where) )
							{
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
							else
							{
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
	}*/
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueEffectView::MouseMoved( BPoint where)
{
	/*
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
	}*/
}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	Resized
//---------------------------------------------------------------------
//
//	Effect view was resized.  Make needed adjustments
//

void TCueEffectView::Resized()
{

}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueEffectView::Draw(BRect updateRect)
{
	//	Fill it
	fCue->SetHighColor(kLightGrey);
	fCue->FillRect(fBounds);

	//	Outline entire content area
	fCue->SetHighColor(kWhite);
	fCue->StrokeRect(fBounds);

	//	Draw name text
	BFont font(be_plain_font);
	font.SetSize(9.0);
	fCue->SetFont(&font);
	BPoint drawPt(fBounds.left, fBounds.bottom-1);
	fCue->SetHighColor(kBlack);
	fCue->SetLowColor(kSteelBlue);
	fCue->DrawString(fEffect->Name().c_str(), drawPt);
}


//---------------------------------------------------------------------
//	DrawSelectionRect
//---------------------------------------------------------------------
//
//

void TCueEffectView::DrawSelectionRect()
{
	//	Draw Selection in red
	if (fCue->LockLooper())
	{
		fCue->PushState();
		fCue->SetHighColor(kRed);
		fCue->StrokeRect(fBounds);
		fCue->PopState();
		fCue->UnlockLooper();
	}
}


//---------------------------------------------------------------------
//	InvalidateSelectionRect
//---------------------------------------------------------------------
//
//

void TCueEffectView::InvalidateSelectionRect()
{
	//	Return to normal framed state
	if (fCue->LockLooper())
	{
		BPoint startPt, endPt;

		fCue->PushState();

		fCue->SetHighColor(kWhite);
		startPt.Set(fBounds.left, fBounds.bottom-1);
		endPt.Set(fBounds.left, fBounds.top);
		fCue->StrokeLine(startPt, endPt);
		endPt.Set(fBounds.right, fBounds.top);
		fCue->StrokeLine(endPt);

		fCue->SetHighColor(kBlack);
		startPt.Set(fBounds.right, fBounds.top+1);
		endPt.Set(fBounds.right, fBounds.bottom);
		fCue->StrokeLine(startPt, endPt);
		endPt.Set(fBounds.left, fBounds.bottom);
		fCue->StrokeLine(endPt);

		fCue->PopState();
		fCue->UnlockLooper();
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

void TCueEffectView::DragPicture(BPoint thePoint)
{
	/*BRect 	outline;
	BPoint 	savePt;
	uint32	buttons = 0;

	fStage->GetMouse(&thePoint, &buttons, true);

	// Save point for future compare
	savePt = thePoint;

	while (buttons)
	{
		if (thePoint != savePt)
		{
			// Move the stage cue
			//

			// Offset mousePt
			float diffX = thePoint.x - savePt.x;
			float diffY = thePoint.y - savePt.y;

			//	Clean up old selection rect
			DrawSelectionRect(false);

			// Adjust cue's stage location
			outline = fChannelCue->CuePosition()->Enclosure();
			outline.left 	+= diffX;
			outline.top  	+= diffY;
			outline.right 	+= diffX;
			outline.bottom  += diffY;
			fChannelCue->CuePosition()->Outline(outline);

			// Update resize zones
			SetResizeZones();

			// Save mouse location for next compare
			savePt = thePoint;

			//	Draw new selection rect
			DrawSelectionRect(false);
		}
		fStage->GetMouse(&thePoint, &buttons, true);
	}

	//	Draw new position
	if ( fStage->LockLooper())
	{
		//fStage->StageDraw(fChannelCue->GetCroppedArea(), GetCurrentTime());
		fStage->StageDraw(fChannelCue->CuePosition()->Enclosure(), GetCurrentTime());
		fStage->Draw(fStage->Bounds());
		fStage->UnlockLooper();
	}*/
}

//---------------------------------------------------------------------
//	UpdateResizeZones
//---------------------------------------------------------------------
//
// 	The effect view has been resized.  Update the mouse over zones that
//	allow horizontal resizing
//

void TCueEffectView::UpdateResizeZones()
{
	//	Set up resize zones
	fLResizeZone.Set(fBounds.left, fBounds.top, fBounds.left+kResizeZoneWidth, fBounds.bottom);
	fRResizeZone.Set(fBounds.right-kResizeZoneWidth, fBounds.top, fBounds.right, fBounds.bottom );
}


#pragma mark -
#pragma mark === Selection Routines ===


//---------------------------------------------------------------------
//	Select
//---------------------------------------------------------------------
//
//	Select and start marching ants

void TCueEffectView::Select()
{
	if (fIsSelected == false)
	{
		//	Deselect all other cue effects

		//	We are now selected
		fIsSelected = true;

		//	Draw selection rect
		DrawSelectionRect();
	}
}

//---------------------------------------------------------------------
//	Deselect
//---------------------------------------------------------------------
//
//	Deselect and stop marching ants

void TCueEffectView::Deselect()
{
	if (fIsSelected)
	{
		fIsSelected = false;

		//	Invalidate selection rect
		InvalidateSelectionRect();
	}
}
