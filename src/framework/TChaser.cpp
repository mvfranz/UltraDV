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
const uint32	MAX_QUEUED_MSGS	= 10;


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TChaser::TChaser( void (*ticked_func)(void *arg, TTick *t), void *arg, const char *name, int32 aPriority) : 
		TThread(name, aPriority)
{
  	Ticked 			= ticked_func;
  	m_Caller 		= arg;
  	m_IsStopping 	= false;
  	m_AreaAddress 	= NULL;
  	m_AreaID 		= -1;
  	m_PortID 		= -1;
  	
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
	m_PortID = create_port(MAX_QUEUED_MSGS, "tickInPort");
	if (m_PortID < 0)
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
	m_IsStopping = true;
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
	int32 		i;
	TTick 		*t;
  	int32 		err = 0;

	while(!IsStopping())
	{
		int32 msg = 0;
		if (err < 0 && err != B_INTERRUPTED)
	  		break;

		ssize_t msgSize = port_buffer_size(m_PortID);
		if (msgSize < 0) 
		{
	  		err = msgSize;
	  		continue;
		}
		
		if (msgSize == sizeof(i)) 
		{
	  		err = read_port(m_PortID, &msg, &i, sizeof(i));
	  		if (err < 0)
	  		{
				continue;
			}	
	  		
	  		if (msg == AREA_MSG)
	  		{
  				app_info appinfo; 
   				
   				if ( be_app->GetAppInfo(&appinfo) == B_OK )
   				{
   					area_info areainfo;
   					get_area_info(i, &areainfo);
   					
   					if(appinfo.team != areainfo.team)
   					{
   						m_AreaID = clone_area( 	"TChaser area", 
												(void **)&m_AreaAddress, 
                     							B_ANY_ADDRESS, 
                  								B_READ_AREA, 
                  								i);
                  	}
                  	else
                  	{
                  		m_AreaID = i;
                  		m_AreaAddress = (char *)areainfo.address;
                  	}
                }
      			
      			if (m_AreaID < B_OK)
      			{
         			return m_AreaID; 
         		}
     		}
	  		
	  		if (msg == TICK_MSG)
	  		{
	  			t = (TTick *)(m_AreaAddress + (sizeof(TTick) * i));
				(*Ticked)(m_Caller, t);
     		}
		}
  	}
  	return B_OK;
}
