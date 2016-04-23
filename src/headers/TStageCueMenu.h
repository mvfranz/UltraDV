//---------------------------------------------------------------------
//
//	File:	TStageCueMenu.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.02.98
//
//	Desc:	Creates and handles stage cue popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TSTAGECUEMENU_H__
#define __TSTAGECUEMENU_H__

// Menu Messages
#define STAGE_CUE_HIDE_MSG		'scHI'
#define STAGE_CUE_LOCK_MSG		'scLO'

#define STAGE_CUE_WIREFRAME_MSG	'scWF'
#define STAGE_CUE_PREVIEW_MSG	'scPR'
#define STAGE_CUE_BETTER_MSG	'scBT'
#define STAGE_CUE_BEST_MSG		'scBS'

#define STAGE_CUE_OPACITY_MSG	'scOP'
#define STAGE_CUE_ROTATE_MSG	'scRO'
#define STAGE_CUE_CROP_MSG		'scCR'
#define STAGE_CUE_SCALE_MSG		'scSC'
#define STAGE_CUE_SHEAR_MSG		'scSH'
#define STAGE_CUE_SKEW_MSG		'scPE'
#define STAGE_CUE_PERSP_MSG		'scPR'
#define STAGE_CUE_MIRROR_MSG	'scMR'

#define STAGE_CUE_COPY_MSG		'scCO'
#define STAGE_CUE_OVER_MSG		'scOV'
#define STAGE_CUE_ERASE_MSG		'scER'
#define STAGE_CUE_INVERT_MSG	'scIN'
#define STAGE_CUE_SELECT_MSG	'scSE'
#define STAGE_CUE_MIN_MSG		'scMI'
#define STAGE_CUE_MAX_MSG		'scMA'
#define STAGE_CUE_ADD_MSG		'scAD'
#define STAGE_CUE_SUBTRACT_MSG	'scSU'
#define STAGE_CUE_BLEND_MSG		'scBL'



// Class Definiton
class TStageCueMenu : public BPopUpMenu
{	
	public:
		// Member Functions
		TStageCueMenu(TVisualCue *target);
		~TStageCueMenu();
		
	
	private:
		//BMenuItem *GetWipesMenuItem(int16 menuID);	
		void	UpdateMenu();
	
	protected:	
		TCueView 	*m_Cue;
	
};

#endif
