//---------------------------------------------------------------------
//
//	File:	TTimeText.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	Header for TTimeText.cpp
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TTIMETEXT_H__
#define __TTIMETEXT_H__

// Enums
typedef enum
{
	kNoCell,
	kHoursCell,
	kMinutesCell,
	kSecondsCell,
	kFramesCell,
	kTotalCells
	
} TextEntryCells;


// Constants
const float kTimeFontSize = 26;

// Class Definition

class TTimeText : public BView
{
	public:
		// Member Functions
		TTimeText(BHandler *target, int32 messageID, BRect bounds, char *name, uint32 resizing);
		TTimeText(const TTimeText *theText);
		TTimeText(BMessage *data);
		~TTimeText();
				
		static BArchivable *Instantiate(BMessage *archive);
		status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	Draw(BRect inRect);
		
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(BPoint where, uint32 code, const BMessage *message);
		void 	KeyDown(const char *bytes, int32 numBytes);
		void 	MakeFocus(bool focusState = TRUE);
		void 	MessageReceived(BMessage *message);
		
		void 	SetText(char *theText);
		void 	SetFontAndColor(BFont *theFont, uint32 properties, rgb_color theColor);
		
		void 	LockText();
		void 	UnlockText();
		
		int32	GetTime();
		
		// Member Variables
		int32		fTime;
		BPoint		fDrawPt;
		
		// Accessor Funcitons
		inline void 	SetTarget(BHandler *newTarget){ fTarget = newTarget; }
		inline void 	GetDrawPoint(BPoint *thePoint){ *thePoint = fDrawPt; }
		inline void 	SetDrawPoint(BPoint *thePoint){ fDrawPt = *thePoint; }
				
	private:
		// Member Functions
		void	Init();
		
		// Member variables
		BBitmap		*fOffscreenBitmap;
		BView		*fOffscreenView;
			
		BHandler 	*fTarget;
		int32 		fMessageID;
		float		fTextSize;		// Default text point
		BFont		fFont;			// Default font
		rgb_color	fColor;		// Default color
		char		fText[12];		// Text to draw		
						
		BRect		fHoursRect;
		BRect		fMinutesRect;
		BRect		fSecondsRect;
		BRect		fFramesRect;
		int16		fCurrentCell;
		bool		fIsLocked;
		
		int16		fKeyCount;
	
	protected:
		bool 	IsBackspace(char theChar);
		bool 	IsArrows(char theChar);
		bool 	IsDelete(char theChar);
		bool 	IsTab(char theChar);
		
		void 	UpdateCurrentCell(char theChar);
		void 	CheckLastEdit();
		
		void 	InvertCurrentCell();
		void 	DecrementCell();
		void 	IncrementCell();
		
		void 	ConvertToTime(int16 theCell);
};

#endif
