//---------------------------------------------------------------------
//
//	File:	TMIDIEditorView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.23.98
//
//	Desc:	MIDI editor view.  Handle display of MIDI mixer and channels
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "AppConstants.h"
#include "AppMessages.h"


#include "TMIDIEditor.h"
#include "TMIDIEditorView.h"


// Constants

	 
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMIDIEditorView::TMIDIEditorView(TMIDIEditor *parent, BRect bounds) : BView(bounds, "MIDIEditorView", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS)
{
	fEditor = parent;	
	
	// Default initialization
	Init();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMIDIEditorView::~TMIDIEditorView()
{			
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TMIDIEditorView::MouseDown(BPoint where)
{	
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TMIDIEditorView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TMIDIEditorView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{

}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TMIDIEditorView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TMIDIEditorView::KeyDown(const char *bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TMIDIEditorView::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TMIDIEditorView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TMIDIEditorView::FrameResized(float new_width, float new_height)
{
	// For now, invalidate the whole screen and get a redraw filled to the 
	// current frame size
	//Invalidate();
}


#pragma mark -
#pragma mark === Initialization ===

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMIDIEditorView::Init()
{
	// Create the correct number of mixer items


}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TMIDIEditorView::MessageReceived(BMessage* message)
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

void TMIDIEditorView::Draw(BRect updateRect)
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



