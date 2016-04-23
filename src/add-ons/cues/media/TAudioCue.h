//---------------------------------------------------------------------
//
//	File:	TAudioCue.h
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

#ifndef __TAUDIOCUE_H__
#define __TAUDIOCUE_H__

//	Includes
#include <Sound.h>
#include <SoundPlayer.h>

#include "TAudioEngine.h"

// Forward Declarations
class TAudioEditor;

// Class Definition
class	TAudioCue : public TCueView
{
	public:
		// Member Functions
		TAudioCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TAudioCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime);
		TAudioCue(BMessage *theMessage);
		~TAudioCue();
		
		__declspec(dllexport) static	BArchivable *Instantiate(BMessage *data);
		__declspec(dllexport) status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	Draw(BRect updateRect);
		void 	MessageReceived(BMessage *message);
		
		void 	ResolutionChanged( int16 theFormat, int16 theResolution);
		
		// Playback Routines
		void 	PrerollCue(uint32 theTime);
		void 	PlayCue(uint32 theTime);
		void	StopCue(uint32 theTime);
		void 	PauseCue();
		void 	ResumeCue();

		void 	HandlePlayback(uint32 theTime);

		// Resizing Routines
		void	Contract(bool force = false);
		void 	Expand(bool force = false);
		
		// Accessor Functions
					
	private:
		// Member Functions
		void 	Init();
		bool 	LoadAudioFile(BMessage *message);
		void 	OpenEditor();
		
		void 	LoadCueIcon();
		
		int32 	GetDefaultLength();
		void 	CreatePreview(BView *previewView);
		void 	InitSoundfileAttributes();
		
	protected:		
		void 	ShowPanel();
		void	HidePanel();
		
		// Member Variables
		TAudioEngine			*m_AudioEngine;
		BSoundFile 				*m_SoundFile;
		BSound					*m_Sound;
		BSoundPlayer::play_id 	m_PlaybackID;
		TAudioEditor			*m_Editor;
		
		bool					m_UpdatePreview;
		BBitmap 				*m_PreviewBitmap;	
		
		int32			m_SamplesPerPixel;	
		int32			m_SampleRate;
		int32			m_NumSamples;		// Number of sample frames
		int16			m_NumChannels;
		int16			m_SampleSize;		// 8 or 16 bit	
};

#endif
