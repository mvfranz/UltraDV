//---------------------------------------------------------------------
//
//	File:	TTimeBevelTextView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.24.98
//
//	Desc:	Header for TTimeBevelTextView.cpp
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TTimeBevelTextView__
#define __TTimeBevelTextView__

// Constants

// Class Definition
class TTimeBevelTextView : public BView
{
	public:
		// Member Functions
		TTimeBevelTextView(BRect bounds, char *name, uint32 resizing);
		~TTimeBevelTextView();
		
		void Draw(BRect inRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(BPoint where, uint32 code, const BMessage *message);
		void KeyDown(const char *bytes, int32 numBytes);
		void MakeFocus(bool focusState = TRUE);
		void MessageReceived(BMessage *message);
		
		// Accessor Functions
		inline TTimeTextView *TextView(){ return m_TextView; }
		
	private:
		// Member Functions
		void Init();
		
		// Member variables
		TTimeTextView	*m_TextView;
	
	protected:
	
};

#endif
