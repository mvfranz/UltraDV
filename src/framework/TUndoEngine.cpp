//---------------------------------------------------------------------
//
//	File:	TUndoEngine.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.05.98
//
//	Desc:	Undo Engine
//			
//			We create one instance of this engine per 
//			document instance
//			
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TUndoEngine.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TUndoEngine::TUndoEngine()
{

}	


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TUndoEngine::~TUndoEngine()
{

}


//---------------------------------------------------------------------
//	CanUndo
//---------------------------------------------------------------------
//
//	Return if last operatin can be undone.  Query objects in
//	undo list. 
//

bool TUndoEngine::CanUndo()
{
	return false;
}



