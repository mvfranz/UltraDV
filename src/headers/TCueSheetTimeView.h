//---------------------------------------------------------------------
//
//	File:	TCueSheetTimeView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUESHEETTIMEVIEW_H__
#define __TCUESHEETTIMEVIEW_H__

// Class Declarations
class TCueSheetTimeView: public BView
{
	public:
		// Member functions
		TCueSheetTimeView( TCueSheetWindow *parent, BRect bounds);
		TCueSheetTimeView( BMessage *message);
		~TCueSheetTimeView();
		
		static	BArchivable *Instantiate(BMessage *data);
		virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		
		void 	AttachedToWindow();
		
		//	Accessor Functions
		void 	SetParent(TCueSheetWindow *parent);
		
		// Member variables	
		bool	fProjectSettings;
		
	private:				
		// Member functions
		void 	Init();
		
		void 	ShowProjectSettingDialog(BPoint where);
				
		// Member variables
		TProjectSettings 	*fProjectWindow;
		TCueSheetWindow		*fCueSheetWindow;
};		

#endif
