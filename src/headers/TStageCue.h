//-------------------------------------------------------------------
//
//	File:	TStageCue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.28.98
//
//-------------------------------------------------------------------

#ifndef __TSTAGECUE_H__
#define __TSTAGECUE_H__ 

#include "TVisualCue.h"
#include "TPolygon.h"
#include "TCuePosition.h"

// Class Definition
class TStageCue
{
	public:
		// Member Functions
		TStageCue(TStageView *theStage, TVisualCue *theCue);
		~TStageCue();
		
		//	Message Handling
		void 	MessageReceived(BMessage *message);
		void 	HandleMouseMessage(BMessage *message);
		
		//	Drawing routines				
		void 	Draw(BRect updateRect);
		void 	DrawData();
		
		void 	MouseDown(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
						
		void 	Select();
		void	Deselect();
		
		void	DrawSelectionRect(bool toOffscreen);
		void 	InvalidateSelectionRect();
		
		bool	PointInResizeZones(BPoint thePoint);		
										
		// Member Variable Handling
		void 	SetChannelCue(TVisualCue *theCue);
				
		// Accessor Functions
		inline TVisualCue 	*GetChannelCue(){ return m_ChannelCue; }
		inline bool			Selected(){ return m_IsSelected; }
		
	private:
		void	Init();
		
		//	Resize Zone Handling
		void 	SetResizeZones();
		void 	TranformResizeZones();
		void 	ClearResizeZones();
		int16 	GetResizeZoneID(BPoint thePoint);
		
		void 	DragPicture(BPoint thePoint);
		
		//	Drawing Routines
		void 	DrawResizingHandles(BView *theView);
		void 	DrawResizeCorners(BView *theView);
		void 	DrawResizeMiddles(BView *theView);
		void	DrawRegistrationPoint(BView *theView);
		
		/*
		//	Scaling Routines
		void 	Scale(BPoint thePoint);
		void 	ScaleTopLeft( BPoint thePoint);
		void 	ScaleTop( BPoint thePoint);
		void 	ScaleTopRight( BPoint thePoint);
		void 	ScaleRight( BPoint thePoint);
		void 	ScaleBottomRight( BPoint thePoint);
		void 	ScaleBottom( BPoint thePoint);
		void 	ScaleBottomLeft( BPoint thePoint);
		void 	ScaleLeft( BPoint thePoint);
		
		// 	Cropping Routines
		void 	Crop(BPoint thePoint);
		void 	CropTopLeft( BPoint thePoint);
		void 	CropTop( BPoint thePoint);
		void 	CropTopRight( BPoint thePoint);
		void 	CropRight( BPoint thePoint);
		void 	CropBottomRight( BPoint thePoint);
		void 	CropBottom( BPoint thePoint);
		void 	CropBottomLeft( BPoint thePoint);
		void 	CropLeft( BPoint thePoint);
		
		//	Rotation Routines
		void 	Rotate(BPoint thePoint);
		
		//	Mirroring Routines
		void 	Mirror(BPoint thePoint);
		void 	MirrorHorizontal(BPoint thePoint);
		void 	MirrorVertical(BPoint thePoint);
		*/
		//	Cursor Routines
		void 	SetMoveCursor(BPoint thePoint);
		void 	SetRotateCursor(BPoint thePoint);
		void 	SetCropCursor(BPoint thePoint);
		void 	SetScaleCursor(BPoint thePoint);
		void 	SetShearCursor(BPoint thePoint);
		void 	SetSkewCursor(BPoint thePoint);		
		void 	SetPerspectiveCursor(BPoint thePoint);
		void 	SetMirrorCursor(BPoint thePoint);
		
		void 	OpenStageCueMenu(BPoint menuPt);
		
		void	OpenCurrentToolDialog();				
					
	protected:
		TStageView	*m_Stage;	
		TVisualCue	*m_ChannelCue;		
		bool		m_IsSelected;
		TTimer		*m_AntThread;
		
		// Resize control zones
		TPolygon m_TopLeftResize;
		TPolygon m_TopMiddleResize;
		TPolygon m_TopRightResize;
		TPolygon m_RightMiddleResize;
		TPolygon m_BottomRightResize;
		TPolygon m_BottomMiddleResize;
		TPolygon m_BottomLeftResize;
		TPolygon m_LeftMiddleResize;
									
};

// Constants
enum ResizeZoneID
{
	kTopLeftResize = 0,
	kTopMiddleResize,
	kTopRightResize,
	kRightMiddleResize,
	kBottomRightResize,
	kBottomMiddleResize,
	kBottomLeftResize,
	kLeftMiddleResize	
};

#endif
