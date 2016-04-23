//---------------------------------------------------------------------
//
//	File:	TAudioLevelsView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.29.98
//
//	Desc:	Display audio levels
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"

#include "fft.h"

#include "TAudioLevelsView.h"

// Constants 
static const short shades_g[] = {0, 88, 97, 106, 115, 124, 132, 141, 150, 159, 168, 176, 185, 194, 203, 212, 185, 150, 106, 0};
static const short shades_r[] = {0, 0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   106, 150, 185, 212, 247};
								
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioLevelsView::TAudioLevelsView(BRect r) : BView(r, "TAudioLevelsView", B_FOLLOW_NONE, B_WILL_DRAW)
{
}


								
//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioLevelsView::~TAudioLevelsView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::Init(int8 theStream, int8 theSpeed, int8 oldSpeed)
{
	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);

	m_Done 	= false;
	m_Ready = false;
	
	m_LeftBuffer = new float[512];
	m_RightBuffer = new float[512];
	
	m_BufferLock = new BLocker("BufferLock");
		
	m_UpdateThread = spawn_thread((thread_entry)update, "update thread", B_LOW_PRIORITY, this);
	
	this->m_Stream 	 = theStream;
	this->m_Speed 	 = theSpeed;
	this->m_OldSpeed = oldSpeed;
	
	//m_ADCStream = new BADCStream();	
	//m_ADCStream->SetSamplingRate(44100);
	//m_ADCStream->BoostMic(false);
	
	//m_DACStream = new BDACStream();	
	//m_DACStream->SetSamplingRate(44100);
	
	//m_Subscriber = new BSubscriber();
	
	//if (m_Stream == 0)
	//	m_Subscriber->Subscribe(m_ADCStream);		
	//else if (m_Stream == 1)
	//	m_Subscriber->Subscribe(m_DACStream);
	//else if (m_Stream == 2)
	//{
	//	m_Speed 	= 0;
	//	m_OldSpeed 	= 2;
	//	return;
	//}
	
	//m_Subscriber->EnterStream(NULL, true, (void *)this, (enter_stream_hook)stream_func, NULL, true);
		
	resume_thread(m_UpdateThread);
}


//---------------------------------------------------------------------
//	UpdateData
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::UpdateData(int32 code)
{
	switch (code)
	{
		case 'PASE':
			// Pause the threads
	
			if (m_Speed == 0)
				break;
				
			m_BufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				m_LeftBuffer[i] = 0;
				m_RightBuffer[i] = 0;
			}
			m_BufferLock->Unlock();
			
			//m_Subscriber->ExitStream();
			//m_Subscriber->Unsubscribe();
			
			//suspend_thread(m_UpdateThread);
			m_OldSpeed = m_Speed;
			m_Speed = 0;
			
			m_Stream = 2;
			break;
			
		case 'ADC ':
			// Switch to adc
			
			if (m_Speed == 0)
			{
				m_Speed = m_OldSpeed;
				resume_thread(m_UpdateThread);
			}
			
			m_BufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				m_LeftBuffer[i] = 0;
				m_RightBuffer[i] = 0;
			}
			m_BufferLock->Unlock();
		
			//m_Subscriber->ExitStream();
			//m_Subscriber->Unsubscribe();
			
			//m_Subscriber->Subscribe(m_ADCStream);
		
			m_Stream = 0;
	
			//m_Subscriber->EnterStream(NULL, true, (void *)this, (enter_stream_hook)stream_func, NULL, true);
			break;
			
		// Switch to dac
		case 'DAC ':			
			if (m_Speed == 0)
			{
				m_Speed = m_OldSpeed;
				resume_thread(m_UpdateThread);
			}
			
			m_BufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				m_LeftBuffer[i] = 0;
				m_RightBuffer[i] = 0;
			}
			m_BufferLock->Unlock();
		
			//m_Subscriber->ExitStream();
			//m_Subscriber->Unsubscribe();
			
			//m_Subscriber->Subscribe(m_DACStream);
		
			m_Stream = 1;
	
			//m_Subscriber->EnterStream(NULL, true, (void *)this, (enter_stream_hook)stream_func, NULL, true);
			break;
			
		case '1/25':
			if (m_Speed != 0)
				m_Speed = 1;
			else
				m_OldSpeed = 1;
			break;
			
		case '1/2 ':
			if (m_Speed != 0)
				m_Speed = 2;
			else
				m_OldSpeed = 2;
			break;
			
		case '1   ':
			if (m_Speed != 0)
				m_Speed = 3;
			else
				m_OldSpeed = 3;
			break;
			
		case '2   ':
			if (m_Speed != 0)
				m_Speed = 4;
			else
				m_OldSpeed = 4;
			break;
			
		default:
			break;
	}
}


//---------------------------------------------------------------------
//	update
//---------------------------------------------------------------------
//
//

int32 TAudioLevelsView::update(void *data)
{
	TAudioLevelsView *amv = (TAudioLevelsView *)data;
	amv->Update();
	return 0;
}


//---------------------------------------------------------------------
//	Update
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::Update()
{	
	float			leftHigh 	= 0.0;
	float			rightHigh 	= 0.0;
	
	int8			leftCount 	= 0;
	int8			rightCount 	= 0;
		
	// Set up drawing variables
	Window()->Lock();	
	BRect bounds = Bounds();
	Window()->Unlock();
	bounds.InsetBy(1,1);
	const int32 width 	= bounds.Width();
	const int32 height 	= bounds.Height();
	const int32 bottom 	= bounds.bottom;
	const int32 right 	= bounds.right;
	const int32	divNum 	= height/20;
	
	
	BRect fillRect;	
	fillRect.left 	= bounds.left;
	fillRect.right  = fillRect.left + (width/2)-1;
			
	while (!m_Ready)
	{
	}
	
	while (m_Done == false)
	{						
		leftHigh 	= 0.0;
		rightHigh 	= 0.0;
	
		leftCount 	= 0;
		rightCount 	= 0;
		m_BufferLock->Lock();
		
		fft_real_to_hermitian(m_LeftBuffer, 512);
		fft_real_to_hermitian(m_RightBuffer, 512);
		
		for (int32 i = 0; i < 512; i++)
			leftHigh = m_LeftBuffer[i] > leftHigh ? m_LeftBuffer[i] : leftHigh;
			
		for (int32 i = 0; i < 512; i++)
			rightHigh = m_RightBuffer[i] > rightHigh ? m_RightBuffer[i] : rightHigh;
			
		m_BufferLock->Unlock();
		
		leftCount  = leftHigh	/ 5.0;
		rightCount = rightHigh  / 5.0;
		
		Window()->Lock();				
		// Dray grey boxes
		DrawGreyBoxesLeft();
		DrawGreyBoxesRight();
		Window()->Unlock();
		
		Window()->Lock();
		// Draw Left Channel
		for (int32 index = 0; index < leftCount; index++)
		{
			SetHighColor(shades_r[index], shades_g[index], 0);			
			fillRect.left 	= bounds.left;
			fillRect.right 	= (width/2);
			fillRect.bottom = bottom - (index*divNum);			
			fillRect.top 	= (fillRect.bottom - divNum)+2;
			FillRect(fillRect);
		}
		Window()->Unlock();
		
		Window()->Lock();
		// Draw Right Channel
		for (int32 index = 0; index < rightCount; index++)
		{
			SetHighColor(shades_r[index], shades_g[index], 0);
			fillRect.left 	= (width/2)+2;
			fillRect.right  = right;
			fillRect.bottom = bottom - (index*divNum);			
			fillRect.top 	= (fillRect.bottom - divNum)+2;
			FillRect(fillRect);
		}
		Window()->Unlock();
		
		Flush();
		
		switch (m_Speed)
		{
			case 0:
				suspend_thread(find_thread(NULL));
				break;
				
			case 1:
				snooze(25000);
				break;
				
			case 2:
				snooze(100000);
				break;
				
			case 3:
				snooze(250000);
				break;
				
			case 4:
				snooze(1000000);
				break;
		}		
	}	
}


//---------------------------------------------------------------------
//	stream_func
//---------------------------------------------------------------------
//
//

bool TAudioLevelsView::stream_func(void *data, char *buf, size_t size, void *header)
{	
		
	TAudioLevelsView	*amv = (TAudioLevelsView *)data;
	int32 count = size / 2;
	
	amv->m_BufferLock->Lock();
	
	int32 frame_cnt = 0;
	for (int32 i = 0; i < 512; i++)
	{
		amv->m_LeftBuffer[i] = ((int16 *)buf)[frame_cnt] / 32768.0;
		frame_cnt += 2;
	}
	
	frame_cnt = 1;
	for (int32 i = 0; i < 512; i++)
	{
		amv->m_RightBuffer[i] = ((int16 *)buf)[frame_cnt] / 32768.0;
		frame_cnt += 2;
	}
	
	amv->m_BufferLock->Unlock();
	
	return true;
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::Draw(BRect updateRect)
{
	PushState();
	
	BPoint startPt, endPt;	
	m_Ready = true;
	
	SetHighColor(kBlack);
	FillRect(Bounds());
	
	BRect levelsRect = Bounds();
	
	// Draw outline
	SetHighColor(kBeShadow);
	startPt.Set(levelsRect.left, levelsRect.bottom);
	endPt.Set(levelsRect.left, levelsRect.top);
	StrokeLine(startPt, endPt);
	startPt.Set(levelsRect.left, levelsRect.top);
	endPt.Set(levelsRect.right, levelsRect.top);
	StrokeLine(startPt, endPt);
	
	SetHighColor(kBeHighlight);
	startPt.Set(levelsRect.right, levelsRect.top);
	endPt.Set(levelsRect.right, levelsRect.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(levelsRect.right, levelsRect.bottom);
	endPt.Set(levelsRect.left, levelsRect.bottom);
	StrokeLine(startPt, endPt);
		
	// Draw Greyed Boxes
	DrawGreyBoxesLeft();
	DrawGreyBoxesRight();

	PopState();
}


//---------------------------------------------------------------------
//	DrawGreyBoxesLeft
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::DrawGreyBoxesLeft()
{	
	BRect bounds = Bounds();
	bounds.InsetBy(1,1);
	
	const int32 width 	= bounds.Width();
	const int32 height 	= bounds.Height();
	const int32 right 	= bounds.right;
	const int32 bottom 	= bounds.bottom;
	const int32	divNum 	= height/20;
	
	BRect fillRect;
	fillRect.left 	= bounds.left;
	fillRect.right  = fillRect.left + (width/2)-1;
	
	SetHighColor(75, 75, 75);
		
	for (int32 index = 0; index < 20; index++)
	{
		fillRect.bottom = bottom - (index*divNum);			
		fillRect.top 	= (fillRect.bottom - divNum)+2;

		FillRect(fillRect);
	}	
}	


//---------------------------------------------------------------------
//	DrawGreyBoxesRight
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::DrawGreyBoxesRight()
{	
	BRect bounds = Bounds();
	bounds.InsetBy(1,1);

	const int32 width 	= bounds.Width();
	const int32 height 	= bounds.Height();
	const int32 right 	= bounds.right;
	const int32 bottom 	= bounds.bottom;
	const int32	divNum 	= height/20;

	BRect fillRect;
	fillRect.left 	= (width/2) + 2;
	fillRect.right  = right;
	
	SetHighColor(75, 75, 75);
	
	// Right side
	for (int32 index = 0; index < 20; index++)
	{				
		fillRect.bottom = bottom - (index*divNum);			
		fillRect.top 	= (fillRect.bottom - divNum)+2;
		FillRect(fillRect);
	}
}	


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::AttachedToWindow()
{
	// Start thread
	Init(0, 1, 1);
	
	BView::AttachedToWindow();	
}


//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//

void TAudioLevelsView::DetachedFromWindow()
{	
	if (m_Done == false)
	{
		m_Done = true;
		
		kill_thread(m_UpdateThread);
		m_BufferLock->Unlock();	
		
		//m_Subscriber->ExitStream();
		//m_Subscriber->Unsubscribe();
		
		//delete m_Subscriber;
		//delete m_ADCStream;
		//delete m_DACStream;
		
		m_BufferLock->Lock();		
		delete m_LeftBuffer;
		delete m_RightBuffer;	
		m_BufferLock->Unlock();
		
		delete m_BufferLock;
	}
	
	BView::DetachedFromWindow();
}

