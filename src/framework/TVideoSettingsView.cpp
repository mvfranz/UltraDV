//---------------------------------------------------------------------
//
//	File:	TVideoSettingsView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Background view of TVideoSettingsDialog
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppAlerts.h"

#include "TVideoSettingsDialog.h"

#include "TVideoSettingsView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoSettingsView::TVideoSettingsView(BRect bounds, TVideoSettingsDialog *parent) : BView(bounds, "VideoSettingsView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	m_Parent = parent;
	
	// Perform default intitialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoSettingsView::~TVideoSettingsView()
{	
	// Cleanup
	
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoSettingsView::Init()
{

	// Grey background
	SetViewColor(kBeGrey);
	
	// Create OK and Cancel Buttons
	//
	
	BRect bounds = Bounds();

	// Set up dimensions of our buttons
	const BRect cancelFrame( bounds.left + 30, bounds.bottom - 35, bounds.left + 90, bounds.bottom - 55);
	const BRect okFrame( bounds.right - 90, bounds.bottom - 35, bounds.right - 30, bounds.bottom - 55);
	
	// Create the Cancel button
	m_CancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	AddChild(m_CancelButton);
	
	// Create the OK button
	m_OKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);							 
	AddChild(m_OKButton);
	m_OKButton->MakeDefault(true);
	
	//	Extract the groups and controls form the VideoSource ParameterWeb
	m_ParameterWeb = m_Parent->ParameterWeb();
	
	if (m_ParameterWeb)
	{
		//	Video Input:  BDiscreteParameter, has four settings
		m_VideoInputParam = m_ParameterWeb->ParameterAt(0);
		printf("Parameter::Name() %s\n", m_VideoInputParam->Name());
		
		//	Audio Input:  BDiscreteParameter, has four settings
		m_AudioInputParam = m_ParameterWeb->ParameterAt(1);
		printf("Parameter::Name() %s\n", m_AudioInputParam->Name());
		
		//	Ignore channel
		//m_ChannelInput = m_ParameterWeb->ParameterAt(2);
		
		//	Brightness: BContinuousParameter
		m_BrightnessParam = m_ParameterWeb->ParameterAt(3);
		printf("Parameter::Name() %s\n", m_BrightnessParam->Name());
		
		//	Brightness: BContinuousParameter
		m_ContrastParam = m_ParameterWeb->ParameterAt(4);
		printf("Parameter::Name() %s\n", m_ContrastParam->Name());

		//	Brightness: BContinuousParameter
		m_SaturationParam = m_ParameterWeb->ParameterAt(5);
		printf("Parameter::Name() %s\n", m_SaturationParam->Name());

		//	Brightness: BContinuousParameter
		m_HueParam = m_ParameterWeb->ParameterAt(6);
		printf("Parameter::Name() %s\n", m_HueParam->Name());
	}
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVideoSettingsView::MessageReceived(BMessage *theMessage)
{
	
	// get pointer to control
	BControl *cntlPtr;
	theMessage->FindPointer("source", (void **)&cntlPtr);


	switch( theMessage->what)
	{					
		case CANCEL_MSG:
		case OK_MSG:
			m_Parent->MessageReceived(theMessage);
			break;
		
		case msg_start_capture:
			//m_VideoSource->StartCapture();
			break;
			
		case msg_stop_capture:
			//m_VideoSource->StopCapture();
			break;
			
		case msg_hauppauge_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppaugePhilips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppaugeTemic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_400_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge400Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_400_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge400Temic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_401_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge401Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_401_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge401Temic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_405_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge405Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_405_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge405Temic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_406_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge406Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_406_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge406Temic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_418_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge418Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_hauppauge_452_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kHauppauge452Philips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_avermedia_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kAvermediaPhilips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_AVERMEDIA);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_intel_svriii:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kIntelSVRIII;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_INTEL);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_turbotv_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kTurboTVPhilips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_IMS);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_turbotv_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kTurboTVTemic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_IMS);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_miro_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kMiroPhilips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_MIRO);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_miro_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kMiroTemic;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_MIRO);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_diamond_philips:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kDiamondPhilips;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_DIAMOND);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_diamond_temic:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kDiamondTemic;			
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_DIAMOND);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case msg_osprey_100:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer = kOsprey100;
			//((Bt848Source *)m_VideoSource)->SetCardBrand(BT848HW_OSPREY);
			//m_VideoSource->VideoMux()->SetSource(m_VideoSource->VideoMux()->Source());
			//m_VideoSource->AudioMux()->SetSource(m_VideoSource->AudioMux()->Source());
			break;
			
		case BRIGHTNESS_SLIDER_MSG:
			{
				/*GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_BrightnessValue = theMessage->FindInt32("be:value");
				int32 value = theMessage->FindInt32("be:value");				
				int32 floatValue = (int32) *((float *)value);
				
				bigtime_t 	lastTime;
				int32 	  	oldValue;
				size_t		size;
				m_BrightnessParam->GetValue(&oldValue, &size, &lastTime);
				printf("Old Brightness: %d\n", oldValue);
				m_BrightnessParam->SetValue(&value, sizeof(int32), 0);				
				printf("New Brightness: %d\n", value);*/
			}
			break;	
				
		case CONTRAST_SLIDER_MSG:
			{
				GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_ContrastValue = theMessage->FindInt32("be:value");
				int32 value = theMessage->FindInt32("be:value");
				m_ContrastParam->SetValue(&value, sizeof(int32), 0);
			}
			break;	
				
		case HUE_SLIDER_MSG:
			{
				GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_HueValue = theMessage->FindInt32("be:value");
				int32 value = theMessage->FindInt32("be:value");
				m_HueParam->SetValue(&value, sizeof(int32), 0);
			}
			break;	
				
		case SATURATION_SLIDER_MSG:
			{
				GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_SaturationValue = theMessage->FindInt32("be:value");
				int32 value = theMessage->FindInt32("be:value");
				m_SaturationParam->SetValue(&value, sizeof(int32), 0);
			}
			break;
	
		case msg_gamma:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters.m_GammaCorrection = cntlPtr->Value();
			//m_VideoSource->VideoControls()->SetGammaCorrectionRemoval(cntlPtr->Value());
			break;	
						
		case msg_coring:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters.m_LumaCoring = cntlPtr->Value();
			//m_VideoSource->VideoControls()->SetLumaCoring(cntlPtr->Value());
			break;	
						
		case msg_luma_comb:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters.m_LumaComb = cntlPtr->Value();
			//m_VideoSource->VideoControls()->SetLumaCombFilter(cntlPtr->Value());
			break;	
						
		case msg_chroma_comb:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters.m_ChromaComb = cntlPtr->Value();
			//m_VideoSource->VideoControls()->SetChromaCombFilter(cntlPtr->Value());
			break;	
						
		case msg_error_diffusion:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters.m_ErrorDiffusion = cntlPtr->Value();			
			//m_VideoSource->VideoControls()->SetErrorDiffusion(cntlPtr->Value());
			break;	
						
		case msg_composite:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input = kComposite;
			//m_VideoSource->VideoMux()->SetSource(B_COMPOSITE_0);
			//m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
			break;	
				
		case msg_tuner:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input = kTuner;
			//m_VideoSource->VideoMux()->SetSource(B_TUNER_0);
			//m_VideoSource->AudioMux()->SetSource(B_TUNER_AUDIO_0);
			break;
					
		case msg_svideo:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input = kSVideo;
			//m_VideoSource->VideoMux()->SetSource(B_SVIDEO_0);
			//m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
			break;
					
		case msg_color_bars:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input = kColorBars;
			//m_VideoSource->VideoMux()->SetSource(B_COLOR_BARS);
			//m_VideoSource->AudioMux()->SetSource(B_MUTE_AUDIO);
			break;	
				
		case msg_camera:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input = kCamera;
			//m_VideoSource->VideoMux()->SetSource(B_COMPOSITE_1);
			//m_VideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
			break;
					
		case msg_ntsc_m:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Format = kNTSC_M;
			//m_VideoSource->SetVideoFormat(B_NTSC_M);
			break;
					
		case msg_ntsc_j:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Format = kNTSC_J;
			//m_VideoSource->SetVideoFormat(B_NTSC_J);
			break;
					
		case msg_pal_bg:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Format = kPAL_BG;
			//m_VideoSource->SetVideoFormat(B_PAL_M);
			break;	
				
		case msg_pal_i:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Format = kPAL_I;
			//m_VideoSource->SetVideoFormat(B_PAL_N);			
			break;
					
		case msg_secam:
			GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Format = kSECAM;
			//m_VideoSource->SetVideoFormat(B_SECAM);
			break;
					
		case msg_usa_air:
			//m_VideoSource->Tuner()->SetTunerLocale(B_US_NTSC_AIR);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_usa_irc:
			//m_VideoSource->Tuner()->SetTunerLocale(B_US_NTSC_CABLE_IRC);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_usa_hrc:
			//m_VideoSource->Tuner()->SetTunerLocale(B_US_NTSC_CABLE_HRC);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_europe_air:
			//m_VideoSource->Tuner()->SetTunerLocale(B_EUROPE_PAL_AIR);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_europe_cable:
			//m_VideoSource->Tuner()->SetTunerLocale(B_EUROPE_PAL_CABLE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_france_air:
			//m_VideoSource->Tuner()->SetTunerLocale(B_FRANCE_SECAM_AIR);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_france_cable:
			//m_VideoSource->Tuner()->SetTunerLocale(B_FRANCE_SECAM_CABLE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_gb_air:
			//m_VideoSource->Tuner()->SetTunerLocale(B_GREAT_BRITAIN_PAL_AIR);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_gb_cable:
			//m_VideoSource->Tuner()->SetTunerLocale(B_GREAT_BRITAIN_PAL_CABLE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_japan_air:
			//m_VideoSource->Tuner()->SetTunerLocale(B_JAPAN_NTSC_AIR);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
			
		case msg_japan_cable:
			//m_VideoSource->Tuner()->SetTunerLocale(B_JAPAN_NTSC_CABLE);
			//m_VideoSource->Tuner()->Tune(m_CurrentChannel);
			break;
				
		default:
			BView::MessageReceived(theMessage);
			break;			
	}
}
