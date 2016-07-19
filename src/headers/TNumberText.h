//---------------------------------------------------------------------
//
//	File:	TNumberTextView.h
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

#ifndef _TNUMBERTEXT_H_
#define _TNUMBERTEXT_H_

// Forward Declarations
class TCueView;

// Class Definition

class TNumberTextView : public BTextView
{
	public:
		// Member Functions
		TNumberTextView(BRect bounds, char *name, uint32 resizing);
		~TNumberTextView();
		
		void Draw(BRect inRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(BPoint where, uint32 code, const BMessage *message);
		void KeyDown(const char *bytes, int32 numBytes);
		void MakeFocus(bool focusState = TRUE);
		void MessageReceived(BMessage *message);
		
		bool AcceptsDrop(const BMessage *message);
		bool AcceptsPaste(BClipboard *clipboard);		
		
	private:
		// Member Functions
		
		// Member variables
		float		fTextSize;		// Default text point
		BFont		fFont;			// Default font
		rgb_color	fColor;		// Default color
	
	protected:
		void InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs);		
		void LockOutAlphabet();
		bool IsBackspace(char theChar);
		bool IsArrows(char theChar);
		bool IsDelete(char theChar);
		bool IsTab(char theChar);
};

#endif	// _TNUMBERTEXT_H_
