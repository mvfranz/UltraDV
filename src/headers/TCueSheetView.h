//-------------------------------------------------------------------
//
//	File:	TCueSheetView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __TCUESHEETVIEW_H__
#define __TCUESHEETVIEW_H__

#include <TimeCode.h>
#include <assert.h>

#include "AppTypes.h"

// Class Declarations
class TCueSheetView: public BView
{
	public:
		// Member functions
		TCueSheetView(BRect bounds, TCueSheetWindow *parent);
		TCueSheetView( BMessage *archive);
		~TCueSheetView();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		void 	Draw(BRect updateRect);
		void 	DrawAfterChildren(BRect updateRect);
				
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
		void 	MessageReceived(BMessage* message);
		void 	WindowActivated(bool state);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	KeyUp(const char *bytes, int32 numBytes);
		void 	FrameMoved(BPoint new_position);
		void 	FrameResized(float new_width, float new_height);	
					
		// Selection routines
		bool 	HasCues();
		void 	SelectAllCues();
		void 	DeselectAllCues();
		void 	DeselectAllCues(TCueView *theCue);
		bool 	HasMultipleCueChannelsSelected();
		
		// Cue Alignment Routines
		void 	AlignCueStartTimes();
		void 	AlignCueEndTimes();
		
		// Channel Handling
		void	SendMessageToAllChannels(BMessage *theMessage);
		bool 	AddChannel();
		void	InsertChannel(BMessage *message);
		void 	DeleteChannel(BMessage *message);
		void	DragInsertChannel(TCueChannel *insertChannel, TCueChannel *afterChannel);
				
		void 	AdjustChannelPositions(TCueChannel *theChannel);
		
		void	SoloAllChannels();
		void	UnsoloAllChannels();
		bool 	AreOtherChannelsSoloed( TCueChannel *theChannel);
		void	MuteAllChannels();
		void	MuteAllUnsoloedChannels();
		void	UnmuteAllChannels();
		void 	ContractAllChannels();
		void 	ExpandAllChannels();
		void	UnlockAllChannels();
		void	LockAllChannels();
		
		void 	ResolutionChanged(BMessage *message);
						
		// Clipboard handling
		bool 	ItemsSelected();
		void 	Undo();
		void 	Cut();
		void 	Copy();
		void 	Paste();
		void 	Clear();
		void 	SelectAll();
		
		//	Visibility
		void 	AttachedToWindow();
		
		// 	Member Variable Handling.  We don't do these as inline
		//	so the Undo functionality will work properly
		void	SetCurrentTime(uint32 theTime);
		void 	SetResolution(int16 theResolution);
		void 	SetDirty(bool theVal);
		void 	SetExportStartTime(uint32 theTime);
		void 	SetExportStopTime(uint32 theTime);
		void 	SetParent(TCueSheetWindow *parent);
		void 	SetLiveUpdate(bool drag);
				
		// Accessor function
		inline 	TCueSheetWindow *GetParent(){ ASSERT(m_Parent); return m_Parent; }
		inline 	BList 			*GetChannelList(){ printf("TCSV:GetChannelList.h: before return\n"); 
									ASSERT(m_ChannelList);
									return m_ChannelList; }
		inline 	int32 			GetTotalChannels(){ return m_ChannelList->CountItems(); }
				
		inline	uint32		StartTime(){ return m_StartTime; }
		inline	uint32		Duration(){ return m_Duration; }
		inline	uint32		GetExportStartTime(){ return m_ExportStartTime; }
		inline	uint32		GetExportStopTime(){ return m_ExportStopTime; }
		inline	uint32		GetCurrentTime(){ return m_CurrentTime; }		
		
		inline	int16			GetResolution(){ return m_Resolution; }
		inline	timecode_type	GetTimeFormat(){ return m_TimeFormat; }
		inline	bool			IsDirty(){ return m_IsDirty; }
		inline	bool			GetLiveUpdate(){ return m_LiveUpdate; }
		
		// Member variables
				
	private:
		// Member functions
		void 	Init();
		void 	FreeChannelList();
		void 	UpdateChannelIDS();
		
		void 	UpdateTimeline();		
		void 	UpdateTimeFormat(BMessage *message);
				
		void 	DeleteSelectedCues();
		void 	NudgeSelectedCues(const char *bytes);
				
		// Clipboard Handling
		void 	PasteCues(BList *cueList);
		void 	HandleCuePaste(BMessage *theMessage);
						
		// Member variables
		int16			m_Resolution;
		timecode_type	m_TimeFormat;
	
		uint32			m_StartTime;
		uint32			m_Duration;
		uint32			m_CurrentTime;
		
		uint32			m_ExportStartTime;
		uint32			m_ExportStopTime;
		
		bool			m_IsDirty;			// Used to track user save state
		
		rgb_color		m_StageColor;
		
		TCueSheetWindow	*m_Parent;
		BList 			*m_ChannelList;		
		
		bool			m_StageToFront;
		bool			m_SnapToGrid;
		bool			m_ShowDuration;		
		bool			m_HideMenu;
		bool			m_StageGridOn;
		bool			m_CenterStage;
		bool			m_IsCompressedView;
		bool			m_HideCursor;
		bool			m_LiveUpdate;
};

#endif
