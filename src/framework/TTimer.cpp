//---------------------------------------------------------------------
//
//	File:	TTimer.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.13.98
//
//	Desc:	Generic timer that notifies target view with provided message
//			at regular intervals set by user.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "TTimer.h"


// Local Prototypes
int32 Timer(void* data);


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimer::TTimer(BHandler *theTarget, BMessage *theMessage, uint32 theInterval)
{
	fTarget 	= theTarget;
	fMessage 	= theMessage;
	fInterval 	= theInterval;
	
	fMessenger = new BMessenger(fTarget);
	
	// Spawn timer thread	
	fTimerThread = spawn_thread( start_timer, "GenericTimer", B_NORMAL_PRIORITY, (void *)this);
	
	if( fTimerThread != B_NO_MORE_THREADS && fTimerThread != B_NO_MEMORY)
	{
		resume_thread(fTimerThread);
	}				       	
		
}
		
		
//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimer::~TTimer()
{
	kill_thread(fTimerThread);
	
	if (fMessage)
		delete fMessage;
		
	if (fMessenger)	
		delete fMessenger;
}



//---------------------------------------------------------------------
//	Timer
//---------------------------------------------------------------------
//
//	Timer thread routine

		
int32 TTimer::Timer()
{			
	while( true )
	{
		snooze(fInterval);
		
		// Is window still alive?  If not, exit.
		if (fMessenger->LockTarget())
		{
       		BLooper *myLooper; 
       		BHandler *myHandler = fMessenger->Target(&myLooper); 
       		myLooper->Unlock(); 
       		
       		fMessenger->SendMessage(fMessage);
   		}  
		else
		{
			printf("LockTarget failure...\n");
			return 0;									
		}
	}
		
	return true;
}


//---------------------------------------------------------------------
//	start_timer
//---------------------------------------------------------------------
//
//	Static thread function
//

int32 TTimer::start_timer(void *arg)
{				
	TTimer *obj = (TTimer *)arg;
	return(obj->Timer() );		
}

			