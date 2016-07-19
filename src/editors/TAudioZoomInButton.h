//---------------------------------------------------------------------
//
//	File:	TAudioZoomInButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOZOOMINBUTTON_H__
#define __TAUDIOZOOMINBUTTON_H__ 

// Forward Declarations
class TAudioEditorToolbar;

// Class Declarations
class TAudioZoomInButton: public BView
{
	public:
		// Member functions
		TAudioZoomInButton( TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		~TAudioZoomInButton();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void WindowActivated(bool state);
		void KeyDown(const char *bytes, int32 numBytes);
		void KeyUp(const char *bytes, int32 numBytes);
		void FrameMoved(BPoint new_position);
		void FrameResized(float new_width, float new_height);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		void 	DoClick();
		
		// Member variables
		TAudioEditorToolbar	*fParent;
		BHandler			*fHandler;
		bool				fMouseDown;
		BBitmap				*fOffBitmap;
		BBitmap				*fOnBitmap;
		
};		

#endif
