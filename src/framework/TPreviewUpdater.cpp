//---------------------------------------------------------------------
//
//	File:	TPreviewUpdater.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.29.98
//
//	Desc:	
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include "TPreviewUpdater.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPreviewUpdater::TPreviewUpdater(BHandler *theTarget, BMessage *theMessage, uint32 theInterval)
{
	m_Target 	= theTarget;
	m_Message 	= theMessage;
	m_Interval 	= theInterval;
	
	BLooper *theLooper = theTarget->Looper();
	
	status_t myErr;
	m_Messenger = new BMessenger(m_Target, NULL, &myErr);
	
	if (myErr != B_OK)
	{
		switch(myErr)
		{
			case B_BAD_VALUE:
				break;
				
			case B_BAD_TEAM_ID:
				break;

			case B_MISMATCHED_VALUES:
				break;
				
			case B_BAD_HANDLER:
				break;					
		}
	}
	
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

TPreviewUpdater::~TPreviewUpdater()
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

		
int32 TPreviewUpdater::Timer()
{			
	while( true )
	{
		snooze(m_Interval);
		
		// Is window still alive?  If not, exit.
		if ( m_Messenger->LockTarget() )
		{
       		BLooper *myLooper; 
       		BHandler *myHandler = m_Messenger->Target(&myLooper); 
       		myLooper->Unlock(); 
       		
       		m_Messenger->SendMessage(m_Message);
			//m_Target->Looper()->Lock();
       		//m_Target->MessageReceived(m_Message);
       		//m_Target->Looper()->Unlock();       		
       		//static_cast<TVideoPreviewView *>(m_Target)->UpdateVideo();
   		}  
		else
		{
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

int32 TPreviewUpdater::start_timer(void *arg)
{				
	TPreviewUpdater *obj = (TPreviewUpdater *)arg;
	return(obj->Timer() );		
}

			