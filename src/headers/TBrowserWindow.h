//---------------------------------------------------------------------
//
//	File:	TBrowserWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TBROWSERWINDOW_H__
#define __TBROWSERWINDOW_H__

// Forward Declarations
class TBrowserTabView;

// Class definitions
class TBrowserWindow: public BWindow
{
	public:
		TBrowserWindow(BRect bounds);
		~TBrowserWindow();
		
		void	MessageReceived(BMessage* message);	
		
		void 	WindowActivated(bool active);
		bool 	QuitRequested();
						
	private:	
		// Member Functions	
		void		Init();	
		
		// Member Variable
		TBrowserTabView *m_BrowserTabView;
};

#endif
