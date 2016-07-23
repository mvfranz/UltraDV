//-------------------------------------------------------------------
//
//	File:	TBitmapView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.02.97
//
//-------------------------------------------------------------------

#ifndef __TBITMAPVIEW_H__
#define __TBITMAPVIEW_H__

//#include <interface/Bitmap.h>

// ABH added BBitmap because of GetBitmap()->Bits() error
class TBitmapView : public BView //, public BBitmap
{	
	public:
		TBitmapView( BRect area, char *name, BBitmap *bitmap, bool own, uint32 resizeFlags = B_FOLLOW_NONE);
		~TBitmapView();
						
		virtual void Draw( BRect area);
		BBitmap *fBitmap; // ABH moved from private to public
					
	private:
		bool	fOwnership;
					
};

#endif
