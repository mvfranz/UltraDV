//---------------------------------------------------------------------
//
//	File:	TCueTransitionButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.14.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCueTransitionButton__
#define __TCueTransitionButton__

#include "TChannelCueButton.h"

// Class Declarations
class TCueTransitionButton: public TChannelCueButton
{
	public:
		// Member functions
		TCueTransitionButton( TCueView *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler, uint32 flags, bool transitionIn);
		TCueTransitionButton(const TCueTransitionButton *theButton);
		TCueTransitionButton(BMessage *data);
		~TCueTransitionButton();
		
		static BArchivable *Instantiate(BMessage *archive);
		status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	AttachedToWindow();
		
		void MouseDown(BPoint where);
		
		void MessageReceived(BMessage *message);
		
		// Member variables
		
		// Accessor Functions
		inline void SetParent(TCueView *theCue){ m_Cue = theCue;   }	
		
	private:				
		// Member functions
		void 	DoClick();
		
		// Member variables
		bool	m_TransitionIn;
				
};		

#endif
