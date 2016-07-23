//---------------------------------------------------------------------
//
//	File:	TTimeTextView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.09.98
//
//	Desc:	Header for TTimeTextView.cpp
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TTimeTextView__
#define __TTimeTextView__

// Constants
const float kNumberFontSize = 10;

// Class Definition
class TTimeTextView : public BTextView
{
	public:
		// Member Functions
		TTimeTextView(BHandler *target, int32 messageID, BRect bounds, char *name, uint32 resizing);
		~TTimeTextView();
		
		virtual void Draw(BRect inRect);
		virtual void MouseDown(BPoint where);
		virtual void MouseUp(BPoint where);
		virtual void MouseMoved(BPoint where, uint32 code, const BMessage *message);
		virtual void KeyDown(const char *bytes, int32 numBytes);
		virtual void MakeFocus(bool focusState = TRUE);
		virtual void MessageReceived(BMessage *message);
		
		bool AcceptsDrop(const BMessage *message);
		bool AcceptsPaste(BClipboard *clipboard);		
		
		void CheckLastEdit();
		
		void LockText();
		void UnlockText();
		
		// Member Variables
		int32		fTime;
		
	private:
		
		// Member variables
		BHandler 	*fTarget;
		int32 		fMessageID;
		float		fTextSize;		// Default text point
		BFont		fFont;			// Default font
		rgb_color	fColor;		// Default color
		BRect		fHoursRect;
		BRect		fMinutesRect;
		BRect		fSecondsRect;
		BRect		fFramesRect;
		int16		fCurrentCell;
		bool		fIsLocked;
		
	
	protected:
		// Member Functions
		virtual void Init();
		
		void 	InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs);		
		bool 	IsBackspace(char theChar);
		bool 	IsArrows(char theChar);
		bool 	IsDelete(char theChar);
		bool 	IsTab(char theChar);
						
		void	ConvertToTime(int16 theCell);		
		void 	Reselect();
		
};

#endif
