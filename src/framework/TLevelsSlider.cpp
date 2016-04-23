//---------------------------------------------------------------------
//
//	File:	TLevelsSlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.06.98
//
//	Desc:	Slider to control levels
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "DebugUtils.h"
#include "TMouseWatcher.h"

#include "TLevelsSlider.h"

// Constants 
const int8 kTopOffset 		= 1;
const int8 kBottomOffset 	= 3;
								
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TLevelsSlider::TLevelsSlider(BRect r, AudioSliderType type) : BView(r, "LevelsSlider", B_FOLLOW_NONE, B_WILL_DRAW)
{
	m_Type = type;
	
	Init();
}


								
//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TLevelsSlider::~TLevelsSlider()
{
	// Free slider bitmaps
	if (m_LeftSlider)
		delete m_LeftSlider;
		
	if (m_RightSlider)
		delete m_RightSlider;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TLevelsSlider::Init()
{
	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Load slider bitmaps
	LoadSliders();
	
	/*
	// Get output device
	BADCStream adcStream;
	int32 curADCDevice = 0;
	if ( adcStream.ADCInput(&curADCDevice) != B_OK)
		DebugAlert("AudioServer not runnng!");
		
	// Get volume settings
	BDACStream 	dacStream;
	float 		volLeft, volRight;
	
	// Input
	if (m_Type == kAudioInputSlider)
	{
		
		dacStream.GetVolume(B_ADC_IN, &volLeft, &volRight, NULL);
	}
	// Output
	else
	{
		dacStream.GetVolume(B_DAC_OUT, &volLeft, &volRight, NULL); 
	}
	*/
		
	float volLeft 	= 0.0;
	float volRight	= 0.0;
	
	// Set up slider rects
	const BRect bounds 			= Bounds();
	const uint32 sliderWidth 	= m_LeftSlider->Bounds().IntegerWidth();
	const uint32 sliderHeight 	= m_LeftSlider->Bounds().IntegerHeight();
	const uint32 height 		= (bounds.Height()-kTopOffset) - (sliderHeight+kBottomOffset);
	
	// Get pixel location of levels
	int32 sliderLeft	= height * volLeft;
	int32 sliderRight 	= height * volRight;
	
	sliderLeft  += sliderHeight+kBottomOffset;
	sliderRight += sliderHeight+kBottomOffset;

	m_LeftSliderRect.left 	 = (bounds.Width()/2) - sliderWidth;
	m_LeftSliderRect.right 	 = m_LeftSliderRect.left + sliderWidth;	
	m_LeftSliderRect.top 	 = bounds.Height() + (-sliderLeft);
	m_LeftSliderRect.bottom  = m_LeftSliderRect.top - sliderHeight;
	
	m_RightSliderRect.left 	 = (bounds.Width()/2);
	m_RightSliderRect.right	 = m_RightSliderRect.left + sliderWidth;	
	m_RightSliderRect.top 	 = bounds.Height() + (-sliderRight);
	m_RightSliderRect.bottom = m_RightSliderRect.top - sliderHeight;		
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TLevelsSlider::MessageReceived(BMessage *theMessage)
{

	switch( theMessage->what)
	{					
		// Mouse Tracking Messages
		case MW_MOUSE_MOVED:
			TrackSliders();
			break;

		default:
			BView::MessageReceived(theMessage);
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

void TLevelsSlider::Draw(BRect updateRect)
{
	PushState();
	
	// Set up clipping
	BRegion clipRegion, saveRegion;
	GetClippingRegion(&saveRegion);
	clipRegion.Set(updateRect);
	ConstrainClippingRegion( &clipRegion );
	
	BPoint startPt, endPt;
	
	const BRect bounds = Bounds();
	
	// Fill with background
	SetHighColor(kPaleGrey);
	FillRect(updateRect);
	
	// Create left frame
	SetHighColor(kDarkGrey);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.right-2, bounds.top);
	StrokeLine(endPt);

	// Create left shadow
	SetHighColor(kBeShadow);
	BRect shadowOutline = bounds;
	shadowOutline.InsetBy(1,1);
	StrokeRect(shadowOutline);
	
	startPt.Set(shadowOutline.left+1, shadowOutline.bottom);
	endPt.Set(shadowOutline.left+1, shadowOutline.top+1);
	StrokeLine(startPt, endPt);
	endPt.Set(shadowOutline.right, shadowOutline.top+1);
	StrokeLine(endPt);
	startPt.Set(shadowOutline.right-1, shadowOutline.top+1);
	endPt.Set(shadowOutline.right-1, shadowOutline.bottom-1);
	StrokeLine(startPt, endPt);
	endPt.Set(shadowOutline.left-2, shadowOutline.bottom-1);
	StrokeLine(endPt);
	
	// Create right hilite
	SetHighColor(kPaleGrey);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(endPt);
	
	startPt.Set(bounds.right-1, bounds.top+1);
	endPt.Set(bounds.right-1, bounds.bottom-1);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.left, bounds.bottom-1);
	StrokeLine(endPt);
	
	// Create channel slot
	BRect slot 	= bounds;
	slot.left 	= (bounds.Width()/2) - 3;
	slot.right 	= (bounds.Width()/2) + 2;
	slot.top 	+= 1;
	slot.bottom -= 3;
	SetHighColor(kBlack);
	FillRect(slot);
	
	// Draw slot hilites
	startPt.Set(slot.left, slot.top);
	endPt.Set(slot.left, slot.bottom);
	SetHighColor(kDarkGrey);
	StrokeLine(startPt, endPt);
	slot.top 	+=1;
	slot.left 	= slot.right-2;
	rgb_color dkGrey = {35,35,35,255};
	SetHighColor(dkGrey);
	FillRect(slot);
	
	// Draw ticks
	DrawSliderTicks();
	
	// Draw Sliders
	DrawSliders();
	
	ConstrainClippingRegion( &saveRegion );	
	PopState();
}


//---------------------------------------------------------------------
//	DrawSliderTicks
//---------------------------------------------------------------------
//
//	Draw indicator ticks

void TLevelsSlider::DrawSliderTicks()
{	

}

//---------------------------------------------------------------------
//	DrawLeftSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::DrawSliders()
{
	DrawLeftSlider();
	DrawRightSlider();
}

//---------------------------------------------------------------------
//	DrawLeftSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::DrawLeftSlider()
{	
	BPoint drawPt( m_LeftSliderRect.left, m_LeftSliderRect.top);
	DrawBitmap(m_LeftSlider, drawPt);
}


//---------------------------------------------------------------------
//	DrawRightSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::DrawRightSlider()
{	
	BPoint drawPt( m_RightSliderRect.left, m_RightSliderRect.top);
	DrawBitmap(m_RightSlider, drawPt);
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TLevelsSlider::MouseDown(BPoint where)
{
	// Check for which button is pressed
	uint32 	buttons = 0;				
	Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);
	
	// Determine which button has been clicked
	switch(buttons)
	{
		
		case B_PRIMARY_MOUSE_BUTTON:
			{
				const BRect bounds = Bounds();
				
				// Get old positions
				BRect oldLeftRect 	= m_LeftSliderRect;
				BRect oldRightRect 	= m_RightSliderRect;
				
				// Move both left and right slider under mouse
				const uint32 leftHeight	 = m_LeftSlider->Bounds().IntegerHeight();
				const uint32 rightHeight = m_RightSlider->Bounds().IntegerHeight();
				
				m_LeftSliderRect.top 		= where.y - leftHeight/2;
				m_LeftSliderRect.bottom 	= m_LeftSliderRect.top + leftHeight;
				m_RightSliderRect.top 		= where.y - rightHeight/2;
				m_RightSliderRect.bottom 	= m_RightSliderRect.top + rightHeight;
				
				// Now clip the location of the slider if out of bounds
				ClipSliders();
				
				// Clean up old position
				BRect updateRect 	= oldLeftRect;
				updateRect.right 	= oldRightRect.right;
				Draw(Bounds());
				
				// Draw updated position				
				DrawLeftSlider();
				DrawRightSlider();	
				
				// Update the volumes
				UpdateVolumes(where);	
								
				// Wait a while for the mouse button to be realeased
				snooze(100 * 1000);
												
				// Is button down?  They are dragging or resizing the cue...	
				// Check to see if button is down				
				Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);	
				if (buttons)
				{
					//StartMouseWatcher(this);
					TrackSliders();
				}			
			}
			break;
			
		// User is dragging one half of the slider
		case B_SECONDARY_MOUSE_BUTTON:
			{
				const BRect bounds = Bounds();
				
				// Determine area mouse is in for slider drag
				//
				
				// Left Side
				if (where.x <= bounds.Width()/2)
				{				
					// Get old positions
					BRect oldRect 	= m_LeftSliderRect;
					
					// Move both left slider under mouse
					const uint32 leftHeight	 = m_LeftSlider->Bounds().IntegerHeight();
					
					m_LeftSliderRect.top 	= where.y - leftHeight/2;
					m_LeftSliderRect.bottom = m_LeftSliderRect.top + leftHeight;
					
					// Now clip the location of the slider if out of bounds
					ClipLeftSlider();
					
					// Clean up old position
					Draw(oldRect);
					
					// Draw updated position				
					DrawLeftSlider();
					
					// Update the volume
					UpdateLeftVolume(where);	
									
					// Wait a while for the mouse button to be realeased
					snooze(100 * 1000);
		
					// Is button down?  They are dragging or resizing the cue...	
					// Check to see if button is down				
					Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);	
					if (buttons)
					{
						TrackLeftSlider();
					}			
				
				}
				// Right Side
				else
				{
					const BRect bounds = Bounds();
					
					// Get old positions
					BRect oldRect = m_RightSliderRect;
					
					// Move both left and right slider under mouse
					const uint32 rightHeight = m_RightSlider->Bounds().IntegerHeight();
					
					m_RightSliderRect.top 		= where.y - rightHeight/2;
					m_RightSliderRect.bottom 	= m_RightSliderRect.top + rightHeight;
					
					// Now clip the location of the slider if out of bounds
					ClipRightSlider();
					
					// Clean up old position
					Draw(oldRect);
					
					// Draw updated position				
					DrawRightSlider();	
					
					// Update the volume
					UpdateRightVolume(where);	
									
					// Wait a while for the mouse button to be realeased
					snooze(100 * 1000);
		
					// Is button down?  They are dragging or resizing the cue...	
					// Check to see if button is down				
					Window()->CurrentMessage()->FindInt32("buttons", (long *)&buttons);	
					if (buttons)
					{
						TrackRightSlider();
					}							
				}
			}
			break;
	}	
}

#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//

void TLevelsSlider::AttachedToWindow()
{
	/*
	// Update current levels
	BADCStream adcStream;
	int32 curADCDevice = 0;
	if ( adcStream.ADCInput(&curADCDevice) != B_OK)
		DebugAlert("AudioServer not runnng!");
		
	// Get volume settings
	BDACStream 	dacStream;
	float 		volLeft, volRight;
	
	// Input
	if (m_Type == kAudioInputSlider)
		dacStream.GetVolume(B_ADC_IN, &volLeft, &volRight, NULL);
	// Output
	else
		dacStream.GetVolume(B_DAC_OUT, &volLeft, &volRight, NULL); 
	*/
	
	float volLeft 	= 0.0;
	float volRight	= 0.0;

	// Set up slider rects
	const BRect bounds 			= Bounds();
	const uint32 sliderWidth 	= m_LeftSlider->Bounds().IntegerWidth();
	const uint32 sliderHeight 	= m_LeftSlider->Bounds().IntegerHeight();
	const uint32 height 		= (bounds.Height()-kTopOffset) - (sliderHeight+kBottomOffset);
	
	// Get pixel location of levels
	int32 sliderLeft	= height * volLeft;
	int32 sliderRight 	= height * volRight;
		
	sliderLeft  += sliderHeight+kBottomOffset;
	sliderRight += sliderHeight+kBottomOffset;

	m_LeftSliderRect.left 	 = (bounds.Width()/2) - sliderWidth;
	m_LeftSliderRect.right 	 = m_LeftSliderRect.left + sliderWidth;	
	m_LeftSliderRect.top 	 = bounds.Height() + (-sliderLeft);
	m_LeftSliderRect.bottom  = m_LeftSliderRect.top - sliderHeight;
	
	m_RightSliderRect.left 	 = (bounds.Width()/2);
	m_RightSliderRect.right	 = m_RightSliderRect.left + sliderWidth;	
	m_RightSliderRect.top 	 = bounds.Height() + (-sliderRight);
	m_RightSliderRect.bottom = m_RightSliderRect.top - sliderHeight;
	
	BView::AttachedToWindow();	
}


//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//

void TLevelsSlider::DetachedFromWindow()
{	
	BView::DetachedFromWindow();
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	LoadSliders
//---------------------------------------------------------------------
//
//	Loas slider halves from resources
//

void TLevelsSlider::LoadSliders()
{	

	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);	
	if (file.InitCheck())
		return;
	
	size_t 		size;
	BBitmap 	*data;
	
	BResources res(&file);
	
	// Load left slider
	BRect bounds;
	bounds.Set(0, 0, 8, 23);
	m_LeftSlider = new BBitmap(bounds, B_COLOR_8_BIT);
	ASSERT(m_LeftSlider);
	data = (BBitmap *)res.FindResource('bits', "SliderButtonLeft", &size);
	if (!data)			
		return;
	m_LeftSlider->SetBits(data, size, 0, B_COLOR_8_BIT);
	
	// Load right slider
	m_RightSlider = new BBitmap(bounds, B_COLOR_8_BIT);
	ASSERT(m_RightSlider);
	data = (BBitmap *)res.FindResource('bits', "SliderButtonRight", &size);
	if (!data)			
		return;
	m_RightSlider->SetBits(data, size, 0, B_COLOR_8_BIT);
}


//---------------------------------------------------------------------
//	ClipSliders
//---------------------------------------------------------------------
//
//
void TLevelsSlider::ClipSliders()
{
	ClipLeftSlider();
	ClipRightSlider();
}

//---------------------------------------------------------------------
//	ClipLeftSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::ClipLeftSlider()
{
	const BRect bounds 	= Bounds();
	const uint32 height	= m_LeftSlider->Bounds().IntegerHeight();
	
	// Clip top
	if ( m_LeftSliderRect.top < bounds.top+kTopOffset)
	{
		m_LeftSliderRect.top 	= bounds.top+kTopOffset;
		m_LeftSliderRect.bottom = m_LeftSliderRect.top + height;
	}

	// Clip bottom
	if ( m_LeftSliderRect.bottom > bounds.bottom-kBottomOffset)
	{
		m_LeftSliderRect.bottom = bounds.bottom-kBottomOffset;
		m_LeftSliderRect.top 	= m_LeftSliderRect.bottom - height;
	}
}

//---------------------------------------------------------------------
//	ClipRightSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::ClipRightSlider()
{
	const BRect bounds 	= Bounds();
	const uint32 height	= m_RightSlider->Bounds().IntegerHeight();
	
	// Clip top
	if ( m_RightSliderRect.top < bounds.top+kTopOffset)
	{
		m_RightSliderRect.top 	 = bounds.top+kTopOffset;
		m_RightSliderRect.bottom = m_RightSliderRect.top + height;
	}

	// Clip bottom
	if ( m_RightSliderRect.bottom > bounds.bottom-kBottomOffset)
	{
		m_RightSliderRect.bottom = bounds.bottom-kBottomOffset;
		m_RightSliderRect.top 	 = m_RightSliderRect.bottom - height;
	}
}


//---------------------------------------------------------------------
//	TrackSliders
//---------------------------------------------------------------------
//
//

void TLevelsSlider::TrackSliders()
{
	BPoint newPt, oldPt;
	
	// Check to see if button is down
	uint32 	buttons = 0;
	GetMouse(&newPt, &buttons, true);	
	
	while(buttons)
	{
		// Idle if in same mouse location
		if (newPt.y != oldPt.y)
		{						
			// Get old positions
			BRect oldLeftRect 	= m_LeftSliderRect;
			BRect oldRightRect 	= m_RightSliderRect;
				
			// Move both left and right slider under mouse
			const uint32 leftHeight	 = m_LeftSlider->Bounds().IntegerHeight();
			const uint32 rightHeight = m_RightSlider->Bounds().IntegerHeight();
			
			m_LeftSliderRect.top 		= newPt.y - leftHeight/2;
			m_LeftSliderRect.bottom 	= m_LeftSliderRect.top + leftHeight;
			m_RightSliderRect.top 		= newPt.y - leftHeight/2;
			m_RightSliderRect.bottom 	= m_RightSliderRect.top + rightHeight;
			
			// Now clip the location of the slider if out of bounds
			ClipSliders();
						
			// Clean up old position
			BRect updateRect = oldLeftRect;
			updateRect.right = oldRightRect.right;
			Draw(updateRect);
			
			// Draw new location
			DrawSliders();
			
			// Set new volume level
			UpdateVolumes(newPt);
		
			// Save mouse location
			oldPt = newPt;
		}
		
		// Let other events through
		snooze(20 * 1000);
			
		// Get new mouse location and button state
		GetMouse(&newPt, &buttons, true);
		
		// Clip location
		const BRect bounds = Bounds();
		if ( newPt.y < bounds.top+kTopOffset)
			newPt.y = bounds.top+kTopOffset;
	
		if ( newPt.y > bounds.bottom-kBottomOffset)
			newPt.y = bounds.bottom-kBottomOffset;
	}
}


//---------------------------------------------------------------------
//	TrackLeftSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::TrackLeftSlider()
{
	BPoint newPt, oldPt;
	
	// Check to see if button is down
	uint32 	buttons = 0;
	GetMouse(&newPt, &buttons, true);	
	
	while(buttons)
	{
		// Idle if in same mouse location
		if (newPt.y != oldPt.y)
		{						
			// Get old positions
			BRect oldRect 	= m_LeftSliderRect;
				
			// Move both left and right slider under mouse
			const uint32 leftHeight	 = m_LeftSlider->Bounds().IntegerHeight();
			
			m_LeftSliderRect.top 		= newPt.y - leftHeight/2;
			m_LeftSliderRect.bottom 	= m_LeftSliderRect.top + leftHeight;
			
			// Now clip the location of the slider if out of bounds
			ClipLeftSlider();
						
			// Clean up old position
			Draw(oldRect);
			
			// Draw new location
			DrawLeftSlider();
			
			// Set new volume level
			UpdateLeftVolume(newPt);

			// Save mouse location
			oldPt = newPt;
		}
		
		// Let other events through
		snooze(20 * 1000);
			
		// Get new mouse location and button state
		GetMouse(&newPt, &buttons, true);
		
		// Clip location
		const BRect bounds = Bounds();
		if ( newPt.y < bounds.top+kTopOffset)
			newPt.y = bounds.top+kTopOffset;
	
		if ( newPt.y > bounds.bottom-kBottomOffset)
			newPt.y = bounds.bottom-kBottomOffset;

	}
}


//---------------------------------------------------------------------
//	TrackRightSlider
//---------------------------------------------------------------------
//
//

void TLevelsSlider::TrackRightSlider()
{
	BPoint newPt, oldPt;
	
	// Check to see if button is down
	uint32 	buttons = 0;
	GetMouse(&newPt, &buttons, true);	
	
	while(buttons)
	{
		// Idle if in same mouse location
		if (newPt.y != oldPt.y)
		{						
			// Get old positions
			BRect oldRect 	= m_RightSliderRect;
				
			// Move right slider under mouse
			const uint32 rightHeight = m_RightSlider->Bounds().IntegerHeight();
			
			m_RightSliderRect.top 		= newPt.y - rightHeight/2;
			m_RightSliderRect.bottom 	= m_RightSliderRect.top + rightHeight;
			
			// Now clip the location of the slider if out of bounds
			ClipRightSlider();
						
			// Clean up old position
			Draw(oldRect);
			
			// Draw new location
			DrawRightSlider();
			
			// Set new volume level
			UpdateRightVolume(newPt);

			// Save mouse location
			oldPt = newPt;
		}
		
		// Let other events through
		snooze(20 * 1000);
			
		// Get new mouse location and button state
		GetMouse(&newPt, &buttons, true);
		
		// Clip location
		const BRect bounds = Bounds();
		if ( newPt.y < bounds.top+kTopOffset)
			newPt.y = bounds.top+kTopOffset;
	
		if ( newPt.y > bounds.bottom-kBottomOffset)
			newPt.y = bounds.bottom-kBottomOffset;

	}
}

#pragma mark -
#pragma mark === Volume Update Routines ===

//---------------------------------------------------------------------
//	UpdateVolumes
//---------------------------------------------------------------------
//
//	Update both volume channels based on point location
//

void TLevelsSlider::UpdateVolumes(BPoint where)
{		
	float 		volLeft, volRight;
	//BDACStream 	dacStream;
		
	// Set up slider rects
	const BRect bounds 			= Bounds();
	const uint32 sliderWidth 	= m_LeftSlider->Bounds().IntegerWidth();
	const uint32 sliderHeight 	= m_LeftSlider->Bounds().IntegerHeight();
	uint32 height 				= (bounds.Height()-kTopOffset) - (sliderHeight+kBottomOffset);
	
	// Invert where.y position
	where.y = bounds.bottom - where.y;
			
	// Protect against zero division
	if (height == 0)
		height++;
	
	if (where.y == 0)
		where.y++;
			
	volLeft = volRight = where.y / height;
	
	// Protect against clipping
	if (volLeft > 1.0)
		volLeft = 1.0;
		
	if (volLeft < 0.0)
		volLeft = 0.0;
	
	if (volRight > 1.0)
		volRight = 1.0;

	if (volRight < 0.0)
		volRight = 0.0;
	 	
	// Set volumes
	
	/*	
	// Input
	if (m_Type == kAudioInputSlider)
	{		
		dacStream.SetVolume(B_ADC_IN, volLeft, volRight);
	}
	// Output
	else
	{
		dacStream.SetVolume(B_DAC_OUT, volLeft, volRight); 
	}
	*/
}


//---------------------------------------------------------------------
//	UpdateLeftVolume
//---------------------------------------------------------------------
//
//	Update left volume channel based on point location
//

void TLevelsSlider::UpdateLeftVolume(BPoint where)
{
	float 		volLeft;
	//BDACStream 	dacStream;
		
	// Get current settings
	float oldLeft, oldRight;
	//if (m_Type == kAudioInputSlider)		
	//	dacStream.GetVolume(B_ADC_IN, &oldLeft, &oldRight, NULL);
	//else
	//	dacStream.GetVolume(B_DAC_OUT, &oldLeft, &oldRight, NULL); 

	// Set up slider rects
	const BRect bounds 			= Bounds();
	const uint32 sliderWidth 	= m_LeftSlider->Bounds().IntegerWidth();
	const uint32 sliderHeight 	= m_LeftSlider->Bounds().IntegerHeight();
	uint32 height 				= (bounds.Height()-kTopOffset) - (sliderHeight+kBottomOffset);
	
	// Invert where.y position
	where.y = bounds.bottom - where.y;
			
	// Protect against zero division
	if (height == 0)
		height++;
	
	if (where.y == 0)
		where.y++;
	
	volLeft	 = where.y / height;
	
	// Protect against clipping
	if (volLeft > 1.0)
		volLeft = 1.0;
		
	if (volLeft < 0.0)
		volLeft = 0.0;
			
	// Set volumes
	/*	
	// Input
	if (m_Type == kAudioInputSlider)
	{		
		dacStream.SetVolume(B_ADC_IN, volLeft, oldRight);
	}
	// Output
	else
	{
		dacStream.SetVolume(B_DAC_OUT, volLeft, oldRight); 
	}
	*/	
}


//---------------------------------------------------------------------
//	UpdateRightVolume
//---------------------------------------------------------------------
//
//	Update right volume channel based on point location
//

void TLevelsSlider::UpdateRightVolume(BPoint where)
{
	float 		volRight;
	//BDACStream 	dacStream;
		
	// Get current settings
	float oldLeft, oldRight;
	//if (m_Type == kAudioInputSlider)		
	//	dacStream.GetVolume(B_ADC_IN, &oldLeft, &oldRight, NULL);
	//else
	//	dacStream.GetVolume(B_DAC_OUT, &oldLeft, &oldRight, NULL); 

	// Set up slider rects
	const BRect bounds 			= Bounds();
	const uint32 sliderWidth 	= m_RightSlider->Bounds().IntegerWidth();
	const uint32 sliderHeight 	= m_RightSlider->Bounds().IntegerHeight();
	uint32 height 				= (bounds.Height()-kTopOffset) - (sliderHeight+kBottomOffset);
	
	// Invert where.y position
	where.y = bounds.bottom - where.y;
			
	// Protect against zero division
	if (height == 0)
		height++;
	
	if (where.y == 0)
		where.y++;
	
	volRight = where.y / height;
	
	// Protect against clipping
	if (volRight > 1.0)
		volRight = 1.0;

	if (volRight < 0.0)
		volRight = 0.0;
		
	// Set volumes
	/*	
	// Input
	if (m_Type == kAudioInputSlider)
	{		
		dacStream.SetVolume(B_ADC_IN, oldLeft, volRight);
	}
	// Output
	else
	{
		dacStream.SetVolume(B_DAC_OUT, oldLeft, volRight); 
	}*/	
}
