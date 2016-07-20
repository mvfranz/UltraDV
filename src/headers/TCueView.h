//---------------------------------------------------------------------
//
//	File:	TCueView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header file
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUEVIEW_H__
#define __TCUEVIEW_H__

//	Includes
#include <MessageRunner.h>

#include "AppTypes.h"
#include "TUndo.h"
// ABH added
#include "BaseCueChunk.h"

// Constants
const int16 kResizeZoneWidth = 7;

//	Enumerations

//	Define current display mode of cue content area
typedef enum
{
	kCueControls,		//	Default display
	kCueContents		// 	Cue contents are displayed
} CueDisplay;

//	Forward declararations
class TCueEffectView;

// Class Definition
class TCueView : public BView
{
	public:
		// ABH missing functions ???
		uint32 GetDuration();
	void SetTransitionInID(uint32);
	void SetTransitionOutID(uint32);
	void SetTransitionInDuration(uint32);
	void SetTransitionOutDuration(uint32);
	uint32 GetTransitionInDuration();
	uint32 GetTransitionOutDuration();
			
//		void SetDuration(uint32);
		float GetRotation();
		void SetRotation(float);
		void SetScale(BPoint);
		BRect GetArea();
		void SetScaledArea(BPoint);
		BPoint GetScaledArea();
		
		// Member Functions
		TCueView(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime, char *name);
		TCueView(int16 id, TCueChannel *parent, BRect bounds, BPoint startPoint, uint32 startTime, char *name);
		TCueView(const TCueView *theCue);
		TCueView(BMessage *data);
		// ABH add missing prototype
		TCueView(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds, char *name);
		virtual ~TCueView();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;
		
		virtual void 	Draw(BRect updateRect);
		
		virtual void 	MouseDown(BPoint where);
		virtual void 	MouseUp(BPoint where);
		virtual void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		virtual void 	WindowActivated(bool state);
		virtual void 	KeyDown(const char *bytes, int32 numBytes);
		virtual void 	KeyUp(const char *bytes, int32 numBytes);
		virtual void 	FrameMoved(BPoint new_position);
		virtual void 	FrameResized(float new_width, float new_height);
		
		void 			ScrollBy(float horz, float vert);
		
		virtual void 	MessageReceived(BMessage *message);
		
		void 			AttachedToWindow();	
		void			DetachedFromWindow();
		void 			Show();
		void 			Hide();
		virtual void 	ChannelInsert();
			
		void 			UpdateResizeZones();
		void 			UpdateStartTimeText();		
		void			UpdateEndTimeText();
		
		// Time Format and Resolution Handling
		virtual void ResolutionChanged( int16 theFormat, int16 theResolution);
				
		// Selection Routines
		void 		Select();
		void 		Deselect();
		void 		SelectAllEffects();
		void 		DeselectAllEffects();
		
		// Muting Routines
		void 		Mute();
		void 		Unmute();
		
		// Locking Routines
		void 		LockCue();
		void 		UnlockCue();
		
		// Sizing Routines
		virtual void	Contract(bool force = false);
		virtual void	Expand(bool force = false);
		virtual void	HideEffects();
		virtual void	ShowEffects();
				
		// File loading
		 virtual void	ShowPanel();
		 virtual void	HidePanel();
				
		// Layer Routines
		int32 			GetChannelID();
										
		// Playback Routines
		virtual void 	PrerollCue(uint32 theTime);
		virtual void 	PlayCue(uint32 theTime);
		virtual	void	StopCue(uint32 theTime);
		virtual void 	PauseCue();
		virtual void 	ResumeCue();

		virtual void 	HandlePlayback(uint32 theTime);
		
		// Clipboard Routines
		virtual BMessage *Copy();
		virtual void	 PasteComplete();
		virtual void 	OpenEditor();
				
		// Accessors handled in object for undo capability
		void	SetDuration(uint32 theDuration);
		void	SetStartTime(uint32 theTime);
		void 	SetInstantiated(bool theVal);
		void	SetEditorOpen(bool theVal);
		void 	SetChannel( TCueChannel *channel);

		// Accessor Methods
		inline TCueChannel	 	*GetChannel(){return fChannel;}
		inline bool 			IsLocked(){ return fIsLocked; }
		inline bool 			IsExpanded(){ return fIsExpanded; }
		inline BFilePanel		*GetPanel(){ return fPanel; }
		inline bool				CanTransition(){ return fCanTransition; }
		inline bool				HasEditor(){ return fHasEditor; }		
		inline bool				HasDuration(){ return fHasDuration; }				
		inline uint32			Duration(){ return fDuration; }
		inline uint32			StartTime(){ return fStartTime; }
		inline bool				Selected(){ return fIsSelected; }
		inline bool				IsVisible(){ return fIsVisible; }
		inline bool				IsInstantiated(){ return fIsInstantiated; }
		inline bool				IsMuted(){ return fIsMuted; }
						
		inline entry_ref		*GetFileRef(){ return &fFileRef; }
		inline BFile			*GetFile(){ return fFile; }
		inline drawing_mode		GetDrawingMode(){ return fDrawingMode; }
		inline CueDisplay		GetDisplayMode(){ return fCueDisplayMode; }
						
		inline bool				IsShowingDuration(){ return fShowDuration; }
		inline bool				IsShowingControls(){ return fCueDisplayMode == kCueControls; }
		inline bool				IsShowingContents(){ return fCueDisplayMode == kCueContents; }
		inline bool				IsEffectsVisible(){ return fEffectsVisible; }
														
		// Member Variables			
							
	private:
		

	protected:
					
		// Member Functions
		virtual void 	Init();
		void Init(BaseCueChunk *);
		
		void			MouseDownControls(BPoint where);
		void			MouseDragControls(BPoint where);
		 
		void 			DrawDurationDelta();
		virtual void	DrawEffectsTray(BRect updateRect);
		virtual void	DrawEffects(BRect updateRect);
		
		virtual void 	ResizeRightSide();
		virtual void 	ResizeLeftSide();
						
		// Cue Position
		void			AdjustCueStartTime(uint32 theTime);
		void			AdjustCueEndTime(uint32 theTime);
		
		void			DoMarchingAnts(BRect antsRect);
		
		// 	Cue Button Handling 		
		void 			OpenCueMenu(BPoint menuPt);
		void 			OpenTransitionMenu(BPoint menuPt);
				
		// 	Cue Contents
		virtual void	ShowContents();
		virtual void	HideContents();
		virtual void 	ShowControls();
		virtual void 	HideControls();
		void			ShowDuration();
		void			HideDuration();
		virtual void 	SetDisplayMode(CueDisplay theMode);
										
		// Utility functions
		virtual void 	LoadCueIcon();
		void 			AddToBrowser();
		bool 			EffectsClick(BPoint where);
		TCueEffectView	*ClickedEffect(BPoint where);
		void 			DeleteSelectedEffects();
												
		//	Member Variables			
	// ABH missing transparency???
		float fTransparency; //SetTransparency / GetTransparency; 
		uint32 fTransitionInDuration;
		uint32 fTransitionOutDuration;
		uint32 fTransitionInID;
		uint32 fTransitionOutID;
	// ABH missing rotation???
		float fRotation;
	// ABH
		BPoint fInsertPoint;
		
		rgb_color		fLowColor;				// Low color for content and env
		rgb_color		fHighColor;			// High color for content and env
		drawing_mode	fDrawingMode;			// Drawing ink effect
		bool			fIsInstantiated;		// True if cue is fully realized
		bool			fIsPrepared;			// Ready for playback
		bool			fIsPaused;
		
		uint32			fFileTime;
		uint32			fStartTime;			// in milliseconds
		uint32			fPreparedTime;
		uint32			fDuration;				// in milliseconds
		uint32			fStartOffset;
								
		bool			fCanStretch;			// true if cue is stretchable
		bool			fCanWindow;			// true if cue can window into file
		bool			fCanLoop;				// true if cue can loop
		bool			fCanEnvelope;			// true if cue can volume envelope
		bool			fHasDuration;			// true if cue has a duration
		bool			fIsVisible;			// true if cue is visual
		bool			fCanCrop;				// true if cue can visual crop
		
		bool			fCanTransition;		// true if cue can transition
		bool			fCanPath;				// true if cue can path
						
		bool			fHasEditor;			// true if cue has internal editor		
		bool			fEditorOpen;			// true if internal editor is open
			
		bool			fIsSelected;			// true if it has been click selected
		bool			fHasExternalEditor;	// true if use external editor
		
		BRect			fLResizeZone;			// Left side resize zone
		BRect			fRResizeZone;			// Right side resize zone				
		BRect 			fDurationDeltaRect;	// Location of duration delta indicator
		BRect			fEffectsTray;			// Area of cues effects tray	
		
		bool			fAppCursorSet;
		uint32			fInsertTime;	
		
		BFilePanel 		*fPanel; 				// File panel for cue media loading
		
		TCueTimeText	*fStartText;
		TCueTimeText	*fEndText;
				
		// Cue Buttons
		TBitmapButton	*fLockButton;
		TBitmapButton	*fMuteButton;		
		TBitmapButton	*fEffectsButton;
				
		int16			fID;					// 	Cue identification ID
		
		bool			fMouseDown;			// 	For mouse down/up tracking
		bool			fResizing;				// 	For mouse dragging
							
		bool			fIsLocked;				// 	Cue is locked into channel
		bool			fIsExpanded;			//	Cue is expanded in channel	
		bool			fIsMuted;				//	Cue data is muted or non-visible
		bool			fEffectsVisible;		//	Effects tray is visible
		
		BList			*fEffectsList;			//	List to hold effects applied to cue
		
		TBitmapView		*fCueIcon;				// Visual display of cue		
		TCueChannel		*fChannel;
							
		int32			fMinCueWidth;			// Minimum width of a cue
					
		TTimer			*fAntThread;
		
		entry_ref		fFileRef;				// Cues data file entry_ref	
		BFile			*fFile;				// Cues file
		
		BPoint			fLastClickPt;			// Mouse click tracking					
		
		bool			fShowDuration;			//	Show duration delta
		
		CueDisplay		fCueDisplayMode;		//	Type of cue display in cue data area
		
		bool			fIsPlaying;
		bool			fIsStopping;
		
		//pattern	cueAnts = {0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F};
		pattern			fCueAnts;
		BMessageRunner* fRunner;
};

#endif

