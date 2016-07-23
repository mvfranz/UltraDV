//-------------------------------------------------------------------
//
//	File:	TUndo.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.05.98
//
//-------------------------------------------------------------------

#ifndef __TUNDO_H__
#define __TUNDO_H__

// Class Declarations
class TUndo
{
	public:
		TUndo();
		virtual ~TUndo();
		
		virtual void 	CreateObjectSnapshot();
		virtual void 	SaveObjectSnapshot();
		virtual void	Undo();
		
	private:
	
	protected:
	
};

#endif
