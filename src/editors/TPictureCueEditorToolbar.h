//-------------------------------------------------------------------
//
//	File:	TPictureCueEditorToolbar.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.11.98
//
//
//	Copyright ©1998 mediapede Software
//
//-------------------------------------------------------------------

#ifndef __TPictureCueEditorToolbar__
#define __TPictureCueEditorToolbar__ 

// Forward Declarations
class TBitmapButton;

// Class Definition
class TPictureCueEditorToolbar: public BView
{
	public:
		TPictureCueEditorToolbar(BRect bounds, TCueView *theCue);
		~TPictureCueEditorToolbar();
		
		void 	Draw(BRect updateRect);
		// ABH missing declaration ???
		void MessageReceived(BMessage *);
		void KeyDown(const char *, long int);

		
	private:
		// Member Functions
		void Init();
		
		// Member Variables						
		TBitmapButton 	*m_DurationButton;
		BRect 					m_DurationRect;
		TCueView				*m_Cue;
};

// Messages
#define PICTURE_DURATION_MSG 'pcDM'

#endif
