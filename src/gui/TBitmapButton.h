//---------------------------------------------------------------------
//
//	File:	TBitmapButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	A class which allows a BBitmap to be used on a BPictureButton.
//			NOTE: this object takes ownership of the BBitmaps passed in to it.
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TBITMAPBUTTON_H__
#define __TBITMAPBUTTON_H__ 

// Forward Declarations

// Class Declarations
class TBitmapButton: public BPictureButton
{
	public:
		// Member functions
			// ctor variant ala BPictureButton (client must call SetHandler())
		TBitmapButton(BRect bounds, const char* name, BBitmap* off, 
				BBitmap* on, BMessage *message, bool ownership = true, 
				uint32 behavior = B_ONE_STATE_BUTTON, uint32 resizingMode = B_FOLLOW_LEFT | 
				B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
			// ctor variant which lets you set the handler right off
		TBitmapButton(BRect bounds, const char* name, BBitmap* off, 
				BBitmap* on, BHandler *handler, BMessage *message, bool ownership = true, 
				uint32 behavior = B_ONE_STATE_BUTTON, uint32 resizingMode = 
				B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | 
				B_NAVIGABLE);
			// TODO: variant which allows you to pass in a bitmap resource
			// name?
			// Destructor
		virtual ~TBitmapButton();
			// This method is needed to render the bitmap into a picture in
			// useable form.
		virtual void AttachedToWindow();

	protected:
		// Member variables	
			// Hang on to these until we are attached to a window. Then
			// you can render them into a picture. Takes ownership of these
			// when passed into the ctor.
		BBitmap* fOffBitmap;
		BBitmap* fOnBitmap;
			// If the handler has no BLooper at ctor time, save it for
			// AttachedToWindow() to call SetTarget().
		BHandler *fHandlerHadNoLooper;
		
			//	If this is true, we need to delete bitmaps passed in
		bool fOwnership;
};		

#endif
