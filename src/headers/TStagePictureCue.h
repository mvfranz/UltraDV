//-------------------------------------------------------------------
//
//	File:	TStagePictureCue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.28.98
//
//-------------------------------------------------------------------

#pragma once 

#include "TStageCue.h"

// Forward Declarations
class	TTransition;

// Class Definition
class TStagePictureCue : public TStageCue
{
	public:
		// Memeber Functions
		TStagePictureCue(BRect bounds, char *name, TCueView *theCue);
		TStagePictureCue(BMessage *theMessage);
		~TStagePictureCue();
		
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
		BRect		m_BitmapArea;		
		TTransition	*m_Transition;
		
		
		// Resize control zones
		BRect		m_TopLeftResize;
		BRect		m_TopMiddleResize;
		BRect		m_TopRightResize;
		BRect		m_RightMiddleResize;
		BRect		m_BottomRightResize;
		BRect		m_BottomMiddleResize;
		BRect		m_BottomLeftResize;
		BRect		m_LeftMiddleResize;
												
};
