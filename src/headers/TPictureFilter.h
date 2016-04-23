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

#pragma once

// Class Definition
class TPictureFilter : public BRefFilter 
{
	public:
		TPictureFilter();
		
		bool	Filter(const entry_ref *, BNode *, struct stat *, const char *mimetype) = 0;

};