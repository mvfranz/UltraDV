//---------------------------------------------------------------------
//
//	File:	TMediaCuePalette.h
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

#ifndef __TMEDIACUEPALETTE_H__
#define __TMEDIACUEPALETTE_H__

// Forward Declarations
class TPaletteStatusView;

// Class Definition
class TMediaCuePalette: public BWindow
{
	public:
		TMediaCuePalette();
		~TMediaCuePalette();
		
		void	MessageReceived(BMessage* message);	
		
		bool 	QuitRequested();
		
	private:
		// Member functions
		virtual void		Init();
		
		// Member variables
		TMediaCuePaletteView	*fCueView;
		TPaletteStatusView		*fCueStatusView;
		
};

#define MEDIA_PALETTE_TEXT_MSG	'MPtm'

#endif
