//---------------------------------------------------------------------
//
//	File:	TVideoSettingsView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.2.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOSETTINGSVIEW_H__
#define __TVIDEOSETTINGSVIEW_H__

// Forward Declarations
class TVideoSettingsDialog;
class BVideoSource;
class BVideoImage;

// Class Declarations
class TVideoSettingsView: public BView
{
	public:
		// Member functions
		TVideoSettingsView(BRect bounds, TVideoSettingsDialog *parent);
		~TVideoSettingsView();
		
		void	MessageReceived(BMessage *theMessage);		
				
		// Video Capture
		void 	Start();
		void 	Stop();
		
		// Member variables	
		
		// Inlines
		TVideoSettingsDialog 	*GetParent(){ return fParent; }		
		//BVideoSource 			*GetVideoSource(){ return fVideoSource; }
		
	private:				
		// Member functions
		void 	Init();
		
		// Video Capture
		void 	Shutdown();
				
		// Member variables
		TVideoSettingsDialog 	*fParent;		
		BButton					*fCancelButton;
		BButton					*fOKButton;
		
		BParameterWeb 			*fParameterWeb;
		
		BParameter				*fVideoInputParam;
		BParameter				*fAudioInputParam;
		BParameter				*fBrightnessParam;
		BParameter				*fContrastParam;
		BParameter				*fSaturationParam;
		BParameter				*fHueParam;
		
		
};		



// Messages
#define HUE_SLIDER_MSG  		'hSmg'
#define SATURATION_SLIDER_MSG  	'sSmg'
#define BRIGHTNESS_SLIDER_MSG  	'bSmg'
#define CONTRAST_SLIDER_MSG  	'cSmg'

enum 
{
	msg_start_capture 	= '_str',
	msg_stop_capture 	= '_stp',
	msg_quit 			= '_qut',
	
	msg_hauppauge_philips 		= '_hp ',
	msg_hauppauge_temic 		= '_ht ',
	msg_hauppauge_400_philips 	= '_hp0',
	msg_hauppauge_400_temic 	= '_ht0',
	msg_hauppauge_401_philips 	= '_hp1',
	msg_hauppauge_401_temic 	= '_ht1',
	msg_hauppauge_405_philips 	= '_hp5',
	msg_hauppauge_405_temic 	= '_ht5',
	msg_hauppauge_406_philips	= '_hp6',
	msg_hauppauge_406_temic 	= '_ht6',
	msg_hauppauge_418_philips 	= '_hp8',
	msg_hauppauge_452_philips 	= '_hp2',
	msg_avermedia_philips 		= '_ap0',
	msg_intel_svriii 			= '_svr',
	msg_turbotv_philips 		= '_ttp',
	msg_turbotv_temic 			= '_ttt',
	msg_miro_philips 			= '_mp0',
	msg_miro_temic 				= '_mt0',
	msg_diamond_philips 		= '_dp0',
	msg_diamond_temic 			= '_dt0',
	msg_osprey_100 				= '_osp',
		
	msg_gamma 			= '_gam',
	msg_coring 			= '_cor',
	msg_luma_comb 		= '_lcm',
	msg_chroma_comb 	= '_ccm',
	msg_error_diffusion = '_erd',
	
	msg_composite 		= '_cmp',
	msg_tuner 			= '_tun',
	msg_svideo			= '_svd',
	msg_color_bars 		= '_cbr',
	msg_camera 			= '_cam',
	
	msg_ntsc_m 		= '_ntm',
	msg_ntsc_j 		= '_ntj',
	msg_pal_bg 		= '_plb',
	msg_pal_i 		= '_pli',
	msg_secam 		= '_scm',
	
	msg_usa_air 		= '_usa',
	msg_usa_irc 		= '_irc',
	msg_usa_hrc 		= '_hrc',
	msg_japan_air 		= '_jpa',
	msg_japan_cable 	= '_jpc',
	msg_europe_air 		= '_eua',
	msg_europe_cable 	= '_euc',
	msg_france_air 		= '_fra',
	msg_france_cable 	= '_frc',
	msg_gb_air 			= '_gba',
	msg_gb_cable 		= '_gbc',
	
	msg_channel 		= '_chn',
	
	msg_scanup 			= '_scu',
	msg_scandn 			= '_scd'
};

#endif
