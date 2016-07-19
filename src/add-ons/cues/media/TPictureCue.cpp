//---------------------------------------------------------------------
//
//	File:	TPictureCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Picture Cue
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppAlerts.h"
#include "AppMessages.h"
#include "DebugUtils.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TCueChannel.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueMenu.h"
#include "TStageCueMenu.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TTimelineView.h"
#include "TBrowserWindow.h"
#include "TRefFilter.h"
#include "TTransition.h"
#include "TCompositor.h"

#include "TCueEffectView.h"
#include "TVisualEffect.h"

//	Dialogs
#include "TOpacityDialog.h"
#include "TScaleDialog.h"
#include "TRotateDialog.h"

#include "TPictureCue.h"

#pragma mark -
#pragma mark === Constructors ===

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TPictureCue::TPictureCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) :
	TVisualCue(id, parent, bounds, startTime, "PictureCue")
{
	// Load picture file
	ShowPanel();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TPictureCue::TPictureCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime) :
	TVisualCue(theRef, id, parent, bounds, startTime, "PictureCue")
{
	// Init member variables
	fEditor                 = NULL;
	fBitmap                         = NULL;
	fTransformBitmap        = NULL;

	//
	// Attempt to load data file
	//

	BNode theNode(&theRef);
	if (theNode.InitCheck() != B_OK)
		return;

	BNodeInfo nodeInfo(&theNode);
	if (nodeInfo.InitCheck() != B_OK)
		return;

	// First, make sure we have a valid ref
	if ( IsImage(nodeInfo) ) {
		//      Create a BMessage that includes the entry_ref to send to our open routine
		BMessage *theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);

		bool retVal = LoadPictureFile(theMessage);

		// We are succesful.  Init the cue
		if (retVal) {
			Init();
		}
		// If we have an error, ask the user to locate a new data file
		else{
			ShowPanel();
		}
	}
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TPictureCue::TPictureCue(BMessage *theMessage) : TVisualCue(theMessage)
{
	// Load cue icon
	LoadCueIcon();

	// Init member varibles
	fEditor                         = NULL;
	fBitmap                         = NULL;
	fTransformBitmap        = NULL;

	// Find our data in the archive

	//
	//	Load in bitmap

	//      Create a BMessage that includes the entry_ref to send to our open routine
	BMessage *fileMessage = new BMessage(B_REFS_RECEIVED);
	theMessage->AddRef("refs", &fFileRef);

	bool retVal = LoadPictureFile(fileMessage);
	/*
	   // We are succesful.  Init the cue
	   if (retVal)
	   {
	        Init();
	   }
	   // If we have an error, ask the user to locate a new data file
	   else
	   {
	        ShowPanel();
	   }
	 */
	//	Clean up
	delete fileMessage;

	// We are now fully instantiated
	fIsInstantiated = true;

}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPictureCue::~TPictureCue()
{
	// Close editor
	if ( fEditorOpen && fEditor) {
		fEditor->Hide();
		fEditor->Lock();
		fEditor->Quit();
	}

	// Delete image file reference
	if (fFile) {
		delete  fFile;
		fFile = NULL;
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TPictureCue::Init()
{
	// Default initialization
	TVisualCue::Init();

	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertTime, true)) {
		fChannel->InsertCue(this, fInsertTime);
		Select();

		// We are now fully instantiated
		fIsInstantiated = true;
	}

	// Set expanded status
	if (fChannel->IsExpanded()) {
		fIsExpanded = false;
		Expand();
	} else   {
		fIsExpanded = true;
		Contract();
	}

}

#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TPictureCue::Instantiate(BMessage *archive)
{
	if ( validate_instantiation(archive, "TPictureCue") )
		return new TPictureCue(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TPictureCue::Archive(BMessage *data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TVisualCue::Archive(data, deep);

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TPictureCue");
	}

	Looper()->Unlock();

	return myErr;
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TPictureCue::Draw(BRect updateRect)
{
	// Return if were not done cooking...
	if (!fIsInstantiated)
		return;

	// Pass up to parent
	TVisualCue::Draw(updateRect);

}

//---------------------------------------------------------------------
//	RenderBitmapData
//---------------------------------------------------------------------
//
//	Render internal bitmap modifications
//

void TPictureCue::RenderBitmapData()
{

	//	Handle effects stack
	if (fEffectsList->CountItems() > 0) {
		//	If we are here, the effect must be a VisualEffect
		for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
			//	Get the effects in the list
			TCueEffectView *effectView = (TCueEffectView *)fEffectsList->ItemAt(index);
			if (effectView) {
				TVisualEffect *effect = (TVisualEffect *)effectView->Effect();

				//	Setup transformation bitmap buffer
				if (fTransformBitmap) {
					delete fTransformBitmap;
					fTransformBitmap = NULL;
				}

				//	Get current time and convert to cue local time
				uint32 theTime = GetCurrentTime();
				theTime -= fStartTime;

				//	Is it in the effects time span?
				if ( (theTime >= effect->StartTime()) && (theTime <= effect->EndTime()) )
					fTransformBitmap = effect->TransformBitmap(theTime, fBitmap, *fCuePosition, kBestQuality);
			}
		}
	}

	//	Perform operation in this order:
	//		-- Rotate
	//		-- Scale
	//		-- Transform
	//
	//	This will maintain the best image quality
	//

	//	Setup transformation bitmap buffer
	//if (fTransformBitmap)
	//{
	//	delete fTransformBitmap;
	//	fTransformBitmap = NULL;
	//}

	//	gzr: to do... The right way to do this is rotate, scale, translate

	//	Scale next...
	//DoScale();

	//	Start by rotating
	//DoRotation();

	//	Mirror next..
	//DoMirror();


	//
	//	Composite to stage
	//

	//	Handle bitmap created by earlier transformations
	//if (fTransformBitmap)
	//{
	//	CompositeBitmapToStage(fTransformBitmap);
	//}
	//	Handle basic source bitmap
	//else
	//{
	//	CompositeBitmapToStage(fBitmap);
	//}
}

//---------------------------------------------------------------------
//	CompositeBitmapToStage
//---------------------------------------------------------------------
//
//	Create bitmap to be composited onto the stage.  This routine
//	handles transparency, opacity, etc.  It is really complicated and
//	be simplified sometime.
//

void TPictureCue::CompositeBitmapToStage(BBitmap *srcBitmap)
{
	/*// Get stage offscreen for analysis
	   BBitmap *offscreen = static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage()->GetStageView()->GetOffscreenBitmap();

	   // Determine up offscreen bounds
	   const BRect offBounds = offscreen->Bounds();

	   //	Determine visible bitmap bounds
	   BRect        drawRect, croppedArea, scaledArea;
	   BPoint	drawPt;

	   drawRect     = GetDrawArea();
	   croppedArea	= GetCroppedArea();
	   scaledArea   = GetScaledArea();
	   drawPt		= GetDrawPoint();

	   //	Get draw polygon
	   TPolygon drawPoly;
	   GetTransformedDrawPolygon(&drawPoly);

	   //	Get source bitmap bounds, width and height
	   BRect srcBounds        = srcBitmap->Bounds();
	   const int32 srcWidth  = srcBounds.IntegerWidth();
	   const int32 srcHeight = srcBounds.IntegerHeight();

	   //	Calculate center of source bitmap
	   int32 srcCenterX = srcWidth  / 2;
	   int32 srcCenterY = srcHeight / 2;

	   //	Calculate center of draw rect by using the non-rotated
	   //	bitmap area of the source bitmap
	   int32 drawCenterX = scaledArea.Width()  / 2;
	   int32 drawCenterY = scaledArea.Height() / 2;

	   //	Calculate difference between center points
	   int32 diffX = srcCenterX - drawCenterX;
	   int32 diffY = srcCenterY - drawCenterY;

	   //	Offset srcBounds and set up drawPt
	   drawPt.x += (-diffX);
	   drawPt.y += (-diffY);

	   // Set up bytes per row constants
	   const int32 offBPR  = offscreen->BytesPerRow();
	   const int32 srcBPR  = srcBitmap->BytesPerRow();

	   //	Calculate pointer increment amount based on bitmap depth
	   const int32 offIncrement  = GetBitmapPixelSize(offscreen->ColorSpace());
	   const int32 srcIncrement  = GetBitmapPixelSize(srcBitmap->ColorSpace());

	   //	Calculate pixel inset amounts for source bitmap and offscreen bitmap
	   int32 insetOffLeft,	insetOffTop, insetOffRight, insetOffBottom;
	   int32 insetSrcLeft, insetSrcTop, insetSrcRight, insetSrcBottom;

	   insetOffLeft = insetOffTop = insetOffRight = insetOffBottom = 0;
	   insetSrcLeft = insetSrcTop = insetSrcRight = insetSrcBottom = 0;

	   //	Clip the left side of the bitmap if it is offscreen
	   if (drawPt.x < 0)
	        insetSrcLeft = offBounds.left + (-drawPt.x);
	   else
	        insetOffLeft = drawPt.x;

	   //	Clip the left side of the bitmap if it is offscreen
	   if ( drawPt.y < 0)
	        insetSrcTop = offBounds.top + (-drawPt.y);
	   else
	        insetOffTop = drawPt.y;

	   //	Clip the right side of the bitmap if it is offscreen
	   if ( drawPt.x + srcBounds.Width() > offBounds.Width() )
	        insetSrcRight = (drawPt.x + srcBounds.Width()) - offBounds.Width();

	   //	Clip the bottom of the bitmap if it is offscreen
	   if ( drawPt.y + srcBounds.Height() > offBounds.Height() )
	        insetSrcBottom = (drawPt.y + srcBounds.Height()) - offBounds.Height();

	   //	Setup image data width and height.
	   const int32 dataWidth        = (srcWidth - insetSrcRight) * srcIncrement;
	   const int32 dataHeight       = srcHeight - insetSrcBottom;

	   // Get pointer to bitmap bits for pointer arithmatic
	   uchar *srcBits	= (uchar *)srcBitmap->Bits();
	   uchar *offBits       = (uchar *)offscreen->Bits();

	   //   Set up pointers.  Inset the source and offscreen the proper amount
	   //	from the top
	   srcBits += insetSrcTop * srcBPR;
	   offBits += insetOffTop * offBPR;


	   //
	   // Traverse through bitmap, performing opacity pixel addition and copy
	   //

	   uint32       row;
	   uchar        *offRowPtr, *srcRowPtr;
	   BPoint	includePt, lineStart, lineEnd;

	   //	Handle rows
	   for (row = insetSrcTop; row <= dataHeight; row++)
	   {
	        // Set up row pointers
	        offRowPtr = offBits;
	        srcRowPtr = srcBits;

	        // Inset source and offscreen proper amount from left side
	        srcRowPtr += insetSrcLeft * srcIncrement;
	        offRowPtr += insetOffLeft * offIncrement;

	        //	Set up Y location of includePt
	        includePt.y = row + drawPt.y;

	        //	Determine scanline of polygon for point inclusion test
	        //lineStart.Set(  ,includePt.y);
	        //lineEnd.Set(  ,includePt.y;

	        //      Traverse through pixel columns.
	        uint32 byteCounter, ptIndexX;
	        for ( byteCounter = (insetSrcLeft * srcIncrement), ptIndexX = insetSrcLeft; byteCounter <= dataWidth; byteCounter += srcIncrement, ptIndexX++)
	        {
	                //	Calculate point location of memory location for polygon inclusion test
	                includePt.x = ptIndexX + drawPt.x;

	                if ( drawPoly.Contains(includePt) )
	                //if ( PtInLine(includePt, lineStart, lineEnd) )
	                {
	                        //	Draw opaque
	                        if (fTransparency == 1.0)
	                        {
	 *(uint32 *)offRowPtr =  *(uint32 *)srcRowPtr;
	                        }
	                        //	Draw with transparency
	                        else
	                        {
	                                //	Here is an explanation of what is going on here:
	                                //	We use pointer arithmetic to increment the bitmap row and offset into the row.
	                                //	Each uint32 sized chunk contains a 32-bit pixel in Blue | Green | Red | Alpha
	                                //	order.  By masking we can get the individual pixel components.  We perform our
	                                //	calcualtion and reconstruct the uint32 pixel by logical ORing the elements and
	                                //	assigning the pixel to the offscreen.

	                                //	If point is in polygon, perform blend operation.
	                                //	gzr: to do... Determine a more efficient way to perform this clipping.
	                                //	Perhaps I need to determine polygon line start and end points and
	                                //	do a compare that way.
	 *(uint32 *)offRowPtr =  (uint32) (((*(uint32 *)offRowPtr & kBlueMask)  + (*(uint32 *)srcRowPtr & kBlueMask))  * fTransparency) & kBlueMask |
	                                                                                (uint32) (((*(uint32 *)offRowPtr & kGreenMask) + (*(uint32 *)srcRowPtr & kGreenMask)) * fTransparency) & kGreenMask |
	                                                                                (uint32) (((*(uint32 *)offRowPtr & kRedMask)   + (*(uint32 *)srcRowPtr & kRedMask))	  * fTransparency) & kRedMask;
	                        }
	                }

	                //	Increment row pointers
	                offRowPtr += offIncrement;
	                srcRowPtr += srcIncrement;
	        }

	        // Offset pixel rows
	        offBits += offBPR;
	        srcBits += srcBPR;
	   } */
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TPictureCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case B_OK:
	case B_REFS_RECEIVED:
	{
		fPanel->Hide();

		// Attempt to load image
		if ( LoadPictureFile(message) ) {
			Init();
			AddToBrowser();
		}
	}
	break;

	default:
		TVisualCue::MessageReceived(message);
		break;
	}
}



#pragma mark -
#pragma mark === File Handling ===


//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TPictureCue::ShowPanel()
{
	TVisualCue::ShowPanel();

	//      Create messenger to send panel messages to our channel.  We cannot send it to
	//  ourself as we are not part of the view heirarchy.
	BMessenger *messenger = new BMessenger( fChannel,  ((MuseumApp *)be_app)->GetCueSheet());

	// Create message containing pointer to ourself
	BMessage *message = new BMessage();
	message->AddPointer("TheCue", this);

	// Create a RefFilter for an "image" type
	TRefFilter *refFilter = new TRefFilter(kImageFilter);

	// Construct a file panel and set it to modal
	fPanel = new BFilePanel( B_OPEN_PANEL, messenger, NULL, B_FILE_NODE, false, message, refFilter, true, true );

	// Set it to application's home directory
	app_info appInfo;
	be_app->GetAppInfo(&appInfo);
	BEntry entry(&appInfo.ref);
	BDirectory parentDir;
	entry.GetParent(&parentDir);
	fPanel->SetPanelDirectory(&parentDir);

	// Center Panel
	CenterWindow(fPanel->Window());
	fPanel->Show();

	// Clean up
	delete messenger;
	delete message;

}



//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TPictureCue::HidePanel()
{
	if(fPanel) {
		// Clean up any RefFilters
		TRefFilter *theFilter = static_cast<TRefFilter *>(fPanel->RefFilter() );
		if (theFilter)
			delete theFilter;

		delete fPanel;
		fPanel = NULL;
	}

	TVisualCue::HidePanel();
}


//---------------------------------------------------------------------
//	LoadPictureFile
//---------------------------------------------------------------------
//
//	Load the picture file.  We support any format handled by Datatypes
//	that can be classified as DATA_BITMAP and DATA_PICTURE
//

bool TPictureCue::LoadPictureFile(BMessage *message)
{

	status_t myErr;

	message->FindRef("refs", 0, &fFileRef);

	// Resolve possible symlink...
	BEntry entry(&fFileRef, true);
	entry.GetRef(&fFileRef);

	// Create BFile from ref...
	fFile = new BFile(&fFileRef, B_READ_ONLY);

	//	Check that we have a valid file
	if (fFile->InitCheck() == B_NO_INIT)
		return false;

	// Try to determine the file type by looking at MIME type
	char mimeStr[B_FILE_NAME_LENGTH+30];
	translator_info info;

	// Get application translator
	BTranslatorRoster *theTranslator = ((MuseumApp *)be_app)->GetTranslator();

	// We got an attribute.
	if (B_OK <= fFile->ReadAttr("BEOS:TYPE", B_STRING_TYPE, 0, mimeStr, B_FILE_NAME_LENGTH) )
		myErr = theTranslator->Identify(fFile, NULL, &info, 0, mimeStr);

	// Try without MIME type hint
	if (myErr != B_OK)
		myErr = theTranslator->Identify(fFile, NULL, &info);

	//	Unrecognized file format.  Inform the user and exit
	if (myErr != B_OK) {
		switch(myErr)
		{
		//	No handler exists for data
		case B_NO_TRANSLATOR:
		case B_BAD_VALUE:
		case B_NOT_INITIALIZED:
			UnhandledImageFormat(&fFileRef);
			return false;

		default:
			return false;
		}
	}

	// We also will only handle data of types DATA_BITMAP and DATA_PICTURE
	BMallocIO outStream;

	switch (info.group)
	{

	case B_TRANSLATOR_BITMAP:
	{
		// Create bitmap
		BBitmapStream bitmapStream;
		status_t myErr = theTranslator->Translate(fFile, &info, NULL, &bitmapStream, B_TRANSLATOR_BITMAP);
		if (myErr) {
			UnhandledImageFormat(&fFileRef);
			return false;
		}

		// Get data
		myErr = bitmapStream.DetachBitmap(&fBitmap);
		if (myErr) {
			UnhandledImageFormat(&fFileRef);
			return false;
		}
	}
	break;

	/*
	   case B_TRANSLATOR_PICTURE:
	        {
	                status_t myErr = theTranslator->Translate(fFile, NULL, NULL, &outStream, B_TRANSLATOR_PICTURE);
	                if (myErr != B_OK)
	                {
	                        UnhandledImageFormat(&fFileRef);
	                        return false;
	                }

	                // Create picture from file
	                long offset = 0;
	                long size = outStream.BufferLength();
	                const void *data = outStream.Buffer();
	                fPicture = new BPicture( (char *) data+offset, size);
	        }
	        break;
	 */

	default:
		UnhandledImageFormat(&fFileRef);
		return false;
	}

	// We can handle the file
	return true;
}

#pragma mark -
#pragma mark === Editor ===

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TPictureCue::OpenEditor()
{

	// If editor is already open, bring it to front
	if (fEditorOpen) {
		if (fEditor) {
			fEditor->Show();
			fEditor->Activate(true);
		}
	} else   {
		BRect bounds(50, 50, 350, 350);
		fEditorOpen = true;
		fEditor = new TPictureCueEditor(bounds, this);

		if (fEditor)
			fEditor->Show();
		else
			fEditorOpen = false;
	}
}

#pragma mark -
#pragma mark === Playback Routines ====


//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//	Prepare cue for playback
//

void TPictureCue::PrerollCue(uint32 theTime)
{
	//	gzr: to do ... Load data into RAM
}

//---------------------------------------------------------------------
//	PlayCue
//---------------------------------------------------------------------
//
//	Tell cue to start playback at millisecond time
//

void TPictureCue::PlayCue(uint32 theTime)
{
	//DrawData(theTime, fCroppedArea, fStageOffscreen);

	TCueView::PlayCue(theTime);
}

//---------------------------------------------------------------------
//	StopCue
//---------------------------------------------------------------------
//
//	Tell cue we are stopping
//

void TPictureCue::StopCue(uint32 theTime)
{
	//	gzr: to do ... Purge bitmap data from memory

	//	Pass up to parent
	TCueView::StopCue(theTime);
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Tell cue we are pausing
//

void TPictureCue::PauseCue()
{
	TCueView::PauseCue();
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Tell cue we are resuming playback
//

void TPictureCue::ResumeCue()
{
	TCueView::ResumeCue();
}

//---------------------------------------------------------------------
//	HandlePlayback
//---------------------------------------------------------------------
//
//	Process tick received from playback engine.  We receive a tick each
//	millisecond.
//

void TPictureCue::HandlePlayback(uint32 theTime)
{
	RenderData(theTime, fCuePosition->Enclosure(false));
}

#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TPictureCue::LoadCueIcon()
{
	BBitmap *cueIcon = GetAppIcons()->fPictureUpIcon;

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(fCueIcon);
	}

	//	Pass up to parent
	TCueView::LoadCueIcon();

}


//---------------------------------------------------------------------
//	GetBitmap
//---------------------------------------------------------------------
//
//	Return visual display of data at the time specified.  If theTime is
//	NULL, return the data for the current time
//

BBitmap *TPictureCue::GetBitmap(uint32 theTime)
{
	// Return modified bitmap if we have created it
	if (fTransformBitmap)
		return fTransformBitmap;
	else
		return fBitmap;
}
