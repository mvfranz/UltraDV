//-------------------------------------------------------------------
//
//	File:	TTransitionView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.12.98
//
//-------------------------------------------------------------------

#if !defined(__TTRANSITIONVIEW_H__)
#define __TTRANSITIONVIEW_H__ 

//	Includes
#include "TCueEffectView.h"
#include "TTransitionEffect.h"
#include "TCueView.h"

// Class Definition
class TTransitionView : public TCueEffectView
{
	public:
		// Member Functions
		TTransitionView(BRect bounds, TCueView *theCue, TCueEffect *theEffect);
		virtual ~TTransitionView();
		
		//	Drawing routines				
		virtual void 	Draw(BRect updateRect);
		
		virtual void 	MouseDown(BPoint where, bool doubleClick = false);
		virtual void 	MouseMoved(	BPoint where);
		
		virtual void 	Resized();			
		
		// Accessor Functions
		
	private:
		void	Init();
		
		void 	ResizeRightSide();
		void 	ResizeLeftSide();
							
	protected:
	
};


#endif
