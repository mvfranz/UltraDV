//-------------------------------------------------------------------
//
//	File:	TStatusView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.09.98
//
//-------------------------------------------------------------------

#ifndef __TSTATUSVIEW_H__
#define __TSTATUSVIEW_H__
 

// Class Declarations
class TStatusView: public BView
{
	public:
		TStatusView(BRect bounds);
		~TStatusView();
		
		void 	MessageReceived(BMessage* message);
		void 	Draw(BRect updateRect);
		
	private:
					
		// Member Functions
		void 	Init();
					
		// Member variables
		char fText[256];				
};

#define UPDATE_STATUS_TEXT_MSG	'upST'

#endif
