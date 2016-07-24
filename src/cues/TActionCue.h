//---------------------------------------------------------------------
//
//	File:	TActionCue.h
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

#ifndef _TACTIONCUE_H_
#define _TACTIONCUE_H_

// Forward Declarations
// ABH class TCueView;
#include "TCueView.h"

// Class Definition
class	TActionCue : public TCueView
{
	public:
		// Member Functions
		TActionCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TActionCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TActionCue(BMessage *theMessage);
		~TActionCue();
						
		__declspec(dllexport) static	BArchivable *Instantiate(BMessage *data);
		__declspec(dllexport) status_t 	Archive(BMessage *data, bool deep) const;
		
		void Draw(BRect updateRect);

	private:
		// Member Functions
		void 	Init();		
		void 	Init(BaseCueChunk *theChunk);
		void 	OpenEditor();
		
		void 	LoadCueIcon();

	protected:
				
		// Member Variables
			
};







#endif	// _TACTIONCUE_H_
