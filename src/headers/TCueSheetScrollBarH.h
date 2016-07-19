//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Horizontal scroll bar to scroll CueSheet
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TCUESHEETSCROLLBARH_H__
#define __TCUESHEETSCROLLBARH_H__

// Class Definition
class TCueSheetScrollBarH : public BScrollBar 
{

	public:
	
		// Member Functions
		TCueSheetScrollBarH(TCueSheetWindow *parent, BRect frame, BView *target, float min, float max);
		TCueSheetScrollBarH( BMessage *data);
		~TCueSheetScrollBarH();
		
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
		float			fLastValue;
		
		TCueSheetWindow	*fCueSheetWindow;


};

#endif
