//---------------------------------------------------------------------
//
//	File:	TMIDITrack.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.25.98
//
//	Desc:	Container for MIDI track data
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TMIDITRACK_H__
#define __TMIDITRACK_H__

#include "MuseumMidiDefs.h"
#include "TArray.h"


//
// Constants
//


#define AT_END			2147483647		// Index for InsertAtIndex to put at											// 	end of track.
const int16 END_OF_TRACK = -1;			// Generic end of track flag				


//
// Enumerations
//

// Possible values of extType ********/
enum 
{
	kNotExtended = 0,	// Not extended data
	kSysexType	= 1 	// MIDI Sysex data
};



//
// Structs
//


//
//	Extended info storage
//

typedef struct 
{
	int16			length;		// length of data
	unsigned char	*data;		// the data
} ExtData, *ExtDataPtr, **ExtDataBuffer;


// MIDI event storage
typedef struct 
{
	int32	time;				// Delta time of event
	union 
	{
		ExtDataBuffer	data;	// Handle to extended data
		unsigned char	bytes[4];
	} d;
	
	unsigned char	flags;
	unsigned char	extType;	// Type of extended data (0 if not extended)
} Event;

class TMIDICue;

// Class Definition
class	TMIDITrack : public BList
{

	public:	
		// Member Functions
		TMIDITrack(TMIDICue *theCue);
		~TMIDITrack();
			
		void	ExtractInfo();
		void	SetTrackName(char *name);
		void	GetTrackName(char name[256]);
		void	InsertExtended(int16 type, int32 time, ExtDataBuffer data);
		void	SetPosition(int32 where);
		void	Reset();
		
		// Member variables
		//DrumMapPtr	fDrumMaps;
		bool		fMuted;
		bool		fSoloed;
		bool		fDrumTrack;
		int16		fDevice;
		int16		fBank;
		int16		fProgram;
		int16		fChannel;
		int16		fOctave;
		int16		fVolume;
		int16		fPan;
		int16		fReverb;
		int16		fChorus;
		int16		fSrcPBRange;
		int16		fDstPBRange;
		int16		fSrcDrumMap;
		int16		fDstDrumMap;
		
		int16		fNoteOn;
		int16		fLastVol;
		
		
	protected:
		TMIDICue	*fCue;
		
		char		fTrackName[256];	
	
		// Playtime instance variables
		int32		fCurrentEvent;
		int32		fCurrentDelta;
	
		BList		*fExternalData;

	
		
};

#endif
