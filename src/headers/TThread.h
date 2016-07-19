//---------------------------------------------------------------------
//
//	File:	TThread.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.15.98
//							
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TTHREAD_H__
#define __TTHREAD_H__

#include <OS.h>

class TThread
{
	public:

		TThread(const char *aName, long aPriority = B_NORMAL_PRIORITY);
		virtual	~TThread();
			
		status_t	GetInfo(thread_info *info);
		status_t	Kill();
		status_t	Run();
		status_t	Suspend();
		status_t	Rename(const char *newName);
		status_t	SetPriority(long newPriority);
		status_t	WaitForExit (long *thread_return_value);
		
		thread_id	ThreadID() { return fThreadID; }
		
		// This is the main routine that should be implemented
		// It is called by the Run() method when it's time to
		// actually start running the thread.
		virtual	status_t	ThreadMain() = 0;
	
	protected:
		thread_id	fThreadID;
	
	private:

};

#endif
