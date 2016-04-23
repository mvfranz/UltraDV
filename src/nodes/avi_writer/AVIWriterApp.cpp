//---------------------------------------------------------------------
//
//	File:	AVIWriterApp.cpp
//
//	Date:	11.08.98
//
//	Desc:	AVI File Consumer/Writer
//
//---------------------------------------------------------------------


//	Includes
#include "BuildApp.h"

#include <scheduler.h>
#include <string.h>
#include <stdio.h>

#include <OS.h>
#include <MediaRoster.h>
#include <MediaTheme.h>
#include <TimeSource.h>
#include <Alert.h>

#include "AVIConsumer.h"

#include "AVIWriterApp.h"


//	Utility Functions
static void list_nodes()
{
	int nnodes = 50;
	live_node_info * lni = new live_node_info[nnodes];
	while (1) 
	{
		int32 cnt = nnodes;
		status_t err = BMediaRoster::Roster()->GetLiveNodes(
			lni, &cnt, NULL, NULL, NULL, 0);
		if (err < B_OK) 
		{
			fprintf(stderr, "can't get list of nodes: %s [%x]\n", strerror(err), err);
			delete[] lni;
			return;
		}
		if (cnt > nnodes) {
			cnt += 50;
			delete[] lni;
			nnodes = cnt;
			lni = new live_node_info[nnodes];
			continue;
		}
		nnodes = cnt;
		break;
	}
	fprintf(stderr, "%d nodes:\n", nnodes);
	for (int ix=0; ix < nnodes; ix++) {
		fprintf(stderr, "%5d %30s%s%s%s%s\n", lni[ix].node.node, lni[ix].name,
			(lni[ix].node.kind & B_BUFFER_CONSUMER) ? " CONSUMER" : "",
			(lni[ix].node.kind & B_BUFFER_PRODUCER) ? " PRODUCER" : "",
			(lni[ix].node.kind & B_TIME_SOURCE) ? " TIME_SOURCE" : "",
			(lni[ix].node.kind & ~(B_BUFFER_CONSUMER|B_BUFFER_PRODUCER|B_TIME_SOURCE)) ?
				" {other}" : "");
	}
}

static void error(const char * message, status_t err)
{
	char msg[256];
	sprintf(msg, "%s\n%s [%x]", message, strerror(err), err);
	(new BAlert("", msg, "Quit"))->Go();
	list_nodes();
	be_app->PostMessage(B_QUIT_REQUESTED);
}


AVIWriterApp::AVIWriterApp() : BApplication("application/x-vnd.AVIWriterApp")
{
	//	Connect to the media server
	m_Window = NULL;
	m_Roster = BMediaRoster::Roster();
	
	if (!m_Roster) 
	{
		(new BAlert("", "Cannot connect to the media server!", "Quit"))->Go();
		PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	//	Find TimeSource
	status_t err = m_Roster->GetTimeSource(&timesourceNode);
	if (err < B_OK) {
		error("Can't get TimeSource!", err);
		return;
	}
	printf("TimeSource found: %d (%x)\n", timesourceNode.node, timesourceNode.port);
		
	//	Create our AVIWriter node and register it
	m_AVIWriter = new AVIConsumer("AVIConsumer");
	m_Roster->RegisterNode(m_AVIWriter);
	
	//	Find video source
	err = m_Roster->GetVideoInput(&producerNode);
	if (err < B_OK) 
	{
		error("Can't find the video input!", err);
		return;
	}
	printf("input found: %d (%x)\n", producerNode.node, producerNode.port);

	//	Find output from video source
	int32 cnt = 0;
	err = m_Roster->GetFreeOutputsFor(producerNode, &m_From, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		error("The video input is busy!", err);
		return;
	}

	// create control panel
	BParameterWeb 	*web  = NULL;
	BView 			*view = NULL;
	err = m_Roster->GetParameterWebFor(producerNode, &web);
	if (err >= B_OK && web != NULL) 
	{
		view = BMediaTheme::ViewFor(web);
		//	Create control window
		m_Window = new ControlWindow(view, producerNode);
		BMediaRoster::Roster()->StartWatching(BMessenger(NULL, m_Window));
	}
	else 
	{
		//	Create an empty window
		m_Window = new ControlWindow(BRect(64,64,364,164));
	}
	m_Window->Show();
	
	
	//	Find free input
	cnt = 0;
	err = m_Roster->GetFreeInputsFor(m_AVIWriter->Node(), &m_To, 1, &cnt, B_MEDIA_RAW_VIDEO);
	if (err < 0 || cnt < 1) 
	{
		error("The video output is busy!", err);
		return;
	}

	//	Connect them
	media_format format;
	format.type = B_MEDIA_RAW_VIDEO;
	format.u.raw_video = media_raw_video_format::wildcard;
	err = m_Roster->Connect(m_From.source, m_To.destination, &format, &m_From, &m_To);
	if (err < B_OK) 
	{
		error("Couldn't connect video input to video display!", err);
		return;
	}

	//	Set time source for output node
	err = m_Roster->SetTimeSourceFor( m_AVIWriter->Node().node, timesourceNode.node);
	if (err < B_OK) 
	{
		error("Couldn't set TimeSource for video display!", err);
		return;
	}

	//	Set time source for video input
	err = m_Roster->SetTimeSourceFor( producerNode.node, timesourceNode.node);
	if (err < B_OK) 
	{
		error("Couldn't set TimeSource for video input!", err);
		return;
	}

	BTimeSource *source = BMediaRoster::Roster()->MakeTimeSourceFor(timesourceNode);

	//	Start them
	bigtime_t start_delay = 0;
	BMediaRoster::Roster()->GetStartLatencyFor(timesourceNode, &start_delay);
	start_delay += estimate_max_scheduling_latency(find_thread(NULL));

	bool running = source->IsRunning();
	printf("running: %s\n", running ? "true" : "false");
	bigtime_t perf = source->Now() + start_delay;
	bigtime_t real = BTimeSource::RealTime();
	source->Release();

	printf("perf = %.4f real = %.4f\n", (double)perf/1000000., (double)real/1000000.);
	
	err = m_Roster->StartNode(m_To.node, perf);
	if (err < B_OK) 
	{
		error("Couldn't start video displayer!", err);
		return;
	}
	
	err = m_Roster->StartNode(m_From.node, perf);
	if (err < B_OK) 
	{
		error("Couldn't start video input!", err);
		return;
	}
	
	err = m_Roster->StartNode(timesourceNode, real);
	if (err < B_OK) 
	{
		error("Couldn't start TimeSource!", err);
		return;
	}
}

bool AVIWriterApp::QuitRequested()
{
	if (!m_Roster) 
	{
		return true;
	}
	
	//	Close window (if necessary)
	if (m_Window->Lock()) 
	{
		m_Window->Quit();
	}
	m_Window = NULL;

	//	Close running connections
	m_Roster->StopNode(m_From.node, 0, true);
	m_Roster->StopNode(m_To.node, 0, true);
	m_Roster->Disconnect(m_From.node.node, m_From.source, m_To.node.node, m_To.destination);
	m_Roster->ReleaseNode(m_From.node);
	m_Roster->ReleaseNode(m_To.node);
	m_Roster->ReleaseNode(timesourceNode);

	return true;
}

static BRect offset_rect(BRect r, float x, float y)
{
	r.OffsetBy(x, y);
	return r;
}


ControlWindow::ControlWindow(BView * controls, media_node node) :
	BWindow(offset_rect(controls->Bounds(), 64, 64), "TV Controls",
		B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	m_view = controls;
	m_node = node;
	AddChild(m_view);
}

ControlWindow::ControlWindow(const BRect & frame) :
	BWindow(frame, "TV Controls",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
}

void
ControlWindow::MessageReceived(BMessage * message) 
{
	BParameterWeb * web = NULL;
	BView * panel = NULL;
	status_t err;
	int32 cnt;
	
	switch (message->what) {
		case B_MEDIA_WEB_CHANGED:
			printf("Number children: %d\n", CountChildren());
			cnt = CountChildren();
			for (int i = 0; i < cnt; i++)
				printf("   Child view: %08x\n", ChildAt(i));
			printf("REMOVING VIEW \n");
			RemoveChild(m_view);
			printf("DELETING VIEW \n");
			delete m_view;
			printf("GETTING PARAMETER WEB\n");
			err = BMediaRoster::Roster()->GetParameterWebFor(m_node, &web);
			if (err >= B_OK && web != NULL) {
				printf("GETTING NEW VIEW\n");
				m_view = BMediaTheme::ViewFor(web);
				AddChild(m_view);
			}
			break;
	default:
		BWindow::MessageReceived(message);
	}
}

bool ControlWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return false;
}


int
main()
{
	AVIWriterApp app;
	app.Run();
	return 0;
}
