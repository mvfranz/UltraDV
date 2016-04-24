//---------------------------------------------------------------------
//
//	File:	TMuseumFilter.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.25.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef _TPICTUREFILTER_H_
#define _TPICTUREFILTER_H_

// Class Definition
class TPictureFilter : public BRefFilter 
{
	public:
		TPictureFilter();
		
		bool	Filter(const entry_ref *, BNode *, struct stat *, const char *mimetype) = 0;

};

#endif	// _TPICTUREFILTER_H_
