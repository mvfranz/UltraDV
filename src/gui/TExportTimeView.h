//---------------------------------------------------------------------
//
//	File:	TExportTimeView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.16.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TEXPORTTIMEVIEW_H__
#define __TEXPORTTIMEVIEW_H__ 

// Class Declarations
class TExportTimeView: public BView
{
	public:
		// Member functions
		TExportTimeView( TCueSheetWindow *parent, BRect bounds);
		TExportTimeView( BMessage *message);
		~TExportTimeView();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;
		
		void 	Draw(BRect updateRect);
		void	DrawInText();
		void	DrawOutText();
		
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
		
		void 	AttachedToWindow();
		
		//	Accessor Functions
		void 	SetParent(TCueSheetWindow *parent);

		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables
		TCueSheetWindow	*fCueSheetWindow;		
};		

#endif
