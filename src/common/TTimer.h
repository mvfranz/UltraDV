//---------------------------------------------------------------------
//
//	File:	TTimer.h
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


#ifndef __TTIMER_H__
#define __TTIMER_H__


// Class Definitions
class TTimer
{
	public:
		//TTimer(BView *theTarget, BMessage *theMessage, uint32 theInterval);
		TTimer(BHandler *theTarget, BMessage *theMessage, uint32 theInterval);
		~TTimer();
		
		int32	GetTimerThread(){ return fTimerThread; }
		
	private:
		static int32 	start_timer(void *arg);
		int32 			Timer();

		BHandler 	*fTarget;
		BMessage 	*fMessage;
		BMessenger 	*fMessenger;
		uint32		fInterval;
		thread_id	fTimerThread;
		
};

#endif
