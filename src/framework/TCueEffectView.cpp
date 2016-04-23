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
	m_Bounds = bounds;
			
	// Save cue pointer
	m_Cue = theCue;

	// Save effect pointer
	m_Effect = theEffect;
			
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
	delete m_Effect;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TCueEffectView::Init()
{
	// We start life unselected
	m_IsSelected = false;
		
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
		if (m_IsSelected == false)
			m_Cue->DeselectAllEffects();
		
		//	Select
		Select();
	}
	
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

void TCueEffectView::MouseMoved( BPoint where)
{
	/*
	//	Do nothing if window is not active
	if (m_Stage->Window()->IsActive() == false)
		return;
		
	//	Determine tool mode and set proper cursor
	switch(m_Stage->GetToolMode())
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
	m_Cue->SetHighColor(kLightGrey);
	m_Cue->FillRect(m_Bounds);
				
	//	Outline entire content area
	m_Cue->SetHighColor(kWhite);
	m_Cue->StrokeRect(m_Bounds);
	
	//	Draw name text
	BFont font(be_plain_font); 
	font.SetSize(9.0);   	
	m_Cue->SetFont(&font);
	BPoint drawPt(m_Bounds.left, m_Bounds.bottom-1);
	m_Cue->SetHighColor(kBlack);
	m_Cue->SetLowColor(kSteelBlue);
	m_Cue->DrawString(m_Effect->Name().c_str(), drawPt);
}


//---------------------------------------------------------------------
//	DrawSelectionRect
//---------------------------------------------------------------------
//
//

void TCueEffectView::DrawSelectionRect()
{			
	//	Draw Selection in red
	if (m_Cue->LockLooper())
	{
		m_Cue->PushState();
		m_Cue->SetHighColor(kRed);
		m_Cue->StrokeRect(m_Bounds);
		m_Cue->PopState();
		m_Cue->UnlockLooper();	
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
	if (m_Cue->LockLooper())
	{
		BPoint startPt, endPt;
		
		m_Cue->PushState();
		
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
		
		m_Cue->PopState();
		m_Cue->UnlockLooper();	
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
	
	m_Stage->GetMouse(&thePoint, &buttons, true);
		
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
			outline = m_ChannelCue->CuePosition()->Enclosure();
			outline.left 	+= diffX;
			outline.top  	+= diffY;
			outline.right 	+= diffX;
			outline.bottom  += diffY;
			m_ChannelCue->CuePosition()->Outline(outline);
			
			// Update resize zones
			SetResizeZones();
			
			// Save mouse location for next compare
			savePt = thePoint;
			
			//	Draw new selection rect
			DrawSelectionRect(false);		
		}			
		m_Stage->GetMouse(&thePoint, &buttons, true);
	}
	
	//	Draw new position
	if ( m_Stage->LockLooper())
	{
		//m_Stage->StageDraw(m_ChannelCue->GetCroppedArea(), GetCurrentTime());
		m_Stage->StageDraw(m_ChannelCue->CuePosition()->Enclosure(), GetCurrentTime());
		m_Stage->Draw(m_Stage->Bounds());
		m_Stage->UnlockLooper();
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
	m_LResizeZone.Set(m_Bounds.left, m_Bounds.top, m_Bounds.left+kResizeZoneWidth, m_Bounds.bottom);
	m_RResizeZone.Set(m_Bounds.right-kResizeZoneWidth, m_Bounds.top, m_Bounds.right, m_Bounds.bottom );
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
	if (m_IsSelected == false)
	{		
		//	Deselect all other cue effects
		
		//	We are now selected
		m_IsSelected = true;
		
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
	if (m_IsSelected)
	{
		m_IsSelected = false;
		
		//	Invalidate selection rect
		InvalidateSelectionRect();	
	}
}
