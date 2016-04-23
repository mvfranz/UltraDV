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
	m_OnBitmap(on),
	m_OffBitmap(off),
	m_HandlerHadNoLooper(NULL),
	m_Ownership(ownership)
{
	// These cannot be zero
	assert(on && off && handler);
	// handler may not have a Looper at this point, if it hasn't
	// been added to a window. In that case, wait until AttachedToWindow()
	// to set it as a target.
	if (handler->Looper() == 0)
		m_HandlerHadNoLooper = handler;
	else {
		m_HandlerHadNoLooper = NULL;
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
	m_OnBitmap(on),
	m_OffBitmap(off),
	m_HandlerHadNoLooper(NULL),
	m_Ownership(ownership)
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
	if (m_Ownership)
	{
		if (m_OffBitmap)
			delete m_OffBitmap;
		
		if (m_OffBitmap)
			delete m_OnBitmap;
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
	if (m_OnBitmap != 0) {
		parent->BeginPicture(new BPicture);
		parent->DrawBitmap(m_OnBitmap, Bounds());
		SetEnabledOn(parent->EndPicture());

		if (m_Ownership) {
			delete m_OnBitmap;
			m_OnBitmap = 0;
		}
	}

	if (m_OffBitmap != 0) {
		parent->BeginPicture(new BPicture);
		parent->DrawBitmap(m_OffBitmap, Bounds());
		SetEnabledOff(parent->EndPicture());

		if (m_Ownership) {
			delete m_OffBitmap;
			m_OffBitmap = 0;
		}
	}

	// Connect with a looper
	if (m_HandlerHadNoLooper) {
		// Should have one now!
		assert(m_HandlerHadNoLooper->Looper());
		SetTarget(m_HandlerHadNoLooper);
		m_HandlerHadNoLooper = 0;
	}
}

