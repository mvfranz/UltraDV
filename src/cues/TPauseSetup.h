//
// TPauseSetup.h
//
// Recreate missing include file
// ABH 12/17/2012
//

#ifndef __TPAUSESETUP__
#define __TPAUSESETUP__

#define PAUSE_SETUP_MSG 'pdMG'

#include <Window.h>

//#include "AppConstants.h"
//#include "AppMessages.h"

//#include "TPlaybackList.h"
//#include "THeaderContainerView.h"
//#include "TPlaybackEngine.h"
//#include "TCueSheetWindow.h"
//#include "TCueSheetView.h"
//#include "TCueChannel.h"
//#include "TPauseCue.h"

//class TPauseCue;


class TPauseSetup : public BWindow {
	public:
		TPauseSetup(BWindow *parent, BMessage *data);
		~TPauseSetup();
		
		void MessageReceived(BMessage* theMessage);
		
	private:
		void Init();
		
		TPauseSetup *fEditor;

//		Show();
//		Activate();

	
};






#endif

