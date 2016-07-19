//---------------------------------------------------------------------
//
//	File:	TMediaTabView.h
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	Sub-class of TElementsView. Handles the tab labelled 'Media'
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TMEDIATABVIEW_H__
#define __TMEDIATABVIEW_H__ 

// Forware decl
class TRadioBitmapButton;

// Class Declarations
class TMediaTabView : public BView
{
	public:
		// Member functions
		TMediaTabView(BRect bounds);
		~TMediaTabView();
		
		// Overrides
		virtual void MessageReceived(BMessage* message);
			// This override is needed to resize this tab view when
			// the window is shown. A bug in the BTabView class keeps
			// the resize information from getting to tabs unless they
			// are visible.
		virtual void Show();

	private:
		// Member data
			// These show the order of the children added to TMediaTabView.
			// NOTE: Keeping pointers to objects around seems to fail when the tabbed 
			// view changes. Using indexes is more reliable. Apparent bug in BTabView
		enum EChildID {kElementsView, kThumbnailView, kIconView, 
				kElementsButton, kThumbnailButton, kIconButton};
			// This is the index message value of the current set view in the
			// media tab. It defaults to the list view.
		EChildID fCurrentView;
			// This is the array of control buttons that gets assigned to
			// the current view. Created here but passed from child to child.
		TRadioBitmapButton* fbuttons[3];

		// Member functions
		void Init();
			// Activate or de-activate the given view. Activating means
			// give the view control of fbuttons (as children) and make
			// the view visible. Deactivating means remove control of the
			// buttons and hide.
		void ActivateView(EChildID which);
		void DeactivateView(EChildID which);
			// Create a size/scroll child view with the given target.
		BView* AddSizeScrollChild(BView* target);
};		

#endif
