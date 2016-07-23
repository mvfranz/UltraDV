//---------------------------------------------------------------------
//
//	File:	TCueButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUEBUTTON_H__
#define __TCUEBUTTON_H__

// Forward Declarations

// Class Declarations
class TCueButton: public BView
{
	public:
		// Member functions
		TCueButton( BRect bounds, const char *name, BBitmap *offBitmap, 
					BBitmap *onBitmap, BMessage *message, int16 ID);
		~TCueButton();
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
		
		
		// Member variables
		bool		fMouseDown;
		int16		fID;
		BBitmap		*fOffBitmap;
		BBitmap		*fOnBitmap;
		BMessage 	*fMessage;
		bool		fState;
};		

#endif
