//---------------------------------------------------------------------
//
//	File:	TAudioZoomOutButton.h
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

#ifndef __TAUDIOZOOMOUTBUTTON_H__
#define __TAUDIOZOOMOUTBUTTON_H__ 

// Forward Declarations
class TAudioEditorToolbar;

// Class Declarations
class TAudioZoomOutButton: public BView
{
	public:
		// Member functions
		TAudioZoomOutButton( TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		~TAudioZoomOutButton();
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
		TAudioEditorToolbar	*m_Parent;
		BHandler			*m_Handler;
		bool				m_MouseDown;
		BBitmap				*m_OffBitmap;
		BBitmap				*m_OnBitmap;
		
};		

#endif
