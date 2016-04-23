//---------------------------------------------------------------------
//
//	File:	TActionCuePalette.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#pragma once

class TActionCuePalette: public BWindow
{
	public:
		TActionCuePalette();
		~TActionCuePalette();
		
		void		MessageReceived(BMessage* message);	
		void		Refresh();	
		
		bool 		QuitRequested();
		
	private:
		// Member functions
		virtual void		Init();
		
		// Member variables
		TActionCuePaletteView	*m_CueView;
		
};

