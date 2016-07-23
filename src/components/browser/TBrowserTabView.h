//---------------------------------------------------------------------
//
//	File:	TBrowserTabView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TBROWSERTABVIEW_H__
#define __TBROWSERTABVIEW_H__


// Class Definition
class TBrowserTabView : public BTabView 
{
	public:
		// Member Functions
		TBrowserTabView(BRect frame, const char *name);
		TBrowserTabView(BMessage*);
		~TBrowserTabView();	
								
		static	BArchivable*	Instantiate(BMessage*);
		status_t				Archive(BMessage*, bool deep=true) const;

		void 		MessageReceived(BMessage *msg);

		void 		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

		void 		Draw(BRect);
	
	private:
		// Member Functions
		void Init();		
		
		// Member Variables
		//TElementsView *fElementsView;
};

#endif
