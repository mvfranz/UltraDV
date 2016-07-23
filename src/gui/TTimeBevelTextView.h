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
		void MessageReceived(BMessage *message);
		
		// Accessor Functions
		inline TTimeTextView *TextView(){ return fTextView; }
		
	private:
		// Member Functions
		void Init();
		
		// Member variables
		TTimeTextView	*fTextView;
	
	protected:
	
};

#endif
