//---------------------------------------------------------------------
//
//	File:	AVIView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVIView
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TBitmapView.h"

#include "AppAlerts.h"
#include "AppUtils.h"
#include "DebugUtils.h"

#include "AVIApp.h"
#include "AVIWindow.h"

#include "AVIView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

AVIView::AVIView(BRect frame, const char *name, uint32 resizingMode, uint32 flags) : 
		 BView(frame, name, resizingMode, flags)
{
		
	bool retVal = Init();			
	
	if (!retVal)
		be_app->PostMessage(B_QUIT_REQUESTED);

}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

AVIView::~AVIView()
{

}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void AVIView::Draw(BRect updateRect)
{		
	BView::Draw(updateRect);	
}


#pragma mark -
#pragma mark === Initialization Routines ===

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Basic initialization
//

bool AVIView::Init()
{			
	return true;
}

#pragma mark -
#pragma mark === Input Routines ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void AVIView::MouseDown(BPoint where)
{
	MakeFocus(true);	
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down events
//

void AVIView::KeyDown(const char *bytes, int32 numBytes)
{
	if (numBytes == 1)
	{
		switch(bytes[0])
		{	
			/*
			case B_DELETE:
			case B_BACKSPACE:
				break;
			
			//	Play or stop
			case B_SPACE:
				if (m_IsPlaying)
					Stop();
				else
					Play();
				break;
								
			//	Previous frame or start
			case B_LEFT_ARROW:
				{
					if ( IsOptionKeyDown() )
						ShowFrame(0);	
					else
						PreviousVideoFrame();
				}
				break;
							
			//	Next frame or end
			case B_RIGHT_ARROW:
				if ( IsOptionKeyDown() )
					ShowFrame(m_Reader->FrameCount() - 1);
				else
					NextVideoFrame();
				break;	
			*/	
			default:
				BView::KeyDown(bytes, numBytes);
				break;
		}
	}
	else
		BView::KeyDown(bytes, numBytes);
}

#pragma mark -
#pragma mark === Playback Routines === 

/*
//------------------------------------------------------------------
//	AVITick
//------------------------------------------------------------------
//
//	Ticker callback function
//

void AVITick(void *arg, TTick *tick)
{	
	AVIView *aviView = static_cast<AVIView *>(arg);		
	aviView->SetCurrentTime(tick->m_Timecode.m_Milliseconds);
	
	//	Load next frame
	aviView->NextVideoFrame();
}


//------------------------------------------------------------------
//	Play
//------------------------------------------------------------------
//
//
//

void AVIView::Play()
{
	if (m_PlaybackEngine && !m_IsPlaying)
	{
		m_PlaybackEngine->Play();
		//m_AudioEngine->Play();
		m_IsPlaying = true;
	}
}


//------------------------------------------------------------------
//	Stop
//------------------------------------------------------------------
//
//
//

void AVIView::Stop()
{
	if (m_PlaybackEngine && m_IsPlaying)
	{
		m_PlaybackEngine->Stop();
		//m_AudioEngine->Stop();
		m_IsPlaying = false;
	}
}


//------------------------------------------------------------------
//	NextFrame
//------------------------------------------------------------------
//
//	Play next audio/video frame

void AVIView::NextFrame()
{
}


//------------------------------------------------------------------
//	PreviousFrame
//------------------------------------------------------------------
//
//	Play previous audio/video frame

void AVIView::PreviousFrame()
{
}



//---------------------------------------------------------------------
//	PreviousVideoFrame
//---------------------------------------------------------------------
//
//

void AVIView::PreviousVideoFrame()
{	
	//BStopWatch watch("PreviousVideoFrame");
	
	m_CurrentVideoFrame--;
	
	if (m_CurrentVideoFrame == 0 || m_CurrentVideoFrame == 0xFFFFFFFF)
	{
		#ifdef DEBUG
			printf("Rewind...");
		#endif
		m_CurrentVideoFrame = 0;
	}
	
	#ifdef DEBUG
		//	Cough up current frame
		printf("Frame: %d\n", m_CurrentVideoFrame);
	#endif
	
	//	Get the frame
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
	
	//	Valid frame?
	if (!theFrame)
		return;
			
	//	Make sure we are handling a video chunk
	switch(theFrame->ChunkID)
	{
		//	Video data
		case kRiff_00db_Chunk:
		case kRiff_00dc_Chunk:
		case kRiff_01dc_Chunk:
			{
				//	Decode frame
				bool retVal = m_VideoCodec->DecodeFrame(theFrame, m_BitmapBits);
	
				//	Update if valid decoded frame
				if (retVal)
				{
					m_BitmapView->Looper()->Lock();
					m_BitmapView->Invalidate();	
					m_BitmapView->Looper()->Unlock();
				}		
			}
			break;
			
		default:
			#ifdef DEBUG
				printf("Non-video chunk...\n");
			#endif
			return;
	}	
}


//---------------------------------------------------------------------
//	NextVideoFrame
//---------------------------------------------------------------------
//
//

void AVIView::NextVideoFrame()
{	
	//BStopWatch watch("NextVideoFrame");
	
	m_CurrentVideoFrame++;
	
	//	Handle looping?
	if (m_CurrentVideoFrame >= m_Reader->FrameCount())
	{		
		if (m_IsPlaying)
			m_CurrentVideoFrame = 0;
			
		Stop();		
		return;
	}
	
	#ifdef DEBUG
		printf("Frame: %d\n", m_CurrentVideoFrame);
	#endif
	
	//	Get the frame
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
			
	//	Make sure we are handling a video chunk
	switch(theFrame->ChunkID)
	{
		//	Video data
		case kRiff_00db_Chunk:
		case kRiff_00dc_Chunk:
		case kRiff_01dc_Chunk:
			{
				//	Decode frame
				bool retVal = m_VideoCodec->DecodeFrame(theFrame, m_BitmapBits);
	
				//	Update if valid decoded frame
				if (retVal)
				{
					m_BitmapView->Looper()->Lock();
					m_BitmapView->Invalidate();
					m_BitmapView->Looper()->Unlock();
				}		
			}
			break;
			
		default:
			#ifdef DEBUG
				printf("Non-video chunk...\n");
				DumpRIFFID(theFrame->ChunkID);
			#endif
			return;
	}	
}



//---------------------------------------------------------------------
//	ShowFrame
//---------------------------------------------------------------------
//
//	Display a specific frame
//

void AVIView::ShowFrame(uint32 frameNum)
{
	bool retVal = false;
	
	//	Check for bad frame
	if (frameNum >= m_Reader->FrameCount() )
		return;
	
	m_CurrentVideoFrame = frameNum;
	
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
	
	if (theFrame)
		retVal = m_VideoCodec->DecodeFrame(theFrame, m_BitmapBits);
		
	if (retVal)
	{
		m_BitmapView->Looper()->Lock();
		m_BitmapView->Invalidate();
		m_BitmapView->Looper()->Unlock();
	}
}
*/
#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetBitmapView
//---------------------------------------------------------------------
//
//

void AVIView::SetBitmapView(TBitmapView *theView)
{
	m_BitmapView = theView;
	
	//	Get a pointer to the bits
	m_BitmapBits = m_BitmapView->m_Bitmap->Bits();
	
//m_BitmapBits = m_BitmapView->GetBitmap()->Bits();
// ABH pBits = m_BitmapView->Bits();
//	m_BitmapBits = tmpBitmap->Bits();;

}



