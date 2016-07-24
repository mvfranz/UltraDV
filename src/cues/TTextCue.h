//---------------------------------------------------------------------
//
//	File:	TTextCue.h
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

#ifndef __TEXTCUE_H__
#define __TEXTCUE_H__

// Class Definition
class	TTextCue : public TVisualCue
{
	public:
		// Member Functions
		TTextCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TTextCue(BMessage *theMessage);
		~TTextCue();
				
		static		__declspec(dllexport) BArchivable *Instantiate(BMessage *data);
		status_t 	__declspec(dllexport) Archive(BMessage *data, bool deep) const;
		
		void 	Draw(BRect updateRect);
		void 	RenderBitmapData();

	private:
		// Member Functions
		void 	Init();
		void 	OpenEditor();
		
		void 	LoadCueIcon();

	protected:
				
		// Member Variables
			
};

#endif
