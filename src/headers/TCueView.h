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
#include <MediaNode.h>

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
class TCueView : public BView, public BMediaNode
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
		
		//	Media Node
		port_id 	ControlPort() const;
		BMediaAddOn	*AddOn( int32 *internal_id) const;
		
		// Accessor Methods
		inline TCueChannel	 	*GetChannel(){return m_Channel;}
		inline bool 			IsLocked(){ return m_IsLocked; }
		inline bool 			IsExpanded(){ return m_IsExpanded; }
		inline BFilePanel		*GetPanel(){ return m_Panel; }
		inline bool				CanTransition(){ return m_CanTransition; }
		inline bool				HasEditor(){ return m_HasEditor; }		
		inline bool				HasDuration(){ return m_HasDuration; }				
		inline uint32			Duration(){ return m_Duration; }
		inline uint32			StartTime(){ return m_StartTime; }
		inline bool				Selected(){ return m_IsSelected; }
		inline bool				IsVisible(){ return m_IsVisible; }
		inline bool				IsInstantiated(){ return m_IsInstantiated; }
		inline bool				IsMuted(){ return m_IsMuted; }
						
		inline entry_ref		*GetFileRef(){ return &m_FileRef; }
		inline BFile			*GetFile(){ return m_File; }
		inline drawing_mode		GetDrawingMode(){ return m_DrawingMode; }
		inline CueDisplay		GetDisplayMode(){ return m_CueDisplayMode; }
						
		inline bool				IsShowingDuration(){ return m_ShowDuration; }
		inline bool				IsShowingControls(){ return m_CueDisplayMode == kCueControls; }
		inline bool				IsShowingContents(){ return m_CueDisplayMode == kCueContents; }
		inline bool				IsEffectsVisible(){ return m_EffectsVisible; }
														
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
		
		//	Thread Functions
		static status_t service_routine(void *data);
		void 			ServiceRoutine();
		static status_t run_routine(void *data);
		void 			RunRoutine();
												
		//	Member Variables			
	// ABH missing transparency???
		float m_Transparency; //SetTransparency / GetTransparency; 
		uint32 m_TransitionInDuration;
		uint32 m_TransitionOutDuration;
		uint32 m_TransitionInID;
		uint32 m_TransitionOutID;
	// ABH missing rotation???
		float m_Rotation;
	// ABH
		BPoint m_InsertPoint;
		
		rgb_color		m_LowColor;				// Low color for content and env
		rgb_color		m_HighColor;			// High color for content and env
		drawing_mode	m_DrawingMode;			// Drawing ink effect
		bool			m_IsInstantiated;		// True if cue is fully realized
		bool			m_IsPrepared;			// Ready for playback
		bool			m_IsPaused;
		
		uint32			m_FileTime;
		uint32			m_StartTime;			// in milliseconds
		uint32			m_PreparedTime;
		uint32			m_Duration;				// in milliseconds
		uint32			m_StartOffset;
								
		bool			m_CanStretch;			// true if cue is stretchable
		bool			m_CanWindow;			// true if cue can window into file
		bool			m_CanLoop;				// true if cue can loop
		bool			m_CanEnvelope;			// true if cue can volume envelope
		bool			m_HasDuration;			// true if cue has a duration
		bool			m_IsVisible;			// true if cue is visual
		bool			m_CanCrop;				// true if cue can visual crop
		
		bool			m_CanTransition;		// true if cue can transition
		bool			m_CanPath;				// true if cue can path
						
		bool			m_HasEditor;			// true if cue has internal editor		
		bool			m_EditorOpen;			// true if internal editor is open
			
		bool			m_IsSelected;			// true if it has been click selected
		bool			m_HasExternalEditor;	// true if use external editor
		
		BRect			m_LResizeZone;			// Left side resize zone
		BRect			m_RResizeZone;			// Right side resize zone				
		BRect 			m_DurationDeltaRect;	// Location of duration delta indicator
		BRect			m_EffectsTray;			// Area of cues effects tray	
		
		bool			m_AppCursorSet;
		uint32			m_InsertTime;	
		
		BFilePanel 		*m_Panel; 				// File panel for cue media loading
		
		TCueTimeText	*m_StartText;
		TCueTimeText	*m_EndText;
				
		// Cue Buttons
		TBitmapButton	*m_LockButton;
		TBitmapButton	*m_MuteButton;		
		TBitmapButton	*m_EffectsButton;
				
		int16			m_ID;					// 	Cue identification ID
		
		bool			m_MouseDown;			// 	For mouse down/up tracking
		bool			m_Resizing;				// 	For mouse dragging
							
		bool			m_IsLocked;				// 	Cue is locked into channel
		bool			m_IsExpanded;			//	Cue is expanded in channel	
		bool			m_IsMuted;				//	Cue data is muted or non-visible
		bool			m_EffectsVisible;		//	Effects tray is visible
		
		BList			*m_EffectsList;			//	List to hold effects applied to cue
		
		TBitmapView		*m_CueIcon;				// Visual display of cue		
		TCueChannel		*m_Channel;
							
		int32			m_MinCueWidth;			// Minimum width of a cue
					
		TTimer			*m_AntThread;
		
		entry_ref		m_FileRef;				// Cues data file entry_ref	
		BFile			*m_File;				// Cues file
		
		BPoint			m_LastClickPt;			// Mouse click tracking					
		
		bool			m_ShowDuration;			//	Show duration delta
		
		CueDisplay		m_CueDisplayMode;		//	Type of cue display in cue data area
		
		bool			m_TimeToQuit;
		bool			m_IsPlaying;
		bool			m_IsStopping;

		port_id 		m_Port;
		
		thread_id		m_ServiceThread;
		thread_id		m_RunThread;
		
		//pattern	cueAnts = {0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F};
		pattern			m_CueAnts;
};

#endif

