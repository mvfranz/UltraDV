//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollBarV.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Vertical scroll bar to scroll CueSheet
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TCUESHEETSCROLLBARV_H__
#define __TCUESHEETSCROLLBARV_H__

// Class Definition
class TCueSheetScrollBarV : public BScrollBar 
{

	public:
	
		// Member Functions
		TCueSheetScrollBarV(TCueSheetWindow *parent, BRect frame, BView *target, float min, float max);
		TCueSheetScrollBarV(BMessage *data);
		~TCueSheetScrollBarV();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void	ValueChanged(float newValue);
		
		void 	AttachedToWindow();
		
		//	Accessor Functions
		void 	SetParent(TCueSheetWindow *parent);

	private:
	
		//Member Functions
		void Init();
		
		// Member Variables
		float			m_LastValue;
		
		TCueSheetWindow	*m_CueSheetWindow;

};

#endif
