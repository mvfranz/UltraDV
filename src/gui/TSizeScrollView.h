//---------------------------------------------------------------------
//
//	File:	TSizeScrollView.h
//
//	Author:	Mike Ost
//
//	Date:	08.18.98
//
//	Desc:	This class is derived from BScrollView, with an additional
//			wrinkle: it has a size control and sends sizing messages
//			to its container window. Other interface behaves like 
//			BScrollView.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSizeScrollView__
#define __TSizeScrollView__ 

// Class Declarations
class TSizeScrollView: public BScrollView
{
	public:
		// Member functions
		TSizeScrollView(const char *name, BView *target,
				uint32 resizeMask = B_FOLLOW_LEFT |	B_FOLLOW_TOP,
				uint32 flags = 0, bool horizontal = false,
				bool vertical = false, border_style border = 
				B_FANCY_BORDER);
		// TODO: TSizeScrollView(BMessage *data);
		~TSizeScrollView();
		
			// BView overrides
		virtual void Draw(BRect updateRect);
		virtual void MouseDown(BPoint where);
		virtual void MessageReceived(BMessage *theMessage);
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		void 	AdjustScrollBars();
			// Draw the simulated drag rect.
		void 	DrawDragRect();
			// Return the location of the drag rect
		void	GetDragRect(BRect *dragRect);
};		

#endif
