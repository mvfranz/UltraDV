//---------------------------------------------------------------------
//
//	File:	TMarkerCue.h
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

#ifndef _TMARKERCUE_H_
#define _TMARKERCUE_H_

// Forward Declarations
class TCueView;

// Class Definition
class	TMarkerCue : public TCueView
{
	public:
		// Member Functions
		TMarkerCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TMarkerCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TMarkerCue(BMessage *theMessage);
		~TMarkerCue();
		
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;
		
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);		
		void MouseUp(BPoint where);		
		void MouseMoved( BPoint where, uint32 code, const BMessage *a_message );
		void WindowActivated(bool state);
		void KeyDown(const char *bytes, int32 numBytes);
		void KeyUp(const char *bytes, int32 numBytes);
		void MessageReceived(BMessage *message);
				
		// Member Variables
		
	private:
		// Member Functions
		void 	Init();
		void 	Init(BaseCueChunk *theChunk);
		void 	OpenEditor();
		
		void 	LoadCueIcon();

	protected:
				
		// Member Variables
			
};







#endif	// _TMARKERCUE_H_
