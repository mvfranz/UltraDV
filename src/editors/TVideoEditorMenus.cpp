//---------------------------------------------------------------------
//
//	File:	TVideoEditorMenus.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	Menu creation and handling routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <Application.h>

#include "MuseumApp.h"
#include "TVideoEditor.h"

#include "TVideoEditorMenus.h"




// Constants

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//


TVideoEditorMenus::TVideoEditorMenus(BMenuBar* mbar, TVideoEditor *parent)
{
	
	// Save the BMenuBar into our member variable
	m_MenuBar = mbar;
	
	// Store the Editor Window
	m_Parent = parent;
	
	//
	// Add our individual menu items
	//
	
	// File menu
	AddFileMenu();
	
	// Edit menu
	AddEditMenu();
}


//-------------------------------------------------------------------
//
//	Function:	Default Destructor
//
//	Desc:
//	
//-------------------------------------------------------------------
//
//

TVideoEditorMenus::~TVideoEditorMenus()
{
}


//-------------------------------------------------------------------
//	AddFileMenu
//-------------------------------------------------------------------
//
//	Add file menu to the BMenuBar
//

bool TVideoEditorMenus::AddFileMenu()
{
	m_FileMenu = new BMenu("File");
	m_MenuBar->AddItem(m_FileMenu);
	
	// New...
	BMenuItem* newItem = new BMenuItem("New...", new BMessage(FILE_NEW_MSG), 'N');
	m_FileMenu->AddItem(newItem);
	
	// Open...
	BMenuItem* openItem = new BMenuItem("Open...", new BMessage(FILE_OPEN_MSG), 'O');
	m_FileMenu->AddItem(openItem);
	
	// Seperator01
	m_FileMenu->AddSeparatorItem();
	
	// Close
	BMenuItem* closeItem = new BMenuItem("Close", new BMessage(FILE_CLOSE_MSG), 'W');
	m_FileMenu->AddItem(closeItem);
	
	// Save...
	BMenuItem* saveItem = new BMenuItem("Save...", new BMessage(FILE_SAVE_MSG), 'S');
	m_FileMenu->AddItem(saveItem);
	
	// Save As...
	BMenuItem* saveAsItem = new BMenuItem("Save As...", new BMessage(FILE_SAVEAS_MSG));
	m_FileMenu->AddItem(saveAsItem);
	
	// Revert
	BMenuItem* revertItem = new BMenuItem("Revert to Saved", new BMessage(FILE_REVERT_MSG));
	m_FileMenu->AddItem(revertItem);
								
	// All these items message to the application
	m_FileMenu->SetTargetForItems(m_Parent);
	
	return true;
}


//-------------------------------------------------------------------
//	AddEditMenu
//-------------------------------------------------------------------
//
//	Add edit menu to the BMenuBar
//

bool TVideoEditorMenus::AddEditMenu()
{

	m_EditMenu = new BMenu("Edit");
	m_MenuBar->AddItem(m_EditMenu);
	
	// Undo
	BMenuItem* undoItem = new BMenuItem("Undo", new BMessage(EDIT_UNDO_MSG), 'Z');
	m_EditMenu->AddItem(undoItem);
	
	// Seperator01
	m_EditMenu->AddSeparatorItem();
	
	// Cut
	BMenuItem* cutItem = new BMenuItem("Cut", new BMessage(EDIT_CUT_MSG), 'X');
	m_EditMenu->AddItem(cutItem);
	
	// Copy
	BMenuItem* copyItem = new BMenuItem("Copy", new BMessage(EDIT_COPY_MSG), 'C');
	m_EditMenu->AddItem(copyItem);
	
	// Paste
	BMenuItem* pasteItem = new BMenuItem("Paste", new BMessage(EDIT_PASTE_MSG), 'V');
	m_EditMenu->AddItem(pasteItem);
	
	// Clear
	BMenuItem* clearItem = new BMenuItem("Clear", new BMessage(EDIT_CLEAR_MSG));
	m_EditMenu->AddItem(clearItem);
	
	// Select All
	BMenuItem* selectAllItem = new BMenuItem("Select All", new BMessage(EDIT_SELECTALL_MSG), 'A');
	selectAllItem->SetTarget(be_app);
	m_EditMenu->AddItem(selectAllItem);
	
	// All these items message to the window
	m_EditMenu->SetTargetForItems(m_Parent);

	
	return true;
}
