//---------------------------------------------------------------------
//
//	File:	TVideoCue.h
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

#ifndef _TVIDEOCUE_H_
#define _TVIDEOCUE_H_

// Class Definition
class	TVideoCue : public TCueView
{
	public:
		// Member Functions
		TVideoCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TVideoCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TVideoCue(BMessage *theMessage);
		~TVideoCue();
						
		__declspec(dllexport) static	BArchivable *Instantiate(BMessage *data);
		__declspec(dllexport) status_t 	Archive(BMessage *data, bool deep) const;
		
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







#endif	// _TVIDEOCUE_H_
