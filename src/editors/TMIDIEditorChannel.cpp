//---------------------------------------------------------------------
//
//	File:	TMIDIEditorChannel.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.23.98
//
//	Desc:	MIDI editor channel.
//			Display the channel control which includes the volume slider,
//			channel number, instrument name, mute and solo buttons, 
//			pannimg control and track name.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include <app/Application.h>

#include "AppConstants.h"
#include "AppMessages.h"


#include "TMIDIEditor.h"
#include "TMIDIEditorChannel.h"
#include <interface/View.h>

// Constants

	 
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEditorChannel::TMIDIEditorChannel(TMIDIEditor *parent, BRect bounds) : 
	BView(bounds, "MIDIEditorView", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS)
{
	m_Parent = parent;		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIEditorChannel::~TMIDIEditorChannel()
{			
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TMIDIEditorChannel::MouseDown(BPoint where)
{	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TMIDIEditorChannel::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TMIDIEditorChannel::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{

}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TMIDIEditorChannel::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TMIDIEditorChannel::KeyDown(const char *bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TMIDIEditorChannel::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TMIDIEditorChannel::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TMIDIEditorChannel::FrameResized(float new_width, float new_height)
{
	// For now, invalidate the whole screen and get a redraw filled to the 
	// current frame size
	//Invalidate();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TMIDIEditorChannel::MessageReceived(BMessage* message)
{	
	switch (message->what)
	{
						
	}		
}	


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TMIDIEditorChannel::Draw(BRect updateRect)
{	
	
	BPoint startPt, endPt;
	
	// Set up environment
	rgb_color saveColor = HighColor();
	drawing_mode saveMode = DrawingMode();	
	float savePen = PenSize();
	SetPenSize(0.0);
	SetDrawingMode(B_OP_COPY);
			
	// Fill backgound
	SetHighColor(kLightGrey);
	FillRect( Bounds() );
	
	// Set pen color
	SetHighColor(kDarkGrey);
							
	// Restore environment
	SetPenSize(savePen);
	SetDrawingMode(saveMode);
	SetHighColor(saveColor);
}



