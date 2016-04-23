//---------------------------------------------------------------------
//
//	File:	AVIWriterApp.h
//
//	Author:	Gene Z. Ragan
//
//	Desc:	AVI File Consumer/Writer
//
//---------------------------------------------------------------------

#if !defined(__AVIWRITERAPP_H__)
#define __AVIWRITERAPP_H__

#include <MediaNode.h>

//	Forward Declarations
class BMediaRoster;
class ControlWindow;
class AVIConsumer;

//	Class Definition
class AVIWriterApp : public BApplication 
{

	public:
		AVIWriterApp();

		bool QuitRequested();

	private:
		BMediaRoster 	*m_Roster;
		ControlWindow 	*m_Window;
		AVIConsumer		*m_AVIWriter;
		
		media_node timesourceNode;
		//media_node consumerNode;
		media_node producerNode;

		media_output 	m_From;
		media_input 	m_To;
};

#include <Window.h>

class ControlWindow : public BWindow {

public:

		ControlWindow(BView * controls, media_node node);
		ControlWindow(const BRect & r);
		void MessageReceived(BMessage * message);
virtual	bool QuitRequested();

private:

		BView * m_view;
		media_node m_node;

};


#endif

