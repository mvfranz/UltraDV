//---------------------------------------------------------------------
//
//	File:	TTransitionMenuLooper.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.03.98
//
//	Desc:	Cue transition routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TTransitionMenuLooper__
#define __TTransitionMenuLooper__

// Class Definition			  
class	TTransitionMenuLooper : public BLooper
{	
	public:
		TTransitionMenuLooper( TVisualCue *srcView);
		~TTransitionMenuLooper();
		
		void	MessageReceived(BMessage *message);
		
	private:
		TVisualCue	*m_CueView;
	
	protected:

};

#endif
