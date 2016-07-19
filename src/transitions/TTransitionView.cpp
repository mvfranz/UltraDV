//---------------------------------------------------------------------
//
//	File:	TTransitionView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.12.98
//
//	Desc:	Visual appearence class of a TTransitionEffect.  This is not based
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
#include "DebugUtils.h"

#include "MuseumApp.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TTimelineView.h"

#include "TTransitionView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTransitionView::TTransitionView(BRect bounds, TCueView *theCue, TCueEffect *theEffect) :
	TCueEffectView(bounds, theCue, theEffect)
{
	//	Do default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransitionView::~TTransitionView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TTransitionView::Init()
{
	//
	//	Calculate bounds based on effect times
	//

	const uint32 startTime = fEffect->StartTime();
	const uint32 duration  = fEffect->Duration();

	//	Convert to pixels
	int32 startPix  = TimeToPixels(startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	int32 lengthPix = TimeToPixels(duration,  GetCurrentTimeFormat(), GetCurrentResolution());

	//	In Effect is pegged to left side
	if ( ((TTransitionEffect *)fEffect)->TransitionIn() ) {
		fBounds.left    = startPix + 4; // Add width of tray edge
		fBounds.right   = fBounds.left + lengthPix;
	}
	//	Out Effect is pegged to right side
	else{
		fBounds.right   = fCue->Bounds().Width() - 4;   // Subtract width of tray edge
		fBounds.left    = fBounds.right - lengthPix;
	}

	TCueEffectView::Init();
}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//
//

void TTransitionView::Draw(BRect updateRect)
{
	BPoint startPt, endPt;

	//	Fill main bounds
	fCue->SetHighColor(kLightGrey);
	fCue->FillRect(fBounds);

	//	Draw side draggers.  If we are an "In" transition, draw right dragger
	//	If we are an "Out" transition, draw left dragger

	//	Transition In
	if ( ((TTransitionEffect *)fEffect)->TransitionIn()) {
		// Right side highlights
		fCue->SetHighColor(kSteelBlue);
		fCue->FillRect(fRResizeZone);

		fCue->SetHighColor(kDarkSteelBlue);
		startPt.Set(fRResizeZone.right, fRResizeZone.top);
		endPt.Set(fRResizeZone.right, fRResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);
		fCue->SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		fCue->StrokeLine(startPt, endPt);

		fCue->SetHighColor(kDarkSteelBlue);
		startPt.Set(fRResizeZone.left, fRResizeZone.top);
		endPt.Set(fRResizeZone.left, fRResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);
		fCue->SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		fCue->StrokeLine(startPt, endPt);

		// Right Shadow
		fCue->SetHighColor(kDarkGrey);
		startPt.Set(fRResizeZone.left-1, fLResizeZone.top);
		endPt.Set(fRResizeZone.left-1, fLResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);

		//	Draw icon.  It should be 16x16
		BBitmap *icon = dynamic_cast<TTransitionEffect *>(fEffect)->Icon();
		if (icon) {
			BPoint drawPt(fBounds.left+2, fBounds.top+2);
			fCue->DrawBitmap(icon, drawPt);
		}
	}
	//	Transition Out
	else{
		// Left side highlights
		fCue->SetHighColor(kSteelBlue);
		fCue->FillRect(fLResizeZone);

		fCue->SetHighColor(kDarkSteelBlue);
		startPt.Set(fLResizeZone.left, fLResizeZone.top);
		endPt.Set(fLResizeZone.left, fLResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);
		fCue->SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		fCue->StrokeLine(startPt, endPt);

		fCue->SetHighColor(kDarkSteelBlue);
		startPt.Set(fLResizeZone.right, fLResizeZone.top);
		endPt.Set(fLResizeZone.right, fLResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);
		fCue->SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		fCue->StrokeLine(startPt, endPt);

		// Left Shadow
		fCue->SetHighColor(kDarkGrey);
		startPt.Set(fLResizeZone.right+1, fLResizeZone.top);
		endPt.Set(fLResizeZone.right+1, fLResizeZone.bottom);
		fCue->StrokeLine(startPt, endPt);

		//	Draw icon.  It should be 16x16
		BBitmap *icon = dynamic_cast<TTransitionEffect *>(fEffect)->Icon();
		if (icon) {
			BPoint drawPt(fBounds.right-18, fBounds.top+2);
			fCue->DrawBitmap(icon, drawPt);
		}
	}

	//	Draw slection rect
	if(fIsSelected) {
		DrawSelectionRect();
	}
	//	Outline entire content area
	else{
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
	}

	//	Draw name text
	/*BFont font(be_plain_font);
	   font.SetSize(9.0);
	   fCue->SetFont(&font);
	   BPoint drawPt(fBounds.left, fBounds.bottom-1);
	   fCue->SetHighColor(kBlack);
	   fCue->SetLowColor(kSteelBlue);
	   fCue->DrawString(fEffect->Name().c_str(), drawPt);*/
}

#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TTransitionView::MouseDown(BPoint where, bool doubleClick)
{
	TCueEffectView::MouseDown(where, doubleClick);
	/*
	   // Do nothing if we are playing
	   if ( IsPlaying() )
	        return;

	   //	Double click?
	   if (doubleClick)
	   {
	   }
	   else
	   {
	   }
	 */

	/*
	   // Make sure point falls within our bounds or resize zones
	   if ( fChannelCue->CuePosition()->Contains(where) || PointInResizeZones(where) )
	   {

	        uint32  type;
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
	                        uint32  buttons = 0;
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

void TTransitionView::MouseMoved(BPoint where)
{
	//
	//	Check and see if they are resizing
	//	First check and see if they are in the right resize zone...
	if( fRResizeZone.Contains(where) && ((TTransitionEffect *)fEffect)->TransitionIn() ) {
		ResizeRightSide();
	}
	//	Left size?
	else if( fLResizeZone.Contains(where)) {
		ResizeLeftSide();
	}
	//
	// They must be moving the enter EffectView...
	//
	else{
		/*BRect bounds = Bounds();

		   BMessage message(CUE_DRAG_MSG);
		   message.AddPointer("CueView", this);
		   BRect frame = Frame();
		   BPoint dragPt = where;
		   ConvertToParent(&dragPt);
		   uint32 delta = dragPt.x - (frame.left + fChannel->Bounds().left);
		   message.AddInt32("CueDelta", delta);

		   // Center the rect within the mouse for a better drag appearence
		   static_cast<MuseumApp *>(be_app)->fIsCueDrag = true;
		   static_cast<MuseumApp *>(be_app)->fCueDragDelta = delta;

		   DragMessage(&message, bounds);*/
	}

	//	Restore
	fCue->LockLooper();
	fCue->PopState();
	fCue->UnlockLooper();

}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	Resized
//---------------------------------------------------------------------
//
//	Effect view was resized.  Adjust if neede
//

void TTransitionView::Resized()
{
	//	Adjust bounds and resize zones
	const uint32 startTime = fEffect->StartTime();
	const uint32 duration  = fEffect->Duration();

	//	Convert to pixels
	int32 startPix  = TimeToPixels(startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	int32 lengthPix = TimeToPixels(duration,  GetCurrentTimeFormat(), GetCurrentResolution());

	//	In Effects are pegged to left side
	if ( ((TTransitionEffect *)fEffect)->TransitionIn() ) {
		fBounds.left    = startPix + 4; // Add width of tray edge
		fBounds.right   = fBounds.left + lengthPix;
	}
	//	Out Effects are pegged to right side
	else{
		fBounds.right   = fCue->Bounds().Width() - 5;   // Subtract width of tray edge
		fBounds.left    = fBounds.right - lengthPix;

		//	Adjust time to account for shift to the right
		uint32 newStart = fCue->Duration() - duration;
		fEffect->StartTime(newStart);
	}

	// Fix drag zones
	UpdateResizeZones();

	//	Check for out of bounds conditions

}

//---------------------------------------------------------------------
//	ResizeRightSide
//---------------------------------------------------------------------
//
//	Resize right side of effect view
//

void TTransitionView::ResizeRightSide()
{
	// Get frame and bounds area
	fCue->LockLooper();
	const BRect cueBounds = fCue->Bounds();
	BRect oldFrame(0,0,-1,-1);
	fCue->UnlockLooper();

	//	Get the mouse state
	BPoint mousePt, oldPt;
	uint32 buttons = 0;
	fCue->GetMouse(&mousePt, &buttons, true);

	bool wasResized = false;

	while(buttons) {
		//	We have been modified
		wasResized = true;

		if (mousePt != oldPt) {
			//	Modify right side of bounds
			fBounds.right = mousePt.x;

			// Check for minimum bounds violation
			if (fBounds.Width() < 1)
				fBounds.right = fBounds.left + 1;

			//	Check for out of bounds
			if (fBounds.right > (cueBounds.right-4))
				fBounds.right = (cueBounds.right-4);

			//	Force a cue redraw
			if (fCue->LockLooper()) {
				fCue->PushState();
				drawing_mode saveMode = fCue->DrawingMode();
				fCue->SetDrawingMode(B_OP_INVERT);
				fCue->SetPenSize(2.0);
				fCue->SetHighColor(kBlack);
				fCue->StrokeRect(oldFrame);
				fCue->StrokeRect(fBounds);
				fCue->SetDrawingMode(saveMode);
				fCue->PopState();
				fCue->UnlockLooper();
			}

			//	Update effect duration
			uint32 newDuration = PixelsToTime(fBounds.Width(), GetCurrentTimeFormat(), GetCurrentResolution());
			fEffect->Duration( fEffect->StartTime() + newDuration);

			//	Redraw stage
			BMessage *stageMsg = new BMessage(TIMELINE_DRAG_MSG);
			stageMsg->AddInt32("TheTime", GetCurrentTime());
			((MuseumApp *)be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(stageMsg);
			delete stageMsg;

			//	Notify timeline of mouse location.  Clip to effect bounds
			BMessage *timeMsg = new BMessage(UPDATE_TIMELINE_MSG);
			BPoint resizePt = fCue->ConvertToParent(BPoint(fBounds.right, fBounds.top));
			timeMsg->AddPoint("Where", resizePt);
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(timeMsg);
			delete timeMsg;

			// Save point and frame for future compare
			oldPt    = mousePt;
			oldFrame = fBounds;
		}

		// Let other events through
		snooze(20 * 1000);

		// Get new mouse location and button state
		fCue->GetMouse(&mousePt, &buttons, true);
	}

	//	Were we resized?
	if( wasResized ) {
		// Fix drag zones
		UpdateResizeZones();

		//	Update effect duration
		uint32 newDuration = PixelsToTime(fBounds.Width(), GetCurrentTimeFormat(), GetCurrentResolution());
		fEffect->Duration( fEffect->StartTime() + newDuration);

		//	Redraw cue
		if (fCue->LockLooper()) {
			fCue->Draw(cueBounds);
			fCue->UnlockLooper();
		}

		//	Redraw stage
		BMessage *message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		((MuseumApp *)be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(message);

	}
}


//---------------------------------------------------------------------
//	ResizeLeftSide
//---------------------------------------------------------------------
//
//	Resize left side of effect view.  This adjust the effect start time
//

void TTransitionView::ResizeLeftSide()
{
	// Get frame and bounds area
	fCue->LockLooper();
	const BRect cueBounds = fCue->Bounds();
	BRect oldFrame(0,0,-1,-1);
	fCue->UnlockLooper();

	//	Get the mouse state
	BPoint mousePt, oldPt;
	uint32 buttons = 0;
	fCue->GetMouse(&mousePt, &buttons, true);

	bool wasResized = false;

	while(buttons) {
		//	We have been modified
		wasResized = true;

		if (mousePt != oldPt) {
			//	Modify left side of bounds
			fBounds.left = mousePt.x;

			// Check for minimum bounds violation
			if (fBounds.Width() < 1)
				fBounds.left = fBounds.right - 1;

			//	Check for out of bounds
			if (fBounds.left < (cueBounds.left+4))
				fBounds.left = (cueBounds.left+4);

			//	Force a cue redraw
			if (fCue->LockLooper()) {
				fCue->PushState();
				drawing_mode saveMode = fCue->DrawingMode();
				fCue->SetDrawingMode(B_OP_INVERT);
				fCue->SetPenSize(2.0);
				fCue->SetHighColor(kBlack);
				fCue->StrokeRect(oldFrame);
				fCue->StrokeRect(fBounds);
				fCue->SetDrawingMode(saveMode);
				fCue->PopState();
				fCue->UnlockLooper();
			}

			//	Convert pixels to time
			const uint32 newStart = PixelsToTime(fBounds.left,  GetCurrentTimeFormat(), GetCurrentResolution());
			fEffect->StartTime(newStart);

			const uint32 newDuration = fCue->Duration() - fEffect->StartTime();
			fEffect->Duration(newDuration);

			//	Redraw stage
			BMessage *stageMsg = new BMessage(TIMELINE_DRAG_MSG);
			stageMsg->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(stageMsg);
			delete stageMsg;

			//	Notify timeline of mouse location.  Clip to effect bounds
			BMessage *timeMsg = new BMessage(UPDATE_TIMELINE_MSG);
			BPoint resizePt = fCue->ConvertToParent(BPoint(fBounds.left, fBounds.top));
			timeMsg->AddPoint("Where", resizePt);
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(timeMsg);
			delete timeMsg;

			// Save point and frame for future compare
			oldPt    = mousePt;
			oldFrame = fBounds;
		}

		// Let other events through
		snooze(20 * 1000);

		// Get new mouse location and button state
		fCue->GetMouse(&mousePt, &buttons, true);
	}

	// Only resize
	if( wasResized ) {
		// Fix drag zones
		UpdateResizeZones();

		//	Convert pixels to time
		const uint32 newStart = PixelsToTime(fBounds.left,  GetCurrentTimeFormat(), GetCurrentResolution());
		fEffect->StartTime(newStart);

		const uint32 newDuration = fCue->Duration() - fEffect->StartTime();
		fEffect->Duration(newDuration);

		//	Redraw cue
		if (fCue->LockLooper()) {
			fCue->Draw(cueBounds);
			fCue->UnlockLooper();
		}

		//	Redraw stage
		BMessage *message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		((MuseumApp *)be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(message);
	}
}