//-------------------------------------------------------------------
//
//	File:	TAudioScalerView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//-------------------------------------------------------------------

#ifndef __TAUDIOSCALERVIEW_H__
#define __TAUDIOSCALERVIEW_H__
 

// TimeScaler states
//const 
enum
{
	kOneFrame,
	kQuarterSecond,
	kHalfSecond,
	kOneSecond,
	kTwoSecond,
	kFiveSecond,
	kTenSecond,
	kThirtySecond,
	kOneMinute,
	kFiveMinute,
	kTotalTicks
};

// Forward Declarations
class TAudioEditorView;
class TBitmapView;

// Class Declarations
class TAudioScalerView: public BView
{
	public:
		TAudioScalerView(BRect bounds, TAudioEditorView *parent);
		~TAudioScalerView();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void WindowActivated(bool state);
		void KeyDown(const char *bytes, int32 numBytes);
		void KeyUp(const char *bytes, int32 numBytes);
		void FrameMoved(BPoint new_position);
		void FrameResized(float new_width, float new_height);
		
	private:
					
		// Member Functions
		void 	Init();
		
		void 	DrawTimeScaleTicks();
		void 	DrawTickIndicator();
		
		int16 	GetClickRect(BPoint where);
		
		// Member variables
		TAudioEditorView	*fEditor;
		BPoint				fLocation;
		TBitmapView			*fIndicator;
		BRect				fTextRect;
		BRect				fControlRect;
		BRect				fClickArray[kTotalTicks];
						
};

#endif
