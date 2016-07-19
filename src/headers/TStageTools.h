//---------------------------------------------------------------------
//
//	File:	TStageTools.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TSTAGETOOLS_H__
#define __TSTAGETOOLS_H__

// Constants
const int16 kStageToolWidth		= 27;
const int16 kStageToolHeight	= 27;

// Forward Declarations
class TStageToolsView;
class TStatusView;

// Class Definition
class TStageTools: public BWindow
{
	public:
		TStageTools();
		~TStageTools();
		
		void	MessageReceived(BMessage* message);	
		
		bool 	QuitRequested();
		
		//	Accessor Functions
		TStageToolsView		*GetToolsView(){ return fToolsView; }
		TStatusView			*GetStatusView(){ return fToolsStatusView; }
		
	private:
		// Member functions
		virtual void		Init();
		
		// Member variables
		TStageToolsView		*fToolsView;
		TStatusView			*fToolsStatusView;
		
};

#define MEDIA_PALETTE_TEXT_MSG	'MPtm'

#endif
