//---------------------------------------------------------------------
//
//	File:	TCueTimeText.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	This class impliments the start and end time fields
//			that are displayed in the cue rect.  The user can select
//			individual fields and modify the time therein.
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TCUETIMETEXT_H__
#define __TCUETIMETEXT_H__

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


typedef enum
{
	kTextBevel = 1,
	kTextOutline
	
} TextDrawModes;


// Class Definition
class TCueTimeText : public BView
{
	public:
		// Member Functions
		TCueTimeText(BHandler *target, int32 messageID, BRect bounds, const char *name, uint32 resizing, TextDrawModes drawMode);
		TCueTimeText(const TCueTimeText *theText);
		TCueTimeText(BMessage *data);
		~TCueTimeText();
				
		static BArchivable 	*Instantiate(BMessage *archive);
		virtual status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	AttachedToWindow();
		
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
		
		void 	CheckLastEdit();
		int32	GetTime();
		
		// Member Variables
		int32		fTime;
						
	private:
		// Member Functions
		void	Init();
		void 	DrawHighliteRect();
		void 	ClearHighliteRect();
		
		// Member variables
		BHandler 	*fTarget;
		int32 		fMessageID;
		float		fTextSize;		// Default text point
		BFont		fFont;			// Default font
		rgb_color	fColor;		// Default color
		char		fText[256];	// Text to draw		
		BRect		fHoursRect;
		BRect		fMinutesRect;
		BRect		fSecondsRect;
		BRect		fFramesRect;
		int16		fCurrentCell;
		bool		fIsLocked;
		int16		fDrawMode;
		
		int16		fKeyCount;
	
	protected:
		bool 	IsBackspace(char theChar);
		bool 	IsArrows(char theChar);
		bool 	IsDelete(char theChar);
		bool 	IsTab(char theChar);
		
		void 	UpdateCurrentCell(char theChar);		
		
		void 	InvertCurrentCell();
		void 	DecrementCell();
		void 	IncrementCell();
		
		void 	ConvertToTime(int16 theCell);
};

#endif
