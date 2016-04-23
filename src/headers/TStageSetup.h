//-------------------------------------------------------------------
//
//	File:	TStageSetup.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.15.98
//
//	Desc:	Dialog for the setting of project defaults
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TSTAGESETUP_H__
#define __TSTAGESETUP_H__

// Messages
#define	CENTER_CHECK_MSG	'Cmsc'
#define	TITLE_CHECK_MSG		'Dstb'
#define	STAGE_COLOR_MSG		'StCm'

// Class Definition
class TStageSetup : public BWindow
{
	public:
		TStageSetup(TStageWindow *theStage, BMessage *theMessage);
		~TStageSetup();
		
		void	MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		void	Init();	
		void 	GetDialogSettings();
		
		// Member Variables
		TStageWindow	*m_Stage;
		
		rgb_color		m_SaveColor; 
		
		BBox			*m_DimesionsBox;
		BBox			*m_SettingsBox;
		BBox			*m_ColorBox;
		
		BTextControl	*m_Width;
		BTextControl	*m_Height;
		BTextControl	*m_XPos;
		BTextControl	*m_YPos;
		
		BTextControl	*m_TitleText;
		
		BCheckBox		*m_CenterCheck;	
		BCheckBox		*m_TitleCheck;	
		
		BColorControl	*m_StageColor;
	
				
};

#endif
