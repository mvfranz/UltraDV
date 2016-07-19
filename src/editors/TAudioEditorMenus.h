//---------------------------------------------------------------------
//
//	File:	TAudioEditorMenus.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.10.98
//
//	Desc:	Menu creation and handling routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOEDITORMENUS_H__
#define __TAUDIOEDITORMENUS_H__

#include "MenuMessages.h"

class TAudioEditorMenus
{
	public:
		// Member Functions
		TAudioEditorMenus(BMenuBar* mbar, TAudioEditor *parent);
		~TAudioEditorMenus();
								
		// Member variables
		BMenu	*fFileMenu;
		BMenu	*fEditMenu;
		
	private:
		BMenuBar 		*fMenuBar;
		TAudioEditor	*fParent;

		bool	AddFileMenu();
		bool 	AddEditMenu();
};

#endif
