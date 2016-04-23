//---------------------------------------------------------------------
//
//	File:	TTransitionMenu.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.16.98
//
//	Desc:	Creates and handles transition popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TTransitionMenu__
#define __TTransitionMenu__


// Menu Messages
#define TRAN_NONE_MSG							'trNO'
#define TRAN_WIPE_RIGHT_MSG						'trWR'
#define TRAN_WIPE_LEFT_MSG						'trWL'
#define TRAN_WIPE_DOWN_MSG						'trWD'

#define TRAN_WIPE_UP_MSG						'trWI'
#define TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG		'tTLB'
#define TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG		'tTLR'
#define TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG		'tLTR'
#define TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG		'tBTL'

#define TRAN_REVEAL_RIGHT_MSG					'tRVR'
#define TRAN_REVEAL_LEFT_MSG					'tRVL'
#define TRAN_REVEAL_DOWN_MSG					'tRVD'
#define TRAN_REVEAL_UP_MSG						'tRVU'
#define TRAN_REVEAL_TOP_LEFT_BOTTOM_RIGHT_MSG	'tRLB'
#define TRAN_REVEAL_TOP_RIGHT_BOTTON_LEFT_MSG	'tRLR'
#define TRAN_REVEAL_BOTTOM_LEFT_TOP_RIGHT_MSG	'tRTR'
#define TRAN_REVEAL_BOTTOM_RIGHT_TOP_LEFT_MSG	'tRTL'

#define	TRAN_CURTAINS_IN_MSG					'tCTI'
#define	TRAN_CURTAINS_OUT_MSG					'tCTO'

#define TRAN_DURATION							'tDUR'


// Class Definiton
class TTransitionMenu : public BPopUpMenu
{	
	public:
		// Member Functions
		TTransitionMenu(TCueView *target, bool transitionIn);
		~TTransitionMenu();
		
		BMenuItem *GetMenuItem(int16 menuID);
	
	private:
		BMenuItem *GetStraightWipesMenuItem(int16 menuID);	
		BMenuItem *GetDiagonalWipesMenuItem(int16 menuID);
		BMenuItem *GetRevealMenuItem(int16 menuID);
		BMenuItem *GetCurtainsMenuItem(int16 menuID);
	
	protected:	
	
};


// Menu Item Enumeration
// const 
enum transitionIDs
{
	kTransitionNone,
	kWipeRight,
	kWipeLeft,
	kWipeDown,
	kWipeTop,
	kWipeTopLeftBottomRight,
	kWipeTopRightBottomLeft,
	kWipeBottomLeftTopRight,
	kWipeBottomRightTopLeft,
	kRevealRight,
	kRevealLeft,
	kRevealDown,
	kRevealUp,
	kRevealTopLeftBottomRight,
	kRevealTopRightBottomLeft,
	kRevealBottomLeftTopRight,
	kRevealBottomRightTopLeft,
	kCurtainsIn,
	kCurtainsOut,
	kTotalTransitions			
};

#endif
