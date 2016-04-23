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
		
		int32	GetTimerThread(){ return m_TimerThread; }
		
	private:
		static int32 	start_timer(void *arg);
		int32 			Timer();

		BHandler 	*m_Target;
		BMessage 	*m_Message;
		BMessenger 	*m_Messenger;
		uint32		m_Interval;
		thread_id	m_TimerThread;
		
};

#endif
