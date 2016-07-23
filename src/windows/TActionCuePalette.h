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

#ifndef __TACTIONCUE_PALETTE_H
#define __TACTIONCUE_PALETTE_H


class TActionCuePalette: public BWindow
{
	public:
		TActionCuePalette();
		~TActionCuePalette();

		void		Refresh();	
		
		bool 		QuitRequested();
		
	private:
		// Member functions
		virtual void		Init();
		
		// Member variables
		TActionCuePaletteView	*fCueView;
		
};

#endif
