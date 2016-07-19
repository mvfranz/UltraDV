//---------------------------------------------------------------------
//
//	File:	TMIDIConductor.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.25.98
//
//	Desc:	MIDI conductor class
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------



#ifndef __TMIDIConductor__
#define __TMIDIConductor__				

// Constants
#define kTempoDivisor	1000	// Tempo ratio is in 1/1000ths

// Enumerations

// Possible event types for conductor events 
enum 
{
	meterChange,
	tempoChange,
	endOfTrack
};


// Conductor event storgage
typedef struct 
{
	int32	time;					//time
	int16	type;					// Event type - meter, tempo or end of track
	
	union 
	{
		// Either time signature
		struct 
		{					
			int16	num;			//	numerator
			int16	den;			// 	denominator
		} meter;							
		int32	tempo;				// Or tempo (uSecs/tick)
	} d;
} ConductorEvent;


// Marker structure
typedef struct 
{
	int32		location;		// Absolute ticks into file
	int32		time;			// Absolute milliseconds into file
	char		text[33];		// We only store the first 32 characters
} MarkerRec, *MarkerPtr;


// Forward Declarations
class TMIDICue;

// Class Definitions
class TMIDIConductor : public BList
{

	public:	
		TMIDIConductor(TMIDICue *supervisor);
		~TMIDIConductor();
		
		// Member Functions
		
		// Insertion Methods
		void	InsertMarker(char *str, int32 time);
		void	InsertTempo(int32 tempo, int32 time);
		void	InsertMeter(int16 num, int16 den, int32 time);
	
		void	SetStartTime(int32 sTime, int32 time);	// sTime is SMPTE start time
		int32	StartTime();							// returns mSec start time
		void	SetEndTime(int32 eTime);				// eTime is Ticks from last event to end
														//	of score
													
		int32	GetStartDelta(int32 mSecLocation);
		void	SetPosition(int32 where);
																
		void	Reset();
		
		int32 	TicksToMSec(int32 location);
		int32	GetTotalTimeMSec();
		
		int16 	GetTempoRatio();
		void 	SetTempoRatio(int16 ratio);
		
		void 	CalcMarkerTimes();
						
		// Member Variables
		TMIDICue	*fParent;			// Parent cue
		BList		*fMarkerList;
		
	protected:

			
		// Member Variables
		int32	fCurrentEvent;		// Index to current play event
		int32	fCurrentDelta;		// How many ticks till our next play event
		int32	fCurrentLocation;	// Absolute location when loading file
		int32	fTotalTime;		// Total number of clocks in track
		int32	fPlayTime;			// Ticks remaining in play
		int32	fLastTempo;		// Last tempo change encountered when locating		
		int16	fTempoRatio;
};

#endif
