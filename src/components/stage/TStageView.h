//-------------------------------------------------------------------
//
//	File:	TStageView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __TSTAGEVIEW_H__
#define __TSTAGEVIEW_H__


//	Enumerations
typedef enum
{
	kMoveMode,
	kRotateMode,
	kCropMode,
	kScaleMode,
	kShearMode,
	kSkewMode,
	kPerspectiveMode,
	kMirrorMode
	
}  ToolMode;


// Class Definition
class TStageView : public BView
{
	public:
		// Member Functions
		TStageView(BRect bounds, TStageWindow *parent);
		TStageView(BMessage *archive);
		~TStageView();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;
				
		void 	Draw(BRect updateRect);
		void 	StageDraw(BRect updateRect, uint32 theTime);
		
		void 	MouseDown(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void 	FrameResized(float new_width, float new_height);
				
		void 	MessageReceived(BMessage *message);
		void	Pulse();
	
		// Compositing Routines
		void	ClearOffscreen();
		void 	BlitOffscreen();		
		BBitmap *CreateComposite(int32 start, int32 stop, int32 theTime, BRect theRect);
		BBitmap *CopyStageBitmap(BRect theRect);

		// Inlines
		inline BBitmap 	*GetOffscreenBitmap(){ return m_OffscreenBitmap; }
		inline BView 	*GetOffscreenView(){ return m_OffscreenView; }
		inline ToolMode GetToolMode(){ return m_ToolMode; }
		
	private:		
		// Member functions	
		void			Init();
		
		thread_id 		CreateStageOffscreen();
		static int32 	stage_offscreen(void *arg);
		int32 			StageOffscreen();
		
		//	Message handling
		void			SendMessageToAllCues(BMessage *message);
		void			SendMessageToSelectedCues(BMessage *message);
		
		// StageCue list handling
		void 			ClearStageCueList();
		
		//	Stage Tools
		void 			SetToolMode(uint32 theTool);
		
		// Member Variables
		TStageWindow 	*m_Parent;
		BView 			*m_OffscreenView;
		BBitmap 		*m_OffscreenBitmap;
		thread_id		m_StageOffscreenThread;	
		BList			*m_StageCueList;
		bool			m_SelectionMode;
		ToolMode		m_ToolMode;
};

#endif
