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


TVideoEditorMenus::TVideoEditorMenus(BMenuBar* mbar, TVideoEditor* parent)
{

	// Save the BMenuBar into our member variable
	fMenuBar = mbar;

	// Store the Editor Window
	fParent = parent;

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
	fFileMenu = new BMenu("File");
	fMenuBar->AddItem(fFileMenu);

	// New...
	BMenuItem* newItem = new BMenuItem("New...", new BMessage(FILE_NEW_MSG), 'N');
	fFileMenu->AddItem(newItem);

	// Open...
	BMenuItem* openItem = new BMenuItem("Open...", new BMessage(FILE_OPEN_MSG), 'O');
	fFileMenu->AddItem(openItem);

	// Seperator01
	fFileMenu->AddSeparatorItem();

	// Close
	BMenuItem* closeItem = new BMenuItem("Close", new BMessage(FILE_CLOSE_MSG), 'W');
	fFileMenu->AddItem(closeItem);

	// Save...
	BMenuItem* saveItem = new BMenuItem("Save...", new BMessage(FILE_SAVE_MSG), 'S');
	fFileMenu->AddItem(saveItem);

	// Save As...
	BMenuItem* saveAsItem = new BMenuItem("Save As...", new BMessage(FILE_SAVEAS_MSG));
	fFileMenu->AddItem(saveAsItem);

	// Revert
	BMenuItem* revertItem = new BMenuItem("Revert to Saved", new BMessage(FILE_REVERT_MSG));
	fFileMenu->AddItem(revertItem);

	// All these items message to the application
	fFileMenu->SetTargetForItems(fParent);

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

	fEditMenu = new BMenu("Edit");
	fMenuBar->AddItem(fEditMenu);

	// Undo
	BMenuItem* undoItem = new BMenuItem("Undo", new BMessage(EDIT_UNDO_MSG), 'Z');
	fEditMenu->AddItem(undoItem);

	// Seperator01
	fEditMenu->AddSeparatorItem();

	// Cut
	BMenuItem* cutItem = new BMenuItem("Cut", new BMessage(EDIT_CUT_MSG), 'X');
	fEditMenu->AddItem(cutItem);

	// Copy
	BMenuItem* copyItem = new BMenuItem("Copy", new BMessage(EDIT_COPY_MSG), 'C');
	fEditMenu->AddItem(copyItem);

	// Paste
	BMenuItem* pasteItem = new BMenuItem("Paste", new BMessage(EDIT_PASTE_MSG), 'V');
	fEditMenu->AddItem(pasteItem);

	// Clear
	BMenuItem* clearItem = new BMenuItem("Clear", new BMessage(EDIT_CLEAR_MSG));
	fEditMenu->AddItem(clearItem);

	// Select All
	BMenuItem* selectAllItem = new BMenuItem("Select All", new BMessage(EDIT_SELECTALL_MSG), 'A');
	selectAllItem->SetTarget(be_app);
	fEditMenu->AddItem(selectAllItem);

	// All these items message to the window
	fEditMenu->SetTargetForItems(fParent);


	return true;
}
