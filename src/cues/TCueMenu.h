//---------------------------------------------------------------------
//
//	File:	TCueMenu.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.23.98
//
//	Desc:	Creates and handles transition popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TCUEMENU_H__
#define __TCUEMENU_H__

// Menu Messages
#define CUE_LOCK_MSG			'cuLO'
#define CUE_HIDE_MSG			'cuHI'
#define CUE_DURATION_MSG		'cuDU'

#define CUE_CONTROLS_MSG		'cuCT'
#define CUE_CONTENTS_MSG		'cuCO'

// Class Definiton
class TCueMenu : public BPopUpMenu
{	
	public:
		// Member Functions
		TCueMenu(TCueView *target);
		~TCueMenu();
		
	
	private:
		//BMenuItem *GetWipesMenuItem(int16 menuID);	
	
	protected:	
	
};

#endif
