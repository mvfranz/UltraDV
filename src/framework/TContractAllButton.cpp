//---------------------------------------------------------------------
//
//	File:	TContractAllButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
///	Desc:	Contract All Toolbar button.  Clicking causes all channels to
//			contract to collapsed size 
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
#include "MuseumApp.h"
#include "ResourceManager.h"

#include "TContractAllButton.h"
#include "TToolbar.h"
#include "TCueSheetWindow.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TContractAllButton::TContractAllButton(TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler) : BView(bounds, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	fParent = parent;
	
	// Set MouseDown/MouseUp flag
	fMouseDown = false;
	
	// Store bitmaps
	fOffBitmap = offBitmap;
	fOnBitmap  = onBitmap;
		
	// Store target and handler
	fHandler = handler;
	
	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TContractAllButton::TContractAllButton(BMessage *data) : BView(data)
{
	// Set MouseDown/MouseUp flag
	fMouseDown = false;
		
	fOffBitmap = GetIcon16FromResource("ContractDown");
	fOnBitmap  = GetIcon16FromResource("ContractUp");

}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TContractAllButton::~TContractAllButton()
{
	// Free data
	delete fOffBitmap;
	delete fOnBitmap;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TContractAllButton::Init()
{        	
}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TContractAllButton::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TContractAllButton") ) 
		return new TContractAllButton(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TContractAllButton::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TContractAllButton");
		
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

void TContractAllButton::Draw(BRect updateRect)
{
	// Draw proper bitmap state, if fState is true, draw on bitmap
	if (fMouseDown)
		DrawBitmap(fOffBitmap, B_ORIGIN);		
	else
		DrawBitmap(fOnBitmap, B_ORIGIN);
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TContractAllButton::MouseDown(BPoint where)
{				
	DoClick();
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

void TContractAllButton::AttachedToWindow()
{			
	if(fParent == NULL)
	{
		fParent 	= (TToolbar *)Parent();
		fHandler 	= (TToolbar *)Parent();					
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Click Handling ===

//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Expand All button has been clicked
//

void TContractAllButton::DoClick()
{
	
	// We will always be in the up position when we start.
	// First, set the button state to down and force a redraw...
	fMouseDown = true; 
	Draw(Bounds()); 
			
	// Create and send message to channel and header		
	BMessage *message = new BMessage(CONTRACT_ALL_BUTTON_MSG);
	(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, (BHandler *)static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView(), NULL );
		(static_cast<MuseumApp *>(be_app)->GetCueSheet())->PostMessage(message, (BHandler *)static_cast<MuseumApp *>(be_app)->GetCueSheet(), NULL );
	delete message;
	
	// Wait a short while before restoring to up position
	snooze(100 * 1000);
	
	// Restore and invalidate
	fMouseDown = false; 
	Draw(Bounds()); 
}
