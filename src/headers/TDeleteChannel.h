//-------------------------------------------------------------------
//
//	File:	TDeleteChannel.h
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

#ifndef __TDELETECHANNEL_H__
#define __TDELETECHANNEL_H__


// Message Definitions
#define	DELETE_THECHANNEL_MSG		'DDCt'
#define	DELETE_AFTER_CHANNEL_MSG	'DFCt'

// Class Definition
class TDeleteChannel : public BWindow
{
	public:
		TDeleteChannel(BRect bounds, TCueSheetView *theCueSheet);
		~TDeleteChannel();
		
		virtual void		MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		virtual void			Init();		
		
		// Member Variables
		TCueSheetView		*m_CueSheet;
		BView				*m_BackView;
		BStringView			*m_AddText;
		TNumberTextControl 	*m_DeleteChannelText;
		TNumberTextControl 	*m_AfterChannelText;
		
		BButton 			*m_OKButton;
		BButton 			*m_CancelButton;
	
};

#endif
