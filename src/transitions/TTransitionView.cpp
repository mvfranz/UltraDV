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
		
	const uint32 startTime = m_Effect->StartTime();
	const uint32 duration  = m_Effect->Duration();

	//	Convert to pixels
	int32 startPix  = TimeToPixels(startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	int32 lengthPix = TimeToPixels(duration,  GetCurrentTimeFormat(), GetCurrentResolution());				

	//	In Effect is pegged to left side
	if ( ((TTransitionEffect *)m_Effect)->TransitionIn() )
	{
		m_Bounds.left 	= startPix + 4;	// Add width of tray edge
		m_Bounds.right	= m_Bounds.left + lengthPix;
	}
	//	Out Effect is pegged to right side
	else
	{
		m_Bounds.right	= m_Cue->Bounds().Width() - 4;	// Subtract width of tray edge
		m_Bounds.left 	= m_Bounds.right - lengthPix;	
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
	m_Cue->SetHighColor(kLightGrey);
	m_Cue->FillRect(m_Bounds);
				
	//	Draw side draggers.  If we are an "In" transition, draw right dragger
	//	If we are an "Out" transition, draw left dragger
	
	//	Transition In
	if ( ((TTransitionEffect *)m_Effect)->TransitionIn())
	{				
		// Right side highlights
		m_Cue->SetHighColor(kSteelBlue);
		m_Cue->FillRect(m_RResizeZone);

		m_Cue->SetHighColor(kDarkSteelBlue);
		startPt.Set(m_RResizeZone.right, m_RResizeZone.top);		
		endPt.Set(m_RResizeZone.right, m_RResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);	
		m_Cue->SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		m_Cue->StrokeLine(startPt, endPt);	
		
		m_Cue->SetHighColor(kDarkSteelBlue);
		startPt.Set(m_RResizeZone.left, m_RResizeZone.top);		
		endPt.Set(m_RResizeZone.left, m_RResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);	
		m_Cue->SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		m_Cue->StrokeLine(startPt, endPt);		
		
		// Right Shadow
		m_Cue->SetHighColor(kDarkGrey);	
		startPt.Set(m_RResizeZone.left-1, m_LResizeZone.top);		
		endPt.Set(m_RResizeZone.left-1, m_LResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);
		
		//	Draw icon.  It should be 16x16
		BBitmap *icon = dynamic_cast<TTransitionEffect *>(m_Effect)->Icon();
		if (icon)
		{
			BPoint drawPt(m_Bounds.left+2, m_Bounds.top+2);
			m_Cue->DrawBitmap(icon, drawPt);			
		}
	}
	//	Transition Out
	else
	{		
		// Left side highlights
		m_Cue->SetHighColor(kSteelBlue);
		m_Cue->FillRect(m_LResizeZone);

		m_Cue->SetHighColor(kDarkSteelBlue);
		startPt.Set(m_LResizeZone.left, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.left, m_LResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);	
		m_Cue->SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		m_Cue->StrokeLine(startPt, endPt);	
		
		m_Cue->SetHighColor(kDarkSteelBlue);
		startPt.Set(m_LResizeZone.right, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.right, m_LResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);	
		m_Cue->SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		m_Cue->StrokeLine(startPt, endPt);	
		
		// Left Shadow
		m_Cue->SetHighColor(kDarkGrey);	
		startPt.Set(m_LResizeZone.right+1, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.right+1, m_LResizeZone.bottom);
		m_Cue->StrokeLine(startPt, endPt);
		
		//	Draw icon.  It should be 16x16		
		BBitmap *icon = dynamic_cast<TTransitionEffect *>(m_Effect)->Icon();
		if (icon)
		{
			BPoint drawPt(m_Bounds.right-18, m_Bounds.top+2);
			m_Cue->DrawBitmap(icon, drawPt);			
		}
	}
	
	//	Draw slection rect
	if(m_IsSelected)
	{
		DrawSelectionRect();
	}
	//	Outline entire content area
	else
	{		
		m_Cue->SetHighColor(kWhite);
		startPt.Set(m_Bounds.left, m_Bounds.bottom-1);
		endPt.Set(m_Bounds.left, m_Bounds.top);	
		m_Cue->StrokeLine(startPt, endPt);
		endPt.Set(m_Bounds.right, m_Bounds.top);
		m_Cue->StrokeLine(endPt);
		
		m_Cue->SetHighColor(kBlack);
		startPt.Set(m_Bounds.right, m_Bounds.top+1);
		endPt.Set(m_Bounds.right, m_Bounds.bottom);
		m_Cue->StrokeLine(startPt, endPt);
		endPt.Set(m_Bounds.left, m_Bounds.bottom);
		m_Cue->StrokeLine(endPt);
	}
	
	//	Draw name text
	/*BFont font(be_plain_font); 
	font.SetSize(9.0);   	
	m_Cue->SetFont(&font);
	BPoint drawPt(m_Bounds.left, m_Bounds.bottom-1);
	m_Cue->SetHighColor(kBlack);
	m_Cue->SetLowColor(kSteelBlue);
	m_Cue->DrawString(m_Effect->Name().c_str(), drawPt);*/
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
	if ( m_ChannelCue->CuePosition()->Contains(where) || PointInResizeZones(where) )
	{	
		
		uint32 	type;
		int32	count = 0;
		BMessage *message = m_Stage->Window()->CurrentMessage();
		if (B_OK == message->GetInfo("clicks", &type, &count) )
		{
			int32 clickCount = message->FindInt32("clicks", count-1);
			
			// Is this a double click?  If so, open editor
			if (clickCount == 2)
			{
				//if(m_ChannelCue->HasEditor())
				//	m_ChannelCue->OpenEditor();		
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
				BMessage *message = m_Stage->Window()->CurrentMessage();
				message->FindInt32("buttons", (long *)&buttons);
			
				switch(buttons)
				{
					case B_PRIMARY_MOUSE_BUTTON:
						{							
							//	Find location of click.  If it is the main body of the picture, they are moving the picture.
							//	Otherwise, they are using a stage tool
							if ( PointInResizeZones(where) )
							{																					
								switch(m_Stage->GetToolMode())
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
	if( m_RResizeZone.Contains(where) && ((TTransitionEffect *)m_Effect)->TransitionIn() )
	{										
		ResizeRightSide();
	}
	//	Left size?
	else if( m_LResizeZone.Contains(where))
	{
		ResizeLeftSide();
	}	
	//		
	// They must be moving the enter EffectView...
	//
	else
	{
		/*BRect bounds = Bounds();
		
		BMessage message(CUE_DRAG_MSG);
		message.AddPointer("CueView", this);
		BRect frame = Frame();
		BPoint dragPt = where;
		ConvertToParent(&dragPt);
		uint32 delta = dragPt.x - (frame.left + m_Channel->Bounds().left);
		message.AddInt32("CueDelta", delta);
					
		// Center the rect within the mouse for a better drag appearence
		static_cast<MuseumApp *>(be_app)->m_IsCueDrag = true;
		static_cast<MuseumApp *>(be_app)->m_CueDragDelta = delta;
		
		DragMessage(&message, bounds);*/
	}
	
	//	Restore
	m_Cue->LockLooper();
	m_Cue->PopState();
	m_Cue->UnlockLooper();

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
	const uint32 startTime = m_Effect->StartTime();
	const uint32 duration  = m_Effect->Duration();

	//	Convert to pixels
	int32 startPix  = TimeToPixels(startTime, GetCurrentTimeFormat(), GetCurrentResolution());
	int32 lengthPix = TimeToPixels(duration,  GetCurrentTimeFormat(), GetCurrentResolution());				

	//	In Effects are pegged to left side
	if ( ((TTransitionEffect *)m_Effect)->TransitionIn() )
	{
		m_Bounds.left 	= startPix + 4;	// Add width of tray edge
		m_Bounds.right	= m_Bounds.left + lengthPix;
	}
	//	Out Effects are pegged to right side
	else
	{
		m_Bounds.right	= m_Cue->Bounds().Width() - 5;	// Subtract width of tray edge
		m_Bounds.left 	= m_Bounds.right - lengthPix;
		
		//	Adjust time to account for shift to the right
		uint32 newStart = m_Cue->Duration() - duration;
		m_Effect->StartTime(newStart);
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
	m_Cue->LockLooper();	
	const BRect cueBounds = m_Cue->Bounds();	
	BRect oldFrame(0,0,-1,-1);	
	m_Cue->UnlockLooper();

	//	Get the mouse state
	BPoint 	mousePt, oldPt;
	uint32	buttons = 0;
	m_Cue->GetMouse(&mousePt, &buttons, true);	

	bool wasResized = false;
	
	while(buttons)
	{
		//	We have been modified
		wasResized = true;
		
		if (mousePt != oldPt)
		{
			//	Modify right side of bounds
			m_Bounds.right = mousePt.x;
			
			// Check for minimum bounds violation
			if (m_Bounds.Width() < 1)
				m_Bounds.right = m_Bounds.left + 1;
				
			//	Check for out of bounds
			if (m_Bounds.right > (cueBounds.right-4))
				m_Bounds.right = (cueBounds.right-4);
							
			//	Force a cue redraw
			if (m_Cue->LockLooper())
			{
				m_Cue->PushState();
				drawing_mode saveMode = m_Cue->DrawingMode();
				m_Cue->SetDrawingMode(B_OP_INVERT);	
				m_Cue->SetPenSize(2.0);
				m_Cue->SetHighColor(kBlack);
				m_Cue->StrokeRect(oldFrame);
				m_Cue->StrokeRect(m_Bounds);
				m_Cue->SetDrawingMode(saveMode);
				m_Cue->PopState();
				m_Cue->UnlockLooper();
			}
						
			//	Update effect duration		
			uint32 newDuration = PixelsToTime(m_Bounds.Width(), GetCurrentTimeFormat(), GetCurrentResolution());
			m_Effect->Duration( m_Effect->StartTime() + newDuration);
		
			//	Redraw stage
			BMessage *stageMsg = new BMessage(TIMELINE_DRAG_MSG);
			stageMsg->AddInt32("TheTime", GetCurrentTime());
			((MuseumApp *)be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(stageMsg);
			delete stageMsg;
			
			//	Notify timeline of mouse location.  Clip to effect bounds
			BMessage *timeMsg = new BMessage(UPDATE_TIMELINE_MSG);
			BPoint resizePt = m_Cue->ConvertToParent(BPoint(m_Bounds.right, m_Bounds.top));
			timeMsg->AddPoint("Where", resizePt);
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(timeMsg);
			delete timeMsg;
		
			// Save point and frame for future compare
			oldPt    = mousePt;	
			oldFrame = m_Bounds;		
		}
					
		// Let other events through
		snooze(20 * 1000);
	
		// Get new mouse location and button state
		m_Cue->GetMouse(&mousePt, &buttons, true);	
	}
	
	//	Were we resized?
	if( wasResized )
	{		
		// Fix drag zones
		UpdateResizeZones();
		
		//	Update effect duration		
		uint32 newDuration = PixelsToTime(m_Bounds.Width(), GetCurrentTimeFormat(), GetCurrentResolution());
		m_Effect->Duration( m_Effect->StartTime() + newDuration);
		
		//	Redraw cue
		if (m_Cue->LockLooper())
		{
			m_Cue->Draw(cueBounds);
			m_Cue->UnlockLooper();
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
	m_Cue->LockLooper();	
	const BRect cueBounds = m_Cue->Bounds();	
	BRect oldFrame(0,0,-1,-1);
	m_Cue->UnlockLooper();

	//	Get the mouse state
	BPoint 	mousePt, oldPt;
	uint32	buttons = 0;
	m_Cue->GetMouse(&mousePt, &buttons, true);	

	bool wasResized = false;
	
	while(buttons)
	{
		//	We have been modified
		wasResized = true;
		
		if (mousePt != oldPt)
		{
			//	Modify left side of bounds
			m_Bounds.left = mousePt.x;
			
			// Check for minimum bounds violation
			if (m_Bounds.Width() < 1)
				m_Bounds.left = m_Bounds.right - 1;
				
			//	Check for out of bounds
			if (m_Bounds.left < (cueBounds.left+4))
				m_Bounds.left = (cueBounds.left+4);
							
			//	Force a cue redraw
			if (m_Cue->LockLooper())
			{
				m_Cue->PushState();
				drawing_mode saveMode = m_Cue->DrawingMode();
				m_Cue->SetDrawingMode(B_OP_INVERT);	
				m_Cue->SetPenSize(2.0);
				m_Cue->SetHighColor(kBlack);
				m_Cue->StrokeRect(oldFrame);
				m_Cue->StrokeRect(m_Bounds);
				m_Cue->SetDrawingMode(saveMode);
				m_Cue->PopState();
				m_Cue->UnlockLooper();
			}

			//	Convert pixels to time
			const uint32 newStart = PixelsToTime(m_Bounds.left,  GetCurrentTimeFormat(), GetCurrentResolution());				
			m_Effect->StartTime(newStart);
	
			const uint32 newDuration = m_Cue->Duration() - m_Effect->StartTime();
			m_Effect->Duration(newDuration);
			
			//	Redraw stage
			BMessage *stageMsg = new BMessage(TIMELINE_DRAG_MSG);
			stageMsg->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(stageMsg);
			delete stageMsg;
			
			//	Notify timeline of mouse location.  Clip to effect bounds
			BMessage *timeMsg = new BMessage(UPDATE_TIMELINE_MSG);
			BPoint resizePt = m_Cue->ConvertToParent(BPoint(m_Bounds.left, m_Bounds.top));
			timeMsg->AddPoint("Where", resizePt);
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(timeMsg);
			delete timeMsg;

			// Save point and frame for future compare
			oldPt    = mousePt;	
			oldFrame = m_Bounds;		
		}
					
		// Let other events through
		snooze(20 * 1000);
	
		// Get new mouse location and button state
		m_Cue->GetMouse(&mousePt, &buttons, true);	
	}
	
	// Only resize
	if( wasResized )
	{		
		// Fix drag zones
		UpdateResizeZones();
				
		//	Convert pixels to time
		const uint32 newStart = PixelsToTime(m_Bounds.left,  GetCurrentTimeFormat(), GetCurrentResolution());				
		m_Effect->StartTime(newStart);

		const uint32 newDuration = m_Cue->Duration() - m_Effect->StartTime();
		m_Effect->Duration(newDuration);

		//	Redraw cue
		if (m_Cue->LockLooper())
		{
			m_Cue->Draw(cueBounds);
			m_Cue->UnlockLooper();
		}
		
		//	Redraw stage
		BMessage *message = new BMessage(TIMELINE_DRAG_MSG);
		message->AddInt32("TheTime", GetCurrentTime());
		((MuseumApp *)be_app)->GetCueSheet()->GetCueSheetView()->MessageReceived(message);
	}	
}