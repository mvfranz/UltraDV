//---------------------------------------------------------------------
//
//	File:	TElementsScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Custom Horizontal scroll bar to scroll Elements View
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TElementsScrollBarH__
#define __TElementsScrollBarH__

// Class Definition
class TElementsScrollBarH : public BScrollBar 
{

	public:
	
		// Member Functions
		TElementsScrollBarH(BRect frame, const char *name, BView *target, float min, float max);
		~TElementsScrollBarH();
			
		void	AttachedToWindow();
		void	ValueChanged(float newValue);
		void	MessageReceived(BMessage *msg);
		void	MouseDown(BPoint pt);
		void	MouseUp(BPoint pt);
		void	MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
		void	DetachedFromWindow();
		void	Draw(BRect updateRect);
		void	FrameMoved(BPoint new_position);
		void	FrameResized(float new_width, float new_height);


	private:
	
		//Member Functions
		void Init();
		
		// Member Variables
		float			m_LastValue;

};

#endif
