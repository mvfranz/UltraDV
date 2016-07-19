//---------------------------------------------------------------------
//
//	File:	TChaser.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.16.98
//
///	Desc:	Timecode follower based on Doug Wright's code
//
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <support/Debug.h>

#include "TChaser.h"
#include "TTick.h"

// Constants
const uint32 MAX_QUEUED_MSGS = 10;


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChaser::TChaser( void (*ticked_func)(void* arg, TTick *t), void* arg, const char* name, int32 aPriority) :
	TThread(name, aPriority)
{
	Ticked                  = ticked_func;
	fCaller                 = arg;
	fIsStopping     = false;
	fAreaAddress    = NULL;
	fAreaID                 = -1;
	fPortID                 = -1;

	// Begin listening
	Start();
}


//---------------------------------------------------------------------
//	Start
//---------------------------------------------------------------------
//
//
status_t TChaser::Start()
{
	fPortID = create_port(MAX_QUEUED_MSGS, "tickInPort");
	if (fPortID < 0)
		return PortID();

	return Run();
}


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//

void TChaser::Stop()
{
	fIsStopping = true;
	int32 dummy;

	WaitForExit(&dummy);
}

//---------------------------------------------------------------------
//	ThreadMain
//---------------------------------------------------------------------
//
//

status_t TChaser::ThreadMain()
{
	int32 i;
	TTick* t;
	int32 err = 0;

	while(!IsStopping()) {
		int32 msg = 0;
		if (err < 0 && err != B_INTERRUPTED)
			break;

		ssize_t msgSize = port_buffer_size(fPortID);
		if (msgSize < 0) {
			err = msgSize;
			continue;
		}

		if (msgSize == sizeof(i)) {
			err = read_port(fPortID, &msg, &i, sizeof(i));
			if (err < 0) {
				continue;
			}

			if (msg == AREA_MSG) {
				app_info appinfo;

				if ( be_app->GetAppInfo(&appinfo) == B_OK ) {
					area_info areainfo;
					get_area_info(i, &areainfo);

					if(appinfo.team != areainfo.team) {
						fAreaID = clone_area(   "TChaser area",
						                        (void**)&fAreaAddress,
						                        B_ANY_ADDRESS,
						                        B_READ_AREA,
						                        i);
					} else {
						fAreaID = i;
						fAreaAddress = (char*)areainfo.address;
					}
				}

				if (fAreaID < B_OK) {
					return fAreaID;
				}
			}

			if (msg == TICK_MSG) {
				t = (TTick*)(fAreaAddress + (sizeof(TTick) * i));
				(*Ticked)(fCaller, t);
			}
		}
	}
	return B_OK;
}
