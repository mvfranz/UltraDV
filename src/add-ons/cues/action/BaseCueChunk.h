// Attempt to recreate missing include file

#ifndef __BASECUECHUNK_H__
#define __BASECUECHUNK_H__

#include <SupportDefs.h>

// Need to get Cue id and startTime

class BaseCueChunk
{
	public:
		// Member functions
		BaseCueChunk();
		~BaseCueChunk();


		// Member Variables
		int16 CueID;
		float startTime;

	private:
		// Member functions
};


#endif

