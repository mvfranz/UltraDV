//---------------------------------------------------------------------
//
//	File:	TElementsScrollBarV.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Vertical scroll bar to scroll Elements View
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "TSorterContainer.h"
#include "TElementsSorter.h"

#include "TElementsScrollBarV.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TElementsScrollBarV::TElementsScrollBarV(BRect frame, const char *name, TElementsSorter *target, float min, float max ) :
					BScrollBar( frame, name, NULL, min, max, B_VERTICAL)
{	
	m_Parent = target;
	
	// Do default initialization
	Init();
}					


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TElementsScrollBarV::~TElementsScrollBarV()
{

}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::Init()
{
	// 	Get current scrollvalue and save it in our tracking variable.  
	//	We use this value to scroll the HeaderZone in concert with
	//	the CueSheetView
	m_LastValue = Value();
}



//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::AttachedToWindow()
{
}


//---------------------------------------------------------------------
//	ValueChanged
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::ValueChanged(float newValue)
{
	// Pass message to base class
	BScrollBar::ValueChanged(newValue);
	
	//  We need to scroll the SorterList areas of our attached SorterContainers
	if (newValue != m_LastValue)
	{
		bool negative;
		
		// Construct message to send parent
		BMessage *theMessage = new BMessage(SORTER_SCROLL_V_MSG);
		
		if (newValue > m_LastValue)
		{
			negative = false;
			theMessage->AddBool("Negative", negative); 
			theMessage->AddFloat("NewValue", newValue); 
			theMessage->AddFloat("LastValue", m_LastValue); 
			m_Parent->MessageReceived(theMessage);
		}
		else
		{
			negative = true;
			theMessage->AddBool("Negative", negative); 
			theMessage->AddFloat("NewValue", newValue); 
			theMessage->AddFloat("LastValue", m_LastValue); 
			m_Parent->MessageReceived(theMessage);
		}
			
		// Save value for next compare
		m_LastValue = newValue;
		
		delete theMessage;
	}		
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::MessageReceived(BMessage *msg)
{
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::MouseDown(BPoint pt)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::MouseUp(BPoint pt)
{
}



//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
}



//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::DetachedFromWindow()
{
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::Draw(BRect updateRect)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::FrameMoved(BPoint new_position)
{
}



//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TElementsScrollBarV::FrameResized(float new_width, float new_height)
{
}



