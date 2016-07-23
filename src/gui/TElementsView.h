//---------------------------------------------------------------------
//
//	File:	TElementsView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Child view of TBrowserTabView.  Handles display and sorting
//			of project media elements
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TElementsView__
#define __TElementsView__

// Forward Declarations
class TElementsSorter;	
class TElementsScrollBarH;
class TElementsScrollBarV;

// Class Declarations
class TElementsView: public BView
{
	public:
		// Member functions
		TElementsView(BRect bounds);
		~TElementsView();
		
		void	MessageReceived(BMessage *theMessage);		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void	WindowActivated(bool state);
		void 	FrameResized(float w,float h);
				
			// Make room for the buttons which get superimposed over this
			// view. They go down by the horizontal scroll bar.
		void MakeRoomForButtons(float w);

		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void 	DrawDragRect();
		
		void 	GetDragRect(BRect *dragRect);
		void 	AdjustScrollBars();
				
		// Member variables
		TElementsSorter			*fSorter;
		TElementsScrollBarH		*fHScroll;
		TElementsScrollBarV		*fVScroll;
		
		
};		

#endif
