//---------------------------------------------------------------------
//
//	File:	TPreset.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.18.98
//
//	Desc:	Object that contains Project preset data.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TPRESET_H__
#define __TPRESET_H__

#include <TimeCode.h>

#include "AppTypes.h"

// Enumerations
//

// Video Compressors
enum video_compressor_type
{
	M_V_RAW_TYPE,	
	M_V_ANIMATION_TYPE,
	M_V_CINEPAK_TYPE,
	M_V_DV_TYPE,
	M_V_INDEO_TYPE,
	M_V_JPEG_TYPE,
	M_V_MOTION_JPEG_TYPE,
	M_V_MPEG_1_TYPE,
	M_V_MPEG_2_TYPE,
	M_V_SORENSON_TYPE
};


// Video Compressors
enum audio_compressor_type
{
	M_A_RAW_TYPE,	
	M_A_8_BIT_PCM_TYPE,
	M_A_16_BIT_PCM_TYPE,
	M_A_DPCM_TYPE,
	M_A_IMA_ADPCM_TYPE,
	M_A_ALAW_TYPR,
	M_A_MULAW_TYPE,			
	M_A_MPEG_TYPE
};

// Class Definition
class TPreset : public BArchivable
{
	public:
		TPreset(char *theName);
		TPreset(BMessage *data);
		~TPreset();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;
		
		// Utility Routines		
		void	SetName(char *theName);
		void	SetTimebase(timecode_type theTimebase);
		void	WriteToFile(char *theName);
	
		// Inlines
		inline char 			*GetName(){ return m_Name; }
		inline timecode_type		GetTimebase(){ return m_Timebase; }
		
		// Member Variables		
		char					m_Name[65];		
		char					m_Description01[40];
		char					m_Description02[40];
		char					m_Description03[40];
		char					m_Description04[40];
		char					m_Description05[40];		
		timecode_type			m_Timebase;
		video_compressor_type	m_VideoCompressor;
		audio_compressor_type	m_AudioCompressor;
		int32					m_FrameWidth;
		int32					m_FrameHeight;

	private:
				
	protected:
		
};

// Preset Chunk
const int32 kPresetChunkID = 'pres';

typedef struct
{
	void *ArchivedData;
} PresetChunk;

#endif
