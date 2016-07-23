//-------------------------------------------------------------------
//
//	File:	TVideoSettingsSlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoSettingsSlider__
#define __TVideoSettingsSlider__

//	Includes
#include "TMuseumSlider.h"


//	Class Definition
class TVideoSettingsSlider : public TMuseumSlider
{
	public:
		TVideoSettingsSlider( BRect frame, 
						const char *name, const char *label, BMessage *message,
			     		int32 minValue, int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
				 		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 		uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);

		TVideoSettingsSlider(BMessage *message);
		
		~TVideoSettingsSlider();
				
		void  	SetValue(int32 value);
		
	private:
		//	Member Variables
		//TVideoSettingsView *fParent;
	
	protected:
	
};

#endif
