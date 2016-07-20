//-------------------------------------------------------------------
//
//	File:	TCueChannel.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __TCUECHANNEL_H__
#define __TCUECHANNEL_H__

#include <TimeCode.h>

#include "AppTypes.h"

//	Constants
const int32 kEffectsTrayHeight = 100;

// Class Declarations
class TCueChannel: public BView
{
	public:
		TCueChannel(BRect bounds, TCueSheetView *parent, int32 ID);
		TCueChannel(BMessage *data);
		~TCueChannel();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;
		void 		CreateHeader(THeaderContainerView *container, float height);
		
		void 	Draw(BRect updateRect);		
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	KeyUp(const char *bytes, int32 numBytes);
		
		void 	ScrollBy(float horz, float vert);
		
		void 	MessageReceived(BMessage *message);
		
		void 	AttachedToWindow();
		
		// Cue Handling
		void 		SendMessageToAllCues(BMessage *theMessage);
		void 		SendMessageToAllSelectedCues(BMessage *theMessage);
		bool 		HasCues();
		void 		SelectAllCues();
		void 		DeselectAllCues();
		void 		DeselectAllCues(TCueView *theCue);
		bool 		HasSelectedCues();
		BList 		*GetSelectedCues();
				
		bool 		CanInsertCue(TCueView *insertCue, uint32 insertTime, bool showAlert);
		bool		CanInsertCue(TCueView *insertCue, BPoint insertPoint, bool showAlert); // needs to be implemented
		void 		AddCue(TCueView *cueView);
		void 		InsertCue(TCueView *cueView, uint32 time);
		void		InsertCue(TCueView *cueView, BPoint insertPoint, uint32 insertTime);
		void 		RemoveCue(TCueView *cueView);
		
		TCueView 	*GetCueAtTime(uint32 theTime);
		TVisualCue 	*GetVisualCueAtTime(uint32 theTime);
		
		void		DeleteSelectedCues();
		void		NudgeSelectedCues(bool nudgeLeft);
		
		// Time and Resolution Handling
		timecode_type GetTimeFormat();
		int16 		GetResolution();
		void 		TimeFormatChanged();
		void		ResolutionChanged(int32 resizePixels);
		
		TCueView 	*GetNeighborLeft(TCueView *theView);
		TCueView 	*GetNeighborRight(TCueView *theView);
		TCueView 	*GetNeighborRight( BPoint *where );		
		bool 		GetNeighborLeftPoint(TCueView *theView, BPoint *where);
		bool 		GetNeighborRightPoint(TCueView *theView, BPoint *where);
		
		// Utility Functions
		void 	SetChannelName(const char *theName);
		void 	SetChannelID(int16 ID);
		void	Solo();
		void	Unsolo();
		void	Mute();
		void	Unmute();
		void 	Contract(bool force = false);
		void 	Expand(bool force = false);
		void 	ContractAllCues();
		void 	ExpandAllCues();
		
		// Drawing Functions
		void 	DrawIndicatorTick(BPoint drawPt);
		void 	DrawTimelineGuides(BRect updateRect);
		
		// Locking Functions		
		void	LockChannel();
		void	UnlockChannel();
		void	UnlockAllCues();
		void	LockAllCues();
				
		// Member Varible Handling
		void 	SetID(int32 theID);
		void 	SetParent(TCueSheetView *parent);
		void 	SetHeader(TCueChannelHeader *header);
		
		// Accessor functions
		inline TCueSheetView 		*GetCueSheet(){ return fCueSheet; }
		inline TCueChannelHeader	*GetChannelHeader(){ return fHeader; }		
		inline int16		 		GetID(){ return fID; }
		inline char		 			*GetName(){ return fName; }
		inline bool 				GetSolo(){ return fIsSoloed; }
		inline void 				SetSolo(bool state){ fIsSoloed = state; }
		inline bool 				GetMute(){ return fIsMuted; }
		inline void 				SetMute(bool state){ fIsMuted = state; }
		inline bool 				IsExpanded(){ return fIsExpanded; }
		inline bool 				IsLocked(){ return fIsLocked; }
		inline BList 				*GetCueList(){ return fCueList; }
		inline int32 				GetTotalCues(){ return fCueList->CountItems(); }
			
		// Member Variables
				
	protected:
		// Friend classes
		friend class TCueView;
		friend class TAudioCue;
		friend class TMIDICue;
		friend class TPictureCue;
		
		// Member functions
		void 	RemoveCueFromList(TCueView *cueView);	
		
	private:	
						
		// Member variables	
		TCueSheetView		*fCueSheet;	
		TCueChannelHeader	*fHeader;
		BList 				*fCueList;		
		
		int32				fID;
		char				fName[255];
		
		bool				fIsMuted;
		bool				fIsSoloed;
		bool				fIsExpanded;
		bool				fIsLocked;
		
		BPoint				fTickPt;
		BPoint				fOldTickPt;
		
		// Member functions
		void 	Init();
		
		void 	AddCueToList(TCueView *cueView);		
		void	FreeCueList();
		
		void 	CreateCue(BPoint point, int16 cueIconID);	
		void 	CreateCue(BPoint point, entry_ref &theRef);
		
		void 	HandleSoloMessage();
		void 	HandleMuteMessage();
		void 	HandleExpandMessage();
		void	HandleLockMessage();
		void 	HandleEffectExpandMessage();
		
		void 	RedrawAllCues();
		void 	UpdateAllResizeZones();
};

#endif
