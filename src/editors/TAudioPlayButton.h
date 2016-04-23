//---------------------------------------------------------------------
//
//	File:	TAudioPlayButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOPLAYBUTTON_H__
#define __TAUDIOPLAYBUTTON_H__ 

// Forward Declarations
class TAudioEditorToolbar;

// Class Declarations
class TAudioPlayButton : public BView
{
	public:
		// Member functions
		TAudioPlayButton( TAudioEditorToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		~TAudioPlayButton();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		
		// Member variables	
		
		// Accessor Functions
		inline void SetButtonState(bool theState){ m_ButtonState = theState; }
		
	private:				
		// Member functions
		void 	Init();
		
		
		// Member variables
		TAudioEditorToolbar	*m_Parent;
		BHandler			*m_Handler;
		bool				m_ButtonState;
		BBitmap				*m_OffBitmap;
		BBitmap				*m_OnBitmap;
		
};		

#endif
