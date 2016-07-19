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
	fTarget 	= theTarget;
	fMessage 	= theMessage;
	fInterval 	= theInterval;

	BLooper *theLooper = theTarget->Looper();

	status_t myErr;
	fMessenger = new BMessenger(fTarget, NULL, &myErr);

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

TPreviewUpdater::~TPreviewUpdater()
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


int32 TPreviewUpdater::Timer()
{
	while( true )
	{
		snooze(fInterval);

		// Is window still alive?  If not, exit.
		if ( fMessenger->LockTarget() )
		{
       		BLooper *myLooper;
       		BHandler *myHandler = fMessenger->Target(&myLooper);
       		myLooper->Unlock();

       		fMessenger->SendMessage(fMessage);
			//fTarget->Looper()->Lock();
       		//fTarget->MessageReceived(fMessage);
       		//fTarget->Looper()->Unlock();
       		//static_cast<TVideoPreviewView *>(fTarget)->UpdateVideo();
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

