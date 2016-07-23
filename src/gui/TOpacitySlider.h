//-------------------------------------------------------------------
//
//	File:	TOpacitySlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TOPACITYSLIDER_H__
#define __TOPACITYSLIDER_H__

//	Includes
#include "TSlider.h"

//	Class Definition
class TOpacitySlider : public TSlider
{
	public:
		TOpacitySlider( TOpacityDialog *dialog, TNumberTextControl *rotateText, TVisualCue *theCue, BRect frame, 
						const char *name, const char *label, BMessage *message,
			     		int32 minValue, int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
				 		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 		uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);

		TOpacitySlider(BMessage *message);
		
		~TOpacitySlider();
				
		void  	SetValue(int32 value);
		void 	SetTextControl(TNumberTextControl *textControl);
		
	private:
		//	Member Variables
		TOpacityDialog		*fDialog;
		TNumberTextControl 	*fTextControl;
		TVisualCue			*fCue;
		TStageWindow 		*fStage;
		TStageView 	 		*fStageView;
	
	
	protected:
	
};

#endif
