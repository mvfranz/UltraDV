//---------------------------------------------------------------------
//
//	File:	TVideoPreviewView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Video preview area
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <scheduler.h>
#include <stdio.h>

#include <Buffer.h>
#include <MediaRoster.h>
#include <Application.h>
#include <BufferGroup.h>
#include <TimeSource.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TVideoSettingsDialog.h"

#include "TVideoPreviewView.h"


// Constants
#define M1 ((double)1000000.0)

#define	FUNCTION	printf
#define ERROR		printf
#define PROGRESS	printf
#define LOOP		

media_raw_video_format vid_preview_format = { 29.97,1,0,479,B_VIDEO_TOP_LEFT_RIGHT,1,1,{B_RGB32,640,480,640*4,0,0}};


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoPreviewView::TVideoPreviewView(BRect bounds, TVideoSettingsDialog *parent) : 
					BView(bounds, "VideoPreviewView", B_FOLLOW_ALL, B_WILL_DRAW),
					BMediaNode("VideoPreviewConsumer"),
					BBufferConsumer(B_MEDIA_RAW_VIDEO)
{
	m_Parent = parent;
	
	// Perform default initialization
	Init();	
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoPreviewView::~TVideoPreviewView()
{
	mControlQuit = true;
	mDisplayQuit = true;
	
	write_port(mPort, 0x60000000, NULL, 0);
	
	status_t status;
	wait_for_thread(mThread, &status);
	if (vThread != 0)
		wait_for_thread(vThread, &status);
	
	vThread = 0;
	mThread = 0;

	delete_port(mPort);
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoPreviewView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	AddNodeKind(B_PHYSICAL_OUTPUT);
	
	mPort 				= create_port(3, "TVideoPreviewView input");
	mDestination.port 	= mPort;
	mDestination.id 	= 0;
	mLast 				= 0;
	
	mRunMode = B_DROP_DATA;
	
	m_Bitmap = NULL;
	
	vThread = 0;
	mThread = 0;

	mControlQuit = false;
	mDisplayQuit = false;
	
	mConnected 	= false;
	mRunning 	= false;
	mStarting 	= false;
	mStopping 	= false;
	mSeeking 	= false;
	
	mStartTime 	= 0;		/* when to start in performance time */
	mStopTime 	= 0;		/* when to stop in performance time */
	mSeekTime 	= 0;
	mMediaTime 	= 0;
	mDeltaTime 	= 0;
	
	mMyLatency 			= 10000;
	mDownstreamLatency 	= 0;
	
	mBufferAvailable = 0;

	mServiceLock = create_sem (1, "Video Consumer Service Lock");
	if (mServiceLock < B_NO_ERROR)
	{
		ERROR("TVideoPreviewView: couldn't create ServiceLock semaphore\n");
	}
	
	mThread = spawn_thread(sRun, "TVideoPreviewView:run", B_REAL_TIME_PRIORITY, this);
	resume_thread(mThread);	
			
}

#pragma mark -
#pragma mark === Consumer Routines ===


/********************************
	From BMediaNode
********************************/

//---------------------------------------------------------------

BMediaAddOn *
TVideoPreviewView::AddOn(long *) const
{
	return NULL;
}

//---------------------------------------------------------------

port_id
TVideoPreviewView::ControlPort() const
{
	return mPort;
}


//---------------------------------------------------------------

void
TVideoPreviewView::Start(
	bigtime_t performance_time)
{
	FUNCTION("TVideoPreviewView::Start() @ %.4f, now: %.4f\n", 
	(double)performance_time/M1, (double)TimeSource()->Now()/M1);
	
	if (!mStopping || performance_time > mStopTime)
	{
		if (!mRunning || mStopping)
		{
			mStarting = true;
			mStartTime = performance_time;
		}		
	}
}


//---------------------------------------------------------------

void
TVideoPreviewView::Stop( bigtime_t performance_time, bool immediate)
{
	FUNCTION("TVideoPreviewView::Stop() @ %.4f, now: %.4f\n",
		(double)performance_time/M1, (double)TimeSource()->Now()/M1);
	
	if (!mStarting || performance_time > mStartTime)
	{
		if (mRunning || mStarting)
		{
			mStopping = true;
			mStopTime = performance_time;
		}
	}
	
	if (immediate)
	{
		mRunning = false;
		mStopping = true;
		mStopTime = TimeSource()->Now();
	}
}


//---------------------------------------------------------------

void
TVideoPreviewView::Seek(
	bigtime_t media_time,
	bigtime_t performance_time)
{
	FUNCTION("TVideoPreviewView::Seek\n");
	
	/* we only play in performance_time -- we don't seek */
}


//---------------------------------------------------------------

BMediaNode::run_mode
TVideoPreviewView::RunMode()
{
	FUNCTION("TVideoPreviewView::RunMode\n");	
	
	return mRunMode;	
}

//---------------------------------------------------------------

void
TVideoPreviewView::SetRunMode(
	run_mode mode)
{
	FUNCTION("TVideoPreviewView::SetRunMode\n");
	
	mRunMode = mode;
	
}

//---------------------------------------------------------------

void
TVideoPreviewView::TimeWarp(bigtime_t at_real_time,
	bigtime_t performance_time)
{
	FUNCTION("TVideoPreviewView::TimeWarp perf time %.4f @ %.4f\n",
		(double)performance_time/M1, (double)at_real_time/M1);
	
}

//---------------------------------------------------------------

void
TVideoPreviewView::BufferReceived(BBuffer * buffer)
{
	//FUNCTION("TVideoPreviewView::BufferReceived()\n");
	
	if (mBufferQueue->PushBuffer(buffer, buffer->Header()->start_time) == B_OK)
		release_sem(mBufferAvailable);
	else
	{
		ERROR("TVideoPreviewView::BufferReceived - ERROR PUSHING BUFFER ONTO QUEUE\n");
		buffer->Recycle();
	}		
}


//---------------------------------------------------------------

void
TVideoPreviewView::ProducerDataStatus(
	const media_destination & /*for_whom*/,
	int32 status,
	bigtime_t /*at_media_time*/)
{
	FUNCTION("TVideoPreviewView::ProducerDataStatus()\n");
	
	if (status==B_DATA_NOT_AVAILABLE)
	{
		BMessage m('TRDN');
		m.AddPointer("Consumer",this);
		be_app->PostMessage(&m);
	}
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::Connected(
	const media_source & producer,
	const media_destination & where,
	const media_format & with_format,
	media_input * out_input)
{
	FUNCTION("TVideoPreviewView::Connected()\n");
	
	out_input->node = Node();
	out_input->source = mProducer = producer;
	out_input->destination = mDestination;
	out_input->format = with_format;
	sprintf(out_input->name, "TVideoPreviewView");

	mXSize 		= with_format.u.raw_video.display.line_width;
	mYSize 		= with_format.u.raw_video.display.line_count;	
	mRowBytes 	= with_format.u.raw_video.display.bytes_per_row;
	mColorspace = with_format.u.raw_video.display.format;
	
	mBufferAvailable = create_sem (0, "Video buffer available");
	if (mBufferAvailable < B_NO_ERROR)
	{
		ERROR("TVideoPreviewView: couldn't create semaphore\n");
		return B_ERROR;
	}
	
	mBufferQueue = new BTimedBufferQueue();
			
	//	Create offscreen
	if (m_Bitmap == NULL)
	{
		m_Bitmap = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);
	}

	if (vThread == 0)
	{
		mDisplayQuit = false;
		int drawPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 5000);
		PROGRESS("Suggested draw thread priority: %d\n", drawPrio);
		vThread = spawn_thread(vRun, "TVideoPreviewView:draw", drawPrio, this);
		resume_thread(vThread);
	}	
	
	mConnected = true;
	return B_OK;
}

//---------------------------------------------------------------

void TVideoPreviewView::Disconnected( const media_source &producer, const media_destination &where)
{
	FUNCTION("TVideoPreviewView::Disconnect()\n");

	mConnected = false;
	
	status_t status;
	mDisplayQuit = true;
	if (vThread != 0)
		wait_for_thread(vThread, &status);
	vThread = 0;
	
	if (m_Bitmap != NULL)
	{
		delete m_Bitmap;
		m_Bitmap = NULL;
	}
	
	if (mBufferAvailable != 0)
		delete_sem(mBufferAvailable);
	mBufferAvailable = 0;
	
	while (mBufferQueue->HasBuffers())
	{
		mBufferQueue->PopFirstBuffer(0)->Recycle();
	}
	delete mBufferQueue;
	mBufferQueue = 0;					
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::AcceptFormat(
	const media_destination & /*dest*/,
	media_format * format)
{
	FUNCTION("TVideoPreviewView::AcceptFormat()\n");
	if (format->type != B_MEDIA_NO_TYPE)
	{
		if (format->type != B_MEDIA_RAW_VIDEO)
		{
			ERROR("AcceptFormat - not B_MEDIA_RAW_VIDEO\n");
			return B_MEDIA_BAD_FORMAT;
		}
		if (format->u.raw_video.display.format != B_RGB32 &&
			format->u.raw_video.display.format != B_RGB16 &&
			format->u.raw_video.display.format != B_RGB15 &&			
			format->u.raw_video.display.format != media_raw_video_format::wildcard.display.format)
		{
			ERROR("AcceptFormat - not a format we know about!\n");
			return B_MEDIA_BAD_FORMAT;
		}
	}
	format->type = B_MEDIA_RAW_VIDEO;
	if (format->u.raw_video.display.format == media_raw_video_format::wildcard.display.format) {
		format->u.raw_video.display.format = B_RGB32;
	}
	return B_OK;
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::GetNextInput(
	int32 * cookie,
	media_input * out_input)
{
	FUNCTION("TVideoPreviewView::GetNextInput()\n");
	if (*cookie == 0)
	{
		*cookie = 1;
		out_input->destination = mDestination;
		out_input->source = media_source::null;
		out_input->node = Node();
		out_input->format.u.raw_video = vid_preview_format;
		out_input->format.type = B_MEDIA_RAW_VIDEO;
		sprintf(out_input->name, "TVideoPreviewView");
		return B_OK;
	}
	return B_MEDIA_BAD_DESTINATION;
}

//---------------------------------------------------------------

void
TVideoPreviewView::DisposeInputCookie(int32 /*cookie*/)
{
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::GetLatencyFor(
	const media_destination & /* input */,
	bigtime_t * out_latency,
	media_node_id * out_timesource)
{
	FUNCTION("TVideoPreviewView::GetLatencyFor()\n");
	*out_latency = mMyLatency;
	*out_timesource = TimeSource()->ID();
	return B_OK;
}


//---------------------------------------------------------------

status_t
TVideoPreviewView::FormatChanged(
				const media_source & /*producer*/,
				const media_destination & /*consumer*/, 
				int32 /*from_change_count*/,
				const media_format &format)
{
	FUNCTION("TVideoPreviewView::FormatChanged()\n");
	/*
	// get the new raw_video format
	vid_preview_format=format.u.raw_video;

	// grab the current width, height, and fps
	mXSize		= format.u.raw_video.display.line_width;
	mYSize		= format.u.raw_video.display.line_count;
	mRowBytes	= format.u.raw_video.display.bytes_per_row;
	mColorspace	= format.u.raw_video.display.format;	
	
	fps = format.u.raw_video.field_rate;

	// destroy the old window, view, & bitmap
	mWindow->Lock();
	mWindow->Close();
	delete m_Bitmap;						

	// and create a new set
	mWindow = new BWindow(BRect(40,40,40 + (mXSize-1), 40 + (mYSize - 1)), "Video Window", B_TITLED_WINDOW, B_NOT_CLOSABLE);
	mView = new BView(BRect(0, 0, (mXSize-1),(mYSize - 1)), "Play View", B_FOLLOW_ALL, B_WILL_DRAW);
	mWindow->AddChild(mView);
	mWindow->Show();
	m_Bitmap = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);
	
	return B_OK;
	*/
	
	return B_ERROR;
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::sRun(void * data)
{
	FUNCTION("TVideoPreviewView::sRun()\n");
	((TVideoPreviewView *)data)->ServiceThread();
	return 0;
}

//---------------------------------------------------------------

struct
media_message 
{
	char whatever[B_MEDIA_MESSAGE_SIZE];
};

void
TVideoPreviewView::ServiceThread()
{
	FUNCTION("TVideoPreviewView::ServiceThread()\n");

	while (!mControlQuit)
	{
		status_t err=0;
		int32 code=0;
		media_message msg;
		err = read_port_etc(mPort, &code, &msg, sizeof(msg), B_TIMEOUT, 5000);
		
		if (err == B_TIMED_OUT) 
			continue;
			
		if (err < B_OK)
		{
			ERROR("TVideoPreviewView::Run: Unexpected error in read_port(): %x\n", err);
			continue;
		}

		if (acquire_sem(mServiceLock) == B_NO_ERROR)
		{
			if (BMediaNode::HandleMessage(code, &msg, err) &&
				BBufferConsumer::HandleMessage(code, &msg, err))
			{
				BMediaNode::HandleBadMessage(code, &msg, err);
			}
			release_sem(mServiceLock);

			if (code == 0x60000000)	/* quit! */
				break;
		}
	}
	
	FUNCTION("TVideoPreviewView::ServiceThread - END\n");
}

//---------------------------------------------------------------

status_t
TVideoPreviewView::vRun(void * data)
{
	FUNCTION("TVideoPreviewView::vRun()\n");
	((TVideoPreviewView *)data)->DisplayThread();
	return 0;
}

//---------------------------------------------------------------

void
TVideoPreviewView::DisplayThread()
{
	FUNCTION("TVideoPreviewView::DisplayThread\n");
	
	bigtime_t	timeout = 5000;
	bigtime_t	realTimeNow = 0;
	bigtime_t	perfTimeNow = 0;
	bigtime_t	halfPeriod = (bigtime_t) (500000./29.97);
	bool 		timeSourceRunning = false;
	
	while (!mDisplayQuit)
	{
		if (acquire_sem(mServiceLock) == B_NO_ERROR)
		{
			timeSourceRunning = TimeSource()->IsRunning();
			realTimeNow = BTimeSource::RealTime();
			perfTimeNow = TimeSource()->Now();
			release_sem(mServiceLock);
		}
		
		snooze(timeout);
		
		if (timeSourceRunning)
		{
			
			// if we received a Stop, deal with it
			if (mStopping)
			{
				PROGRESS("VidConsumer::DisplayThread - STOP\n");
				if (perfTimeNow >= mStopTime)
				{
					mRunning = false;
					mStopping = false;
					
					// deal with any pending Seek
					if (mSeeking)
						mSeeking = false;
						
					//if (mConnected)
					//	SendDataStatus(B_DATA_NOT_AVAILABLE, mConnections[0], mStopTime);
						
					continue;
				}
			}	
						
			// if we received a Seek, deal with it
			if (mSeeking)
			{
				PROGRESS("VidConsumer::DisplayThread - SEEK\n");
				if (perfTimeNow >= mSeekTime)
				{
					PROGRESS("VidConsumer::DisplayThread - DO SEEK\n");
					mSeeking = false;
					mDeltaTime = mMediaTime;
					
					continue;
				}
			}
			
			// if we received a Start, deal with it
			if (mStarting)
			{
				PROGRESS("BBt848Controllable::CaptureRun mStartTime = %.4f TimeNow = %.4f\n", (double)mStartTime/M1, (double)perfTimeNow/M1);
				if (perfTimeNow >= mStartTime)
				{
					mRunning = true;
					mStarting = false;
					mDeltaTime = mStartTime;
		
					//if (mConnected)
					//	SendDataStatus(B_DATA_AVAILABLE, mConnections[0], mStartTime);

					continue;
				}
			}
			
			if (mRunning)
			{
				// check for buffer available.		
				status_t err = acquire_sem_etc(mBufferAvailable, 1, B_TIMEOUT, halfPeriod * 2);
					
				if (err == B_TIMED_OUT || !mConnected)
				{
					ERROR("VidConsumer::DisplayThread - Error from acquire_sem_etc: 0x%x\n", err);
					continue;
				}
			
				BBuffer *buffer = mBufferQueue->PopFirstBuffer(0);
		
				LOOP("Popped buffer %08x, Start time: %.4f, system time: %.4f diff: %.4f\n",
					buffer,
					(double) buffer->Header()->start_time/M1,
					(double) perfTimeNow/M1,
					(double) (buffer->Header()->start_time - perfTimeNow)/M1);
					
				// Display frame if we're in B_OFFLINE mode or 
				// within +/- a half frame time of start time
				if ( (mRunMode == B_OFFLINE) ||
					 ((perfTimeNow > (buffer->Header()->start_time - halfPeriod)) &&
					  (perfTimeNow < (buffer->Header()->start_time + halfPeriod))) )
				{
					uint32 bpp = (mColorspace == B_RGB32 ? 4 : 2);	
					memcpy(m_Bitmap->Bits(), buffer->Data(), mRowBytes * mYSize * bpp);
					buffer->Header()->start_time = system_time();
					buffer->Recycle();
					bigtime_t t1 = system_time();
					
					//	Update view
					if (LockLooper())
					{
						DrawBitmap(m_Bitmap, Bounds());
						UnlockLooper();
					}					
					
					t1 = system_time() - t1;
					if (t1/M1 > .030)
						printf("Draw time = %.4f\n",t1/M1);
					continue;
				}
				else
				{
					// If we're too early, push frame back on stack
					if (perfTimeNow < buffer->Header()->start_time)
					{
						LOOP("push buffer back on stack!\n");
						mBufferQueue->PushBuffer(buffer, buffer->Header()->start_time);
						release_sem(mBufferAvailable);
						continue;						
					}
					else
					{
						// if we've already passed a half frame time past the buffer start time
						// and RunMode = INCREASE_LATENCY, increase latency and display the frame
						if ( (perfTimeNow > buffer->Header()->start_time) &&
							 (mRunMode == B_INCREASE_LATENCY))
						{
							mMyLatency += halfPeriod;
							ERROR("VidConsumer::DisplayThread - Increased latency to: %.4f\n", mMyLatency);
							ERROR("	 Performance time: %.4f @ %.4f\n", (double)buffer->Header()->start_time/M1, (double)perfTimeNow/M1);
							uint32 bpp = (mColorspace == B_RGB32 ? 4 : 2);		
							memcpy(m_Bitmap->Bits(), buffer->Data(), mRowBytes * mYSize * bpp);
							buffer->Recycle();
							
							// should send late notice
							if (LockLooper())
							{
								DrawBitmap(m_Bitmap, Bounds());
								UnlockLooper();
							}

							continue;
						}
						else
						{
							// we're more than a half frame time past the buffer start time
							// drop the frame
							ERROR("VidConsumer::DisplayThread - dropped late frame: %.4f @ %.4f\n", (double)buffer->Header()->start_time/M1, (double)perfTimeNow/M1);
							buffer->Recycle();
							// should send late notice
							continue;
						}
					}
				}
			} 
			snooze(timeout);
		}  else snooze(timeout); // if TimeSource stopped	
	} // while (!mTimeToQuit)
}

