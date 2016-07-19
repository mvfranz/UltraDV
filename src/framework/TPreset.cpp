//---------------------------------------------------------------------
//
//	File:	TPreset.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.18.98
//
//	Desc:	Object that contains Project preset data.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppTypes.h"
#include "ResourceManager.h"

#include "TPreset.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPreset::TPreset(char *theName)
{
	// Set up member variables
	strcpy(fName, theName);

}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from message or archive
//

TPreset::TPreset(BMessage *data)
{
	char    *tmpStr;
	int16 tmpInt16;

	data->FindString((const char *)"Name",(const char **) &tmpStr);
	strcpy(fName, tmpStr);

	data->FindString("Description01", (const char **)&tmpStr);
	strcpy(fDescription01, tmpStr);

	data->FindString((const char *)"Description02", (const char **)&tmpStr);
	strcpy(fDescription02, tmpStr);

	data->FindString((const char *)"Description03", (const char **)&tmpStr);
	strcpy(fDescription03, tmpStr);

	data->FindString((const char *)"Description04", (const char **)&tmpStr);
	strcpy(fDescription04, tmpStr);

	data->FindString((const char *)"Description05", (const char **)&tmpStr);
	strcpy(fDescription05, tmpStr);

	data->FindInt16((const char *)"TimeBase", &tmpInt16);
	fTimebase = (timecode_type)tmpInt16;

	data->FindInt16((const char *)"AudioCompressor", &tmpInt16);
	fAudioCompressor = (audio_compressor_type)tmpInt16;

	data->FindInt16((const char *)"VideoCompressor", &tmpInt16);
	fVideoCompressor = (video_compressor_type)tmpInt16;

	data->FindInt32("FrameWidth", &fFrameWidth);
	data->FindInt32("FrameHeight", &fFrameHeight);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPreset::~TPreset()
{

}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TPreset::Instantiate(BMessage *archive)
{

	if ( validate_instantiation(archive, "TPreset") )
		return new TPreset(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TPreset::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	// Start by calling inherited archive
	myErr = BArchivable::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TPreset");

		data->AddString("Name", fName);
		data->AddString("Description01", fDescription01);
		data->AddString("Description02", fDescription02);
		data->AddString("Description03", fDescription03);
		data->AddString("Description04", fDescription04);
		data->AddString("Description05", fDescription05);
		data->AddInt16("TimeBase", fTimebase);
		data->AddInt16("AudioCompressor", fAudioCompressor);
		data->AddInt16("VideoCompressor", fVideoCompressor);
		data->AddInt32("FrameWidth", fFrameWidth);
		data->AddInt32("FrameHeight", fFrameHeight);

		// Add attached data
		if (deep) {
		}
	}

	return myErr;
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	SetName
//---------------------------------------------------------------------
//
//	Set preset name
//

void TPreset::SetName(char *theName)
{
	strcpy(fName, theName);
}


//---------------------------------------------------------------------
//	SetTimebase
//---------------------------------------------------------------------
//
//	Set timeBase type
//

void TPreset::SetTimebase(timecode_type theTimebase)
{
	fTimebase = theTimebase;
}


//---------------------------------------------------------------------
//	WriteToFile
//---------------------------------------------------------------------
//
//	Basic dump to binary file
//

void TPreset::WriteToFile(char *theName)
{
	status_t retVal;

	// Archive our data
	BMessage *archive = new BMessage();
	retVal = Archive(archive, true);


	// Dump shared variables to file
	PresetChunk theChunk;
	theChunk.ArchivedData = archive;

	// Now write preset information out
	ChunkHeader theHeader;

	theHeader.chunkID       = kPresetChunkID;
	theHeader.chunkSize = archive->FlattenedSize();

	BFile theFile;

	// ABH Write file to disk in our apps "presets" directory
	// write file to disk in /boot/home/config/settings/UltraDV/settings

//	app_info appInfo;
//	be_app->GetAppInfo(&appInfo);
//	BEntry entry(&appInfo.ref);
	BDirectory parentDir;
	BEntry entry(kPresetsPathString);
	if (!entry.Exists()) {
		printf("TPresets: kPresets path does not exist, creating...\n");
		if (create_directory(kPresetsPathString,0770) != B_OK) {
			printf("TPreset: can't create kPresetsPathString - %s\n",
			       kPresetsPathString);
		}
	}
//	char fullPath[64];
//	strcpy(fullPath,kPresetsPathString);
//	strcpy(fullPath, "UltraDV");

	if (entry.SetTo(kPresetsPathString) != B_OK) {
		printf("TPreset: SetTo in entry failed\n");
	}
	if (entry.GetParent(&parentDir) != B_OK) {
		printf("TPreset: Getparent failed\n");
	}

	if ((retVal=parentDir.SetTo( kPresetsPathString)) != B_OK) {
		printf("TPreset: dir.SetTo failed\n");
		switch(retVal) {
		case B_OK:
			printf("B_OK\n");
			break;
		case B_ENTRY_NOT_FOUND:
			printf("not found\n");
			break;
		case B_BAD_VALUE:
			printf("bad value\n");
			break;
		case B_NO_MEMORY:
			printf("no mem\n");
			break;
		case B_FILE_ERROR:
			printf("file error\n");
			break;
		default:
			printf("Other\n");
		}
	}

	retVal = parentDir.CreateFile(theName, &theFile, false);

/*
        BEntry entry1(kPresetsPathString);
        if (!entry1.Exists){
                printf("TPresets: presets path does not exist\n");
        }
        BEntry entry2;
        BDirectory currentDir;
        entry_ref ref;
        entry1.GetRef(&ref);
        entry2.SetTo(&ref, true);
 */



	if (retVal == B_OK) {
		// Write out file
		theFile.Write(&theHeader, sizeof(theHeader));
		archive->Flatten(&theFile);

		// Set file type
		BNodeInfo info(&theFile);
		if ( info.InitCheck() == B_OK ) {
			retVal = info.SetType(kCueSheetTypeString);

			// Give it some nice icons
			BBitmap *smallIcon = GetMICNFromResource(kSettingsTypeString);
			ASSERT(smallIcon);
			info.SetIcon( smallIcon, B_MINI_ICON);

			BBitmap *largeIcon = GetICONFromResource(kSettingsTypeString);
			ASSERT(largeIcon);
			info.SetIcon( largeIcon, B_LARGE_ICON);

			delete smallIcon;
			delete largeIcon;
		}
	} else {
		printf("TPresets: CreateFile failed\n");
	}
}

