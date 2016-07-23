//---------------------------------------------------------------------
//
//	File:	TNumberTextControl.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.09.98
//
//	Desc:	Enhanced version of BTextControl that only allows the entry
//			of numbers.  Does not allow a paste, text drag, etc. that
//			contains non-numeric data.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TNUMBERTEXTCONTROL_H__
#define __TNUMBERTEXTCONTROL_H__

// Class Definition
class TNumberTextControl : public BTextControl 
{

	public:
		
		TNumberTextControl(BRect frame, const char *name, const char *label, const char *initial_text, BMessage *message);
		~TNumberTextControl();

		void MouseDown(BPoint where);
		void KeyDown(const char *bytes, int32 numBytes);
		//void MakeFocus(bool focusState = TRUE);
		//void MessageReceived(BMessage *message);
		
		bool AcceptsDrop(const BMessage *message);
		bool AcceptsPaste(BClipboard *clipboard);	
			
	private:
		// Member Functions
		void Init();
		
		// Member variables
		TNumberTextView	*fTextView;
		//float		fTextSize;		// Default text point
		//BFont		fFont;			// Default font
		//rgb_color	fColor;		// Default color
	
	protected:
		void InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs);		
		bool IsBackspace(char theChar);
		bool IsArrows(char theChar);
		bool IsDelete(char theChar);
		bool IsTab(char theChar);

};

#endif
