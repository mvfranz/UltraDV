//-------------------------------------------------------------------
//
//	File:	TAddChannel.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.06.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TADDCHANNEL_H__
#define __TADDCHANNEL_H__


// Message Definitions
#define	ADD_CHANNEL_MSG		'ADCt'
#define	AFTER_CHANNEL_MSG	'AFCt'

// Class Definition
class TAddChannel : public BWindow
{
	public:
		//TAddChannel(BMessage *theMessage, TCueSheetView *theCueSheet);
		TAddChannel(BRect bounds, TCueSheetView *theCueSheet);
		~TAddChannel();
		
		virtual void		MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		virtual void			Init();		
		
		// Member Variables
		TCueSheetView	*fCueSheet;		
		
		BView				*fBackView;
		BStringView			*fAddText;
		TNumberTextControl 	*fAddChannelText;
		TNumberTextControl 	*fAfterChannelText;
		
		BButton 			*fOKButton;
		BButton 			*fCancelButton;
	
};

#endif
