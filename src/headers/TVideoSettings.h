//---------------------------------------------------------------------
//
//	File:	TVideoSettings.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.14.98
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOSETTINGS_H__
#define __TVIDEOSETTINGS_H__

//
//	Video Settings Definitions
//

// VideoCompressor
typedef enum
{
	kNone,
	kAnimation,
	kBMP,
	kCinepak,
	kDV_NTSC,
	kDV_PAL,
	kGraphics,
	kH263,
	kIntelIndeoVideo,
	kIntelRaw,
	kMotionJPEGA,
	kMotionJPEGB,
	kMPEG,
	kPhotoJPEG,
	kPlanarRGB,
	kRaw,		
	kSorensonVideo,
	Video		
} VideoCompressor;

// VideoCompressionSettings
typedef struct
{
	int32	m_Compressor;

} VideoCompressionSettings;

// VideoImageSettings
typedef struct
{
	int32 m_HueValue;
	int32 m_SaturationValue;
	int32 m_BrightnessValue;
	int32 m_ContrastValue;				
} VideoImageSettings;


// VideoInput
typedef enum
{
	kHauppaugePhilips,
	kHauppaugeTemic,
	kHauppauge400Philips,
	kHauppauge400Temic,
	kHauppauge401Philips,
	kHauppauge401Temic,
	kHauppauge405Philips,
	kHauppauge405Temic,
	kHauppauge406Philips,
	kHauppauge406Temic,
	kHauppauge418Philips,
	kHauppauge452Philips,
	kAvermediaPhilips,
	kIntelSVRIII,
	kTurboTVPhilips,
	kTurboTVTemic,
	kMiroPhilips,
	kMiroTemic,
	kDiamondPhilips,
	kDiamondTemic,
	kOsprey100						
} VideoDigitizer;

// VideoSource
typedef enum
{
	kComposite,
	kTuner,
	kSVideo,
	kColorBars,
	kCamera							
} VideoInput;

// VideoFormat
typedef enum
{
	kNTSC_M,
	kNTSC_J,
	kPAL_BG,
	kPAL_I,
	kSECAM		
} VideoFormat;

// VideoFilters
typedef struct
{
	bool 	m_GammaCorrection;
	bool 	m_LumaCoring;
	bool	m_ErrorDiffusion;
	bool	m_LumaComb;
	bool	m_ChromaComb;
} VideoFilters;
	
// VideoSourceSettings
typedef struct
{
	VideoDigitizer	m_Digitizer;
	VideoInput		m_Input;
	VideoFormat		m_Format;
	VideoFilters	m_Filters;
}VideoSourceSettings;

// Video Settings Class
class TVideoSettings
{
	public:
		// Member Functions
		TVideoSettings();
		~TVideoSettings();
		
		// Member Variables	
		VideoCompressionSettings	m_VideoCompressionSettings;
		VideoImageSettings			m_VideoImageSettings;
		VideoSourceSettings			m_VideoSourceSettings;
	
	private:
		// Member Functions
		void Init();
	
	protected:
	
};

#endif
