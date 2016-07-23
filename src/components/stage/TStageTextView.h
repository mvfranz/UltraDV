//-------------------------------------------------------------------
//
//	File:	TStageTextView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.28.98
//
//-------------------------------------------------------------------

#ifndef _TSTAGETEXTVIEW_H_
#define _TSTAGETEXTVIEW_H_

#include "TStageCue.h"

// Forward Declarations

// Class Definition
class TStageTextView : public TStageCue
{
	public:
		TStageTextView(BRect bounds, char *name, TCueView *theCue);
		~TStageTextView();
		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void 	WindowActivated(bool state);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	KeyUp(const char *bytes, int32 numBytes);
		void 	FrameMoved(BPoint new_position);
		void 	FrameResized(float new_width, float new_height);
			
		
	private:
			
						
};




#endif	// _TSTAGETEXTVIEW_H_
