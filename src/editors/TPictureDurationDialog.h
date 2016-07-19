//-------------------------------------------------------------------
//
//	File:	TPictureDurationDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.11.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TPictureDurationDialog__
#define __TPictureDurationDialog__

// Class Definition
class TPictureDurationDialog : public BWindow
{
	public:
		TPictureDurationDialog(BMessage *theMessage, TCueView *theCue);
		~TPictureDurationDialog();
				
		void MessageReceived(BMessage* message);

		
		// Accessor Functions
		
	private:
	
		// Member Functions
		void			Init();		
		
		// Member Variables
		BView 					*fTextParent;
		TCueTimeText 			*fTextView;
		TCueView				*fCue;
	
};

#endif
