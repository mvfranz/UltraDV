//---------------------------------------------------------------------
//
//	File:	TStageToolButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.13.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TStageToolButton__
#define __TStageToolButton__  

//	Forward Declaration
class TStageToolsView;

// Class Declarations
class TStageToolButton: public BView
{
	public:
		// Member functions
		TStageToolButton( TStageToolsView *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BMessage *message);
		~TStageToolButton();
		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		
		void 	Activate(bool theState);
		
		// Member variables	
		
		// Accessor Functions
		inline bool GetButtonState(){ return m_ButtonState; }
		inline void SetButtonState(bool theState){ m_ButtonState = theState; }
		
	private:				
		// Member functions
		void 	Init();
		void 	DoClick();
		
		
		// Member variables
		TStageToolsView		*m_Parent;
		BMessage			*m_Message;
		bool				m_ButtonState;
		BBitmap				*m_OffBitmap;
		BBitmap				*m_OnBitmap;
		
};		

#endif
