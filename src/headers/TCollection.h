//---------------------------------------------------------------------
//
//	File:	TCollection.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TCOLLECTION_H__
#define __TCOLLECTION_H__

class TCollection
{
	

	public:	
		TCollection();
		~TCollection();
						
		// Member Functions
		virtual int32		GetNumItems();	// Retrieve size of collection
		virtual bool		IsEmpty();		// Is the collection empty?
				
		// Member Variables 
		int32		m_NumItems;				// Number of items in collection


};

#endif
