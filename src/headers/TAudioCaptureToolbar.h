//-------------------------------------------------------------------
//
//	File:	TAudioCaptureToolbar.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.08.98
//
//
//	Copyright ©1998 mediapede Software
//
//-------------------------------------------------------------------

#ifndef __TAUDIOCAPTURETOOLBAR_H__
#define __TAUDIOCAPTURETOOLBAR_H__ 

// Forward Declarations
class	TBitmapButton;

// Class Definition
class TAudioCaptureToolbar: public BView
{
	public:
		TAudioCaptureToolbar(BRect bounds);

		void Draw(BRect updateRect);
		
	private:
		// Member Functions
		void Init();
		
		// Member Variables						
		TBitmapButton	*m_RecordButton; 
};

#endif
