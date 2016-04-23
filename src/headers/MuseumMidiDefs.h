//---------------------------------------------------------------------
//
//	File:	MuseumMidiDefs.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	Defines and structire for MIDI files
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __MUSEUMMIDIDEFS_H__
#define __MUSEUMMIDIDEFS_H__

// === MIDI Status Bytes ====

#define NOTE_OFF		0x80
#define	NOTE_ON			0x90
#define	KEY_PRESSURE	0xA0
#define	CTRL_CHANGE		0xB0
#define	PROG_CHANGE		0xC0
#define	AFTER_TOUCH		0xD0
#define	CHAN_PRESSURE 	0xD0
#define	PITCH_BEND		0xE0
#define	SYS_EXCL		0xF0
#define TIME_CODE		0xF1
#define	SONG_POINTER	0xF2
#define SONG_SELECT		0xF3
#define MTP_STATUS		0xF5
#define TUNE_REQ		0xF6
#define	END_SYS_EXCL	0xF7
#define	MIDI_CLOCK		0xF8
#define	MIDI_START		0xFA
#define	MIDI_CONT		0xFB
#define	MIDI_STOP		0xFC
#define ACT_SENSE		0xFE
#define	STATUS_MASK		0xF0


// === Meta Events ====
#define META_EVENT		0xFF
#define MF_TEXT			0x01
#define MF_COPYRIGHT	0x02
#define MF_TRK_NAME		0x03
#define MF_INST_NAME	0x04
#define MF_LYRIC		0x05
#define MF_MARKER		0x06
#define MF_CUE_POINT	0x07
#define MF_MIDI_CHAN	0x20
#define MF_END_TRK		0x2F
#define MF_TEMPO		0x51
#define MF_SMPTE		0x54
#define MF_METER		0x58
#define MF_KEY_SIG		0x59


// === Controller Numbers ====
enum 
{
	BANK_CHANGE_MSB	= 0x00,
	MOD_WHEEL 		= 0x01,
	VOLUME 			= 0x07,
	PAN				= 0x0A,
	BANK_CHANGE_LSB	= 0x20,
	REVERB			= 91,
	CHORUS			= 93
};


// === Chunk Types ====
const uint32	HEADER_CHUNK_TYPE 	=	'MThd';
const uint32 	TRACK_CHUNK_TYPE 	=	'MTrk';

// === MIDI File Data Structures ====
typedef struct 
{
	uint32	chunkType;
	int32			length;
} MidiChunkHeader;

typedef struct 
{
	int16		format;
	int16		numTracks;
	int16		division;
} HeaderChunk;


struct MIDIPacket 
{
	uint8	flags;
	uint8	len;
	int32	tStamp;
	uint8	data[249];
};

#endif
