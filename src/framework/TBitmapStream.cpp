//---------------------------------------------------------------------
//
//	File:	TBitmapStream.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.26.98
//
//	Desc:	Alert Handling Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#include "TBitmapStream.h"

#include <Bitmap.h>
#define DEBUG 1
#include <Debug.h>
#include <string.h>


TBitmapStream::TBitmapStream( BBitmap *bitmap)
{
	fMap 		= bitmap;
	fDetached 	= false;
	fPosition 	= 0;
	fSize 		= 0;

	//	Extract header if needed
	if (fMap)
	{
		fHeader.magic 		= DATA_BITMAP;
		fHeader.bounds 		= fMap->Bounds();
		fHeader.rowBytes 	= fMap->BytesPerRow();
		fHeader.colors 		= fMap->ColorSpace();
		fHeader.dataSize 	= (fHeader.bounds.Height()+1)*fHeader.rowBytes;
		fSize = sizeof(DATABitmap)+fHeader.dataSize;
	}
}


TBitmapStream::~TBitmapStream()
{
	if (fMap && !fDetached)
		delete fMap;
}


status_t TBitmapStream::ReadAt( off_t pos, void *buffer, size_t size)
{
	if (!fMap)
		return B_ERROR;

	if (!size)
		return B_NO_ERROR;

	if (pos >= fSize)
		return B_ERROR;

	long toRead;
	void *source;

	if ( fPosition < sizeof(DATABitmap) )
	{
		toRead = sizeof(DATABitmap)-pos;
		source = ((char *)&fHeader)+pos;
	}
	else
	{
		toRead = fSize-pos;
		source = ((char *)fMap->Bits())+fPosition-sizeof(DATABitmap);
	}

	if (toRead > size)
		toRead = size;

	memcpy(buffer, source, toRead);
	return toRead;
}


status_t TBitmapStream::WriteAt( off_t pos, const void *	data, size_t size)
{
	if (!size)
		return B_NO_ERROR;

	ssize_t written = 0;
	while (size > 0)
	{
		long 	toWrite;
		void 	*dest;

		//	We depend on writing the header separately in detecting changes to it
		if (pos < sizeof(DATABitmap))
		{
			toWrite = sizeof(DATABitmap)-pos;
			dest = ((char *)&fHeader)+pos;
		}
		else
		{
			toWrite = fHeader.dataSize-pos+sizeof(DATABitmap);
			dest = ((char *)fMap->Bits())+pos-sizeof(DATABitmap);
		}

		if (toWrite > size)
			toWrite = size;

		//	i e we've been told to write too much
		if (!toWrite && size)
			return B_BAD_VALUE;

		memcpy(dest, data, toWrite);
		pos += toWrite;
		written += toWrite;
		data = ((char *)data)+toWrite;
		size -= toWrite;
		if (pos > fSize)
			fSize = pos;

		//	If we change the header, the rest goes
		if (pos == sizeof(DATABitmap))
		{
			if (fMap && ((fMap->Bounds() != fHeader.bounds) ||
					(fMap->ColorSpace() != fHeader.colors) ||
					(fMap->BytesPerRow() != fHeader.rowBytes)))
			{

				//	if someone detached, we don't delete
				if (!fDetached)
					delete fMap;

				fMap = NULL;
			}

			if (!fMap)
			{
				fHeader.bounds.PrintToStream();

				if ((fHeader.bounds.left > 0.0) || (fHeader.bounds.top > 0.0))
					DEBUGGER("non-origin bounds!");

				fMap = new BBitmap(fHeader.bounds, fHeader.colors);
				if (fMap->BytesPerRow() != fHeader.rowBytes)
				{
					return B_MISMATCHED_VALUES;
				}
			}
			if (fMap)
			{
				fSize = sizeof(DATABitmap)+fMap->BitsLength();
			}
		}
	}
	return written;
}


//	returns 0 for success
off_t TBitmapStream::Seek( off_t position, uint32 whence)
{
	if (whence == SEEK_CUR)
		position += fPosition;

	if (whence == SEEK_END)
		position += fSize;

	if (position < 0)
		return B_BAD_VALUE;

	if (position > fSize)
		return B_BAD_VALUE;

	fPosition = position;
	return fPosition;
}


off_t TBitmapStream::Position() const
{
	return fPosition;
}


off_t TBitmapStream::Size() const
{
	return fSize;
}


//	returns 0 for success
status_t TBitmapStream::SetSize( off_t size)
{
	if (size < 0)
		return B_BAD_VALUE;

	if (fMap && (size > fHeader.dataSize+sizeof(DATABitmap)))
		return B_BAD_VALUE;

	/*	Problem:
	 *	What if someone calls SetSize() before writing the header, so we don't know what
	 *	bitmap to create?
	 *	Solution:
	 *	We assume people will write the header before any data,
	 *	so SetSize() is really not going to do anything.
	 */

	//	if we checked that the size was OK
	if (fMap)
		fSize = size;

	return B_NO_ERROR;
}


status_t TBitmapStream::DetachBitmap( BBitmap* &outBitmap)
{
	outBitmap = NULL;

	if (!fMap)
		return B_ERROR;

	if (fDetached)
		return B_ERROR;

	fDetached = true;
	outBitmap = fMap;

	return B_NO_ERROR;
}

