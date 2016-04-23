//---------------------------------------------------------------------
//
//	File:	TPreviewUpdater.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.29.98
//
//	Desc:	Thread to update video preview
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TPreviewUpdater__
#define __TPreviewUpdater__


// Class Definitions
class TPreviewUpdater
{
	public:
		TPreviewUpdater(BHandler *theTarget, BMessage *theMessage, uint32 theInterval);
		~TPreviewUpdater();
		
		int32	GeTPreviewUpdaterThread(){ return m_TimerThread; }
		
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
