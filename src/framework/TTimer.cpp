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
	m_Target 	= theTarget;
	m_Message 	= theMessage;
	m_Interval 	= theInterval;
	
	m_Messenger = new BMessenger(m_Target);
	
	// Spawn timer thread	
	m_TimerThread = spawn_thread( start_timer, "GenericTimer", B_NORMAL_PRIORITY, (void *)this);
	
	if( m_TimerThread != B_NO_MORE_THREADS && m_TimerThread != B_NO_MEMORY)
	{
		resume_thread(m_TimerThread);
	}				       	
		
}
		
		
//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimer::~TTimer()
{
	kill_thread(m_TimerThread);
	
	if (m_Message)
		delete m_Message;
		
	if (m_Messenger)	
		delete m_Messenger;
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
		snooze(m_Interval);
		
		// Is window still alive?  If not, exit.
		if (m_Messenger->LockTarget())
		{
       		BLooper *myLooper; 
       		BHandler *myHandler = m_Messenger->Target(&myLooper); 
       		myLooper->Unlock(); 
       		
       		m_Messenger->SendMessage(m_Message);
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

			