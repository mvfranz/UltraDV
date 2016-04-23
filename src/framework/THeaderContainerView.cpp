//---------------------------------------------------------------------
//
//	File:	THeaderContainerView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.03.98
//
///	Desc:	This view does nothing but hold the channel headers
//			and draw checkered background
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

#include "THeaderContainerView.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

THeaderContainerView::THeaderContainerView(BRect bounds) : BView(bounds, "HeaderContainerView", B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from archive
//

THeaderContainerView::THeaderContainerView(BMessage *message) : BView(message)
{
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

THeaderContainerView::~THeaderContainerView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void THeaderContainerView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *THeaderContainerView::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "THeaderContainerView") ) 
		return new THeaderContainerView(archive); 
		
	return NULL; 
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t THeaderContainerView::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "THeaderContainerView");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{		
		
		}		
	}
	
	Looper()->Unlock();	
	
	return myErr;
}

#pragma mark -
#pragma mark === Drawing Routines ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void THeaderContainerView::Draw(BRect updateRect)
{
	// Save settings
	rgb_color saveColor = HighColor();
	
	pattern squares = { 0xF0, 0xF0, 0xF0, 0xF0, 
						0x0F, 0x0F, 0x0F, 0x0F }; 
						
	// Fill Area behind first header black
	BRect headerFill = Bounds();
	headerFill.bottom = headerFill.top + kChannelMinHeight;
	SetHighColor(kBlack);
	FillRect(headerFill);
	
	// Fill rest with background pattern
	if (updateRect.top <= headerFill.bottom)
		updateRect.top = headerFill.bottom;
		
	SetHighColor(kSteelGrey);
	FillRect(updateRect, squares);
	
	// Resore settings
	SetHighColor(saveColor);
}
