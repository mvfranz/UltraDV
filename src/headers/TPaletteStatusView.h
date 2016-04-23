//-------------------------------------------------------------------
//
//	File:	TPaletteStatusView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.25.98
//
//-------------------------------------------------------------------

#ifndef __TPaletteStatusView__
#define __TPaletteStatusView__ 
 

// Class Declarations
class TPaletteStatusView: public BView
{
	public:
		TPaletteStatusView(BRect bounds);
		~TPaletteStatusView();
		
		void 	MessageReceived(BMessage* message);
		void 	Draw(BRect updateRect);
		
	private:
					
		// Member Functions
		void 	Init();
		
			
		// Member variables
		char m_Text[256];				
};

#define UPDATE_STATUS_TEXT_MSG	'upST'

#endif
