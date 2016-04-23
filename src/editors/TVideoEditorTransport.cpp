//---------------------------------------------------------------------
//
//	File:	TVideoEditorTransport.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//	Desc:	Video editor shuttle control
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"

#include "TVideoEditor.h"

#include "TVideoEditorTransport.h"


// Constants
const int kSliderWidth  = 11;
const int kSliderHeight = 13;

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditorTransport::TVideoEditorTransport(TVideoEditor *editor, BMessage *message) : BView(message)
{	
	m_Editor = editor;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoEditorTransport::~TVideoEditorTransport()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoEditorTransport::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);	
	
	// Set up slider rect and location
	const BRect bounds  = Bounds();
	m_SliderRect.left 	= bounds.left+2;
	m_SliderRect.right 	= m_SliderRect.left + kSliderWidth;
	m_SliderRect.top 	= bounds.top + 4;
	m_SliderRect.bottom = m_SliderRect.top + kSliderHeight;
	
}


#pragma mark -
#pragma mark === Message Handling ===


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draw contents
//

void TVideoEditorTransport::Draw(BRect updateRect)
{
	PushState();
	
	const BRect bounds = Bounds();
	
	//const rgb_color kBeGrey 		= {216, 216, 216, 255};
	// Fill Rect
	SetHighColor(kBeGrey);
	FillRect(bounds);
	
	// Draw outline
	SetHighColor(kTextHilite);
	BPoint startPt, endPt;
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);				
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left+1, bounds.bottom);												
	StrokeLine(startPt, endPt);
	SetHighColor(kTextShadow);
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);												
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right-1, bounds.top);												
	StrokeLine(startPt, endPt);
	
	// Draw black rect
	BRect blackRect = bounds;
	blackRect.InsetBy(1, 1);
	SetHighColor(kBlack);
	StrokeRect(blackRect);
	
	// Draw channel slot
	BRect slot 		= bounds;
	slot.left+=2;
	slot.right-=2;
	slot.top 	+= bounds.top + ( (bounds.Height()/2)-2);
	slot.bottom -= bounds.bottom - ( (bounds.Height()/2)+2);
	SetHighColor(kBlack);
	FillRect(slot);
	
	// Draw slot hilites
	startPt.Set(slot.left, slot.top);
	endPt.Set(slot.right, slot.top);
	SetHighColor(kDarkGrey);
	StrokeLine(startPt, endPt);
	
	slot.top	+= 3;
	slot.left 	+= 1;
	slot.right 	-= 2;
	rgb_color dkGrey = {35,35,35,255};
	SetHighColor(dkGrey);
	FillRect(slot);
		
	// Draw slider
	DrawSlider();
	
	PopState();
}


//---------------------------------------------------------------------
//	DrawSlider
//---------------------------------------------------------------------
//
//	Draw transport thumb
//

void TVideoEditorTransport::DrawSlider()
{	
	PushState();
	
	BPoint startPt, endPt;
	
	// Fill
	SetHighColor(kLightGrey);
	FillRect(m_SliderRect);
	
	// Black outline
	SetHighColor(kBlack);
	StrokeRect(m_SliderRect);
	
	// White highlight
	SetHighColor(kWhite);
	startPt.Set(m_SliderRect.left+1, m_SliderRect.bottom-1);	
	endPt.Set(m_SliderRect.left+1, m_SliderRect.top+1);	
	StrokeLine(startPt, endPt);
	endPt.Set(m_SliderRect.right-1, m_SliderRect.top+1);	
	StrokeLine(endPt);
	
	// Grey shadow
	SetHighColor(kMediumGrey);
	endPt.Set(m_SliderRect.right-1, m_SliderRect.bottom-1);	
	StrokeLine(endPt);
	endPt.Set(m_SliderRect.left+1, m_SliderRect.bottom-1);	
	StrokeLine(endPt);
	
	// Draw thumb notches
	SetHighColor(kDarkGrey);
	startPt.Set(m_SliderRect.left+2, m_SliderRect.top+2);	
	endPt.Set(m_SliderRect.left+2, m_SliderRect.bottom-2);	
	StrokeLine(startPt, endPt);
	startPt.Set(m_SliderRect.left+5, m_SliderRect.top+2);	
	endPt.Set(m_SliderRect.left+5, m_SliderRect.bottom-2);	
	StrokeLine(startPt, endPt);
	startPt.Set(m_SliderRect.left+8, m_SliderRect.top+2);	
	endPt.Set(m_SliderRect.left+8, m_SliderRect.bottom-2);	
	StrokeLine(startPt, endPt);

	PopState();
}
