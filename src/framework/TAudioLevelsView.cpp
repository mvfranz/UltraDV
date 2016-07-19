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

	fDone 	= false;
	fReady = false;
	
	fLeftBuffer = new float[512];
	fRightBuffer = new float[512];
	
	fBufferLock = new BLocker("BufferLock");
		
	fUpdateThread = spawn_thread((thread_entry)update, "update thread", B_LOW_PRIORITY, this);
	
	this->fStream 	 = theStream;
	this->fSpeed 	 = theSpeed;
	this->fOldSpeed = oldSpeed;
	
	//fADCStream = new BADCStream();	
	//fADCStream->SetSamplingRate(44100);
	//fADCStream->BoostMic(false);
	
	//fDACStream = new BDACStream();	
	//fDACStream->SetSamplingRate(44100);
	
	//fSubscriber = new BSubscriber();
	
	//if (fStream == 0)
	//	fSubscriber->Subscribe(fADCStream);		
	//else if (fStream == 1)
	//	fSubscriber->Subscribe(fDACStream);
	//else if (fStream == 2)
	//{
	//	fSpeed 	= 0;
	//	fOldSpeed 	= 2;
	//	return;
	//}
	
	//fSubscriber->EnterStream(NULL, true, (void *)this, (enter_streafhook)streaffunc, NULL, true);
		
	resume_thread(fUpdateThread);
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
	
			if (fSpeed == 0)
				break;
				
			fBufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				fLeftBuffer[i] = 0;
				fRightBuffer[i] = 0;
			}
			fBufferLock->Unlock();
			
			//fSubscriber->ExitStream();
			//fSubscriber->Unsubscribe();
			
			//suspend_thread(fUpdateThread);
			fOldSpeed = fSpeed;
			fSpeed = 0;
			
			fStream = 2;
			break;
			
		case 'ADC ':
			// Switch to adc
			
			if (fSpeed == 0)
			{
				fSpeed = fOldSpeed;
				resume_thread(fUpdateThread);
			}
			
			fBufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				fLeftBuffer[i] = 0;
				fRightBuffer[i] = 0;
			}
			fBufferLock->Unlock();
		
			//fSubscriber->ExitStream();
			//fSubscriber->Unsubscribe();
			
			//fSubscriber->Subscribe(fADCStream);
		
			fStream = 0;
	
			//fSubscriber->EnterStream(NULL, true, (void *)this, (enter_streafhook)streaffunc, NULL, true);
			break;
			
		// Switch to dac
		case 'DAC ':			
			if (fSpeed == 0)
			{
				fSpeed = fOldSpeed;
				resume_thread(fUpdateThread);
			}
			
			fBufferLock->Lock();
			for (int32 i = 0; i < 512; i++)
			{
				fLeftBuffer[i] = 0;
				fRightBuffer[i] = 0;
			}
			fBufferLock->Unlock();
		
			//fSubscriber->ExitStream();
			//fSubscriber->Unsubscribe();
			
			//fSubscriber->Subscribe(fDACStream);
		
			fStream = 1;
	
			//fSubscriber->EnterStream(NULL, true, (void *)this, (enter_streafhook)streaffunc, NULL, true);
			break;
			
		case '1/25':
			if (fSpeed != 0)
				fSpeed = 1;
			else
				fOldSpeed = 1;
			break;
			
		case '1/2 ':
			if (fSpeed != 0)
				fSpeed = 2;
			else
				fOldSpeed = 2;
			break;
			
		case '1   ':
			if (fSpeed != 0)
				fSpeed = 3;
			else
				fOldSpeed = 3;
			break;
			
		case '2   ':
			if (fSpeed != 0)
				fSpeed = 4;
			else
				fOldSpeed = 4;
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
			
	while (!fReady)
	{
	}
	
	while (fDone == false)
	{						
		leftHigh 	= 0.0;
		rightHigh 	= 0.0;
	
		leftCount 	= 0;
		rightCount 	= 0;
		fBufferLock->Lock();
		
		fft_real_to_hermitian(fLeftBuffer, 512);
		fft_real_to_hermitian(fRightBuffer, 512);
		
		for (int32 i = 0; i < 512; i++)
			leftHigh = fLeftBuffer[i] > leftHigh ? fLeftBuffer[i] : leftHigh;
			
		for (int32 i = 0; i < 512; i++)
			rightHigh = fRightBuffer[i] > rightHigh ? fRightBuffer[i] : rightHigh;
			
		fBufferLock->Unlock();
		
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
		
		switch (fSpeed)
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
//	streaffunc
//---------------------------------------------------------------------
//
//

bool TAudioLevelsView::streaffunc(void *data, char *buf, size_t size, void *header)
{	
		
	TAudioLevelsView	*amv = (TAudioLevelsView *)data;
	int32 count = size / 2;
	
	amv->fBufferLock->Lock();
	
	int32 frame_cnt = 0;
	for (int32 i = 0; i < 512; i++)
	{
		amv->fLeftBuffer[i] = ((int16 *)buf)[frame_cnt] / 32768.0;
		frame_cnt += 2;
	}
	
	frame_cnt = 1;
	for (int32 i = 0; i < 512; i++)
	{
		amv->fRightBuffer[i] = ((int16 *)buf)[frame_cnt] / 32768.0;
		frame_cnt += 2;
	}
	
	amv->fBufferLock->Unlock();
	
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
	fReady = true;
	
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
	if (fDone == false)
	{
		fDone = true;
		
		kill_thread(fUpdateThread);
		fBufferLock->Unlock();	
		
		//fSubscriber->ExitStream();
		//fSubscriber->Unsubscribe();
		
		//delete fSubscriber;
		//delete fADCStream;
		//delete fDACStream;
		
		fBufferLock->Lock();		
		delete fLeftBuffer;
		delete fRightBuffer;	
		fBufferLock->Unlock();
		
		delete fBufferLock;
	}
	
	BView::DetachedFromWindow();
}

