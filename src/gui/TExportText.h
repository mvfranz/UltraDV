//---------------------------------------------------------------------
//
//	File:	TExportText.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.16.98
//
//	Desc:	Museum implimentation of BStringView
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TEXPORTTEXT_H__
#define __TEXPORTTEXT_H__

// Class Definition
class TExportText : public BStringView
{

	public:
		TExportText( TExportTimeView *parent, BRect bounds, const char *name, const char *text,
					 uint32 resizeFlags = B_FOLLOW_LEFT | B_FOLLOW_TOP,
					 uint32 flags = B_WILL_DRAW);				
		~TExportText();
		
		void		Draw(BRect bounds);

		void		MouseDown(BPoint pt);

	private:
	
		TExportTimeView	*fParent;
};

#endif
