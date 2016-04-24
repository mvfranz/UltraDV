//	Datatypes.h
//	Header file for the datatypes library.
//	These functions allow for easy translation of 
//	unknown data into known types.

#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include <OS.h>
#include <StorageDefs.h>
#include <InterfaceDefs.h>
#include <Mime.h>

struct Format;


class BBitmap;
class BView;
class BPositionIO;
class BQuery;
class BMessage;


#include "DataFormats.h"

#pragma export on

typedef uint32 DATAID;

struct DATAInfo {								//	Info about a specific translation
	uint32				formatType;				//	B_ASCII_TYPE, ...
	DATAID				formatHandler;			//	Internal to datatypes library
	uint32				formatGroup;			//	DATA_BITMAP, DATA_TEXT, ...
	float				formatQuality;			//	Quality of format in class 0.0-1.0
	float				formatCapability;		//	How much of the format do we do? 0.0-1.0
	char				formatName[B_MIME_TYPE_LENGTH+11];
	char				MIMEName[B_MIME_TYPE_LENGTH+11];
};




enum DATA_ERROR {
	DATA_BASE_ERROR		=	(int)0xDADA0000,
	DATA_NO_HANDLER		=	DATA_BASE_ERROR,	//	no handler exists for data
	DATA_ILLEGAL_DATA,							//	data is not what it said it was
	DATA_NOT_INITIALIZED
};



#define DATA_CURRENT_VERSION	163
#define DATA_MIN_VERSION		161


extern "C" {

			//	you need to initialize the library to use it
			//	you should also shutdown it before quitting

	extern	const char *DATAVersion(	//	returns version string
							int32 & outCurVersion,	//	current version spoken
							int32 & outMinVersion);	//	minimum version understood

	extern	status_t	DATAInit(		//	establish connection
							const char *		app_signature,
							const char *		load_path = NULL);

	extern	status_t	DATAShutdown();	//	don't want to talk anymore

			//	these functions call through to the translators
			//	when wantType is not 0, will only take into consideration 
			//	handlers that can read input data and produce output data

	extern	status_t	DATAIdentify(	//	find out what something is
							BPositionIO &			inSource,
							BMessage *			ioExtension,
							DATAInfo &			outInfo,
							uint32				inHintType = 0,
							const char *		inHintMIME = NULL,
							uint32				inWantType = 0);

	extern	status_t	DATAGetHandlers(//	find all handlers for a type
							BPositionIO &			inSource,
							BMessage *			ioExtension,
							DATAInfo * &		outInfo,	//	call delete[] on outInfo when done
							int32 &				outNumInfo,
							uint32				inHintType = 0,
							const char *		inHintMIME = NULL,
							uint32				inWantType = 0);

	extern	status_t	DATAGetAllHandlers(//	find all handler IDs
							DATAID * &			outList,//	call delete[] when done
							int32 &				outCount);

	extern status_t		DATAGetHandlerInfo(//	given a handler, get user-visible info
							DATAID				forHandler,
							const char * &		outName,
							const char * &		outInfo,
							int32 &				outVersion);

			//	note that handlers may choose to be "invisible" to
			//	the public formats, and just kick in when they 
			//	recognize a file format by its data.

	extern	status_t	DATAGetInputFormats(//	find all input formats for handler
							DATAID				forHandler,
							const Format * &	outFormats,//	don't write contents!
							int32 &				outNumFormats);

	extern	status_t	DATAGetOutputFormats(//	find all output formats for handler
							DATAID				forHandler,
							const Format * &	outFormats,//	don't write contents!
							int32 &				outNumFormats);

			//	actually do some work

	extern	status_t	DATATranslate(	//	morph data into form we want
							BPositionIO &			inSource,
							const DATAInfo *	inInfo,//	may be NULL
							BMessage *			ioExtension,
							BPositionIO &			outDestination,
							uint32 				inWantOutType,
							uint32				inHintType = 0,
							const char *		inHintMIME = NULL);

			//	For configuring options of the handler, a handler can support 
			//	creating a view to cofigure the handler. The handler can save 
			//	its preferences in the database or settings file as it sees fit.
			//	As a convention, the handler should always save whatever 
			//	settings are changed when the view is deleted or hidden.

	extern	status_t	DATAMakeConfig(
							DATAID				forHandler,
							BMessage *			ioExtension,
							BView * &			outView,
							BRect &				outExtent);

			//	For saving settings and using them later, your app can get the 
			//	current settings from a handler into a BMessage that you create 
			//	and pass in empty. Pass this message (or a copy thereof) to the 
			//	handler later in a call to DATATranslate to translate using 
			//	those settings. This config message should also contain any 
			//	DATACapture settings you may want to re-use.

	extern	status_t	DATAGetConfigMessage(
							DATAID				forHandler,
							BMessage *			ioExtension);

			//	Some Datatypes get data from thin air (QuickCam, recorder, ...)
			//	These Datatypes utilize the Capture API, a simple API that lets 
			//	applications run the Datatype and tell it to start generating 
			//	data.

	extern	status_t	DATAListCaptures(		//	experimental - may change
							uint32				captureType,
							DATAID * &			outHandlers,	//	delete[] when done
							int32 &				outNumHandlers);
	extern	status_t	DATAMakeCapturePanel(	//	experimental - may change
							DATAID				handler,
							BMessage *			ioExtension,
							BView * &			outView,
							BRect &				outExtent);
	extern	status_t	DATACapture(			//	experimental - may change
							DATAID				handler,
							BView *				view,	//	can be NULL
							BMessage *			ioExtension,	//	can be NULL
							BPositionIO &		stream,
							uint32				captureType);
}


#pragma export reset


#endif	// _DATATYPES_H_
// _DATATYPES_H_
