//-------------------------------------------------------------------
//
//	File:	TAudioTimelineView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.21.98
//
//-------------------------------------------------------------------

#ifndef __TAUDIOTIMELINEVIEW_H__
#define __TAUDIOTIMELINEVIEW_H__ 

// Forward Declarations
class TAudioEditor;

// Class Declarations
class TAudioTimelineView: public BView
{
	public:
		TAudioTimelineView(BRect bounds, TAudioEditor *parent);
		~TAudioTimelineView();
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void 	WindowActivated(bool state);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	KeyUp(const char *bytes, int32 numBytes);
		void 	FrameMoved(BPoint new_position);
		void 	FrameResized(float new_width, float new_height);
		void 	MessageReceived(BMessage *message);
		
		void 	SetTimelineViewBounds(BRect bounds);
		
		// Accessor Functions
		inline BRect GetIndicatorZone(){ return fIndicatorZone; }
		
		// Member Variables
		
	private:
					
		// Member functions
		void 	Init();
		void 	UpdateTimeTick(BPoint where);
		void 	DrawIndicator();
		
		void 	ClipIndicatorRect();
						
		// Member variables
		TAudioEditor	*fAudioEditor;
		BRect			fIndicatorZone;
		BPoint			fLastTick;
		BBitmap			*fIndicator;
		//BBitmap		*fOffscreen;
		BRect			fIndicatorRect;
		
};

#endif
