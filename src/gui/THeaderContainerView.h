//---------------------------------------------------------------------
//
//	File:	THeaderContainerView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.03.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __THEADERCONTAINERVIEW_H__
#define __THEADERCONTAINERVIEW_H__ 

// Class Declarations
class THeaderContainerView: public BView
{
	public:
		// Member functions
		THeaderContainerView(BRect bounds);
		THeaderContainerView( BMessage *message);
		~THeaderContainerView();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;
		
		void Draw(BRect updateRect);
		
		// Member variables	

		
	private:				
		// Member functions
		void 	Init();
		
};		

#endif
