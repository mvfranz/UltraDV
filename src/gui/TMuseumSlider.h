//---------------------------------------------------------------------
//
//	File:	TMuseumSlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Custom BSlider
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TMuseumSlider__
#define __TMuseumSlider__

// Class Definition

class TMuseumSlider : public BSlider
{
	public:
		TMuseumSlider( 	BRect frame, const char *name, const char *label, BMessage *message,
						int32 minValue, int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
						uint32 resizingMode = B_FOLLOW_LEFT|B_FOLLOW_TOP, 
						uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
		
		TMuseumSlider(BMessage *message);
		
		~TMuseumSlider();
			
		// Drawing Functions
		void 			DrawText();
						
	private:
	
};	

#endif
