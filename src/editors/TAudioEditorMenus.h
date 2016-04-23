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
		BMenu	*m_FileMenu;
		BMenu	*m_EditMenu;
		
	private:
		BMenuBar 		*m_MenuBar;
		TAudioEditor	*m_Parent;

		bool	AddFileMenu();
		bool 	AddEditMenu();
};

#endif
