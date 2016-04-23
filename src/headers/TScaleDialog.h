//-------------------------------------------------------------------
//
//	File:	TScaleDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.21.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TScaleDialog__
#define __TScaleDialog__


// Message Definitions
#define	SCALE_ASPECT_MSG		'SCLa'
#define	SCALE_WIDTH_TEXT_MSG	'SCLw'
#define	SCALE_HEIGHT_TEXT_MSG	'SCLh'
#define	SCALE_CLOSE_MSG			'SCLl'

#define	SCALE_W_PIXELS_MSG		'SCpw'
#define	SCALE_W_INCHES_MSG		'SCiw'
#define	SCALE_W_PERCENT_MSG		'SCrw'

#define	SCALE_H_PIXELS_MSG		'SCph'
#define	SCALE_H_INCHES_MSG		'SCih'
#define	SCALE_H_PERCENT_MSG		'SCrh'

//	enums
typedef enum
{
	kUnitsPixels,
	kUnitsInches,
	kUnitsPercent
} UnitType;

// Class Definition
class TScaleDialog : public BWindow
{
	public:
		TScaleDialog(TVisualCue *theCue, BMessage *message);
		~TScaleDialog();
		
		virtual void		MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		void	Init();
		
		void	ConvertWidthToPixels();
		void	ConvertWidthToInches();
		void	ConvertWidthToPercent();
		void	ConvertHeightToPixels();
		void	ConvertHeightToInches();
		void	ConvertHeightToPercent();
		void	UpdateCue();
		
		float 	GetNewWidth();
		float 	GetNewHeight();
		
		// Member Variables		
		TVisualCue 			*m_Cue;
		BView				*m_BackView;
		
		TNumberTextControl 	*m_WidthText;
		TNumberTextControl 	*m_HeightText;
		
		BMenuField			*m_WidthMenu;
		BMenuField			*m_HeightMenu;
		
		BCheckBox			*m_AspectCheck;
		
		BPoint				m_SavedScale;
		
		bool				m_AspectRatio;
		
		BButton 			*m_OKButton;
		BButton 			*m_ApplyButton;
		BButton 			*m_CancelButton;
		
		UnitType			m_HeightUnitType;
		UnitType			m_WidthUnitType;
		
		float				m_AspectWidth;
		float				m_AspectHeight;	
};

#endif
