//---------------------------------------------------------------------
//
//	File:	TRadioBitmapButton.h
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

#ifndef __TRadioBitmapButton__
#define __TRadioBitmapButton__

#include "TBitmapButton.h"

class TRadioBitmapButton : public TBitmapButton
{
	public:
		// Member functions
			// ctor variant ala BPictureButton (client must call SetHandler())
		TRadioBitmapButton(BRect bounds, const char* name, BBitmap* off, 
				BBitmap* on, BMessage *message, uint32 resizingMode = 
				B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | 
				B_NAVIGABLE);
			// ctor variant which lets you set the handler right off
		TRadioBitmapButton(BRect bounds, const char* name, BBitmap* off, 
				BBitmap* on, BHandler *handler, BMessage *message, 
				uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, 
				uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
			// TODO: variant which allows you to pass in a bitmap resource
			// name?
			// Destructor
		virtual ~TRadioBitmapButton();
		
			// Override this to implement radio button behavior. Turns off
			// any sibling TRadioBitmapButtons when this is on. And it never
			// sets the value to 'off' if it is being tracked with the mouse
			// or keyboard.
		virtual void SetValue(int32 value);
			// Don't let the control be turned off if it is being tracked
			// by either one of these functions.
		virtual void KeyDown(const char* bytes, int32 numBytes);
		virtual void MouseDown(BPoint point);

	private:
			// If this is true we never set an off state or send an off
			// message for this object.
		bool fBeingTracked;
};

#endif
