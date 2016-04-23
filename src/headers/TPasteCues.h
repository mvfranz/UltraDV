//-------------------------------------------------------------------
//
//	File:	TPasteCues.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.07.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TPASTECUES_H__
#define __TPASTECUES_H__


// Message Definitions
#define	IN_CHANNEL_MSG		'INTt'

// Class Definition
class TPasteCues : public BWindow
{
	public:
		TPasteCues(TCueSheetView *cueSheet, BList *cueList, BMessage *message);
		~TPasteCues();
		
		virtual void		MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		virtual void			Init();		
		
		// Member Variables
		TCueSheetView		*m_CueSheet;
		BList				*m_CueList;		// Pointer to BList of cues to pasted
		BView				*m_BackView;
		TCueTimeText		*m_PasteTime;
		TNumberTextControl 	*m_ChannelText;
			
};

#endif
