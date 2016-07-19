//	Copyright (c) 1998, Be Incorporated, All Rights Reserved.
//	SMS
//	TVideoCaptureView.cpp

#include <Buffer.h>
#include <MediaRoster.h>
#include <Application.h>
#include <BufferGroup.h>
#include <unistd.h>
#include <fcntl.h>
#include <TimeSource.h>
#include <string.h>
#include <scheduler.h>
#include <stdio.h>

#include "TVideoCaptureWindow.h"

#include "TVideoCaptureView.h"

#define M1 ((double)1000000.0)

#define FUNCTION        printf
#define ERROR           printf
#define PROGRESS        printf
#define LOOP

media_raw_video_format vid_format = { 29.97,1,0,239,B_VIDEO_TOP_LEFT_RIGHT,1,1,{B_RGB32,320,240,320*4,0,0}};

//---------------------------------------------------------------

TVideoCaptureView::TVideoCaptureView(BRect bounds, TVideoCaptureWindow *parent) :
	BView(bounds, "VideoPreviewView", B_FOLLOW_ALL, B_WILL_DRAW),
	BMediaNode("VideoPreviewConsumer"),
	BBufferConsumer(B_MEDIA_RAW_VIDEO)
{

	FUNCTION("TVideoCaptureView::TVideoCaptureView()\n");

	AddNodeKind(B_PHYSICAL_OUTPUT);

	mPort = create_port(3, "TVideoCaptureView input");

	mRunMode = B_DROP_DATA;

	mConnectionCount = 0;
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		mConnectionActive[i] = false;
		mFormat[i] = vid_format;
		mBitmap[i] = NULL;
	}

	mDisplayThread = 0;
	mServiceThread = 0;

	mControlQuit = false;
	mDisplayQuit = false;

	mRunning = false;
	mStarting = false;
	mStopping = false;
	mSeeking = false;

	mStartTime = 0;         /* when to start in performance time */
	mStopTime = 0;          /* when to stop in performance time */
	mSeekTime = 0;
	mMediaTime = 0;
	mDeltaTime = 0;

	mMyLatency = 10000;
	mDownstreamLatency = 0;

	// create a buffer queue and buffer available semaphore
	mBufferQueue = new BTimedBufferQueue();
	mBufferAvailable = create_sem (0, "Video buffer available");
	if (mBufferAvailable < B_NO_ERROR) {
		ERROR("TVideoCaptureView: couldn't create semaphore\n");
	}

	// create a service thread locking semaphore
	mServiceLock = create_sem (1, "Video Consumer Service Lock");
	if (mServiceLock < B_NO_ERROR) {
		ERROR("TVideoCaptureView: couldn't create ServiceLock semaphore\n");
	}

	// start the port service thread
	mServiceThread = spawn_thread(sRun, "TVideoCaptureView:run", B_REAL_TIME_PRIORITY, this);
	resume_thread(mServiceThread);

	// start the drawing thread
	int drawPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 5000);
	PROGRESS("Suggested draw thread priority: %d\n", drawPrio);
	mDisplayThread = spawn_thread(vRun, "TVideoCaptureView:draw", drawPrio, this);
	resume_thread(mDisplayThread);
}

//---------------------------------------------------------------

TVideoCaptureView::~TVideoCaptureView()
{
	FUNCTION("TVideoCaptureView::~TVideoCaptureView()\n");

	// signal the two healper threads to quit
	mControlQuit=true;
	mDisplayQuit = true;

	write_port(mPort, 0x60000000, NULL, 0);

	// wait for them to die
	status_t status;
	wait_for_thread(mServiceThread, &status);
	if (mDisplayThread != 0)
		wait_for_thread(mDisplayThread, &status);

	mDisplayThread = 0;
	mServiceThread = 0;

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		delete mBitmap[i];
	}

	while (mBufferQueue->HasBuffers()) {
		mBufferQueue->PopFirstBuffer(0)->Recycle();
	}

	delete mBufferQueue;
	mBufferQueue = 0;

	delete_port(mPort);
}

/********************************
        From BMediaNode
********************************/

//---------------------------------------------------------------

BMediaAddOn *
TVideoCaptureView::AddOn(long *) const
{
	return NULL;
}

//---------------------------------------------------------------

port_id
TVideoCaptureView::ControlPort() const
{
	return mPort;
}


//---------------------------------------------------------------

void
TVideoCaptureView::Start(
	bigtime_t performance_time)
{
	FUNCTION("TVideoCaptureView::Start() @ %.4f, now: %.4f\n",
	         (double)performance_time/M1, (double)TimeSource()->Now()/M1);

	if (!mStopping || performance_time > mStopTime) {
		if (!mRunning || mStopping) {
			mStarting = true;
			mStartTime = performance_time;
		}
	}
}


//---------------------------------------------------------------

void
TVideoCaptureView::Stop(
	bigtime_t performance_time,
	bool immediate)
{
	FUNCTION("TVideoCaptureView::Stop() @ %.4f, now: %.4f\n",
	         (double)performance_time/M1, (double)TimeSource()->Now()/M1);

	if (!mStarting || performance_time > mStartTime) {
		if (mRunning || mStarting) {
			mStopping = true;
			mStopTime = performance_time;
		}
	}

	if (immediate) {
		mRunning = false;
		mStopping = true;
		mStopTime = TimeSource()->Now();
	}
}


//---------------------------------------------------------------

void
TVideoCaptureView::Seek(
	bigtime_t media_time,
	bigtime_t performance_time)
{
	FUNCTION("TVideoCaptureView::Seek\n");

	/* we only play in performance_time -- we don't seek */
}


//---------------------------------------------------------------

BMediaNode::run_mode
TVideoCaptureView::RunMode()
{
	FUNCTION("TVideoCaptureView::RunMode\n");

	return mRunMode;
}

//---------------------------------------------------------------

void
TVideoCaptureView::SetRunMode(
	run_mode mode)
{
	FUNCTION("TVideoCaptureView::SetRunMode\n");

	mRunMode = mode;

}

//---------------------------------------------------------------

void
TVideoCaptureView::TimeWarp(bigtime_t at_real_time,
                            bigtime_t performance_time)
{
	FUNCTION("TVideoCaptureView::TimeWarp perf time %.4f @ %.4f\n",
	         (double)performance_time/M1, (double)at_real_time/M1);

}

//---------------------------------------------------------------

void
TVideoCaptureView::BufferReceived(BBuffer * buffer)
{
	LOOP("TVideoCaptureView::BufferReceived\n");

	uint32 connection = buffer->Header()->destination;

	LOOP("TVideoCaptureView::BufferReceived for connection %d\n", connection);

	if (mBufferQueue->PushBuffer(buffer, buffer->Header()->start_time) == B_OK)
		release_sem(mBufferAvailable);
	else{
		ERROR("TVideoCaptureView::BufferReceived - ERROR PUSHING BUFFER ONTO QUEUE\n");
		buffer->Recycle();
	}
}


//---------------------------------------------------------------

void
TVideoCaptureView::ProducerDataStatus(
	const media_destination & /*for_whom*/,
	int32 status,
	bigtime_t /*at_media_time*/)
{
	FUNCTION("TVideoCaptureView::ProducerDataStatus()\n");

	if (status==B_DATA_NOT_AVAILABLE) {
		BMessage m('TRDN');
		m.AddPointer("Consumer",this);
		be_app->PostMessage(&m);
	}
}

//---------------------------------------------------------------

status_t
TVideoCaptureView::Connected(
	const media_source & producer,
	const media_destination & where,
	const media_format & with_format,
	media_input * out_input)
{
	FUNCTION("TVideoCaptureView::Connected()\n");

	uint32 i;

	// find an unused connection
	for (i = 0; i < MAX_CONNECTIONS; i++)
		if (!mConnectionActive[i]) {
			mConnectionActive[i] = true;
			break;
		}

	if (i == MAX_CONNECTIONS)
		return B_MEDIA_NOT_CONNECTED;

	// fill out the media input struct
	out_input->node = Node();
	out_input->source = producer;
	out_input->destination = mDestination[i];
	out_input->format = with_format;
	sprintf(out_input->name, "TVideoCaptureView %d", i + 1);

	// remember the format
	mFormat[i] = with_format.u.raw_video;

	// create a new window for this connection
	uint32 mXSize = with_format.u.raw_video.display.line_width;
	uint32 mYSize = with_format.u.raw_video.display.line_count;
	uint32 mRowBytes = with_format.u.raw_video.display.bytes_per_row;
	color_space mColorspace = with_format.u.raw_video.display.format;

	mBitmap[i] = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);

	// increment the connection count
	mConnectionCount++;
	return B_OK;
}

//---------------------------------------------------------------

void
TVideoCaptureView::Disconnected(
	const media_source & producer,
	const media_destination & where)
{
	FUNCTION("TVideoCaptureView::Disconnect\n");

	uint32 connection = where.id;

	PROGRESS("TVideoCaptureView::Disconnect Connection #%d\n", connection);
	if (mBitmap[connection] != 0)
		delete mBitmap[connection];

	mConnectionActive[connection] = false;

}

//---------------------------------------------------------------

status_t
TVideoCaptureView::AcceptFormat(
	const media_destination & /*dest*/,
	media_format * format)
{
	FUNCTION("TVideoCaptureView::AcceptFormat()\n");
	if (format->type != B_MEDIA_NO_TYPE) {
		if (format->type != B_MEDIA_RAW_VIDEO) {
			ERROR("AcceptFormat - not B_MEDIA_RAW_VIDEO\n");
			return B_MEDIA_BAD_FORMAT;
		}
		if (format->u.raw_video.display.format != B_RGB32 &&
		    format->u.raw_video.display.format != B_RGB16 &&
		    format->u.raw_video.display.format != B_RGB15 &&
		    format->u.raw_video.display.format != media_raw_video_format::wildcard.display.format) {
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
TVideoCaptureView::GetNextInput(
	int32 * cookie,
	media_input * out_input)
{
	FUNCTION("TVideoCaptureView::GetNextInput()\n");

	// custom build a destination for this connection
	// put connection number in id
	mDestination[mConnectionCount].port = mPort;
	mDestination[mConnectionCount].id = mConnectionCount;

	if (*cookie == 0) {
		out_input->destination = mDestination[mConnectionCount];
		out_input->source = media_source::null;
		out_input->node = Node();
		out_input->format.u.raw_video = vid_format;
		out_input->format.type = B_MEDIA_RAW_VIDEO;
		sprintf(out_input->name, "TVideoCaptureView %d", mConnectionCount);
		*cookie = 1;
		return B_OK;
	}
	return B_MEDIA_BAD_DESTINATION;
}

//---------------------------------------------------------------

void
TVideoCaptureView::DisposeInputCookie(int32 /*cookie*/)
{
}

//---------------------------------------------------------------

status_t
TVideoCaptureView::GetLatencyFor(
	const media_destination & /* input */,
	bigtime_t * out_latency,
	media_node_id * out_timesource)
{
	FUNCTION("TVideoCaptureView::GetLatencyFor()\n");
	*out_latency = mMyLatency;
	*out_timesource = TimeSource()->ID();
	return B_OK;
}


//---------------------------------------------------------------

status_t
TVideoCaptureView::FormatChanged(
	const media_source & producer,
	const media_destination & consumer,
	int32 from_change_count,
	const media_format &format)
{
	FUNCTION("TVideoCaptureView::FormatChanged()\n");

	uint32 connection = consumer.id;

	// get the new raw_video format
	mFormat[connection] = format.u.raw_video;

	// grab the current width, height
	uint32 mXSize = format.u.raw_video.display.line_width;
	uint32 mYSize = format.u.raw_video.display.line_count;
	uint32 mRowBytes = format.u.raw_video.display.bytes_per_row;
	color_space mColorspace = format.u.raw_video.display.format;

	// destroy bitmap
	delete mBitmap[connection];

	// and create a new one
	mBitmap[connection] = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);

	return B_OK;
}

//---------------------------------------------------------------

status_t
TVideoCaptureView::sRun(void * data)
{
	FUNCTION("TVideoCaptureView::sRun()\n");
	((TVideoCaptureView *)data)->ServiceThread();
	return 0;
}

//---------------------------------------------------------------

struct
media_message {
	char whatever[B_MEDIA_MESSAGE_SIZE];
};

void
TVideoCaptureView::ServiceThread()
{
	FUNCTION("TVideoCaptureView::ServiceThread()\n");

	while (!mControlQuit) {
		status_t err=0;
		int32 code=0;
		media_message msg;
		err = read_port_etc(mPort, &code, &msg, sizeof(msg), B_TIMEOUT, 250000);
		if (err == B_TIMED_OUT) continue;
		if (err < B_OK) {
			ERROR("TVideoCaptureView::Run: Unexpected error in read_port(): %x\n", err);
			continue;
		}

		if (acquire_sem(mServiceLock) == B_NO_ERROR) {
			if (BMediaNode::HandleMessage(code, &msg, err) &&
			    BBufferConsumer::HandleMessage(code, &msg, err)) {
				BMediaNode::HandleBadMessage(code, &msg, err);
			}
			release_sem(mServiceLock);

			if (code == 0x60000000) /* quit! */
				break;
		}
	}

	FUNCTION("TVideoCaptureView::ServiceThread - END\n");
}

//---------------------------------------------------------------

status_t
TVideoCaptureView::vRun(void * data)
{
	FUNCTION("TVideoCaptureView::vRun()\n");
	((TVideoCaptureView *)data)->DisplayThread();
	return 0;
}

//---------------------------------------------------------------

void
TVideoCaptureView::DisplayThread()
{
	FUNCTION("TVideoCaptureView::DisplayThread\n");

	bigtime_t timeout = 5000;
	bigtime_t realTimeNow = 0;
	bigtime_t perfTimeNow = 0;
	bigtime_t halfPeriod = (bigtime_t) (500000./29.97);
	bool timeSourceRunning = false;

	while (!mDisplayQuit) {
		if (acquire_sem(mServiceLock) == B_NO_ERROR) {
			timeSourceRunning = TimeSource()->IsRunning();
			realTimeNow = BTimeSource::RealTime();
			perfTimeNow = TimeSource()->Now();
			release_sem(mServiceLock);
		}

		snooze(timeout);

		if (timeSourceRunning) {

			// if we received a Stop, deal with it
			if (mStopping) {
				PROGRESS("VidConsumer::DisplayThread - STOP\n");
				if (perfTimeNow >= mStopTime) {
					mRunning = false;
					mStopping = false;

					// deal with any pending Seek
					if (mSeeking)
						mSeeking = false;

					continue;
				}
			}

			// if we received a Seek, deal with it
			if (mSeeking) {
				PROGRESS("VidConsumer::DisplayThread - SEEK\n");
				if (perfTimeNow >= mSeekTime) {
					PROGRESS("VidConsumer::DisplayThread - DO SEEK\n");
					mSeeking = false;
					mDeltaTime = mMediaTime;
					continue;
				}
			}

			// if we received a Start, deal with it
			if (mStarting) {
				PROGRESS("VidConsumer::DisplayThread mStartTime = %.4f TimeNow = %.4f\n", (double)mStartTime/M1, (double)perfTimeNow/M1);
				if (perfTimeNow >= mStartTime) {
					mRunning = true;
					mStarting = false;
					mDeltaTime = mStartTime;
					continue;
				}
			}

			if (mRunning) {
				// check for buffer available.
				status_t err = acquire_sem_etc(mBufferAvailable, 1, B_TIMEOUT, halfPeriod * 2);

				if (err == B_TIMED_OUT) {
					ERROR("VidConsumer::DisplayThread - Error from acquire_sem_etc: 0x%x\n", err);
					continue;
				}

				BBuffer *buffer = mBufferQueue->PopFirstBuffer(0);
				uint32 connection = buffer->Header()->destination;

				LOOP("Popped buffer for connection %d, Start time: %.4f, system time: %.4f diff: %.4f\n",
				     connection,
				     (double) buffer->Header()->start_time/M1,
				     (double) perfTimeNow/M1,
				     (double) (buffer->Header()->start_time - perfTimeNow)/M1);

				// Display frame if we're in B_OFFLINE mode or
				// within +/- a half frame time of start time
				if ( (mRunMode == B_OFFLINE) ||
				     ((perfTimeNow > (buffer->Header()->start_time - halfPeriod)) &&
				      (perfTimeNow < (buffer->Header()->start_time + halfPeriod))) ) {
					memcpy(mBitmap[connection]->Bits(), buffer->Data(), mBitmap[connection]->BitsLength());
					buffer->Header()->start_time = system_time();
					buffer->Recycle();
					bigtime_t t1 = system_time();
					printf("DrawBitmap01\n");
					if (LockLooperWithTimeout(B_TIMEOUT) == B_OK) {
						//if (LockLooper())
						DrawBitmap(mBitmap[connection], Bounds());
						UnlockLooper();
					}
					t1 = system_time() - t1;
					if (t1/M1 > .030)
						printf("Draw time = %.4f\n",t1/M1);
					continue;
				} else   {
					// If we're too early, push frame back on stack
					if (perfTimeNow < buffer->Header()->start_time) {
						LOOP("push buffer back on stack!\n");
						mBufferQueue->PushBuffer(buffer, buffer->Header()->start_time);
						release_sem(mBufferAvailable);
						continue;
					} else   {
						// if we've already passed a half frame time past the buffer start time
						// and RunMode = INCREASE_LATENCY, increase latency and display the frame
						if ( (perfTimeNow > buffer->Header()->start_time) &&
						     (mRunMode == B_INCREASE_LATENCY)) {
							mMyLatency += halfPeriod;
							ERROR("VidConsumer::DisplayThread - Increased latency to: %.4f\n", mMyLatency);
							ERROR("	 Performance time: %.4f @ %.4f\n", (double)buffer->Header()->start_time/M1, (double)perfTimeNow/M1);
							memcpy(mBitmap[connection]->Bits(), buffer->Data(), mBitmap[connection]->BitsLength());
							buffer->Recycle();
							printf("DrawBitmap02\n");
							if ( LockLooperWithTimeout(B_TIMEOUT) == B_OK) {
								//if (LockLooper())
								DrawBitmap(mBitmap[connection], Bounds());
								UnlockLooper();
							}

							continue;
						} else   {
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

