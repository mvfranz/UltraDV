//---------------------------------------------------------------------
//
//	File:	TMuseumClipboard.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.05.98
//
//	Desc:	Application clipboard handling
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TMUSEUMCLIPBOARD_H__
#define __TMUSEUMCLIPBOARD_H__

class TMuseumClipboard : public BClipboard
{
	public:
		TMuseumClipboard();
		~TMuseumClipboard();
		
		int16	HasData();
		bool 	AddItem(BMessage *theItem);
		
		// Data Retrieval Functions
		BList *GetCueArchiveList();
				
	private:
		
			
	protected:
	

};

// Cliboard Data Types enumeration
typedef enum
{
	kCueListData = 1

} ClipboardDataTypes;

#endif
