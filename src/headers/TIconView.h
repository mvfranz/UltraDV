//---------------------------------------------------------------------
//
//	File:	TIconView.h
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	This view shows the current media list as icons which are
//			freely positionable by the user. It is a child view to the 
//			Media tab in the Browser window.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TICONVIEW_H__
#define __TICONVIEW_H__ 

// Class Declarations
class TIconView: public BView
{
	public:
		// Member functions
		TIconView(BRect bounds);
		~TIconView();
		
			// BView overrides
		virtual void Draw(BRect updateRect);
		virtual void FrameResized(float w, float h);
		virtual void TargetedByScrollView(BScrollView *view);
		
	private:				
		// Member functions
		void 	Init();
		void	AdjustScrollBars();

		// Member variables
		BScrollView* scrollView;
};		

#endif
