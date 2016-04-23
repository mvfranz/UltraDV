#pragma once

// Typedefs
typedef enum
{
	k8BitColor,
	k15BitColor,
	k16BitColor,
	k24BitColor,
	k32BitColor
} RLEDepth;

// Class Definition
class PackerApp : public BApplication
{
	public:
		PackerApp(const char *signature);
	
		void ReadyToRun();
		
		BTranslatorRoster *m_TranslationRoster;
		
	private:
		// Member Functions
		void 		EncodeBits( BBitmapStream *theBitmap, RLEDepth theDepth);
		void 		DecodeBits(BFile *theFile, RLEDepth theDepth);
		
		// 8-bit Codec
		BMallocIO 	*RLEEncodeScanline8(void *scanBuf, uint32 size);
		BMallocIO 	*RLEDecodeScanline8(BMallocIO &scanLine);
		
		// 15-bit Codec
		BMallocIO 	*RLEEncodeScanline15(void *scanBuf, uint32 size);
		BMallocIO 	*RLEDecodeScanline15(BMallocIO &scanLine);
		
		// 16-bit Codec
		BMallocIO 	*RLEEncodeScanline16(void *scanBuf, uint32 size);
		BMallocIO 	*RLEDecodeScanline16(BMallocIO &scanLine);
		
		// 24-bit Codec
		BMallocIO 	*RLEEncodeScanline24(void *scanBuf, uint32 size);
		BMallocIO 	*RLEDecodeScanline24(BMallocIO &scanLine);

		// 32-bit Codec
		BMallocIO 	*RLEEncodeScanline32(void *scanBuf, uint32 size);
		BMallocIO 	*RLEDecodeScanline32(BMallocIO &scanLine);
		
		void 		StoreTranslatorBitmap(BBitmap *bitmap, char *filename, uint32 type);
		
		// Utility Routines
		bool 		CompareScanlineBuffers( BMallocIO *lastBuffer, BMallocIO *outBuffer);
};