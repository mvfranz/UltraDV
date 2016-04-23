//---------------------------------------------------------------------
//
//	File:	AVIView.h
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

#ifndef __AVIVIEW_H__
#define __AVIVIEW_H__

//	Forward Declarations
class TBitmapView;
class TPlaybackEngine;
class TChaser;
class TTick;
class TAudioEngine;
class TAudioCodec;
class TVideoCodec;
class TRIFFReader;

//	Class Definition
class AVIView : public BView
{
	public:
		AVIView(BRect frame, const char *name, uint32 resizingMode, uint32 flags);
		~AVIView();
		
		void	Draw(BRect updateRect);
		
		void 	MouseDown(BPoint where);
		void 	KeyDown(const char *bytes, int32 numBytes);
		
		void 	Play();
		void 	Stop();
		
		void 	NextFrame();
		void 	PreviousFrame();
		
		void 	PreviousVideoFrame();
		void 	NextVideoFrame();		
		void 	ShowFrame(uint32 theFrame);
		
		size_t 	FillSoundBuffer(char *buffer, size_t count);
		
		void 	SetBitmapView(TBitmapView *theView);
				
	private:	
		//	Member Functions
		bool 	Init();
						
		//	Member Variables
		TBitmapView 	*m_BitmapView;
		void			*m_BitmapBits;		
						
	protected:
	
};

//	Function Protoypes
void AVITick(void *arg, TTick *tick);

#endif
