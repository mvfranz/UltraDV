//---------------------------------------------------------------------
//
//	File:	TControlCue.h
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

#ifndef _TCONTROLCUE_H_
#define _TCONTROLCUE_H_

// Forward Declarations
class TCueView;

// Class Definition
class	TControlCue : public TCueView
{
	public:
		// Member Functions
		TControlCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TControlCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TControlCue(BMessage *theMessage);
		~TControlCue();
		
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







#endif	// _TCONTROLCUE_H_
