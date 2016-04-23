//---------------------------------------------------------------------
//
//	File:	TTransition.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.03.98
//
//	Desc:	Cue transition routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TTRANSITION_H__
#define __TTRANSITION_H__

// Callbacks
typedef	void (*TransitionCallback)(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
						  
// Class Definition			  
class TTransition
{	
	public:
		TTransition( TVisualCue *srcView, BView *dstView, TStageView *theStage, int16 transitionID, uint32 duration, bool transitionIn);
		~TTransition();
		
		void	Init();
		
		void 	SetStartTime(uint32 startTime);
		void 	SetDuration(uint32 duration);
		void 	SetTransitionID(int32 theID);
		// ABH missing functions
//		bool 	HasTransitionIn();
//		bool 	HasTransitionOut();
		
		void	Stop();
		void	Reset();
		
		void 	TransitionTask();
		void 	DrawData(uint32 theTime, BView *offView);
		
		// Inlines
		inline bool		IsDone(){ return m_IsDone; }
		
	private:
		// Member Functions		
		
				
		// Member Variables
		bool				m_IsDone;
		bool				m_IsTransitionIn;
		bool				m_IsTransitionOut;
		TStageView			*m_Stage;
		TVisualCue			*m_SrcView;
		BView 				*m_MaskView;
		BView				*m_DstView;
		BRegion				m_TransitionRegion;
		BRect				m_SrcRect;		
		BRect				m_DstRect;
		int16				m_ID;
		uint32				m_Duration;
		uint32				m_StartTime;
		BBitmap				*m_SrcBitmap;
		BBitmap				*m_DstBitmap;
		TransitionCallback	m_TransitionCallback;

};



enum transitionStates 
{
	START_TRANSITION,
	END_TRANSITION,
	RUN_TRANSITION
};

// Transition Prototypes
//

void 	TransitionDrawBitmap(BBitmap *bitmap, TVisualCue *srcView, BView *dstView, BRect srcRect, BRect dstRect);

// In
void	WipeRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeDownIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeUpIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeTopLeftToBottomRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeTopRightToBottomLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeBottomLeftToTopRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeBottomRightToTopLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealDownIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealUpIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealTopLeftBottomRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealTopRightBottomLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealBottomLeftTopRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealBottomRightTopLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	CurtainsInIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	CurtainsOutIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );

// Out
void	WipeRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeDownOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeUpOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeTopLeftToBottomRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeTopRightToBottomLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeBottomLeftToTopRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	WipeBottomRightToTopLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealDownOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealUpOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealTopLeftBottomRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealTopRightBottomLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealBottomLeftTopRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	RevealBottomRightTopLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	CurtainsInOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );
void 	CurtainsOutOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector );

#endif
