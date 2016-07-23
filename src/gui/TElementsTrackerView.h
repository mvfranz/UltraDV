//---------------------------------------------------------------------
//
//	File:	TElementsTrackerView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TELEMENTS_TRACKER_VIEW_H
#define __TELEMENTS_TRACKER_VIEW_H


// Class Declarations
class TElementsTrackerView: public BView
{
	public:
		// Member functions
		TElementsTrackerView(BRect bounds, TElementsView *parent);
		~TElementsTrackerView();
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
		// Member variables
		TElementsView	*fParent;
};		

#endif
