//-------------------------------------------------------------------
//
//	File:	TNewProjectBox.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.18.98
//
//	Desc:	Cusrom BBox for drawing Video Preset info
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TNEWPROJECTBOX_H__
#define __TNEWPROJECTBOX_H__

// Class Definition

class TNewProjectBox : public BBox
{
	public:
		// Memebr Functions
		TNewProjectBox(BMessage *data);
		
		static	BArchivable *Instantiate(BMessage *data);
		
		void	Draw(BRect bounds);
		
	private:
	
	protected:


};

#endif
