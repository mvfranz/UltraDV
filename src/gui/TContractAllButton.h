//---------------------------------------------------------------------
//
//	File:	TContractAllButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCONTRACTALLBUTTON_H__
#define __TCONTRACTALLBUTTON_H__

// Class Declarations
class TContractAllButton: public BView
{
	public:
		// Member functions
		TContractAllButton( TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TContractAllButton(BMessage *data);
		~TContractAllButton();
		
		static BArchivable *Instantiate(BMessage *archive);
		status_t 	Archive(BMessage *data, bool deep) const;

		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);

		void 	AttachedToWindow();
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		void 	DoClick();
		
		// Member variables
		TToolbar	*fParent;
		BHandler	*fHandler;
		bool		fMouseDown;
		BBitmap		*fOffBitmap;
		BBitmap		*fOnBitmap;
		
};		

#endif
