//---------------------------------------------------------------------
//
//	File:	TVideoEditorText.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.12.98
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TVideoEditorText__
#define __TVideoEditorText__

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
const int32 kTransportFontSize = 12;

// Class Definition

class TVideoEditorText : public BView
{
	public:
		// Member Functions
		TVideoEditorText(BHandler *target, int32 messageID, BRect bounds, char *name, uint32 resizing);
		TVideoEditorText(const TVideoEditorText *theText);
		TVideoEditorText(BMessage *data);
		~TVideoEditorText();
				
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
		int32		m_Time;
		BPoint		m_DrawPt;
		
		// Accessor Funcitons
		inline void 	SetTarget(BHandler *newTarget){ m_Target = newTarget; }
				
	private:
		// Member Functions
		void	Init();
		
		// Member variables
		BHandler 	*m_Target;
		int32 		m_MessageID;
		float		m_TextSize;		// Default text point
		BFont		m_Font;			// Default font
		rgb_color	m_Color;		// Default color
		char		m_Text[12];		// Text to draw				
		BRect		m_HoursRect;
		BRect		m_MinutesRect;
		BRect		m_SecondsRect;
		BRect		m_FramesRect;
		int16		m_CurrentCell;
		bool		m_IsLocked;		
		int16		m_KeyCount;
		
	
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
