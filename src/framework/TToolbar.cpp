//---------------------------------------------------------------------
//
//	File:	TToolbar.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Toolbar view.  Contains the following tool buttons:
//
//				-- 	Pointer: Resets cursor to selection cursor
//				--	Contract All:  Contracts all tracks
//				--	Expand All:  Expands all tracks
//				--	Razor:  Allows cutting of cues
//				--	Zoom:	Zoom in or out of a cue channel
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
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TCueSheetWindow.h"
#include "TToolbar.h"
#include "TPointerButton.h"
#include "TContractAllButton.h"
#include "TExpandAllButton.h"
#include "TRazorButton.h"
#include "TZoomButton.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TToolbar::TToolbar(BRect bounds, TCueSheetWindow *parent) : BView(bounds, "TooolbarView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP , B_WILL_DRAW)
{
	
	fParent = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create new toolbar from a BMessage
//

TToolbar::TToolbar(BMessage *data) : BView (data)
{
	fParent = NULL;
	
	//	Find child views
	fPointerButton 	= (TPointerButton *)FindView("PointerButton");
	fContractAllButton = (TContractAllButton *)FindView("ContractAllButton");
	fExpandAllButton 	= (TExpandAllButton *)FindView("ExpandAllButton");
	fRazorButton 		= (TRazorButton *)FindView("RazorButton");
	fZoomButton 		= (TZoomButton *)FindView("ZoomButton");
	
	//	Set up buton targets and handlers

}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TToolbar::~TToolbar()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

const short kButtonLeftOffset 	= 5;
const short kButtonTopOffset 	= 2;
const short kToolButtonWidth	= 16;
const short kToolButtonHeight	= 16;

void TToolbar::Init()
{
	//
	// Create the toolbar buttons
	//
	
	BRect bounds;
	
	// Create Pointer button
	BBitmap *pointerUp = GetIcon16FromResource("PointerUp");
	BBitmap *pointerDn = GetIcon16FromResource("PointerDown");
	bounds = fParent->Bounds();
	bounds.Set( kButtonLeftOffset, bounds.top + kButtonTopOffset, kToolButtonWidth+kButtonLeftOffset, 
				bounds.top + kButtonTopOffset + kToolButtonHeight );
	fPointerButton = new TPointerButton(this, bounds, "PointerButton", pointerDn, pointerUp, this); 
	AddChild(fPointerButton);
	fPointerButton->Show();
	
	// Create ContractAll button
	BBitmap *contractUp = GetIcon16FromResource("ContractUp");
	BBitmap *contractDn = GetIcon16FromResource("ContractDown");
	bounds = fPointerButton->Frame();
	bounds.Set( bounds.right + kButtonLeftOffset, bounds.top, bounds.right + kToolButtonWidth+kButtonLeftOffset, bounds.bottom );
	fContractAllButton = new TContractAllButton(this, bounds, "ContractAllButton", contractDn, contractUp, this); 
	AddChild(fContractAllButton);
	fContractAllButton->Show();

	// Create ExpandAll button
	BBitmap *expandUp = GetIcon16FromResource("ExpandUp");
	BBitmap *expandDn = GetIcon16FromResource("ExpandDown");
	bounds = fContractAllButton->Frame();
	bounds.Set( bounds.right + kButtonLeftOffset, bounds.top, bounds.right + kToolButtonWidth+kButtonLeftOffset, bounds.bottom );
	fExpandAllButton = new TExpandAllButton(this, bounds, "ExpandAllButton", expandDn, expandUp, this); 
	AddChild(fExpandAllButton);
	fExpandAllButton->Show();
	
	// Create Razor button
	BBitmap *razorUp = GetIcon16FromResource("RazorUp");
	BBitmap *razorDn = GetIcon16FromResource("RazorDown");
	bounds = fExpandAllButton->Frame();
	bounds.Set( bounds.right + kButtonLeftOffset, bounds.top, bounds.right + kToolButtonWidth+kButtonLeftOffset, bounds.bottom );
	fRazorButton = new TRazorButton(this, bounds, "RazorButton", razorDn, razorUp, this); 
	AddChild(fRazorButton);
	fRazorButton->Show();
	
	// Create Zoom button
	BBitmap *zoomUp = GetIcon16FromResource("ZoomUp");
	BBitmap *zoomDn = GetIcon16FromResource("ZoomDown");
	bounds = fRazorButton->Frame();
	bounds.Set( bounds.right + kButtonLeftOffset, bounds.top, bounds.right + kToolButtonWidth+kButtonLeftOffset, bounds.bottom );
	fZoomButton = new TZoomButton(this, bounds, "ZoomButton", zoomDn, zoomUp, this); 
	AddChild(fZoomButton);
	fZoomButton->Show();

}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TToolbar::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TToolbar") ) 
		return new TToolbar(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TToolbar::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TToolbar");
		
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
#pragma mark === Drawing Functions ===
			
//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TToolbar::Draw(BRect updateRect)
{
	BPoint startPt, endPt;
	
	const BRect bounds = Bounds();
	
	// Save old colors
	PushState();
		
	// Fill rect
	SetHighColor(kLightGrey);
	FillRect(updateRect);
	
	//	Draw top and bottom lines
	SetHighColor(kBlack);
	startPt.Set(updateRect.left, bounds.top);
	endPt.Set(updateRect.right, bounds.top);
	StrokeLine(startPt, endPt);	
	
	startPt.Set(updateRect.left, bounds.bottom);
	endPt.Set(updateRect.right, bounds.bottom);
	StrokeLine(startPt, endPt);	
	
	// Restore environment
	PopState();
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TToolbar::AttachedToWindow()
{			
	if(fParent == NULL)
	{
		fParent = (TCueSheetWindow *)Window();		
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//
void TToolbar::SetParent(TCueSheetWindow *parent)
{
 	fParent = parent;
}