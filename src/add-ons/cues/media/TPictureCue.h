//---------------------------------------------------------------------
//
//	File:	TPictureCue.h
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

#ifndef __TPICTURECUE_H__
#define __TPICTURECUE_H__

// 	Includes
#include "TVisualCue.h"
#include "TPictureCueEditor.h"

// 	Class Definition
class __declspec(dllexport) TPictureCue : public TVisualCue
{
	public:
		// 	Member Functions				
		TPictureCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TPictureCue(entry_ref &theRef, int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TPictureCue(BMessage *theMessage);
		~TPictureCue();						
									
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;

		void 	Draw(BRect updateRect);
		
		void 	MessageReceived(BMessage *message);
				
		void	ShowPanel();
		void 	HidePanel();
		
		// Playback Routines
		void 	PrerollCue(uint32 theTime);
		void 	PlayCue(uint32 theTime);
		void	StopCue(uint32 theTime);
		void 	PauseCue();
		void 	ResumeCue();
		void 	HandlePlayback(uint32 theTime);
		
		// Utility Functions
		BBitmap *GetBitmap(uint32 theTime = 0);
												
		// Member Variables
		
	private:
		
		// Member Functions
		void 	Init();
		
		void	RenderBitmapData();
		void 	CompositeBitmapToStage(BBitmap *srcBitmap);
		
		bool 	LoadPictureFile(BMessage *message);
		
		void 	OpenEditor();
		
		void 	LoadCueIcon();
		
		//	Accessor Functions
										
		// Member Variables
		TPictureCueEditor	*m_Editor;
		
	protected:
				
		// Member Variables
					
};

#endif
