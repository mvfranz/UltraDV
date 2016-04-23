//---------------------------------------------------------------------
//
//	File:	AVIViewer.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#pragma once

//	Includes
// ABH include file does not exist #include <TrinityDefs.h>
#include <BufferConsumer.h>

//#include "AVINodeTypes.h"
//#include "Benaphore.h"
//#include "TServiceThread.h"

//	Forward Declarations
//class TServiceThread;
//class TRunHandler;

//	Class Definition
class AVIViewer : public BBufferConsumer
{
	public:
		AVIViewer(); 
		~AVIViewer();
		

	protected:
	
	
	private:
		//	Private Methods
		bool		Init();
		
};

