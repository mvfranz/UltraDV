//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollerH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Horizontal scroll bar to scroll CueSheet
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#pragma once


//------------------------------------------------------------------------------

class BCueSheetScrollBar : public BScrollBar {

public:
					BScrollBar(	BRect frame,
								const char *name,
								BView *target,
								float min,
								float max,
								orientation direction);
					BScrollBar(BMessage *data);
	~BScrollBar();
			
	void		AttachedToWindow();

	void		ValueChanged(float newValue);

	void		MessageReceived(BMessage *msg);
	void		MouseDown(BPoint pt);
	void		MouseUp(BPoint pt);
	void		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
	void		DetachedFromWindow();
	void		Draw(BRect updateRect);
	void		FrameMoved(BPoint new_position);
	void		FrameResized(float new_width, float new_height);


	private:
		
		// Member Functions
		void Init();


};


