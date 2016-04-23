//-------------------------------------------------------------------
//
//	File:	TExportZone.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.14.98
//
//-------------------------------------------------------------------

#ifndef __TEXPORTZONE_H__
#define __TEXPORTZONE_H__

//	Constants
const char kExportSliderWidth 	= 9;
const char kExportSliderHeight 	= 14;
 
//	Enumeration
typedef enum
{
	kNoDrag,
	kInDrag,
	kOutDrag,
	kZoneDrag
} ExportDragType;

// Class Declarations
class TExportZone: public BView
{
	public:
		// Member Functions
		TExportZone(BRect bounds, TCueSheetWindow *parent);
		TExportZone(BMessage *archive);
		~TExportZone();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;
		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);

		void 	MessageReceived(BMessage *message);
		
		void 	ResolutionChanged(int32 resizePixels);
		
		void 	AttachedToWindow();
		
		// Accessor Functions
		void			SetParent(TCueSheetWindow *parent);
		
		// Member Variables
		
	private:
					
		// Member functions
		void 	Init();
		
		void 	HandleClick(BPoint where);
		void 	HandleDoubleClick(BPoint where);
		
		void	DrawInMarker(BRect updateRect);
		void	DrawOutMarker(BRect updateRect);
				
		void	TrackInMarker(BPoint mousePt);
		void 	TrackOutMarker(BPoint mousePt);
		void 	TrackZone(BPoint mousePt);
						
		// Member variables
		TCueSheetWindow	*m_CueSheetWindow;
		BRect			m_ExportChannel;
		
		BBitmap			*m_InMarker;
		BBitmap			*m_OutMarker;
		BRect			m_InRect;
		BRect			m_OutRect;
		
		thread_id		m_MouseTracker;
		
		ExportDragType	m_DragType;				
};

#endif
