//---------------------------------------------------------------------
//
//	File:	TThumbnailView.h
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	This view shows the current media list as thumbnails showing
//			a quick overview of their contents. They are displayed in a 
// 			list. It is a child view to the Media tab in the Browser window.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TThumbnailView__
#define __TThumbnailView__

// Class Declarations
class TThumbnailView: public BView
{
	public:
		// Member functions
		TThumbnailView(BRect bounds);
		~TThumbnailView();
		
			// BView overrides
		virtual void Draw(BRect updateRect);
		virtual void FrameResized(float w, float h);
		virtual void TargetedByScrollView(BScrollView *view);

		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		void 	AdjustScrollBars();
};		

#endif
