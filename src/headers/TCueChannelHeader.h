//---------------------------------------------------------------------
//
//	File:	TCueChannelHeader.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header that is part of each cue channel.  This contains
//			the mute and solo buttons, track text label and number.
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUECHANNELHEADER_H__
#define __TCUECHANNELHEADER_H__

// Class Definition
class TCueChannelHeader: public BView
{		
	public:
		TCueChannelHeader(BRect bounds, TCueChannel *channel);
		TCueChannelHeader( BMessage *data);
		~TCueChannelHeader();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);

		void 	MessageReceived(BMessage *message);
		
		void 	AttachedToWindow();
		
		// Utility Functions
		void 	UpdateChannelName();
		
		void 	SetChannel(TCueChannel *channel);
		
		// Accessor Functions
		inline 	TMuteButton *GetMuteButton(){ return m_MuteButton; }
		inline 	TSoloButton *GetSoloButton(){ return m_SoloButton; }
		inline 	TExpandButton *GetExpandButton(){ return m_ExpandButton; }
		inline 	TLockButton *GetLockButton(){ return m_LockButton; }
				
	private:
		
		// Member variables
		TCueChannel			*m_Channel;
		TChannelNameView	*m_Name;
		TSoloButton			*m_SoloButton;
		TMuteButton			*m_MuteButton;	
		TExpandButton		*m_ExpandButton;
		TLockButton			*m_LockButton;
		bool				m_IsHighlighted;
		
		// Member functions
		void 	Init();								
};

#endif
