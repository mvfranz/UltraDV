//-------------------------------------------------------------------
//
//	File:	TVideoCaptureToolbar.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.30.98
//
//
//	Copyright ©1998 mediapede Software
//
//-------------------------------------------------------------------

#ifndef __TVideoCaptureToolbar__
#define __TVideoCaptureToolbar__  

// Forward Declarations
class TBitmapButton;

// Class Definition
class TVideoCaptureToolbar: public BView
{
	public:
		TVideoCaptureToolbar(BRect bounds);
		~TVideoCaptureToolbar();
		void Draw(BRect updateRect);
		
	private:
		// Member Functions
		void Init();
		
		// Member Variables						
		TBitmapButton	*fRecordButton; 
};

#endif
