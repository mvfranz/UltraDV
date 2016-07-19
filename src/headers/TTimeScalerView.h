//-------------------------------------------------------------------
//
//	File:	TTimeScalerView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __TTimeScalerView__
#define __TTimeScalerView__ 
 

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

// Class Declarations
class TTimeScalerView: public BView
{
	public:
		TTimeScalerView(BRect bounds, TCueSheetView *parent);
		TTimeScalerView(BMessage *data);
		~TTimeScalerView();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		
	private:
					
		// Member Functions
		void 	Init();
		
		void 	DrawTimeScaleTicks();
		void 	DrawTickIndicator();
		
		int16 	GetClickRect(BPoint where);
		
		// Member variables
		//TCueSheetView	*fCueSheet;
		BPoint			fLocation;
		TBitmapView		*fIndicator;
		BRect			fTextRect;
		BRect			fControlRect;
		BRect			fClickArray[kTotalTicks];
						
};

#endif
