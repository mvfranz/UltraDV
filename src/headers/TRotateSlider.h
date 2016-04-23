//-------------------------------------------------------------------
//
//	File:	TRotateSlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TROTATESLIDER_H__
#define __TROTATESLIDER_H__

//	Includes
#include "TSlider.h"

//	Class Definition
class TRotateSlider : public TSlider
{
	public:
		TRotateSlider( 	TRotateDialog *dialog, TNumberTextControl *rotateText, TVisualCue *theCue, BRect frame, 
						const char *name, const char *label, BMessage *message,
			     		int32 minValue, int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
				 		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 		uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);

		TRotateSlider(BMessage *message);
		
		~TRotateSlider();
				
		void  	SetValue(int32 value);
		void 	SetTextControl(TNumberTextControl *textControl);
		
	private:
		//	Member Variables
		TRotateDialog		*m_Dialog;
		TNumberTextControl 	*m_TextControl;
		TVisualCue			*m_Cue;
		TStageWindow 		*m_Stage;
		TStageView 	 		*m_StageView;
	
	protected:
	
};

#endif
