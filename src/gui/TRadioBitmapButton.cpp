//---------------------------------------------------------------------
//
//	File:	TRadioBitmapButton.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.13.98
//
//	Desc:	Variant on the Bitmap button object which treats all sibling
//			TRadioBitmapButton objects as radio buttons.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TRadioBitmapButton.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

// Variant ala BPictureButton which requires a call to SetHandler()
TRadioBitmapButton::TRadioBitmapButton(BRect bounds, const char* name, BBitmap* off,
                                       BBitmap* on, BMessage* message, uint32 resizingMode, uint32 flags) :
	// defaults
	//		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP
	//		flags = B_WILL_DRAW | B_NAVIGABLE
	TBitmapButton(bounds, name, off, on, message, B_TWO_STATE_BUTTON, resizingMode, flags),
	fBeingTracked(false)
{
}

TRadioBitmapButton::TRadioBitmapButton(BRect bounds, const char* name, BBitmap* off,
                                       BBitmap* on, BHandler* handler, BMessage* message, uint32 resizingMode,
                                       uint32 flags) :
	// defaults
	//		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP
	//		flags = B_WILL_DRAW | B_NAVIGABLE
	TBitmapButton(bounds, name, off, on, handler, message, B_TWO_STATE_BUTTON,
	              resizingMode, flags),
	fBeingTracked(false)
{
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TRadioBitmapButton::~TRadioBitmapButton()
{
}

//---------------------------------------------------------------------
//	SetValue
//---------------------------------------------------------------------
//

void TRadioBitmapButton::SetValue(int32 value)
{
	if (value == Value())
		return;

	// Never let there be an off value set if the control is being tracked
	if (fBeingTracked && value == 0)
		return;

	// Default behavior
	TBitmapButton::SetValue(value);

	// Turn off all siblings for an 'on' message
	if (value && Parent()) {
		for (BView* sib = Parent()->ChildAt(0); sib; sib = sib->NextSibling()) {
			if (sib != this) {
				TRadioBitmapButton* b = dynamic_cast<TRadioBitmapButton*>(sib);
				if (b)
					b->SetValue(0);
			}
		}
	}
}

//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//

void TRadioBitmapButton::KeyDown(const char* bytes, int32 numBytes)
{
	// Set the tracking flag, used in SetValue
	fBeingTracked = true;
	TBitmapButton::KeyDown(bytes, numBytes);
	fBeingTracked = false;
}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//

void TRadioBitmapButton::MouseDown(BPoint point)
{
	// Set the tracking flag, used in SetValue
	fBeingTracked = true;
	TBitmapButton::MouseDown(point);
	fBeingTracked = false;
}
