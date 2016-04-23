//---------------------------------------------------------------------
//
//	File:	TAudioEditorToolbar.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//	Desc:	Audio editor toolbar.  Contains these tools:
//			--	Play
//			-- 	Stop
//			--	Zoom In
//			-- 	Zoom Out
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TAudioEditorToolbar.h"
#include "TAudioPlayButton.h"
#include "TAudioZoomInButton.h"	
#include "TAudioZoomOutButton.h"


// Constants

	 
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioEditorToolbar::TAudioEditorToolbar(TAudioEditor *parent, BRect bounds) : BView(bounds, "AudioToolbar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{

	m_Parent = parent;
	
	// Default Initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioEditorToolbar::~TAudioEditorToolbar()
{
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Fill with grey background
//

void TAudioEditorToolbar::Draw(BRect updateRect)
{	
	// Setup environment
	rgb_color saveColor = HighColor();
		
	// Fill background
	SetHighColor(kLightGrey);
	FillRect(Bounds());
	
	// Frame it
	BPoint endPt;	
	SetHighColor(kWhite);	
	MovePenTo(Bounds().left, Bounds().top+1);
	endPt.Set( Bounds().right, Bounds().top+1 );
	StrokeLine(endPt);
	
	SetHighColor(kMediumGrey);
	MovePenTo(Bounds().left, Bounds().bottom-1);
	endPt.Set( Bounds().right, Bounds().bottom-1 );
	StrokeLine(endPt);
	
	SetHighColor(kBlack);
	MovePenTo(Bounds().left, Bounds().top);
	endPt.Set( Bounds().right, Bounds().top );
	StrokeLine(endPt);	
	MovePenTo(Bounds().left, Bounds().bottom);
	endPt.Set( Bounds().right, Bounds().bottom );
	StrokeLine(endPt);
				
	// Restore Environment
	SetHighColor(saveColor);	
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAudioEditorToolbar::MouseDown(BPoint where)
{	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioEditorToolbar::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioEditorToolbar::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioEditorToolbar::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioEditorToolbar::KeyDown(const char *bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioEditorToolbar::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioEditorToolbar::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioEditorToolbar::FrameResized(float new_width, float new_height)
{
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Create toolbar buttons
//

const short kButtonLeftOffset 	= 5;
const short kButtonTopOffset 	= 4;
const short kToolButtonWidth	= 16;
const short kToolButtonHeight	= 16;

void TAudioEditorToolbar::Init()
{
	//
	// Create the toolbar buttons
	//
	
	BRect bounds;
	
	// Create Play button
	BBitmap *playUp = GetIcon16FromResource("AudioPlayUp");
	BBitmap *playDn = GetIcon16FromResource("AudioPlayDn");	
	bounds = Bounds();
	bounds.Set( kButtonLeftOffset, bounds.top + kButtonTopOffset, kToolButtonWidth+kButtonLeftOffset, 
				bounds.top + kButtonTopOffset + kToolButtonHeight );
	m_PlayButton= new TAudioPlayButton(this, bounds, "AudioPlayButton", playDn, playUp, Window()); 
	AddChild(m_PlayButton);
	m_PlayButton->Show();
	
	
	// Create Zoom In Button
	BBitmap *zoomInUp = GetIcon16FromResource("AudioZoomInUp");
	BBitmap *zoomInDn = GetIcon16FromResource("AudioZoomInDn");
	bounds = Bounds();
	bounds.Set( m_PlayButton->Frame().right + kButtonLeftOffset, bounds.top + kButtonTopOffset, 
				m_PlayButton->Frame().right + kToolButtonWidth+kButtonLeftOffset, bounds.top + kButtonTopOffset + kToolButtonHeight);
	m_ZoomInButton= new TAudioZoomInButton(this, bounds, "AudioZoomInButton", zoomInDn, zoomInUp, this); 
	AddChild(m_ZoomInButton);
	m_ZoomInButton->Show();
	
	// Create Zoom Out Button
	BBitmap *zoomOutUp = GetIcon16FromResource("AudioZoomOutUp");
	BBitmap *zoomOutDn = GetIcon16FromResource("AudioZoomOutDn");
	bounds = Bounds();
	bounds.Set( m_ZoomInButton->Frame().right + kButtonLeftOffset, bounds.top + kButtonTopOffset, 
				m_ZoomInButton->Frame().right + kToolButtonWidth+kButtonLeftOffset, bounds.top + kButtonTopOffset + kToolButtonHeight);
	m_ZoomOutButton= new TAudioZoomOutButton(this, bounds, "AudioZoomInButton", zoomOutDn, zoomOutUp, this); 
	AddChild(m_ZoomOutButton);
	m_ZoomOutButton->Show();
			
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioEditorToolbar::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{		
	}		
}	
