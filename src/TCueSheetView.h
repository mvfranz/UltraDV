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
		inline 	TCueSheetWindow *GetParent(){ ASSERT(fParent); return fParent; }
		inline 	BList 			*GetChannelList(){ printf("TCSV:GetChannelList.h: before return\n"); 
									ASSERT(fChannelList);
									return fChannelList; }
		inline 	int32 			GetTotalChannels(){ return fChannelList->CountItems(); }
				
		inline	uint32		StartTime(){ return fStartTime; }
		inline	uint32		Duration(){ return fDuration; }
		inline	uint32		GetExportStartTime(){ return fExportStartTime; }
		inline	uint32		GetExportStopTime(){ return fExportStopTime; }
		inline	uint32		GetCurrentTime(){ return fCurrentTime; }		
		
		inline	int16			GetResolution(){ return fResolution; }
		inline	timecode_type	GetTimeFormat(){ return fTimeFormat; }
		inline	bool			IsDirty(){ return fIsDirty; }
		inline	bool			GetLiveUpdate(){ return fLiveUpdate; }
		
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
		int16			fResolution;
		timecode_type	fTimeFormat;
	
		uint32			fStartTime;
		uint32			fDuration;
		uint32			fCurrentTime;
		
		uint32			fExportStartTime;
		uint32			fExportStopTime;
		
		bool			fIsDirty;			// Used to track user save state
		
		rgb_color		fStageColor;
		
		TCueSheetWindow	*fParent;
		BList 			*fChannelList;		
		
		bool			fStageToFront;
		bool			fSnapToGrid;
		bool			fShowDuration;		
		bool			fHideMenu;
		bool			fStageGridOn;
		bool			fCenterStage;
		bool			fIsCompressedView;
		bool			fHideCursor;
		bool			fLiveUpdate;
};

#endif
