//---------------------------------------------------------------------
//
//	File:	TVisualCue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.24.98
//
//	Desc:	Header file
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVISUALCUE_H__
#define __TVISUALCUE_H__

// 	Includes
#include "TCueView.h"
//#include "TPolygon.h"
#include "TCuePosition.h"

//	Forward declarations
class TOpacityDialog;
class TScaleDialog;
class TRotateDialog;


// 	Class Definition
class TVisualCue : public TCueView
{
	public:
		// ABH missing opacity methods???
		BRect GetDrawArea();
		void SetTransparency(float);
		float GetTransparency();
		int GetDuration();
		BRect GetArea();
		void SetArea(BRect);
		BRect GetScaledArea();
		BRect GetCroppedArea();
		float GetRotation();
		void SetRotation(float);
		BPoint GetScale();
		void SetScale(BPoint);
		BPoint fScale;
		bool HasTransitionIn();
		bool HasTransitionOut();
		BHandler *GetCueTransitionInButton();
		BHandler *GetCueTransitionOutButton();

		// 	Member Functions				
		TVisualCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime, char *cueName);
		TVisualCue(entry_ref &theRef, int16 id, TCueChannel *parent, BRect bounds, uint32 startTime, char *cueName);
		TVisualCue(BMessage *theMessage);
		virtual ~TVisualCue();						
									
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;

		virtual void 	Draw(BRect updateRect);
		virtual void 	RenderData(uint32 theTime, BRect bounds);
		
		virtual void 	MessageReceived(BMessage *message);
				
		virtual void 	HidePanel();
						
		// Sizing Routines
		virtual void	Contract(bool force = false);
		virtual void	Expand(bool force = false);
		
		// Utility Functions
		virtual BBitmap	*GetBitmap(uint32 theTime = 0);
		bool 			IsOnStage();
		void 			UpdateStageCue();
				
		//	Inlines
		inline TCuePosition		*CuePosition(){ return fCuePosition; }
		inline sem_id			GetRenderSem(){ return fRenderSem; }				
		inline DisplayQuality	GetDisplayQuality(){ return fDisplayQuality; }
		
	protected:
		// Member Variables
		virtual void 	Init();
		
		virtual void	RenderBitmapData();
		void 		DrawSubTicks(int32 index);
								
		// 	Cue Contents
		virtual void	ShowContents();
		virtual void	HideContents();
		virtual void 	ShowControls();
		virtual void 	HideControls();
		virtual void 	SetDisplayMode(CueDisplay theMode);
		virtual void 	SetQualityMode(DisplayQuality theMode);

												
		// Member Variables
		// ABH
		bool fHasTransitionIn;
		bool fHasTransitionOut;
		
		TCuePosition	*fCuePosition;
			
		sem_id			fRenderSem;	//	Semaphore to handle access to bitmap
		BBitmap			*fBitmap;	// Cue visual data
		
		BBitmap			*fTransformBitmap;
		DisplayQuality	fDisplayQuality;				
};

#endif
