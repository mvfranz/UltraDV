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
	fParent = parent;

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
	fCancelButton = new BButton(cancelFrame, "Cancel", "Cancel", new BMessage(CANCEL_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	AddChild(fCancelButton);

	// Create the OK button
	fOKButton = new BButton(okFrame, "OK", "OK", new BMessage(OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	AddChild(fOKButton);
	fOKButton->MakeDefault(true);

	//	Extract the groups and controls form the VideoSource ParameterWeb
	fParameterWeb = fParent->ParameterWeb();

	if (fParameterWeb) {
		//	Video Input:  BDiscreteParameter, has four settings
		fVideoInputParam = fParameterWeb->ParameterAt(0);
		printf("Parameter::Name() %s\n", fVideoInputParam->Name());

		//	Audio Input:  BDiscreteParameter, has four settings
		fAudioInputParam = fParameterWeb->ParameterAt(1);
		printf("Parameter::Name() %s\n", fAudioInputParam->Name());

		//	Ignore channel
		//fChannelInput = fParameterWeb->ParameterAt(2);

		//	Brightness: BContinuousParameter
		fBrightnessParam = fParameterWeb->ParameterAt(3);
		printf("Parameter::Name() %s\n", fBrightnessParam->Name());

		//	Brightness: BContinuousParameter
		fContrastParam = fParameterWeb->ParameterAt(4);
		printf("Parameter::Name() %s\n", fContrastParam->Name());

		//	Brightness: BContinuousParameter
		fSaturationParam = fParameterWeb->ParameterAt(5);
		printf("Parameter::Name() %s\n", fSaturationParam->Name());

		//	Brightness: BContinuousParameter
		fHueParam = fParameterWeb->ParameterAt(6);
		printf("Parameter::Name() %s\n", fHueParam->Name());
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
		fParent->MessageReceived(theMessage);
		break;

	case msg_start_capture:
		//fVideoSource->StartCapture();
		break;

	case msg_stop_capture:
		//fVideoSource->StopCapture();
		break;

	case msg_hauppauge_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppaugePhilips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppaugeTemic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_400_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge400Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_400_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge400Temic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_401_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge401Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_401_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge401Temic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_405_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge405Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_405_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge405Temic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_406_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge406Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_406_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge406Temic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_418_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge418Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_hauppauge_452_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kHauppauge452Philips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_HAUPPAUGE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_avermedia_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kAvermediaPhilips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_AVERMEDIA);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_intel_svriii:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kIntelSVRIII;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_INTEL);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_turbotv_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kTurboTVPhilips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_IMS);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_turbotv_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kTurboTVTemic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_IMS);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_miro_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kMiroPhilips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_MIRO);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_miro_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kMiroTemic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_MIRO);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_diamond_philips:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kDiamondPhilips;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_DIAMOND);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_diamond_temic:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kDiamondTemic;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_DIAMOND);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case msg_osprey_100:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer = kOsprey100;
		//((Bt848Source *)fVideoSource)->SetCardBrand(BT848HW_OSPREY);
		//fVideoSource->VideoMux()->SetSource(fVideoSource->VideoMux()->Source());
		//fVideoSource->AudioMux()->SetSource(fVideoSource->AudioMux()->Source());
		break;

	case BRIGHTNESS_SLIDER_MSG:
	{
		/*GetParent()->fTempVideoSettings.fVideoImageSettings.fBrightnessValue = theMessage->FindInt32("be:value");
		   int32 value = theMessage->FindInt32("be:value");
		   int32 floatValue = (int32) *((float *)value);

		   bigtime_t    lastTime;
		   int32                oldValue;
		   size_t		size;
		   fBrightnessParam->GetValue(&oldValue, &size, &lastTime);
		   printf("Old Brightness: %d\n", oldValue);
		   fBrightnessParam->SetValue(&value, sizeof(int32), 0);
		   printf("New Brightness: %d\n", value);*/
	}
	break;

	case CONTRAST_SLIDER_MSG:
	{
		GetParent()->fTempVideoSettings.fVideoImageSettings.fContrastValue = theMessage->FindInt32("be:value");
		int32 value = theMessage->FindInt32("be:value");
		fContrastParam->SetValue(&value, sizeof(int32), 0);
	}
	break;

	case HUE_SLIDER_MSG:
	{
		GetParent()->fTempVideoSettings.fVideoImageSettings.fHueValue = theMessage->FindInt32("be:value");
		int32 value = theMessage->FindInt32("be:value");
		fHueParam->SetValue(&value, sizeof(int32), 0);
	}
	break;

	case SATURATION_SLIDER_MSG:
	{
		GetParent()->fTempVideoSettings.fVideoImageSettings.fSaturationValue = theMessage->FindInt32("be:value");
		int32 value = theMessage->FindInt32("be:value");
		fSaturationParam->SetValue(&value, sizeof(int32), 0);
	}
	break;

	case msg_gamma:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters.fGammaCorrection = cntlPtr->Value();
		//fVideoSource->VideoControls()->SetGammaCorrectionRemoval(cntlPtr->Value());
		break;

	case msg_coring:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters.fLumaCoring = cntlPtr->Value();
		//fVideoSource->VideoControls()->SetLumaCoring(cntlPtr->Value());
		break;

	case msg_luma_comb:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters.fLumaComb = cntlPtr->Value();
		//fVideoSource->VideoControls()->SetLumaCombFilter(cntlPtr->Value());
		break;

	case msg_chroma_comb:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters.fChromaComb = cntlPtr->Value();
		//fVideoSource->VideoControls()->SetChromaCombFilter(cntlPtr->Value());
		break;

	case msg_error_diffusion:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters.fErrorDiffusion = cntlPtr->Value();
		//fVideoSource->VideoControls()->SetErrorDiffusion(cntlPtr->Value());
		break;

	case msg_composite:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput = kComposite;
		//fVideoSource->VideoMux()->SetSource(B_COMPOSITE_0);
		//fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
		break;

	case msg_tuner:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput = kTuner;
		//fVideoSource->VideoMux()->SetSource(B_TUNER_0);
		//fVideoSource->AudioMux()->SetSource(B_TUNER_AUDIO_0);
		break;

	case msg_svideo:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput = kSVideo;
		//fVideoSource->VideoMux()->SetSource(B_SVIDEO_0);
		//fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
		break;

	case msg_color_bars:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput = kColorBars;
		//fVideoSource->VideoMux()->SetSource(B_COLOR_BARS);
		//fVideoSource->AudioMux()->SetSource(B_MUTE_AUDIO);
		break;

	case msg_camera:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput = kCamera;
		//fVideoSource->VideoMux()->SetSource(B_COMPOSITE_1);
		//fVideoSource->AudioMux()->SetSource(B_COMPOSITE_AUDIO_0);
		break;

	case msg_ntsc_m:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFormat = kNTSC_M;
		//fVideoSource->SetVideoFormat(B_NTSC_M);
		break;

	case msg_ntsc_j:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFormat = kNTSC_J;
		//fVideoSource->SetVideoFormat(B_NTSC_J);
		break;

	case msg_pal_bg:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFormat = kPAL_BG;
		//fVideoSource->SetVideoFormat(B_PAL_M);
		break;

	case msg_pal_i:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFormat = kPAL_I;
		//fVideoSource->SetVideoFormat(B_PAL_N);
		break;

	case msg_secam:
		GetParent()->fTempVideoSettings.fVideoSourceSettings.fFormat = kSECAM;
		//fVideoSource->SetVideoFormat(B_SECAM);
		break;

	case msg_usa_air:
		//fVideoSource->Tuner()->SetTunerLocale(B_US_NTSC_AIR);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_usa_irc:
		//fVideoSource->Tuner()->SetTunerLocale(B_US_NTSC_CABLE_IRC);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_usa_hrc:
		//fVideoSource->Tuner()->SetTunerLocale(B_US_NTSC_CABLE_HRC);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_europe_air:
		//fVideoSource->Tuner()->SetTunerLocale(B_EUROPE_PAL_AIR);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_europe_cable:
		//fVideoSource->Tuner()->SetTunerLocale(B_EUROPE_PAL_CABLE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_france_air:
		//fVideoSource->Tuner()->SetTunerLocale(B_FRANCE_SECAM_AIR);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_france_cable:
		//fVideoSource->Tuner()->SetTunerLocale(B_FRANCE_SECAM_CABLE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_gb_air:
		//fVideoSource->Tuner()->SetTunerLocale(B_GREAT_BRITAIN_PAL_AIR);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_gb_cable:
		//fVideoSource->Tuner()->SetTunerLocale(B_GREAT_BRITAIN_PAL_CABLE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_japan_air:
		//fVideoSource->Tuner()->SetTunerLocale(B_JAPAN_NTSC_AIR);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	case msg_japan_cable:
		//fVideoSource->Tuner()->SetTunerLocale(B_JAPAN_NTSC_CABLE);
		//fVideoSource->Tuner()->Tune(fCurrentChannel);
		break;

	default:
		BView::MessageReceived(theMessage);
		break;
	}
}
