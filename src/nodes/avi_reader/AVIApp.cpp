//---------------------------------------------------------------------
//
//	File:	AVIApp.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVI Application
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include "BuildApp.h"

#include <Debug.h>
#include <string.h>
#include <Buffer.h>

#include "TimedBufferQueue.h"

#include "TRIFFReader.h"

#include "AppAlerts.h"
#include "AppUtils.h"
#include "AVIProducer.h"

// ABH added for FILE_OPEN_MSG
#include "MenuMessages.h"


#include "AVIApp.h"


//	Defines
#define JITTER_LATENCY 2000
#define M1 ((double)1000000.0)

#define FUNCTION        printf
#define ERROR           printf
#define PROGRESS        printf


//-------------------------------------------------------------------
//	main
//-------------------------------------------------------------------
//
//

int main()
{
	//	Create debugger thread
	//thread_id debugThread = spawn_thread( debug_func, "avi_debug", B_NORMAL_PRIORITY, NULL);
	//resume_thread(debugThread);

	new AVIApp();
	be_app->Run();
}


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

AVIApp::AVIApp() : BApplication("application/x-vnd.mediapede.AVIApp"), BMediaNode("AVIConsumer"), BBufferConsumer(B_MEDIA_MULTISTREAM)
{
	//	Set up defaults
	m_MediaRoster           = NULL;
	m_AVIWindow             = NULL;
	m_Port                          = NULL;
	m_ServiceThread         = NULL;
	m_RunThread                     = NULL;
	m_FrameSize                     = 0;
	m_FrameAmountRead       = 0;

	m_TimeToQuit            = false;
	m_Connected             = false;
	m_Running                       = false;
	m_Starting                      = false;
	m_Stopping                      = false;
	m_Seeking                       = false;

	m_StartTime             = 0;            //	When to start in performance time
	m_StopTime                      = 0;            //	When to stop in performance time
	m_SeekTime                      = 0;
	m_MediaTime             = 0;
	m_DeltaTime             = 0;


	m_DownstreamLatency = 0;
	m_PrivateLatency        = 0;


	m_FrameCount            = 0;

	m_BufferQueue = new BTimedBufferQueue();

	//	Connect to Media Server
	m_MediaRoster = BMediaRoster::Roster();

	if (!m_MediaRoster) {
		// the Media Server appears to be dead -- handle that here
		printf("MediaRoster failure!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Set up identifying media_format structure
	m_MediaFormat.type = B_MEDIA_MULTISTREAM;

	m_MediaFormat.u.multistream.avg_bit_rate        = 1;
	m_MediaFormat.u.multistream.max_bit_rate        = 1;
	m_MediaFormat.u.multistream.avg_chunk_size      = 1;
	m_MediaFormat.u.multistream.max_chunk_size      = 1;
	m_MediaFormat.u.multistream.flags =
		m_MediaFormat.u.multistream.B_HEADER_HAS_FLAGS |
		m_MediaFormat.u.multistream.B_CLEAN_BUFFERS |
		m_MediaFormat.u.multistream.B_HOMOGENOUS_BUFFERS;
	m_MediaFormat.u.multistream.format = m_MediaFormat.u.multistream.B_AVI;

	m_MediaFormat.u.multistream.u.avi.us_per_frame  = 0;
	m_MediaFormat.u.multistream.u.avi.width                 = 0;
	m_MediaFormat.u.multistream.u.avi.height                = 0;
	m_MediaFormat.u.multistream.u.avi.type_count    = 4;
	m_MediaFormat.u.multistream.u.avi.types[0]              = B_MEDIA_RAW_AUDIO;
	m_MediaFormat.u.multistream.u.avi.types[1]              = B_MEDIA_RAW_VIDEO;
	m_MediaFormat.u.multistream.u.avi.types[2]              = B_MEDIA_ENCODED_AUDIO;
	m_MediaFormat.u.multistream.u.avi.types[3]              = B_MEDIA_ENCODED_VIDEO;

	//	Inform system about our node type
	AddNodeKind(B_PHYSICAL_OUTPUT);

	//	Create our port
	m_Port = create_port(61, "AVIReaderNodePort");

	//	Set up destination
	m_Destination.port      = m_Port;
	m_Destination.id        = 0;

	//	Create port service thread
	m_ServiceThread = spawn_thread(service_routine, "AVIConsumer::ServiceThread", B_NORMAL_PRIORITY, this);
	resume_thread(m_ServiceThread);

	//	Register ourself with the roster
	m_MediaRoster->RegisterNode(this);

	//	Create AVIProducer and register it
	m_AVIProducer = new AVIProducer();
	m_MediaRoster->RegisterNode(m_AVIProducer);

	//	Create drawing thr
	int drawPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 5000);
	m_RunThread = spawn_thread(run_routine, "AVIConsumer:Run", drawPrio, this);
	resume_thread(m_RunThread);


	//	Attempt to load AVI file
	ShowFileOpenPanel();

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

AVIApp::~AVIApp()
{
	m_TimeToQuit = true;

	//	Send quit to port
	write_port(m_Port, 0x60000000, NULL, 0);

	//	Wait for threads to exit
	status_t status;
	wait_for_thread(m_ServiceThread, &status);
	wait_for_thread(m_RunThread, &status);

	delete_port(m_Port);

	//	Clean up
	delete m_BufferQueue;
}


#pragma mark -
#pragma mark === Initialization ===


//-------------------------------------------------------------------
//	InitNodes
//-------------------------------------------------------------------
//
//

status_t AVIApp::InitNodes(entry_ref &fileRef)
{
	status_t retVal = B_OK;

	//	Sniff Ref
	BMimeType outMimeType;
	float outCapability=0;
	retVal = m_MediaRoster->SniffRefFor(m_AVIProducer->Node(), fileRef, &outMimeType, &outCapability);
	if(retVal != B_OK) {
		printf("SniffRefFor failure - Node ID: %d\n", m_AVIProducer->Node().node);
		printf("Error: %s - %d\n", strerror(retVal), retVal);

		// ABH should recover and try another file
//		be_app->PostMessage(FILE_OPEN_MSG); // ABH was B_QUIT_REQUESTED
		return retVal;
	}

	//	We have a good ref.  Set it for the node
	bigtime_t outDuration=0;
	retVal = m_MediaRoster->SetRefFor(m_AVIProducer->Node(), fileRef, false, &outDuration);

	if (retVal == B_BAD_PORT_ID)
		printf("SetRefFor: bad port if\n");
	if (retVal == B_TIMED_OUT)
		printf("SetRefFor: timed out\n");
	if (retVal == B_WOULD_BLOCK)
		printf("SetRefFor: would block\n");

	if(retVal == B_OK || retVal == 1) {
		// SetRefFor is returning '1', which is not failure!
		printf("SetRefFor failure - Node ID: %d\n", m_AVIProducer->Node().node);
		printf("Error: %s - %d\n", strerror(retVal), retVal);
	} else {
		// is fatal
		printf("SetRefFor failure - Node ID: %d\n", m_AVIProducer->Node().node);
		printf("Error: %s - %d\n", strerror(retVal), retVal);
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	outDuration = 0;                // ABH remove this! Just a temp hack!

	printf("SetRefFor: OutDuration = %ld\n",outDuration);


	//	Get the free outputs for the AVIProducer
	int32 numOutputs = 1;
	int32 numFreeOutputs = 0;
	retVal = m_MediaRoster->GetFreeOutputsFor(m_AVIProducer->Node(), &m_From, numOutputs, &numFreeOutputs);
	if (retVal < 0 || numFreeOutputs < 1) {
		printf("The video input is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	//	Get the free inputs for the AVIConsumer
	int32 numInputs = 1;
	int32 numFreeInputs = 0;
	retVal = m_MediaRoster->GetFreeInputsFor( Node(), &m_To, numInputs, &numFreeInputs);
	if (retVal < 0 || numFreeInputs < 1) {
		printf("The video output is busy!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	//	Now connect to the proper output
	retVal = m_MediaRoster->Connect(m_From.source, m_To.destination, &m_From.format, &m_From, &m_To);
	if (retVal != B_OK) {
		printf("Connect failure!\n");

		//	Exit
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	//	Create time source
	retVal = m_MediaRoster->GetSystemTimeSource(&m_TimeSourceNode);
	if (retVal != B_OK) {
		printf("GetSystemTimeSource failure!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	//	Set AVIProducer time source
	retVal = m_MediaRoster->SetTimeSourceFor(m_AVIProducer->Node().node, m_TimeSourceNode.node);
	if (retVal != B_OK) {
		printf("Producer SetTimeSourceFor failure!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	//	Set AVIConsumer time source
	retVal = m_MediaRoster->SetTimeSourceFor(this->Node().node, m_TimeSourceNode.node);
	if (retVal != B_OK) {
		printf("Consumer SetTimeSourceFor failure!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}

	return retVal;
}

#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	QuitRequested
//-------------------------------------------------------------------
//
//

bool AVIApp::QuitRequested()
{
	//	Close window
	if (m_AVIWindow->Lock()) {
		m_AVIWindow->Quit();
	}

	m_AVIWindow = NULL;

	//	Close running connections
	m_MediaRoster->StopNode(m_AVIProducer->Node(), 0, true);
	m_MediaRoster->StopNode(this->Node(), 0, true);
	m_MediaRoster->Disconnect(m_From.node.node, m_From.source, m_To.node.node, m_To.destination);

	//	Stop time source
	m_MediaRoster->StopNode(m_TimeSourceNode, 0);

	return true;
}


//-------------------------------------------------------------------
//	MessageReceived
//-------------------------------------------------------------------
//
//

void AVIApp::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
	//	Show Open Dialog
	case FILE_OPEN_MSG:
		ShowFileOpenPanel();
		break;

	//	Quit app if no editors are open
	case B_CANCEL:
		if (!m_AVIWindow)
			PostMessage(B_QUIT_REQUESTED);
		break;

	default:
		BApplication::MessageReceived(message);
		break;
	}
}


//-------------------------------------------------------------------
//	HandleMessage
//-------------------------------------------------------------------
//
//

status_t AVIApp::HandleMessage(int32 message, const void *data, size_t size)
{
	switch(message)
	{

	//	Call derived classes
	default:
	{
		if ( (BBufferConsumer::HandleMessage(message, data, size)) &&
		     (BMediaNode::HandleMessage(message, data, size)) ) {
			//printf("AVIApp::HandleBadMessage\n");
			BMediaNode::HandleBadMessage(message, data, size);
		}
	}
	break;
	}

	return B_OK;
}


//-------------------------------------------------------------------
//	RefsReceived
//-------------------------------------------------------------------
//
//	Called by either File Open or a File Drag message
//

void AVIApp::RefsReceived(BMessage *message)
{
	status_t retVal;

	//	Extract file ref from message
	entry_ref fileRef;
	message->FindRef("refs", 0, &fileRef);

	//	Resolve possible symlink...
	BEntry entry(&fileRef, true);
	entry.GetRef(&fileRef);

	//	Initalize nodes
	retVal = InitNodes(fileRef);
	if (retVal != B_OK) {
		printf("InitNode failure.\n");
		// ABH need to recover from failure here!

		be_app->PostMessage(FILE_OPEN_MSG); // ABH was B_QUIT_REQUESTED
		return;
	} else {


		//	Start playback
		bigtime_t start_delay = 0;
		BMediaRoster::Roster()->GetStartLatencyFor(m_TimeSourceNode, &start_delay);
		start_delay += estimate_max_scheduling_latency(find_thread(NULL));

		bigtime_t perf = system_time() + start_delay;
		bigtime_t real = BTimeSource::RealTime();

		retVal = m_MediaRoster->StartNode(m_To.node, perf);
		if (retVal < B_OK) {
			printf("Couldn't start AVIProducer!\n");
			return;
		}

		retVal = m_MediaRoster->StartNode(m_From.node, perf);
		if (retVal < B_OK) {
			printf("Couldn't start AVIConsumer!\n");
			return;
		}

		//	workaround for possibly broken Now()
		retVal = m_MediaRoster->SeekNode(m_TimeSourceNode, perf-start_delay, real);
		if (retVal < B_OK) {
			printf("Couldn't Seek() TimeSource!\n");
			return;
		}

		retVal = m_MediaRoster->StartNode(m_TimeSourceNode, real);
		if (retVal < B_OK) {
			printf("Couldn't start TimeSource!\n");
			return;
		}
	}
}



//---------------------------------------------------------------------
//	ShowFileOpenPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void AVIApp::ShowFileOpenPanel()
{

	// If the panel has already been constructed, show the panel
	// Otherwise, create the panel
	if (m_FileOpenPanel)
		m_FileOpenPanel->Show();
	else {
		// Create a RefFilter for a "video" type
		//TRefFilter *refFilter = new TRefFilter(kVideoFilter);

		// Construct a file panel and set it to modal
		m_FileOpenPanel = new BFilePanel( B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false, NULL, NULL, true, true );

		// Set it to application's home directory
		app_info appInfo;
		be_app->GetAppInfo(&appInfo);
		BEntry entry(&appInfo.ref);
		BDirectory parentDir;
		entry.GetParent(&parentDir);
		m_FileOpenPanel->SetPanelDirectory(&parentDir);

		// Center Panel
// ABH CenterWindow not defined
//		CenterWindow(m_FileOpenPanel->Window());

		// ABH hack
		((AVIApp *)be_app)->ShowFileOpenPanel();
	}

//	m_FileOpenPanel->Show();	// ABH show twice??
}


#pragma mark -
#pragma mark === MediaNode Functions ===


//---------------------------------------------------------------------
//	ControlPort
//---------------------------------------------------------------------
//
//

port_id AVIApp::ControlPort() const
{
	return m_Port;
}


//---------------------------------------------------------------------
//	AddOn
//---------------------------------------------------------------------
//
//

BMediaAddOn     *AVIApp::AddOn( int32 * internal_id) const
{
	return NULL;
}


#pragma mark -
#pragma mark === BufferConsumer Functions ===

//---------------------------------------------------------------------
//	AcceptFormat
//---------------------------------------------------------------------
//
//

status_t AVIApp::AcceptFormat(const media_destination &dest, media_format *format)
{
	//	Check for wildcards
	if (format->type == 0) {
		//	Set to our preferred type
		format->type = B_MEDIA_MULTISTREAM;

		return B_OK;
	}

	//	We only handle AVI video
	switch(format->type)
	{
	case B_MEDIA_MULTISTREAM:
		return B_OK;

	default:
		return B_MEDIA_BAD_FORMAT;
	}
}


//---------------------------------------------------------------------
//	GetNextInput
//---------------------------------------------------------------------
//
//

status_t AVIApp::GetNextInput( int32 *cookie, media_input *outInput)
{
	if (*cookie) {
		return B_ERROR;
	}

	BAutolock lock(m_Lock);

	//	Fill input
	outInput->node                          = Node();
	outInput->destination           = m_Destination;
	outInput->destination.id        = *cookie;
	outInput->source                        = media_source::null;
	outInput->format                        = m_MediaFormat;
	sprintf(outInput->name, "AVIConsumer");

	//	Increment cookie
	*cookie = *cookie + 1;

	return B_OK;
}


//---------------------------------------------------------------------
//	DisposeInputCookie
//---------------------------------------------------------------------
//
//

void AVIApp::DisposeInputCookie(int32 cookie)
{
}


//---------------------------------------------------------------------
//	BufferReceived
//---------------------------------------------------------------------
//
//	We have received a buffer.  Handle it...
//

void AVIApp::BufferReceived(BBuffer *buffer)
{
	// ABH virtual methhod cannot be called! BBufferConsumer::BufferReceived(buffer);

	m_BufferQueue->PushBuffer(buffer, buffer->Header()->start_time);
}


//---------------------------------------------------------------------
//	ProducerDataStatus
//---------------------------------------------------------------------
//
//

void AVIApp::ProducerDataStatus( const media_destination &for_whom, int32 status, bigtime_t at_media_time)
{
	if (status == B_DATA_NOT_AVAILABLE) {
		BMessage m('TRDN');
		m.AddPointer("Consumer", this);
		be_app->PostMessage(&m);
	}
}


//---------------------------------------------------------------------
//	GetLatencyFor
//---------------------------------------------------------------------
//
//

status_t AVIApp::GetLatencyFor( const media_destination &for_whom, bigtime_t *outLatency, media_node_id *outTimesource)
{
	//	Who do we calculate for?
	//for_whom

	*outLatency             = 8000;
	*outTimesource  = m_TimeSourceNode.node;

	return B_OK;
}


//---------------------------------------------------------------------
//	Connected
//---------------------------------------------------------------------
//
//

status_t AVIApp::Connected( const media_source &producer, const media_destination &whichInput,
                            const media_format &format, media_input *outInput)
{
	// ABH virtual method cannot be called!
	// BBufferConsumer::Connected(producer, whichInput, format, outInput);

	BAutolock lock(m_Lock);

	//	Set up producer info
	m_ConnectedFormat       = format;

	//	Calculate video frame size
	m_FrameSize = (m_ConnectedFormat.u.multistream.u.avi.width * m_ConnectedFormat.u.multistream.u.avi.height) * sizeof(uint32);

	//	Set up output info
	outInput->node                  = Node();
	outInput->source                = m_Producer = producer;
	outInput->destination   = m_Destination;
	outInput->format                = format;
	sprintf(outInput->name, "VideoConsumer");

	//	Get stats for window and view creation
	int32 width      = m_ConnectedFormat.u.multistream.u.avi.width;
	int32 height = m_ConnectedFormat.u.multistream.u.avi.height;
	BRect bounds(0, 0, width-2, height-2);
	bounds.OffsetBy(40, 40);

	//	Create display window
	if (m_AVIWindow == NULL) {
		m_AVIWindow = new BWindow( bounds, "AVIWindow", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		                           B_NOT_CLOSABLE|B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE|B_NOT_ZOOMABLE|B_NOT_MINIMIZABLE);
		m_AVIView       = new BView(BRect(0, 0, width-1, height-1), "AVIView", B_FOLLOW_ALL, B_WILL_DRAW);
		m_AVIWindow->AddChild(m_AVIView);
		m_AVIWindow->Show();

		//	Create display bitmap
		BRect bitBounds(0, 0, width-1, height-1);
		m_Bitmap = new BBitmap(bitBounds, B_RGB32, false, false);
	}

	m_Connected = true;

	return B_OK;

}


//---------------------------------------------------------------------
//	Disconnected
//---------------------------------------------------------------------
//
//

void AVIApp::Disconnected( const media_source &producer, const media_destination & where)
{
	// ABH virtual method cannot be called!
	// BBufferConsumer::Disconnected(producer, where);

	m_Connected = false;

	BAutolock lock(m_Lock);

	if (m_AVIWindow != NULL) {
		m_AVIWindow->Lock();
		m_AVIWindow->Close();
		delete m_Bitmap;
		m_AVIWindow = NULL;
	}
}

//---------------------------------------------------------------------
//	FormatChanged
//---------------------------------------------------------------------
//
//

status_t AVIApp::FormatChanged( const media_source &producer, const media_destination &consumer,
                                int32 from_change_count, const media_format & format)
{
	return B_ERROR;
}



#pragma mark -
#pragma mark === Node Control ===

//-------------------------------------------------------------------
//	Start
//-------------------------------------------------------------------
//
//	Begin node execution
//

void AVIApp::Start(bigtime_t performance_time)
{
	BMediaNode::Start(performance_time);

	BAutolock lock(m_Lock);

	if (!m_Stopping || performance_time > m_StopTime) {
		if (!m_Running || m_Stopping) {
			m_Starting      = true;
			m_StartTime = performance_time;
		}
	}
}


//-------------------------------------------------------------------
//	Stop
//-------------------------------------------------------------------
//
//	Stop node execution
//

void AVIApp::Stop(bigtime_t performance_time, bool immediate)
{
	BMediaNode::Stop(performance_time, immediate);

	BAutolock lock(m_Lock);

	if (!m_Starting || performance_time > m_StartTime) {
		if (m_Running || m_Starting) {
			m_Stopping = true;
			m_StopTime = performance_time;
		}
	}

	if (immediate) {
		m_Running = false;
	}
}


//-------------------------------------------------------------------
//	Seek
//-------------------------------------------------------------------
//
//	Seet to specified time
//

void AVIApp::Seek(bigtime_t media_time, bigtime_t performance_time)
{
	BMediaNode::Seek(media_time, performance_time);
	BBufferConsumer::Seek(media_time, performance_time);

	//	We only play in performance_time -- we don't seek...
}


//-------------------------------------------------------------------
//	SetRunMode
//-------------------------------------------------------------------
//
//

void AVIApp::SetRunMode(run_mode mode)
{
	BMediaNode::SetRunMode(mode);
}


//-------------------------------------------------------------------
//	TimeWarp
//-------------------------------------------------------------------
//
//

void AVIApp::TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time)
{
	BMediaNode::TimeWarp(at_real_time, to_performance_time);
}

#pragma mark -
#pragma mark === Thread Functions ===

//-------------------------------------------------------------------
//	service_routine
//-------------------------------------------------------------------
//
//	Static service thread function
//

status_t AVIApp::service_routine(void * data)
{
	((AVIApp *)data)->ServiceRoutine();
	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void AVIApp::ServiceRoutine()
{
	while (!m_TimeToQuit) {

		status_t err  = 0;
		int32 code = 0;
		media_message msg;

		err = read_port_etc(m_Port, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);

		if (err == B_TIMED_OUT)
			continue;

		if (err < B_OK) {
			ERROR("AVIConsumer::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
			continue;
		}

		//	dispatch message
		if (code == 0x60000000) // quit!
			break;

		HandleMessage(code, &msg, err);
	}
}


//-------------------------------------------------------------------
//	run_routine
//-------------------------------------------------------------------
//
//	Static run thread function
//

status_t AVIApp::run_routine(void *data)
{
	((AVIApp *)data)->RunRoutine();

	return 0;
}


//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void AVIApp::RunRoutine()
{
	bigtime_t timeout         = 5000;
	bigtime_t realTimeNow = 0;
	bigtime_t perfTimeNow = 0;
	bigtime_t halfPeriod      = 0;

	while (!m_TimeToQuit) {
		snooze(timeout);

		bool timeSourceRunning = TimeSource()->IsRunning();

		if (timeSourceRunning) {
			realTimeNow = BTimeSource::RealTime();
			perfTimeNow = TimeSource()->PerformanceTimeFor(realTimeNow);

			//	Handle stop message
			if (m_Stopping) {
				printf("AVIConsumer::RunRoutine - STOP\n");
				if (realTimeNow >= TimeSource()->RealTimeFor(m_StopTime, m_PrivateLatency + m_DownstreamLatency)) {
					m_Running       = false;
					m_Stopping      = false;

					// deal with any pending Seek
					if (m_Seeking)
						m_Seeking = false;

					continue;
				}
			}


			//	Handle seek message
			if (m_Seeking) {
				printf("AVIConsumer::RunRoutine - SEEK\n");
				if (realTimeNow >= TimeSource()->RealTimeFor(m_SeekTime, m_PrivateLatency + m_DownstreamLatency)) {
					printf("AVIConsumer::DisplayThread - DO SEEK\n");
					m_Seeking       = false;
					m_DeltaTime = m_MediaTime;

					continue;
				}
			}

			//	If we received a Start, deal with it
			if (m_Starting) {
				//	Calculate frame period
				halfPeriod = (bigtime_t)m_ConnectedFormat.u.multistream.u.avi.us_per_frame * 1000;

				if (realTimeNow >= TimeSource()->RealTimeFor(m_StartTime, m_PrivateLatency + m_DownstreamLatency)) {
					m_Running       = true;
					m_Starting      = false;
					m_DeltaTime = m_StartTime;

					continue;
				}
			}


			if (m_Running) {
				BBuffer *buffer = m_BufferQueue->PopFirstBuffer(0);

				//printf( "Popped buffer #%d, Start time: %d, system time: %d\n",
				//		buffer->ID(), (uint32) buffer->Header()->start_time, TimeSource()->Now());

				//	Make sure we have a valid buffer
				if (buffer) {
					//	Drop frame if we are past its performance time
					if (perfTimeNow > (buffer->Header()->start_time + halfPeriod)) {
						printf("dropped late frame: %.4f @ %.4f\n", (double)buffer->Header()->start_time/1000000., (double)TimeSource()->PerformanceTimeFor(realTimeNow)/1000000.);
						buffer->Recycle();
						continue;
					} else   {
						if ( buffer->Header()->start_time < perfTimeNow) {
							//printf("Played frame: %.4f @ %.4f\n", (double)buffer->Header()->start_time/1000000., (double)(perfTimeNow)/1000000.);

							memcpy(m_Bitmap->Bits(), buffer->Data(), m_Bitmap->Bounds().Height() * m_Bitmap->BytesPerRow());
							buffer->Recycle();
							if ( m_AVIWindow->Lock()) {
								m_AVIView->DrawBitmap(m_Bitmap);
								m_AVIWindow->Unlock();
							}
							continue;

						}
						// otherwise push the buffer back on the queue
						else{
							//printf("Pushed frame: %.4f @ %.4f\n", (double)buffer->Header()->start_time/1000000., (double)(perfTimeNow - halfPeriod)/1000000.);
							m_BufferQueue->PushBuffer(buffer, buffer->Header()->start_time);
							continue;
						}
					}
				}
			}
		}
	}
}
