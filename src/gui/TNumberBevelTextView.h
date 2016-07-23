//---------------------------------------------------------------------
//
//	File:	TNumberBevelTextView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.24.98
//
//	Desc:	Header for TNumberBevelTextView.cpp
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------


#ifndef __TNumberBevelTextView__
#define __TNumberBevelTextView__

#include "TNumberTextView.h"

// Constants

// Class Definition

class TNumberBevelTextView : public BView
{
	public:
		// Member Functions
		TNumberBevelTextView(BRect bounds, char *name, uint32 resizing, BMessage *theMessage);
		~TNumberBevelTextView();
		
		void Draw(BRect inRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(BPoint where, uint32 code, const BMessage *message);
		void KeyDown(const char *bytes, int32 numBytes);
		void MakeFocus(bool focusState = TRUE);
		void MessageReceived(BMessage *message);
		
		// Accessor Functions
		inline TNumberTextView *TextView(){ return fTextView; }
		
	private:
		// Member Functions
		void Init();
		
		// Member variables
		TNumberTextView	*fTextView;
		BMessage		*fMessage;
	
	protected:
	
};

#endif
