//---------------------------------------------------------------------
//
//	File:	TActionCuePaletteView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	MediaCue sheet view
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "ResourceManager.h"

#include "TMuseumIcons.h"
#include "TActionCuePaletteView.h"
#include "TCueButton.h"


// Constants


// 	I am subtracting l pixel of the width and height to create an actual 
// 	width offset from 0 (ie: 0-31 = 32 pixels)
// 	The media cue button itself is offset form thesides and top of the
//	surrounding palette area.
const BRect	kCueButtonBounds(0, 0, kCueIconWidth-1, kCueIconHeight-1);

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TActionCuePaletteView::TActionCuePaletteView(BRect bounds) : BView(bounds, "ActionCueView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TActionCuePaletteView::~TActionCuePaletteView()
{
	// Cue button bitmaps are owned by TMuseumIcon object
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TActionCuePaletteView::Init()
{
	BBitmap 	*offBitmap;
	BBitmap		*onBitmap;	
	
	// Set view background color
	SetViewColor(kSteelGrey);
	
	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;
		
	BResources res(&file);
	
	
	//
	// Create the media cue icon subviews
	//
	
	BRect cueRect = Bounds();
	
	// Action Cue
	//
	
	// Set button location
	cueRect.Set( kCueButtonOffset, kCueButtonOffset, kCueIconWidth+kCueButtonOffset-1, kCueIconHeight+kCueButtonOffset-1);
	
	// Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_ActionIcon;
// ABH	onBitmap 	= GetAppIcons()->m_ActionIcon;
	
	// Create button
	m_ActionCueButton = new TCueButton(cueRect, "ActionCue", offBitmap, onBitmap, new BMessage(ACTION_CUE_MSG), kActionCue);
	AddChild(m_ActionCueButton);
         
    
    //
    //	Button Cue
    
    // Set button location
    cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;

    // Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_ButtonIcon;
// ABH	onBitmap 	= GetAppIcons()->m_ButtonIcon;

	// Create button
	m_ButtonCueButton = new TCueButton(cueRect, "ButtonCue", offBitmap, onBitmap, new BMessage(BUTTON_CUE_MSG), kButtonCue);
	AddChild(m_ButtonCueButton);
	
	
    // CD Cue
    //
    
    // Set button location
	cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;
	
    // Load bitmaps
//ABH 	offBitmap 	= GetAppIcons()->m_CDIcon;
//ABH	onBitmap 	= GetAppIcons()->m_CDIcon;

	// Create button
	m_CDCueButton = new TCueButton(cueRect, "CDCue", offBitmap, onBitmap, new BMessage(CD_CUE_MSG), kCDCue);
	AddChild(m_CDCueButton);
	
    
    // Control Cue
    //
    
 	// Set button location
	cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;
    
    // Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_ControlIcon;
// ABH	onBitmap 	= GetAppIcons()->m_ControlIcon;

	// Create button
	m_ControlCueButton = new TCueButton(cueRect, "ControlCue", offBitmap, onBitmap, new BMessage(CONTROL_CUE_MSG), kControlCue);
	AddChild(m_ControlCueButton);
	
    
    // Marker Cue
    //
    
    // Set button location
	cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;
				 
    // Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_MarkerIcon;
// ABH	onBitmap 	= GetAppIcons()->m_MarkerIcon;
	
	// Create button
	m_MarkerCueButton = new TCueButton(cueRect, "", offBitmap, onBitmap, new BMessage(MARKER_CUE_MSG), kMarkerCue);
	AddChild(m_MarkerCueButton);
	
	
    
    // Pause Cue
    //
    
	// Set button location
	cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;
				 			 
    // Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_PauseIcon;
// ABH	onBitmap 	= GetAppIcons()->m_PauseIcon;
	
	// Create button
	m_PauseCueButton = new TCueButton(cueRect, "PauseCue", offBitmap, onBitmap, new BMessage(PAUSE_CUE_MSG), kPauseCue);
	AddChild(m_PauseCueButton);


    // Video Cue
    //
    
	// Set button location
	cueRect.top 	= cueRect.top+kCueIconHeight+kCueTextOffset;
	cueRect.bottom 	= cueRect.top+kCueIconHeight-1;

	// Load bitmaps
// ABH	offBitmap 	= GetAppIcons()->m_VideoIcon;
// ABH	onBitmap 	= GetAppIcons()->m_VideoIcon;
	
	// Create button
	m_VideoCueButton = new TCueButton(cueRect, "VideoCue", offBitmap, onBitmap, new BMessage(VIDEO_CUE_MSG), kVideoCue);
	AddChild(m_VideoCueButton);
       	
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TActionCuePaletteView::Draw(BRect updateRect)
{

	rgb_color saveColor = HighColor();
	
	// Setup font
	BFont saveFont; 
   	GetFont(&saveFont); 
	SetFont(be_bold_font);   
	SetFontSize(10);   
   	SetHighColor(kBlack);
   	
	// Draw the cue item text descriptions
	//
	
	float 	stringWidth;
	BPoint 	textPt;
	
	// Action
	textPt.y = kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Action");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Action", textPt); 
	
	// Button
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Button");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Button", textPt); 
	
	// CD
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("CD");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("CD", textPt); 

	// Control
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Control");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Control", textPt); 
	
	// Marker
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Marker");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Marker", textPt); 
	
	// Pause
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Pause");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Pause", textPt); 
	
	// Video
	textPt.y += kCueIconHeight+(kCueButtonOffset*3);
	stringWidth = StringWidth("Video");
	textPt.x = ( (Bounds().right - Bounds().left)/ 2) - (stringWidth /2);
	DrawString("Video", textPt); 
	
	// Restore environment	
	SetFont(&saveFont);  
	SetHighColor(saveColor);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TActionCuePaletteView::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TActionCuePaletteView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TActionCuePaletteView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TActionCuePaletteView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TActionCuePaletteView::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TActionCuePaletteView::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TActionCuePaletteView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TActionCuePaletteView::FrameResized(float new_width, float new_height)
{
}

