//---------------------------------------------------------------------
//
//	File:	TThread.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.15.98
//
///	Desc:	Class encapsulation the BeOS TThread
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <support/Debug.h>

#include "TThread.h"

//---------------------------------------------------------------------
//	thread_mainthread
//---------------------------------------------------------------------
//
// static thread rotuine

static status_t thread_mainthread(void *data)
{
	TThread *aThread = (TThread *)data;
	return aThread->ThreadMain();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TThread::TThread(const char *aName, long aPriority) : fThreadID(-1)
{
	// spawn the TThread
	fThreadID = spawn_thread(thread_mainthread, aName, aPriority, this);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TThread::~TThread()
{
	int32 exitStatus;

	Kill();
	WaitForExit(&exitStatus);
}


//---------------------------------------------------------------------
//	GetInfo
//---------------------------------------------------------------------
//
//

status_t TThread::GetInfo(thread_info *info)
{
	return get_thread_info(fThreadID, info);
}



//---------------------------------------------------------------------
//	Kill
//---------------------------------------------------------------------
//
//

status_t TThread::Kill()
{
	return kill_thread(fThreadID);
}


//---------------------------------------------------------------------
//	Run
//---------------------------------------------------------------------
//
//

status_t TThread::Run()
{
	return resume_thread(fThreadID);
}


//---------------------------------------------------------------------
//	Suspend
//---------------------------------------------------------------------
//
//

status_t TThread::Suspend()
{
	return suspend_thread(fThreadID);
}


//---------------------------------------------------------------------
//	Rename
//---------------------------------------------------------------------
//
//

status_t TThread::Rename(const char *newName)
{
	return rename_thread(fThreadID, newName);
}


//---------------------------------------------------------------------
//	SetPriority
//---------------------------------------------------------------------
//
//

status_t TThread::SetPriority(long newPriority)
{
	return set_thread_priority(fThreadID, newPriority);
}


//---------------------------------------------------------------------
//	WaitForExit
//---------------------------------------------------------------------
//
//

status_t TThread::WaitForExit(long *exitStatus)
{
	return wait_for_thread(fThreadID, exitStatus);
}