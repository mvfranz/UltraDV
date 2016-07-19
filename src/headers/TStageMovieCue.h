//-------------------------------------------------------------------
//
//	File:	TStageMovieCue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.12.98
//
//-------------------------------------------------------------------

#ifndef _TSTAGEMOVIECUE_H_
#define _TSTAGEMOVIECUE_H_

#include "TStageCue.h"

// Forward Declarations
class	TTransition;

// Class Definition
class TStageMovieCue : public TStageCue
{
	public:
		// Memeber Functions
		TStageMovieCue(BRect bounds, char *name, TCueView *theCue);
		TStageMovieCue(BMessage *theMessage);
		~TStageMovieCue();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;
				
		void 	Draw(BRect updateRect);
		void 	DrawData(BRect updateRect, int32 theTime);
		void 	CompositeData(BRect updateRect, BView *offscreen);
		
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void 	WindowActivated(bool state);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	KeyUp(const char *bytes, int32 numBytes);
		void 	FrameMoved(BPoint new_position);
		void 	FrameResized(float new_width, float new_height);
		
		void 	SetBitmap(BBitmap *bitmap);
		
		void 	MessageReceived(BMessage *message);
									
	private:
		void	Init();
		void 	UpdatePictureCueRegion(double theTime);
		
		void	DrawSelectionRect();
		void 	InvalidateSelectionRect();
		
		void	SetResizeZones();
		bool	PointInResizeZones(BPoint thePoint);
		void 	DragPicture(BPoint thePoint);
		void 	ResizeOrCrop(BPoint thePoint);
		int16 	GetResizeZoneID(BPoint thePoint);
				
		void 	ResizeTopLeft( BPoint thePoint);
		void 	ResizeTopRight( BPoint thePoint);
		void 	ResizeRight( BPoint thePoint);
		void 	ResizeBottomRight( BPoint thePoint);
		void 	ResizeBottom( BPoint thePoint);
		void 	ResizeLeft( BPoint thePoint);
							
		void 	Stop();
		void	SetVisibility(double theTime);
		void 	DoTransition(bool transitionIn);
		
		void 	OpenStageCueMenu(BPoint menuPt);
		
		// Member Variables	
		BRect		fBitmapArea;		
		TTransition	*fTransition;
		
		
		// Resize control zones
		BRect		fTopLeftResize;
		BRect		fTopMiddleResize;
		BRect		fTopRightResize;
		BRect		fRightMiddleResize;
		BRect		fBottomRightResize;
		BRect		fBottomMiddleResize;
		BRect		fBottomLeftResize;
		BRect		fLeftMiddleResize;
												
};


#endif	// _TSTAGEMOVIECUE_H_
