//---------------------------------------------------------------------
//
//	File:	TStageWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Stage window creation and handling routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TSTAGEWINDOW_H__
#define __TSTAGEWINDOW_H__

//	Forward Declarations
class TCueSheetWindow;

// Class definitions
class TStageWindow: public BWindow
{
	public:
		TStageWindow(BRect bounds, TCueSheetWindow *cueSheet);
		TStageWindow(BMessage *data);
		~TStageWindow();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		void	MessageReceived(BMessage* message);	
		
		// Accessor Functions
		inline TCueSheetWindow 	*GetCueSheet(){ return fCueSheet; }
		inline TStageView 	   	*GetStageView(){ return fStageView; }
				
	private:
		TCueSheetWindow *fCueSheet;
		TStageView		*fStageView;
			
		void		Init();			
};

#endif
