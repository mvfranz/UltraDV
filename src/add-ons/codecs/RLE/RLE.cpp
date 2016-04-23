
#include <string.h>
#include <support/Debug.h>

#include "RLE.h"

//#define DECODE

// Constants
const uint8 kLiteral 		= 0x01;		// 	The following is a literal pixel
const uint8 kEncoded 		= 0x02;		// 	The following is run length encoded
const uint8 kVRPacket 		= 0x00;		// 	Vertical Replication or Repeat Scan Line Packer
const uint8 kEndOfScanline 	= 0xFF;		//	End of Scanline Flag
const uint8 kMaxRuns 		= 0xFE;		// 	Maximum allowable pixel run
const uint8 kMaxVRRuns 		= 0xFF;		// 	Maximum allowable vertical replications

const int32 kLiteralSize	= sizeof(uint8);
const int32 kRunCountSize	= sizeof(uint8);
const int32 kVRCountSize	= sizeof(uint8);
const int32 kEOSSize		= sizeof(uint8);
const int32 kVRSize			= sizeof(uint8);
const int32 kPixel8Size		= sizeof(uint8);
const int32 kPixel15Size	= kPixel8Size * 2;
const int32 kPixel16Size	= kPixel8Size * 2;
const int32 kPixel24Size	= kPixel8Size * 3;
const int32 kPixel32Size	= kPixel8Size * 4;
const int32 kVRPacketSize	= sizeof(uint8);
const int32 kRunVRBlock		= kVRCountSize  + kVRSize;

// Type Definitions
const typedef uint8* 	Type8BitPixelPtr;
const typedef uint32* 	Type32BitPixelPtr;


//-----------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------
//
//
//

main()
{	
	
	BApplication *theApp = new PackerApp("application/x-vnd.mediapede-packer");
	theApp->Run();	
	delete(theApp);
	
	return(0);
}

//-----------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------
//
//
//

PackerApp::PackerApp(const char *signature) : BApplication(signature)
{

}


//-----------------------------------------------------------------------
// ReadyToRun
//-----------------------------------------------------------------------
//
//

void PackerApp::ReadyToRun()
{
	// Init Translation Kit services
	m_TranslationRoster = new BTranslatorRoster();

   	// Load default translators
	m_TranslationRoster->AddTranslators(NULL);
	
	// Open our test file	
	BFile 		testFile;
	
#ifdef DECODE	
	testFile.SetTo("/boot/InProgress/Museum/Source/add-ons/codecs/RLE/image.rle", B_READ_WRITE);
	if (testFile.InitCheck() == B_OK)
		DecodeBits(&testFile, k24BitColor);
#else
	status_t 	myErr;
	
	testFile.SetTo("/boot/InProgress/Museum/Source/add-ons/codecs/RLE/Test.tga", B_READ_WRITE);
	if (testFile.InitCheck() == B_OK)
	{	
		// Try to determine the file type by looking at MIME type
		char mimeStr[B_FILE_NAME_LENGTH+30];
		translator_info info;
				
		// We got an attribute.
		if (B_OK <= testFile.ReadAttr("BEOS:TYPE", B_STRING_TYPE, 0, mimeStr, B_FILE_NAME_LENGTH) )
			myErr = m_TranslationRoster->Identify(&testFile, NULL, &info, 0, mimeStr);
	         	
		// Try without MIME type hint
		if (myErr != B_OK)
			myErr = m_TranslationRoster->Identify(&testFile, NULL, &info);
		
		//	Unrecognized file format.  Inform the user and exit
		if (myErr != B_OK)
		{
			switch(myErr)
			{
				//	No handler exists for data
				case B_NO_TRANSLATOR:	
				case B_BAD_VALUE:		
				case B_NOT_INITIALIZED:
					return;	
				
				default:
					return;
			}			
		}
		
		// We also will only handle data of types DATA_BITMAP and DATA_PICTURE
		BBitmapStream 	bitmapStream;
		
		switch (info.group)
		{				
	         
			case B_TRANSLATOR_BITMAP:
				{			
					// Create bitmap					
					status_t myErr = m_TranslationRoster->Translate(&testFile, &info, NULL, &bitmapStream, B_TRANSLATOR_BITMAP);
					if (myErr)
						return;
				}								
				break;					
		}				
	
		// 	Pack the bits.  We use the same header as a translator bitmap, only the bits
		//	are packed.
		EncodeBits(&bitmapStream, k24BitColor);
	}
		
	delete m_TranslationRoster;	
#endif 

	// Quit
	be_app->PostMessage(B_QUIT_REQUESTED);

}

#pragma mark -
#pragma mark === Buffer Encoding ===

//-----------------------------------------------------------------------
// EncodeBits
//-----------------------------------------------------------------------
//
//

void PackerApp::EncodeBits( BBitmapStream *bitmapStream, RLEDepth theDepth)
{
	// Get information about the bitmap
	status_t 			myErr;
	TranslatorBitmap 	bmHeader;
	BMallocIO			imageBuffer;
	uint8				vrCount = 0;
	
	ssize_t headerSize = sizeof(bmHeader);
	myErr = bitmapStream->Seek(0, SEEK_SET);	
	if (myErr == B_ERROR)
		return;
		
	// Read in the header information
	myErr = bitmapStream->Read(&bmHeader, headerSize);
	if (myErr < B_OK)
		return;

	// Write header info into buffer
	imageBuffer.Write(&bmHeader, headerSize);

	// Convert scanline data
	BMallocIO 	*outBuffer, *lastBuffer;
	lastBuffer = outBuffer = NULL;
		
	void *scanBuf = (void *)malloc(bmHeader.rowBytes);
	int32 height = bmHeader.bounds.IntegerHeight();
	for (int32 index = 0; index <= height; index++)
	{								
		// Read the next scanline
		bitmapStream->Read(scanBuf, bmHeader.rowBytes);
		
		// Now RLE the scanline 
		switch (theDepth)
		{
			case k8BitColor:
				outBuffer = RLEEncodeScanline8(scanBuf, bmHeader.rowBytes);
				break;
				
			case k15BitColor:
				outBuffer = RLEEncodeScanline15(scanBuf, bmHeader.rowBytes);
				break;
				
			case k16BitColor:
				outBuffer = RLEEncodeScanline16(scanBuf, bmHeader.rowBytes);
				break;
				
			case k24BitColor:
				outBuffer = RLEEncodeScanline24(scanBuf, bmHeader.rowBytes);
				break;
				
			case k32BitColor:
				outBuffer = RLEEncodeScanline32(scanBuf, bmHeader.rowBytes);
				break;	
				
			// Shouldn't be here...
			default:
				return;
							
		}

		// 	Compare outBuffer with lastBuffer and see if we have a match.  If we do,
		//	write a kVRPacket to indicate a repeat scan line
		if ( CompareScanlineBuffers(lastBuffer, outBuffer))
		{
			// Increment count
			vrCount++;
			
			// Output count and vertical replication packet
			if ( vrCount >= kMaxVRRuns)
			{				
				imageBuffer.Write(&kVRPacket, kVRPacketSize);
				imageBuffer.Write(&vrCount, kVRCountSize);
				
				// Reset count
				vrCount = 0;				
			}
		}
		// A new unique buffer
		else
		{			
			// Write replicated scanlines			
			if (vrCount)
			{				
				imageBuffer.Write(&kVRPacket, kVRPacketSize);
				imageBuffer.Write(&vrCount, kVRCountSize);
				
				// Reset count
				vrCount = 0;
				
				// Write new scanline
				imageBuffer.Write(outBuffer->Buffer(), outBuffer->BufferLength());
			}
			// Write RLE encoded data out into memory buffer
			else
			{
				imageBuffer.Write(outBuffer->Buffer(), outBuffer->BufferLength());
			}	
		}
		
		// Store buffer for future comparison
		if (lastBuffer)
			delete lastBuffer;
			
		lastBuffer = outBuffer;									
	}
			
	// 	We have exited the loop.  Check and see if vertical scanlines accumulated since
	//	last write.  Is so , write data out
	if (vrCount)
	{
		imageBuffer.Write(&kVRPacket, kVRPacketSize);
		imageBuffer.Write(&vrCount, kVRCountSize);
	}
				
	// Clean up buffers
	if (lastBuffer)
		delete lastBuffer;
		
	// Write our new file out
	BFile saveFile;
	saveFile.SetTo("/boot/InProgress/Museum/Source/add-ons/codecs/RLE/image.rle", B_CREATE_FILE | B_READ_WRITE | B_ERASE_FILE);
	saveFile.Write(imageBuffer.Buffer(), imageBuffer.BufferLength() );
	
	// Set file type
	BNodeInfo info(&saveFile);
	if ( info.InitCheck() == B_OK )
		info.SetType("image/x-be-bitmap");		
		
	// Clean up
	free(scanBuf);
	saveFile.Unset();
}


//-----------------------------------------------------------------------
// DecodeBits
//-----------------------------------------------------------------------
//
//

void PackerApp::DecodeBits(BFile *theFile, RLEDepth theDepth)
{

	BMallocIO 	fileBuffer, scanLine;	
	BMallocIO	*outBuffer, *lastBuffer;	
	char 		*imageBuffer;
	
	// Set up kRunPixelBlock based on image depth
	int32 kRunPixelBlock;	
	switch (theDepth)
	{
		case k8BitColor:
			kRunPixelBlock = kRunCountSize + kPixel8Size;
			break;
			
		case k15BitColor:
			kRunPixelBlock = kRunCountSize + kPixel15Size;
			break;
			
		case k16BitColor:
			kRunPixelBlock = kRunCountSize + kPixel16Size;
			break;
			
		case k24BitColor:
			kRunPixelBlock = kRunCountSize + kPixel24Size;
			break;
			
		case k32BitColor:
			kRunPixelBlock = kRunCountSize + kPixel32Size;
			break;
	
	}
	
	// Set our buffers to NULL
	lastBuffer = outBuffer = NULL;
	
	// Create output file
	BFile saveFile;
	saveFile.SetTo("/boot/InProgress/Museum/Source/add-ons/codecs/RLE/image.raw", B_CREATE_FILE | B_READ_WRITE | B_ERASE_FILE);
	
	// Load file contents into memory
	off_t fileSize;
	theFile->GetSize(&fileSize);
	int32 theFileSize = fileSize;
	imageBuffer = (char *)malloc(theFileSize);	
	theFile->ReadAt(0, imageBuffer, theFileSize);
	
	// Get header information
	TranslatorBitmap  bmHeader;
	ssize_t headerSize = sizeof(bmHeader);
	scanLine.Seek(0, SEEK_SET);
	int32 index = headerSize;

	// Copy over header and save it out	
	memcpy( &bmHeader, imageBuffer, headerSize);;
	saveFile.Write(&bmHeader, headerSize);
	
	// Hop past header
	*(imageBuffer+=index);

	// Load scanlines into buffer
	while (index < theFileSize)
	{
		// Get run count
		uint8 runCount = *imageBuffer;
				
		// Do we have a run? 
		if (runCount != kEndOfScanline)
		{
			
			// Just load runCount and the pixel
			if (runCount != kVRPacket)
			{
				// Load data into buffer											
				scanLine.Write( imageBuffer, kRunPixelBlock);				
				
				// Hop past runCount and pixelData and increment index
				index += kRunPixelBlock;
				*(imageBuffer += kRunPixelBlock);
			}
			// 	Vertical Replication packet.  Reproduce previous scanlines as
			//	indicated by run value
			else
			{				
				// Hop past VRPacket indicator
				*(imageBuffer += kVRPacketSize);
				index += kVRPacketSize;
				
				// Get number of scanlines to replicate				
				uint8 numLines = *imageBuffer;
				for (int32 index = 0; index < numLines; index++)
				{
					fileBuffer.Write(lastBuffer->Buffer(), lastBuffer->BufferLength());
				}
				
				// Jump past the scanline indicator and increment index
				*(imageBuffer += kVRSize);
				index += kVRSize;
			}
		}		
		// We found a scanLine break.  Decode the scanline
		else
		{						
			// Send buffer in to be decoded
			switch (theDepth)
			{
				case k8BitColor:
					outBuffer = RLEDecodeScanline8(scanLine);
					break;
					
				case k15BitColor:
					outBuffer = RLEDecodeScanline15(scanLine);
					break;
					
				case k16BitColor:
					outBuffer = RLEDecodeScanline16(scanLine);
					break;
					
				case k24BitColor:
					outBuffer = RLEDecodeScanline24(scanLine);
					break;
					
				case k32BitColor:
					outBuffer = RLEDecodeScanline32(scanLine);
					break;	
					
				// Shouldn't be here...
				default:
					be_app->PostMessage(B_QUIT_REQUESTED);
					return;								
			}
			
			// Jump past the scanline indicator and increment index
			*(imageBuffer += kEOSSize);			
			index += kEOSSize;

			// Clear our scanline
			scanLine.SetSize(0);
			
			fileBuffer.Write(outBuffer->Buffer(), outBuffer->BufferLength());
			
			// Clean up
			if (lastBuffer)
				delete lastBuffer;
				
			lastBuffer = outBuffer;
		}
	}
	 			
	// Clean up
	if (lastBuffer)
		delete lastBuffer;

	// Write the data out into our new file
	saveFile.Write(fileBuffer.Buffer(), fileBuffer.BufferLength() );
	
	// Set file type
	BNodeInfo info(&saveFile);
	if ( info.InitCheck() == B_OK )
		info.SetType("image/x-mediapede-raw");		
					
	// Load file contents into memory
	off_t loadSize;
	saveFile.GetSize(&loadSize);
	
	void *imageBuf = malloc(loadSize);	
	saveFile.Seek(0, SEEK_SET);
	ssize_t bytesRead = saveFile.ReadAt(headerSize, imageBuf, loadSize-headerSize);
	
	BBitmap theBitmap(bmHeader.bounds, bmHeader.colors);
	
	uchar *bits = (uchar *)theBitmap.Bits(); 
	memcpy(bits, imageBuf, loadSize-headerSize);
	
	// Create display window
	BWindow *theWindow = new BWindow(bmHeader.bounds, "Spoot", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_WILL_DRAW);
	BView *theView = new BView(theWindow->Bounds(), "View", B_FOLLOW_ALL, 0);
	theWindow->AddChild(theView);
	theView->SetViewColor(B_TRANSPARENT_32_BIT);
	theWindow->Show();
	theView->Show();
	theWindow->Lock();	
	theView->DrawBitmap(&theBitmap);
	theView->Sync();
	theWindow->Unlock();
	
	// Clean up
	free(imageBuffer);
	free(imageBuf);
	saveFile.Unset();
}

#pragma mark -
#pragma mark ==================================
#pragma mark === Scanline Encoding Routines ===
#pragma mark ==================================

#pragma mark -
#pragma mark === 8-bit ===

//-----------------------------------------------------------------------
// RLEEncodeScanline8
//-----------------------------------------------------------------------
//
//	RLE compress a 8-bit scanline.  The scanline is comprised of
//	B_CMAP8 pixels which are lookups into a 256 color table
//
//	We return a RLE scanline of the following format:
//
//

BMallocIO *PackerApp::RLEEncodeScanline8(void *scanBuf, uint32 size)
{	
	// Allocate buffer for scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	// Setup iterator value.  This is the number of pixels in the scanline.
	const uint32 numPixels = size / kPixel8Size;
	
	// Set up our comparative run values
	uint8 runValueA	= NULL;
	uint8 runValueB = NULL;
	
	// Initialize the run count to zero
	uint8 runCount = 1;
	
	// Iterate through buffer, encoding
	for (uint32 index = 0; index < numPixels; index++)
	{
		// Read runValueA from buffer
		runValueA = ((Type8BitPixelPtr)scanBuf)[index];
	
		// 	Are we at the end of the data stream?
		//	If so, write out and exit
		if ( (index + 1) >= numPixels)
		{
			outBuffer->Write(&runCount, kRunCountSize);
			outBuffer->Write(&runValueA, kPixel8Size);
			outBuffer->Write(&kEndOfScanline, kEOSSize);	
			return outBuffer;			
		}
				
		// Read runValueB from buffer
		runValueB = ((uint32 *)scanBuf)[index+1];
		
		// Compare A and B
		if (runValueA == runValueB)
		{
			// Are we at our maximum run count?  Write out run and continue
			if (runCount >= kMaxRuns)
			{
				outBuffer->Write(&runCount, kRunCountSize);
				outBuffer->Write(&runValueA, kPixel8Size);
				runCount = 1;
			}
			else
			{
				// Increment run count
				runCount++;			
			}
		}
		// Write out runValue and runCode
		else
		{
			outBuffer->Write(&runCount,  kRunCountSize);
			outBuffer->Write(&runValueA, kPixel8Size);
			
			// Set runValueA to runValueB an reset runValue
			runValueA 	= runValueB;
			runCount 	= 1;
		}						
	}		
	
	// Set end of scanline
	outBuffer->Write(&kEndOfScanline, kEOSSize);
	return outBuffer;	
}

//-----------------------------------------------------------------------
// RLEDecodeScanline8
//-----------------------------------------------------------------------
//
//	Decode a RLE compressed scanline of B_CMAP8 pixels.
//
//	We return an expanded scanline of the following format:
//
//

BMallocIO *PackerApp::RLEDecodeScanline8(BMallocIO &scanLine)
{
	// Allocate buffer for decompressed scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	uint8 	runCount;
	uint8	pixel;
	
	// Rewind buffer
	scanLine.Seek(0, SEEK_SET);
	
	// 	Set up loop variables.  Clip off end of scanline
	//	indicator from BufferLength
	uint32 	iterator = 0;
	size_t	bufferSize = scanLine.BufferLength() - sizeof (uint8);
	
	// Traverse through buffer, decoding	
	while (iterator < bufferSize)
	{
		scanLine.ReadAt(iterator, &runCount, sizeof(uint8));
		
		// Hop past runCount
		iterator += sizeof(uint8);
		
		// Write out pixel data
		for (int32 index = 0; index < runCount; index++)
		{
			scanLine.ReadAt(iterator, &pixel, sizeof(uchar));
			outBuffer->Write( &pixel, sizeof(uchar));
		}
		
		// Increment iterator the amount we have read from buffer
		iterator += sizeof(uchar);		
	}			
	
	return outBuffer;
}

#pragma mark -
#pragma mark === 15-bit ===

//-----------------------------------------------------------------------
// RLEEncodeScanline15
//-----------------------------------------------------------------------
//
//	RLE compress a 15-bit scanline.  The scanline is comprised of
//	B_RGBA15 pixels in format ARGB 1:5:5:5
//
//	We return a RLE scanline of the following format:
//
//		--	RunCount|Alpha|Blue|Green|Red 1:5:5:5
//

BMallocIO *PackerApp::PackerApp::RLEEncodeScanline15(void *scanBuf, uint32 size)
{
	return NULL;	
}


//-----------------------------------------------------------------------
// RLEDecodeScanline16
//-----------------------------------------------------------------------
//
//	Decode a RLE compressed scanline of B_RGBA15 pixels in 
//	format ARGB 1:5:5:5
//
//	We return an expanded scanline of the following format:
//
//		-- |Alpha|Red|Green|Blue 1:5:5:5
//

BMallocIO *PackerApp::RLEDecodeScanline15(BMallocIO &scanLine)
{
	return NULL;
}


#pragma mark -
#pragma mark === 16-bit ===

//-----------------------------------------------------------------------
// RLEEncodeScanline16
//-----------------------------------------------------------------------
//
//	RLE compress a 16-bit scanline.  The scanline is comprised of
//	B_RGB16 pixels in format RGB 5:6:5
//
//	We return a RLE scanline of the following format:
//
//		--	RunCount|Blue|Green|Red	5:6:5
//

BMallocIO *PackerApp::RLEEncodeScanline16(void *scanBuf, uint32 size)
{
	return NULL;	
}


//-----------------------------------------------------------------------
// RLEDecodeScanline16
//-----------------------------------------------------------------------
//
//	Decode a RLE compressed scanline of B_RGB16 pixels in 
//	format RGB 5:6:5
//
//	We return an expanded scanline of the following format:
//
//		-- |Red|Green|Blue 5:6:5
//

BMallocIO *PackerApp::RLEDecodeScanline16(BMallocIO &scanLine)
{
	return NULL;
}


#pragma mark -
#pragma mark === 24-bit ===

//-----------------------------------------------------------------------
// RLEEncodeScanline32
//-----------------------------------------------------------------------
//
//	RLE compress a 24-bit scanline.  The scanline is comprised of
//	B_RGB_32 pixels in format BGRA 8:8:8:8.  Dispose of the alpha 
//	component
//
//	We return a RLE scanline of the following format:
//
//		--	RunCount|Blue|Green|Red 8:8:8:8
//

BMallocIO *PackerApp::RLEEncodeScanline24(void *scanBuf, uint32 size)
{	
	// Allocate buffer for scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	// Setup iterator value.  This is the number of pixels in the scanline.
	const uint32 numPixels = size / kPixel32Size;
	
	// Set up our comparative run values
	uint32 runValueA	= NULL;
	uint32 runValueB  	= NULL;
	
	// Initialize the run count to zero
	uint8 runCount = 1;
	
	// Iterate through buffer, encoding
	for (uint32 index = 0; index < numPixels; index++)
	{
		// Read runValueA from buffer
		runValueA = ((Type32BitPixelPtr)scanBuf)[index];
	
		// 	Are we at the end of the data stream?
		//	If so, write out and exit
		if ( (index + 1) >= numPixels)
		{
			outBuffer->Write(&runCount, kRunCountSize);
			outBuffer->Write(&runValueA, kPixel24Size);
			outBuffer->Write(&kEndOfScanline, kEOSSize);	
			return outBuffer;			
		}
		
		// Read runValueB from buffer		
		runValueB = ((Type32BitPixelPtr)scanBuf)[index+1];		
		
		// Compare A and B
		if (runValueA == runValueB)
		{
			// Are we at our maximum run count?  Write out run and continue
			if (runCount >= kMaxRuns)
			{
				outBuffer->Write(&runCount, kRunCountSize);
				outBuffer->Write(&runValueA, kPixel24Size);
				runCount = 1;
			}
			else
			{
				// Increment run count
				runCount++;			
			}
		}
		// Write out runValue and runCode
		else
		{
			outBuffer->Write(&runCount,  kRunCountSize);
			outBuffer->Write(&runValueA, kPixel24Size);
			
			// Set runValueA to runValueB an reset runValue
			runValueA 	= runValueB;
			runCount 	= 1;
		}						
	}		
	
	// Set end of scanline
	outBuffer->Write(&kEndOfScanline, kEOSSize);
	return outBuffer;	
}


//-----------------------------------------------------------------------
// RLEDecodeScanline24
//-----------------------------------------------------------------------
//
//	Decode a RLE compressed scanline of 24-bit pixels in 
//	format BGR 8:8:8.
//
//	We return an expanded scanline of the following format:
//
//		-- |Blue|Green|Red|Alpha 8:8:8:8
//

BMallocIO *PackerApp::RLEDecodeScanline24(BMallocIO &scanLine)
{	
	// Allocate buffer for decompressed scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	uint8 	runCount;
	uint32	pixel;
	
	// Rewind buffer
	scanLine.Seek(0, SEEK_SET);
	
	// 	Set up loop variables.  Clip off end of scanline
	//	indicator from BufferLength
	uint32 	iterator = 0;
	size_t	bufferSize = scanLine.BufferLength() - kEOSSize;
	
	// Traverse through buffer, decoding	
	while (iterator < bufferSize)
	{
		scanLine.ReadAt(iterator, &runCount, kRunCountSize);
		
		// Hop past runCount
		iterator += kRunCountSize;
		
		// Write out pixel data
		for (int32 index = 0; index < runCount; index++)
		{
			// Clear out pixel
			pixel = 0;
			scanLine.ReadAt(iterator, &pixel, kPixel24Size);			
			outBuffer->Write( &pixel, kPixel32Size);
		}
		
		// Increment iterator the amount we have read from buffer
		iterator += kPixel24Size;		
	}			
	
	return outBuffer;
}

#pragma mark -
#pragma mark === 32-bit ===

//-----------------------------------------------------------------------
// RLEEncodeScanline32
//-----------------------------------------------------------------------
//
//	RLE compress a 32-bit scanline.  The scanline is comprised of
//	B_RGB_32 pixels in format BGRA 8:8:8:8
//
//	We return a RLE scanline of the following format:
//
//		--	RunCount|Blue|Green|Red|Alpha 8:8:8:8:8
//

BMallocIO *PackerApp::RLEEncodeScanline32(void *scanBuf, uint32 size)
{	
	// Allocate buffer for scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	// Setup iterator value.  This is the number of pixels in the scanline.
	const uint32 numPixels = size / kPixel32Size;
	
	// Set up our comparative run values
	uint32 runValueA	= NULL;
	uint32 runValueB  	= NULL;
	
	// Initialize the run count to zero
	uint8 runCount = 1;
	
	// Iterate through buffer, encoding
	for (uint32 index = 0; index < numPixels; index++)
	{
		// Read runValueA from buffer
		runValueA = ((Type32BitPixelPtr)scanBuf)[index];
	
		// 	Are we at the end of the data stream?
		//	If so, write out and exit
		if ( (index + 1) >= numPixels)
		{
			outBuffer->Write(&runCount, kRunCountSize);
			outBuffer->Write(&runValueA, kPixel32Size);
			outBuffer->Write(&kEndOfScanline, kEOSSize);	
			return outBuffer;			
		}
		
		// Read runValueB from buffer		
		runValueB = ((Type32BitPixelPtr)scanBuf)[index+1];		
		
		// Compare A and B
		if (runValueA == runValueB)
		{
			// Are we at our maximum run count?  Write out run and continue
			if (runCount >= kMaxRuns)
			{
				outBuffer->Write(&runCount, kRunCountSize);
				outBuffer->Write(&runValueA, kPixel32Size);
				runCount = 1;
			}
			else
			{
				// Increment run count
				runCount++;			
			}
		}
		// Write out runValue and runCode
		else
		{
			outBuffer->Write(&runCount,  kRunCountSize);
			outBuffer->Write(&runValueA, kPixel32Size);
			
			// Set runValueA to runValueB an reset runValue
			runValueA 	= runValueB;
			runCount 	= 1;
		}						
	}		
	
	// Set end of scanline
	outBuffer->Write(&kEndOfScanline, kEOSSize);
	return outBuffer;	
}


//-----------------------------------------------------------------------
// RLEDecodeScanline32
//-----------------------------------------------------------------------
//
//	Decode a RLE compressed scanline of B_RGB_32 pixels in 
//	format BGRA 8:8:8:8.
//
//	We return an expanded scanline of the following format:
//
//		-- |Blue|Green|Red|Alpha 8:8:8:8
//

BMallocIO *PackerApp::RLEDecodeScanline32(BMallocIO &scanLine)
{	
	// Allocate buffer for decompressed scanline
	BMallocIO *outBuffer = new BMallocIO();
	
	uint8 	runCount;
	uint32	pixel;
	
	// Rewind buffer
	scanLine.Seek(0, SEEK_SET);
	
	// 	Set up loop variables.  Clip off end of scanline
	//	indicator from BufferLength
	uint32 	iterator = 0;
	size_t	bufferSize = scanLine.BufferLength() - kEOSSize;
	
	// Traverse through buffer, decoding	
	while (iterator < bufferSize)
	{
		scanLine.ReadAt(iterator, &runCount, kRunCountSize);
		
		// Hop past runCount
		iterator += kRunCountSize;
		
		// Write out pixel data
		for (int32 index = 0; index < runCount; index++)
		{
			scanLine.ReadAt(iterator, &pixel, kPixel32Size);
			outBuffer->Write( &pixel, kPixel32Size);
		}
		
		// Increment iterator the amount we have read from buffer
		iterator += kPixel32Size;		
	}			
	
	return outBuffer;
}

#pragma mark -
#pragma mark === File Storage Utilities  ===

//-----------------------------------------------------------------------
// StoreTranslatorBitmap
//-----------------------------------------------------------------------
//
//	Save translator bitmap to disk
//

void PackerApp::StoreTranslatorBitmap(BBitmap *bitmap, char *filename, uint32 type) 
{ 
	BBitmapStream stream(bitmap); // init with contents of bitmap 
	BFile file(filename, B_CREATE_FILE | B_WRITE_ONLY); 
	
	m_TranslationRoster->Translate(&stream, NULL, NULL, &file, type); 
}


#pragma mark -
#pragma mark === Utility Routines ===

//-----------------------------------------------------------------------
// CompareScanlines
//-----------------------------------------------------------------------
//
//	Compare scanlines.  If the same, return true.
//

bool PackerApp::CompareScanlineBuffers( BMallocIO *lastBuffer, BMallocIO *outBuffer)
{
	// Reject NULL pointers
	if ( (lastBuffer == NULL) || outBuffer == NULL )
		return false;
			
	size_t lastLength = lastBuffer->BufferLength();
	
	if ( lastLength != outBuffer->BufferLength() )
		return false;
	
	if ( memcmp( outBuffer->Buffer(), lastBuffer->Buffer(), lastLength) == 0 )
		return true;
		
	// No match
	return false;				
}