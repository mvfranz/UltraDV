//---------------------------------------------------------------------
//
//	File:	TPictureCueEditorView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.26.98
//
//	Desc:	Picture Editor View	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TPictureCueEditorView__
#define __TPictureCueEditorView__


// Forward Declarations
class BBitmap;


class TPictureCueEditorView : public BView
{
	public:
		TPictureCueEditorView( BBitmap *map);

		void	Draw( BRect area);

	protected:

		BBitmap *fBitmap;
	
};

#endif
