//---------------------------------------------------------------------
//
//	File:	TVideoEditorMenus.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//	Desc:	Menu creation and handling routines for VideoEditor
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorMenus__
#define __TVideoEditorMenus__ 

#include "MenuMessages.h"

// Forward Declarations
class TVideoEditor;

// Class Definiton
class TVideoEditorMenus
{
	public:
		// Member Functions
		TVideoEditorMenus(BMenuBar* mbar, TVideoEditor *parent);
		~TVideoEditorMenus();
								
		// Member variables
		BMenu	*fFileMenu;
		BMenu	*fEditMenu;
		
	private:
		BMenuBar 		*fMenuBar;
		TVideoEditor	*fParent;

		bool	AddFileMenu();
		bool 	AddEditMenu();
};

#endif
