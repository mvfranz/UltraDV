//	Copyright (c) 1998, Be Incorporated, All Rights Reserved.
//	SMS
//	AVIConsumer.cpp

#include "BuildApp.h"

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

#include "AppUtils.h"
#include "TRIFFWriter.h"

#include "AVIConsumer.h"

#define M1 ((double)1000000.0)

#define	FUNCTION	printf
#define ERROR		printf
#define PROGRESS	printf
#define LOOP		

media_raw_video_format vid_format = { 29.97,1,0,239,B_VIDEO_TOP_LEFT_RIGHT,1,1,{B_RGB32,320,240,320*4,0,0}};

//---------------------------------------------------------------

AVIConsumer::AVIConsumer(
	const char * name) :
	BMediaNode(name),
	BBufferConsumer(B_MEDIA_RAW_VIDEO)
{

	FUNCTION("AVIConsumer::AVIConsumer()\n");

	m_RIFFWriter = NULL;
	
	AddNodeKind(B_PHYSICAL_OUTPUT);
	
	mPort = create_port(3, "AVIConsumer input");
	
	mRunMode = B_DROP_DATA;
	
	mConnectionCount = 0;
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		mConnectionActive[i] = false;
		mFormat[i] = vid_format;
		mWindow[i] = NULL;
		mView[i] = NULL;
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
	
	mStartTime = 0;		/* when to start in performance time */
	mStopTime = 0;		/* when to stop in performance time */
	mSeekTime = 0;
	mMediaTime = 0;
	mDeltaTime = 0;
	
	mMyLatency = 10000;
	mDownstreamLatency = 0;	

	// create a buffer queue and buffer available semaphore
	mBufferQueue = new BTimedBufferQueue();
	mBufferAvailable = create_sem (0, "Video buffer available");
	if (mBufferAvailable < B_NO_ERROR)
	{
		ERROR("AVIConsumer: couldn't create semaphore\n");
	}	
	
	// create a service thread locking semaphore
	mServiceLock = create_sem (1, "Video Consumer Service Lock");
	if (mServiceLock < B_NO_ERROR)
	{
		ERROR("AVIConsumer: couldn't create ServiceLock semaphore\n");
	}
	
	// start the port service thread
	mServiceThread = spawn_thread(sRun, "AVIConsumer:run", B_REAL_TIME_PRIORITY, this);
	resume_thread(mServiceThread);	

	// start the drawing thread
	int drawPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 5000);
	PROGRESS("Suggested draw thread priority: %d\n", drawPrio);
	mDisplayThread = spawn_thread(vRun, "AVIConsumer:draw", drawPrio, this);
	resume_thread(mDisplayThread);
}

//---------------------------------------------------------------

AVIConsumer::~AVIConsumer()
{
	FUNCTION("AVIConsumer::~AVIConsumer()\n");
	
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
	
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (mWindow[i] != 0)
		{
			mWindow[i]->Lock();
			mWindow[i]->Close();
			delete mBitmap[i];
			mWindow[i] = 0;
		}
	}

	while (mBufferQueue->HasBuffers())
	{
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
AVIConsumer::AddOn(long *) const
{
	return NULL;
}

//---------------------------------------------------------------

port_id
AVIConsumer::ControlPort() const
{
	return mPort;
}


//---------------------------------------------------------------

void
AVIConsumer::Start(
	bigtime_t performance_time)
{
	FUNCTION("AVIConsumer::Start() @ %.4f, now: %.4f\n",
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
AVIConsumer::Stop(
	bigtime_t performance_time,
	bool immediate)
{
	FUNCTION("AVIConsumer::Stop() @ %.4f, now: %.4f\n",
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
AVIConsumer::Seek(
	bigtime_t media_time,
	bigtime_t performance_time)
{
	FUNCTION("AVIConsumer::Seek\n");
	
	/* we only play in performance_time -- we don't seek */
}


//---------------------------------------------------------------

BMediaNode::run_mode
AVIConsumer::RunMode()
{
	FUNCTION("AVIConsumer::RunMode\n");	
	
	return mRunMode;	
}

//---------------------------------------------------------------

void
AVIConsumer::SetRunMode(
	run_mode mode)
{
	FUNCTION("AVIConsumer::SetRunMode\n");
	
	mRunMode = mode;
	
}

//---------------------------------------------------------------

void
AVIConsumer::TimeWarp(bigtime_t at_real_time,
	bigtime_t performance_time)
{
	FUNCTION("AVIConsumer::TimeWarp perf time %.4f @ %.4f\n",
		(double)performance_time/M1, (double)at_real_time/M1);
	
}

//---------------------------------------------------------------

void
AVIConsumer::BufferReceived(BBuffer * buffer)
{
	LOOP("AVIConsumer::BufferReceived\n");
	
	uint32 connection = buffer->Header()->destination;
	
	LOOP("AVIConsumer::BufferReceived for connection %d\n", connection);
	
	if (mBufferQueue->PushBuffer(buffer, buffer->Header()->start_time) == B_OK)
		release_sem(mBufferAvailable);
	else
	{
		ERROR("AVIConsumer::BufferReceived - ERROR PUSHING BUFFER ONTO QUEUE\n");
		buffer->Recycle();
	}		
}


//---------------------------------------------------------------

void
AVIConsumer::ProducerDataStatus(
	const media_destination & /*for_whom*/,
	int32 status,
	bigtime_t /*at_media_time*/)
{
	FUNCTION("AVIConsumer::ProducerDataStatus()\n");
	
	if (status==B_DATA_NOT_AVAILABLE)
	{
		BMessage m('TRDN');
		m.AddPointer("Consumer",this);
		be_app->PostMessage(&m);
	}
}

//---------------------------------------------------------------

status_t
AVIConsumer::Connected(
	const media_source & producer,
	const media_destination & where,
	const media_format & with_format,
	media_input * out_input)
{
	FUNCTION("AVIConsumer::Connected()\n");
	
	uint32 i;
	
	// find an unused connection
	for (i = 0; i < MAX_CONNECTIONS; i++)
		if (!mConnectionActive[i])
		{
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
	sprintf(out_input->name, "AVIConsumer %d", i + 1);

	// remember the format
	mFormat[i] = with_format.u.raw_video;
	
	// create a new window for this connection
	mXSize 		= with_format.u.raw_video.display.line_width;
	mYSize 		= with_format.u.raw_video.display.line_count;	
	mRowBytes 	= with_format.u.raw_video.display.bytes_per_row;
	mColorspace = with_format.u.raw_video.display.format;

	mWindow[i] = new BWindow(BRect(40,40,40 + (mXSize-1), 40 + (mYSize - 1)), "Video Window", B_TITLED_WINDOW, B_NOT_CLOSABLE);
	mView[i]	= new BView(BRect(0, 0, (mXSize-1),(mYSize - 1)), "Play View", B_FOLLOW_ALL, B_WILL_DRAW);
	mWindow[i]->AddChild(mView[i]);
	mWindow[i]->Show();
	mBitmap[i] = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);

	// increment the connection count
	mConnectionCount++;	
	return B_OK;
}

//---------------------------------------------------------------

void
AVIConsumer::Disconnected(
	const media_source & producer,
	const media_destination & where)
{
	FUNCTION("AVIConsumer::Disconnect\n");

	uint32 connection = where.id;
	
	PROGRESS("AVIConsumer::Disconnect Connection #%d\n", connection);
	if (mWindow[connection] != 0)
	{
		mWindow[connection]->Lock();
		mWindow[connection]->Close();
		delete mBitmap[connection];
		mWindow[connection] = 0;
	}
	mConnectionActive[connection] = false;
		
}

//---------------------------------------------------------------

status_t
AVIConsumer::AcceptFormat(
	const media_destination & /*dest*/,
	media_format * format)
{
	FUNCTION("AVIConsumer::AcceptFormat()\n");
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
AVIConsumer::GetNextInput(
	int32 * cookie,
	media_input * out_input)
{
	FUNCTION("AVIConsumer::GetNextInput()\n");

	// custom build a destination for this connection
	// put connection number in id
	mDestination[mConnectionCount].port = mPort;
	mDestination[mConnectionCount].id = mConnectionCount;

	if (*cookie == 0)
	{
		out_input->destination = mDestination[mConnectionCount];
		out_input->source = media_source::null;
		out_input->node = Node();
		out_input->format.u.raw_video = vid_format;
		out_input->format.type = B_MEDIA_RAW_VIDEO;
		sprintf(out_input->name, "AVIConsumer %d", mConnectionCount);
		*cookie = 1;
		return B_OK;
	}
	return B_MEDIA_BAD_DESTINATION;
}

//---------------------------------------------------------------

void
AVIConsumer::DisposeInputCookie(int32 /*cookie*/)
{
}

//---------------------------------------------------------------

status_t
AVIConsumer::GetLatencyFor(
	const media_destination & /* input */,
	bigtime_t * out_latency,
	media_node_id * out_timesource)
{
	FUNCTION("AVIConsumer::GetLatencyFor()\n");
	*out_latency = mMyLatency;
	*out_timesource = TimeSource()->ID();
	return B_OK;
}


//---------------------------------------------------------------

status_t
AVIConsumer::FormatChanged(
				const media_source & producer,
				const media_destination & consumer, 
				int32 from_change_count,
				const media_format &format)
{
	FUNCTION("AVIConsumer::FormatChanged()\n");
	
	uint32 connection = consumer.id;
	
	// get the new raw_video format
	mFormat[connection] = format.u.raw_video;

	// grab the current width, height
	uint32 mXSize = format.u.raw_video.display.line_width;
	uint32 mYSize = format.u.raw_video.display.line_count;
	uint32 mRowBytes = format.u.raw_video.display.bytes_per_row;
	color_space mColorspace = format.u.raw_video.display.format;

	// destroy the old window, view, & bitmap
	mWindow[connection]->Lock();
	mWindow[connection]->Close();
	delete mBitmap[connection];						

	// and create a new set
	mWindow[connection] = new BWindow(BRect(40,40,40 + (mXSize-1), 40 + (mYSize - 1)), "Video Window", B_TITLED_WINDOW, B_NOT_CLOSABLE);
	mView[connection] = new BView(BRect(0, 0, (mXSize-1),(mYSize - 1)), "Play View", B_FOLLOW_ALL, B_WILL_DRAW);
	mWindow[connection]->AddChild(mView[connection]);
	mWindow[connection]->Show();
	mBitmap[connection] = new BBitmap(BRect(0, 0, (mXSize-1), (mYSize - 1)), mColorspace, false, false);
	
	return B_OK;
}

//---------------------------------------------------------------

status_t
AVIConsumer::sRun(void * data)
{
	FUNCTION("AVIConsumer::sRun()\n");
	((AVIConsumer *)data)->ServiceThread();
	return 0;
}

//---------------------------------------------------------------

struct
media_message 
{
	char whatever[B_MEDIA_MESSAGE_SIZE];
};

void
AVIConsumer::ServiceThread()
{
	FUNCTION("AVIConsumer::ServiceThread()\n");

	while (!mControlQuit)
	{
		status_t err=0;
		int32 code=0;
		media_message msg;
		err = read_port_etc(mPort, &code, &msg, sizeof(msg), B_TIMEOUT, 250000);
		if (err == B_TIMED_OUT) continue;
		if (err < B_OK)
		{
			ERROR("AVIConsumer::Run: Unexpected error in read_port(): %x\n", err);
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
	
	FUNCTION("AVIConsumer::ServiceThread - END\n");
}

//---------------------------------------------------------------

status_t
AVIConsumer::vRun(void * data)
{
	FUNCTION("AVIConsumer::vRun()\n");
	((AVIConsumer *)data)->DisplayThread();
	return 0;
}

//---------------------------------------------------------------

void
AVIConsumer::DisplayThread()
{
	FUNCTION("AVIConsumer::DisplayThread\n");
	
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
					//	Complete AVI file write
					if (m_RIFFWriter)
					{
						m_RIFFWriter->CompleteAVIFile();
						delete m_RIFFWriter;
						m_RIFFWriter = NULL;
					}

					mRunning 	= false;
					mStopping 	= false;
					
					// deal with any pending Seek
					if (mSeeking)
						mSeeking = false;
						
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
				PROGRESS("VidConsumer::DisplayThread mStartTime = %.4f TimeNow = %.4f\n", (double)mStartTime/M1, (double)perfTimeNow/M1);
				if (perfTimeNow >= mStartTime)
				{
					//	Initialize AVIFile
					InitAVIFile();	
					
					mRunning = true;
					mStarting = false;
					mDeltaTime = mStartTime;
					continue;
				}
			}
			
			if (mRunning)
			{
				// check for buffer available.		
				status_t err = acquire_sem_etc(mBufferAvailable, 1, B_TIMEOUT, halfPeriod * 2);
					
				if (err == B_TIMED_OUT)
				{
					ERROR("VidConsumer::DisplayThread - Error from acquire_sem_etc: 0x%x\n", err);
					continue;
				}
			
				BBuffer *buffer   = mBufferQueue->PopFirstBuffer(0);
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
					  (perfTimeNow < (buffer->Header()->start_time + halfPeriod))) )
				{
					
					//	Write video frame
					if (m_RIFFWriter)
						m_RIFFWriter->Write00DCChunk(kRiff_00db_Chunk, mBitmap[connection]->BitsLength(), buffer->Data());

					memcpy(mBitmap[connection]->Bits(), buffer->Data(), mBitmap[connection]->BitsLength());
					buffer->Header()->start_time = system_time();
					buffer->Recycle();
					bigtime_t t1 = system_time();
					mWindow[connection]->Lock();
					mView[connection]->DrawBitmap(mBitmap[connection], mView[connection]->Frame());
					mWindow[connection]->Unlock();
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
														
							//	Write video frame
							if (m_RIFFWriter)
								m_RIFFWriter->Write00DCChunk(kRiff_00db_Chunk, mBitmap[connection]->BitsLength(), buffer->Data());

							memcpy(mBitmap[connection]->Bits(), buffer->Data(), mBitmap[connection]->BitsLength());
							buffer->Recycle();
							// should send late notice
							mWindow[connection]->Lock();
							mView[connection]->DrawBitmap(mBitmap[connection], mView[connection]->Frame());
							mWindow[connection]->Unlock();
							
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


#pragma mark -
#pragma mark === AVI Functions ===

//---------------------------------------------------------------
//
//	InitAVIFIle
//
//---------------------------------------------------------------
//
//	Initialize AVI file for capture operation
//

bool AVIConsumer::InitAVIFile()
{
	bool retVal = false;

	//	Create video and audio capture files in appliactions directory					
	app_info appInfo;					
	be_app->GetAppInfo(&appInfo); 	
	BEntry entry(&appInfo.ref);
	BDirectory parentDir;
	entry.GetParent(&parentDir);
	BPath filePath(&parentDir, NULL, false);
	
	/*
	//	Create temp audio capture file
	char tmpFileName[B_FILE_NAME_LENGTH];
	BFile *audioFile = new BFile();
	uint32 time = real_time_clock();
	sprintf(tmpFileName, "%s/%ldAVIGrabber.tmp", filePath.Path(), time);
	audioFile->SetTo(tmpFileName, B_CREATE_FILE | B_READ_WRITE | B_ERASE_FILE);
	
	//	Set up audio capture stucture
	index 			 	= 0;
	aud.buffer 		 	= audio_buffer;
	aud.buffer_index 	= &index;
	aud.audioFile		= audioFile;
	*/
	
	//	Create main video capture file				   	
	char fileName[B_FILE_NAME_LENGTH];
	//sprintf(fileName, "%s/%s", filePath.Path(), videoApplication->fControlWindow->fVideoFileName->Text());
	sprintf(fileName, "%s/%s", filePath.Path(), "Test.AVI");
	
	printf("%s\n", fileName);
		
	BFile *theFile = new BFile();
	if (theFile->SetTo(fileName, B_CREATE_FILE | B_READ_WRITE | B_ERASE_FILE) == B_NO_ERROR)
	{
		
		//	Clean up old RIFFWriter	
		if (m_RIFFWriter)
		{
			delete m_RIFFWriter;
			m_RIFFWriter = NULL;
		}
		
		//	Create RIFFWriter
		m_RIFFWriter = new TRIFFWriter(theFile);
		
		//mColorspace = with_format.u.raw_video.display.format;

		//	Set up initial AVI headers
		AVIHeader *aviHeader = m_RIFFWriter->GetAVIHeader();
		
		aviHeader->TimeBetweenFrames	= 29.97 * 1000 * 1000;	//	In microseconds
		aviHeader->MaximumDataRate		= 0;
		aviHeader->PaddingGranularity	= 0;
		aviHeader->Flags				= kAVIHasIndexFlag | kAVIMustUseIndexFlag;
		aviHeader->TotalNumberOfFrames	= 0;	// Come back and update this!
		aviHeader->NumberOfInitialFrames= 0;
		aviHeader->NumberOfStreams		= 1;	//	One video, one audio
		//aviHeader->SuggestedBufferSize = ((uint32)fRingBuffer2[0]->ImageSize().x * (uint32)fRingBuffer2[0]->ImageSize().y) * 8;
		//aviHeader->Width				= (uint32)fRingBuffer2[0]->ImageSize().x;
		//aviHeader->Height				= (uint32)fRingBuffer2[0]->ImageSize().y;
		aviHeader->SuggestedBufferSize	= mYSize * mRowBytes;
		aviHeader->Width				= mXSize;
		aviHeader->Height				= mYSize;
		aviHeader->TimeScale			= 1;	//	For video...
		aviHeader->DataRate				= 30;	//	Video FPS
		aviHeader->StartTime			= 0;
		aviHeader->DataLength			= 0;	//	Number of video frames...	
		
		//	Set up number of streams
		retVal = m_RIFFWriter->SetStreamCount(aviHeader->NumberOfStreams);
		if (!retVal)
			return retVal;
		
		//	Set up first stream header, in this case video
		AVIStreamHeader	*streamOneHeader 	= m_RIFFWriter->GetStreamOneHeader();
		streamOneHeader->DataType			= kRiff_vids_Chunk;
		streamOneHeader->DataHandler		= RIFF_rgb;
		streamOneHeader->Flags				= 0;
		streamOneHeader->Priority			= 1;
		streamOneHeader->InitialFrames		= 0;
		streamOneHeader->TimeScale			= 1;	//	For video...
		streamOneHeader->DataRate			= 30;	//	Video frames per second
		streamOneHeader->StartTime			= 0;
		streamOneHeader->DataLength			= 0;	// Come back and update this! # of frames for video
		streamOneHeader->SuggestedBufferSize = aviHeader->SuggestedBufferSize;
		streamOneHeader->Quality			= 0;
		streamOneHeader->SampleSize			= 0;
		
		//	Setup VIDSHeader
		AVIVIDSHeader *vidsHeader 	= m_RIFFWriter->GetVIDSHeader();
		vidsHeader->Size			= sizeof(AVIVIDSHeader);
		vidsHeader->Width			= mXSize;
		vidsHeader->Height			= mYSize;
		vidsHeader->Planes			= 1;
		vidsHeader->BitCount		= (int16) GetBitmapPixelSize(mColorspace);
		vidsHeader->Compression		= RIFF_rgb;
		vidsHeader->ImageSize		= 0;
		vidsHeader->XPelsPerMeter	= 0;
		vidsHeader->YPelsPerMeter	= 0;
		vidsHeader->NumColors		= 0;
		vidsHeader->ImpColors		= 0;
		
		/*
		//	Set up second stream header, in this case audio
		AVIStreamHeader	*streamTwoHeader 	= m_RIFFWriter->GetStreamTwoHeader();
		streamTwoHeader->DataType			= kRiff_auds_Chunk;
		streamTwoHeader->DataHandler		= WAVE_FORMAT_PCM;
		streamTwoHeader->Flags				= 0;
		streamTwoHeader->Priority			= 2;
		streamTwoHeader->InitialFrames		= 0;
		streamTwoHeader->TimeScale			= 1;	//	For video...
		streamTwoHeader->DataRate			= 30;	//	Video frames per second
		streamTwoHeader->StartTime			= 0;
		streamTwoHeader->DataLength			= 0;	// Come back and update this!
		streamTwoHeader->SuggestedBufferSize= 0;
		streamTwoHeader->Quality			= 0;
		streamTwoHeader->SampleSize			= 0;
		
		//	Setup AUDSHeader
		AVIAUDSHeader *audsHeader 	= m_RIFFWriter->GetAUDSHeader();						
		audsHeader->Format			= WAVE_FORMAT_PCM;
		audsHeader->Channels		= 2;  
		audsHeader->Rate			= 44100;
		audsHeader->AverageDataRate	= 0;
		audsHeader->BlockAlign		= 0;
		audsHeader->Size			= 16;
		audsHeader->ExtensionSize	= 12;	//	Size of extended data
		audsHeader->SamplesPerBlock	= 0;	// 	S Used by MSADPCM and Intel DVI
		audsHeader->NumCoefficients	= 0;	// 	S Used by MSADPCM  num of follow sets
		audsHeader->Coefficients	= 0;	// 	Used by MSADPCM  coef1, coef2
		audsHeader->Style			= 0;  	// 	- SIGN2 or unsigned
		audsHeader->ByteCount		= 0;	// 	used to keep track of length
		*/
		
		//	Now write header info out
		retVal = m_RIFFWriter->InitAVIFile();
	}
	
	return retVal;
}

// ABH added this

//---------------------------------------------------------------------
//	GetBitmapPixelSize
//---------------------------------------------------------------------
//
//	Return size of bitmap pixle based on color space

int32 AVIConsumer::GetBitmapPixelSize(color_space colorSpace)
{
	switch(colorSpace)
	{
		// Unused and non-values
		case B_NO_COLOR_SPACE:
		case B_RGB24:
		case B_RGB24_BIG:
		case B_YUV422:
		case B_YUV411:
		case B_YUV420:
		case B_YUV444:
		case B_YUV9:
		case B_YUV12:
			return(-1);
			break;	
		
		// 32-bit
			case B_RGB32:
		case B_RGBA32:
		case B_RGB32_BIG:
		case B_RGBA32_BIG:
			return sizeof(uint32);
			break;
		
		// 16-bit
		case B_RGB16:
		case B_RGB15:
		case B_RGBA15:
		case B_RGB16_BIG:
		case B_RGB15_BIG:
		case B_RGBA15_BIG:
			return sizeof(uint16);
			break;
			

		// 8-bit
		case B_CMAP8:
		case B_GRAY8:
			return sizeof(uchar);
			break;

		// Monochrome
		case B_GRAY1:
			return sizeof(uchar);
			break;
		
		default:
			return(-1);
			break;
		
	}
}
