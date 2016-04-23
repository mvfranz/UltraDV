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

// Forward Declarations
//class TElementsView;

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
		// status_t	Perform(perform_code d, void *arg);

		void 		WindowActivated(bool state);

		void 		MessageReceived(BMessage *msg);
		
		//void 		FrameMoved(BPoint new_position);
		//void 		FrameResized(float width, float height);
		//void 		KeyDown(const char * bytes, int32 n);
		void		MouseDown(BPoint);
		void		MouseUp(BPoint);
		void 		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
		//void		Pulse();
		
		//void 		Select(int32);
		
		//void		MakeFocus(bool focusState = true);
		//void		SetFocusTab(int32,bool);
		
		void 		Draw(BRect);
		//BRect		DrawTabs();
		//void		DrawBox(BRect);
		//BRect		TabFrame(int32) const;
				
		//void			SetFlags(uint32 flags);
		//void			SetResizingMode(uint32 mode);
		
		//void 			GetPreferredSize( float *width, float *height);
		//void 			ResizeToPreferred();

		//BHandler		*ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property);
		//status_t		GetSupportedSuites(BMessage *data);
			
		//void 			AddTab(BView* v, BTab* tab=NULL);
		//BTab*			RemoveTab(int32) const;
		//BTab*			TabAt(int32) const;
		
		//void			SetTabWidth(button_width s);
		//button_width	TabWidth() const;
		
		//void			SetTabHeight(float);
		//float			TabHeight() const;		
	
	private:
		// Member Functions
		void Init();		
		
		// Member Variables
		//TElementsView *m_ElementsView;
};

#endif
