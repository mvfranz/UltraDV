//---------------------------------------------------------------------
//
//	File:	AVIWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVIWindow
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __AVIWINDOW_H__
#define __AVIWINDOW_H__

// ABH added this
#include "AVIView.h"
#include "TBitmapView.h"


//	Forward declarations
class TVideoCodec;
class TRIFFReader;
class TBitmapView;

//	Class Definition
class AVIWindow : public BWindow
{
	public:
		AVIWindow(BRect bounds, char *name);
		~AVIWindow();
		
		void 	HandleRef(entry_ref *theRef);
		
		//	Accessors
		TBitmapView *GetBitmapView(){ return m_BitmapView; }
		AVIView 	*GetAVIView(){ return m_AVIView; }
								
	private:
		TBitmapView *m_BitmapView;
		AVIView		*m_AVIView;
		
	protected:
		
};

#endif
