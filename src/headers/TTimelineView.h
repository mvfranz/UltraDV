//-------------------------------------------------------------------
//
//	File:	TTimelineView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __TTIMELINEVIEW_H__
#define __TTIMELINEVIEW_H__


// Class Declarations
class TTimelineView: public BView
{
	public:
		// Member Functions
		TTimelineView(BRect bounds, TCueSheetWindow *parent);
		TTimelineView( BMessage *message);
		~TTimelineView();
		
		static	BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);

		void	FrameResized(float new_width, float new_height);

		void 	MessageReceived(BMessage *message);
		
		void 	ResolutionChanged(int32 resizePixels);
				
		void 	AttachedToWindow();

		void	Pulse();

		// Accessor Functions
		void SetParent(TCueSheetWindow *parent);
		
		// Member Variables
		
	private:
					
		// Member functions
		void 			Init();
		void 			DrawSubTicks(int32 index);
		void 			UpdateTimeTick(BPoint where);
		void 			DrawIndicator();
		void 			DrawIndicator(BRect oldPosition);
		
		void 			ClipIndicatorRect();
		
		void			TrackIndicator(BPoint mousePt);
		void 			TrackPlayback();	
						
		// Member variables
		TCueSheetWindow	*fCueSheetWindow;
		BPoint			fLastTick;
		BBitmap			*fIndicator;
		BRect			fIndicatorRect;
		
		thread_id		fMouseTracker;
		
		bool			fIsPlaying;
		bool			fIsStopping;
};

#endif
