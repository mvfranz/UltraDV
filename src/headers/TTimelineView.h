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

//	Includes
#include <MediaNode.h>

// Class Declarations
class TTimelineView: public BView, public BMediaNode
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
		
		//	Media Node
		port_id 	ControlPort() const;
		BMediaAddOn	*AddOn( int32 *internal_id) const;
		
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
		
		static status_t service_routine(void *data);
		void 			ServiceRoutine();
		static status_t run_routine(void *data);
		void 			RunRoutine();
			
						
		// Member variables
		TCueSheetWindow	*fCueSheetWindow;
		BPoint			fLastTick;
		BBitmap			*fIndicator;
		BRect			fIndicatorRect;
		
		thread_id		fMouseTracker;
		
		bool			fTimeToQuit;
		bool			fIsPlaying;
		bool			fIsStopping;
		
		port_id 		fPort;
		
		thread_id		fServiceThread;
		thread_id		fRunThread;								
};

#endif
