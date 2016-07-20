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
		inline 	TMuteButton *GetMuteButton(){ return fMuteButton; }
		inline 	TSoloButton *GetSoloButton(){ return fSoloButton; }
		inline 	TExpandButton *GetExpandButton(){ return fExpandButton; }
		inline 	TLockButton *GetLockButton(){ return fLockButton; }
				
	private:
		
		// Member variables
		TCueChannel			*fChannel;
		TChannelNameView	*fName;
		TSoloButton			*fSoloButton;
		TMuteButton			*fMuteButton;	
		TExpandButton		*fExpandButton;
		TLockButton			*fLockButton;
		bool				fIsHighlighted;
		
		// Member functions
		void 	Init();								
};

#endif
