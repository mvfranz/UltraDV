//---------------------------------------------------------------------
//
//	File:	TAudioLevelsView.h
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

#ifndef __TAUDIOLEVELSVIEW_H__
#define __TAUDIOLEVELSVIEW_H__

// CLass Definition
class TAudioLevelsView : public BView
{
	public:
		TAudioLevelsView(BMessage *archive);
		TAudioLevelsView(BRect r);
		
		~TAudioLevelsView();
						
		void	Init(int8 stream, int8 speed, int8 old_speed);
		
		void	Draw(BRect updateRect);
		
		void	UpdateData(int32 code);
		
		void 	AttachedToWindow();
		void 	DetachedFromWindow();
		
	private:
		// Member Functions
		static int32	update(void *data);
		void			Update();
		static bool		streaffunc(void *data, char *buf, size_t size, void *header);

		void			DrawGreyBoxesLeft();
		void			DrawGreyBoxesRight();
		
		// Member Varibles
		BLocker			*fBufferLock;
		float			*fLeftBuffer;
		float			*fRightBuffer;
		
		bool			fReady;
		bool			fDone;
		thread_id		fUpdateThread;
		
		int8			fStream;
		int8			fSpeed;
		int8			fOldSpeed;
		
		//BADCStream		*fADCStream;
		//BDACStream		*fDACStream;
		
		//BSubscriber		*fSubscriber;		
};

#endif

