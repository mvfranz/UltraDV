//---------------------------------------------------------------------
//
//	File:	TElementsTrackerView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TELEMENTS_TRACKER_VIEW_H
#define __TELEMENTS_TRACKER_VIEW_H


// Class Declarations
class TElementsTrackerView: public BView
{
	public:
		// Member functions
		TElementsTrackerView(BRect bounds, TElementsView *parent);
		~TElementsTrackerView();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		//void WindowActivated(bool state);
		//void KeyDown(const char *bytes, int32 numBytes);
		//void KeyUp(const char *bytes, int32 numBytes);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
		// Member variables
		TElementsView	*m_Parent;
};		

#endif
