/*	TBitmapStream.h
 */

#pragma once

#include <DataIO.h>
#include "Datatypes.h"

class BBitmap;


class TBitmapStream :
	public BPositionIO
{
public:
								TBitmapStream(
									BBitmap *		map = NULL);
								~TBitmapStream();

		ssize_t					ReadAt(	//	returns 0 for success
									off_t			pos,
									void *			buffer,
									size_t			size);
		ssize_t					WriteAt(	//	returns 0 for success
									off_t			pos,
									const void *	data,
									size_t			size);
		off_t					Seek(	//	returns 0 for success
									off_t			position,
									uint32			whence);
		off_t					Position() const; // returns position
		off_t					Size() const; // returns total size
		status_t				SetSize(	//	returns 0 for success
									off_t			size);

		status_t				DetachBitmap(
									BBitmap * &		outMap);
protected:
		DATABitmap				fHeader;
		BBitmap *				fMap;
		size_t					fPosition;
		size_t					fSize;
		bool					fDetached;
};


