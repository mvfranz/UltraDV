//-------------------------------------------------------------------
//
//	File:	TSlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Derivative of BSlider for live updates
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSLIDER_H__
#define __TSLIDER_H__

//	Class Definition
class TSlider : public BSlider
{
	public:
		TSlider( BRect frame, const char *name, const char *label, BMessage *message,
			     int32 minValue, int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
				 uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);

		TSlider(BMessage *message);
		
		~TSlider();
				
		virtual	void  SetValue(int32 value);
		
	private:
	
	protected:
	
};

#endif
