//---------------------------------------------------------------------
//
//	File:	TBitmapButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	Bitmap button object. Derived from BPictureButton
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TBitmapButton.h"
#include <assert.h>

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TBitmapButton::TBitmapButton(BRect bounds, const char* name, BBitmap* off, 
		BBitmap* on, BHandler *handler, BMessage *message, bool ownership,
		uint32 behavior, uint32 resizingMode, uint32 flags) :
		// defaults
		//		ownership = true
		//		behavior = B_ONE_STATE_BUTTON
		//		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP
		//		flags = B_WILL_DRAW | B_NAVIGABLE		
	BPictureButton(bounds, name, new BPicture(), new BPicture(), message, 
			behavior, resizingMode, flags),
	fOnBitmap(on),
	fOffBitmap(off),
	fHandlerHadNoLooper(NULL),
	fOwnership(ownership)
{
	// These cannot be zero
	assert(on && off && handler);
	// handler may not have a Looper at this point, if it hasn't
	// been added to a window. In that case, wait until AttachedToWindow()
	// to set it as a target.
	if (handler->Looper() == 0)
		fHandlerHadNoLooper = handler;
	else {
		fHandlerHadNoLooper = NULL;
		SetTarget(handler);
	}
			
	//	Set background to tranparent to stop flicker
	SetViewColor(B_TRANSPARENT_32_BIT);
}

	// Variant ala BPictureButton which requires a call to SetHandler()
TBitmapButton::TBitmapButton(BRect bounds, const char* name, BBitmap* off, 
		BBitmap* on, BMessage *message, bool ownership, uint32 behavior, 
		uint32 resizingMode, uint32 flags) :
		// defaults
		//		ownership = true
		//		behavior = B_ONE_STATE_BUTTON
		//		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP
		//		flags = B_WILL_DRAW | B_NAVIGABLE
	BPictureButton(bounds, name, new BPicture(), new BPicture(), message, 
			behavior, resizingMode, flags),
	fOnBitmap(on),
	fOffBitmap(off),
	fHandlerHadNoLooper(NULL),
	fOwnership(ownership)
{

	//	Set background to tranparent to stop flicker
	SetViewColor(B_TRANSPARENT_32_BIT);
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TBitmapButton::~TBitmapButton()
{
	// Free data if we never attached to a window
	if (fOwnership)
	{
		if (fOffBitmap)
			delete fOffBitmap;
		
		if (fOffBitmap)
			delete fOnBitmap;
	}
}

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//

void TBitmapButton::AttachedToWindow()
{
	// Create the pictures now that DrawBitmap will work
	BView* parent = Parent();
	if (fOnBitmap != 0) {
		parent->BeginPicture(new BPicture);
		parent->DrawBitmap(fOnBitmap, Bounds());
		SetEnabledOn(parent->EndPicture());

		if (fOwnership) {
			delete fOnBitmap;
			fOnBitmap = 0;
		}
	}

	if (fOffBitmap != 0) {
		parent->BeginPicture(new BPicture);
		parent->DrawBitmap(fOffBitmap, Bounds());
		SetEnabledOff(parent->EndPicture());

		if (fOwnership) {
			delete fOffBitmap;
			fOffBitmap = 0;
		}
	}

	// Connect with a looper
	if (fHandlerHadNoLooper) {
		// Should have one now!
		assert(fHandlerHadNoLooper->Looper());
		SetTarget(fHandlerHadNoLooper);
		fHandlerHadNoLooper = 0;
	}
}

