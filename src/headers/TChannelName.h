//-------------------------------------------------------------------
//
//	File:	TChannelName.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.06.98
//
//	Desc:	Dialog for the entry and renaming of cue channels
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCHANNELNAME_H__
#define __TCHANNELNAME_H__

// Forward Declarations
class TCueChannel;

// Class Definition
class TChannelName : public BWindow
{
	public:
		TChannelName(BMessage *theMessage, TCueChannel *theChannel);
		~TChannelName();
		
		virtual void		MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		virtual void			Init();		
		
		// Member Variables
		TCueChannel 	*m_Channel;
		BView 			*m_TextParent;
		BTextControl 	*m_TextView;
	
};

#endif
