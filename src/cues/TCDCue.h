//---------------------------------------------------------------------
//
//	File:	TCDCue.h
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

#ifndef _TCDCUE_H_
#define _TCDCUE_H_

// Forward Declarations
class TCueView;

// Class Definition
class	TCDCue : public TCueView
{
	public:
		// Member Functions
		TCDCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TCDCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TCDCue(BMessage *theMessage);
		~TCDCue();
		
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;

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







#endif	// _TCDCUE_H_
