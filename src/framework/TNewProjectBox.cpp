//-------------------------------------------------------------------
//
//	File:	TNewProjectBox.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.18.98
//
//	Desc:	Cusrom BBox for drawing Video Preset info
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"

#include "TNewProjectBox.h"
#include "TPreset.h"

#include "TNewProject.h"

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

TNewProjectBox::TNewProjectBox(BMessage *data) : BBox(data)
{
	
}

//-------------------------------------------------------------------
//	Instantiate
//-------------------------------------------------------------------
//
//

BArchivable *TNewProjectBox::Instantiate(BMessage *data)
{
	if ( validate_instantiation(data, "TNewProjectBox") ) 
		return new TNewProjectBox(data); 
		
	return NULL; 
}


//-------------------------------------------------------------------
//	Draw
//-------------------------------------------------------------------
//
//

void TNewProjectBox::Draw(BRect bounds)
{
	// Inset rect to invalidate our text
	BRect drawRect = Bounds();
	drawRect.InsetBy(15, 15);
	BBox::Draw(drawRect);

	PushState();
	
	// Get TNewProject window	
	TNewProject *theWindow = static_cast<TNewProject *>(Window());
	
	if (theWindow)
	{
		// Get selected item
		int32 theItem = theWindow->GetSelectedItem();
		
		// Get preset list from parent Window
		BList *presetList = theWindow->GetPresetsList();
		if (presetList)
		{
			// Draw our text items
			SetHighColor(kBlack);	
			SetFont(be_plain_font);
						
			TPreset *preset =  static_cast<TPreset *>(presetList->ItemAt(theItem));
			if (preset)
			{
				// Description Header
				BPoint textPt(Bounds().left + 10, Bounds().top + 30 );
				SetFontSize(12);
				DrawString("Description:", textPt);

				// Descriptions
				SetFontSize(10);
				textPt.x += 10;
				textPt.y += 15;
				DrawString(preset->m_Description01, textPt);
				textPt.y += 15;
				DrawString(preset->m_Description02, textPt);
				textPt.y += 15;
				DrawString(preset->m_Description03, textPt);
				textPt.y += 15;
				DrawString(preset->m_Description04, textPt);
				textPt.y += 15;
				DrawString(preset->m_Description05, textPt);
				
				// Timebase
				SetFontSize(12);
				textPt.x -= 10;
				textPt.y += 25;
				DrawString("Timebase:", textPt);
				SetFontSize(10);
				textPt.x += 10;
				textPt.y += 15;
				//DrawString(preset->m_Timebase, textPt);
				DrawString("24fps", textPt);
				
				// Frame Size
				SetFontSize(12);
				textPt.x -= 10;
				textPt.y += 25;
				DrawString("Frame Size:", textPt);
				SetFontSize(10);
				textPt.x += 10;
				textPt.y += 15;
				char tmpStr[65];
				sprintf(tmpStr, "%d x %d", preset->m_FrameWidth, preset->m_FrameHeight);
				DrawString(tmpStr, textPt);
				
				// Video Compression
				SetFontSize(12);
				textPt.x -= 10;
				textPt.y += 25;
				DrawString("Video Compression:", textPt);
				SetFontSize(10);
				textPt.x += 10;
				textPt.y += 15;
				DrawString("Unknown", textPt);
				
				// Audio Compression
				SetFontSize(12);
				textPt.x -= 10;
				textPt.y += 25;
				DrawString("Audio Compression:", textPt);
				SetFontSize(10);
				textPt.x += 10;
				textPt.y += 15;
				DrawString("Unknown", textPt);

				//video_compressor_type	m_VideoCompressor;
				//audio_compressor_type	m_AudioCompressor;				
			}
		}
	} 
		
	PopState();
}
