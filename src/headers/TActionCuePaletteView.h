//---------------------------------------------------------------------
//
//	File:	TActionCuePaletteView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	MediaCue sheet view
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TACTIONCUE_PALETTE_VIEW_H
#define __TACTIONCUE_PALETTE_VIEW_H


// Messages
#define ACTION_CUE_MSG 		'actc'
#define BUTTON_CUE_MSG 		'butc'
#define CD_CUE_MSG 			'cdac'
#define CONTROL_CUE_MSG 	'ctlc'
#define MARKER_CUE_MSG 		'mkrc'
#define PAUSE_CUE_MSG 		'psec'
#define VIDEO_CUE_MSG 		'vdoc'


// Class Declarations
class TActionCuePaletteView: public BView
{
	public:
		// Member functions
		TActionCuePaletteView(BRect bounds);
		~TActionCuePaletteView();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void WindowActivated(bool state);
		void KeyDown(const char *bytes, int32 numBytes);
		void KeyUp(const char *bytes, int32 numBytes);
		void FrameMoved(BPoint new_position);
		void FrameResized(float new_width, float new_height);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		
		// Member variables
		TCueButton	*fActionCueButton;
		TCueButton	*fButtonCueButton;
		TCueButton	*fCDCueButton;
		TCueButton	*fControlCueButton;
		TCueButton	*fMarkerCueButton;
		TCueButton	*fPauseCueButton;
		TCueButton	*fVideoCueButton;				

};		

#endif
