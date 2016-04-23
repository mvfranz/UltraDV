//	DataHandler.h
//	This is the interface published by a data translator module
/*	$Id: DataHandler.h,v 1.1 1997/07/22 21:27:07 hplus Exp $ */

#pragma once

#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#include <SupportDefs.h>
#include <Mime.h>


#define DATA_HANDLER_TYPE "application/x-be-executable"

#pragma export on

//	when you export this struct, end with an empty
//	record that has 0 for "type"
//
struct Format {
	uint32		type;						//	BeOS data type
	uint32		t_cls;						//	class of this format
	float		quality;					//	format quality 0.0-1.0
	float		capability;					//	handler capability 0.0-1.0
	char		MIME[B_MIME_TYPE_LENGTH+11];	//	MIME string
	char		formatName[B_MIME_TYPE_LENGTH+11];	//	only descriptive
};

struct DATASpec;
struct DATAInfo;
class BView;
struct BRect;
class BPositionIO;

extern "C" {

	extern	char			handlerName[];		//	required, C string, ex "Jon's Sound"
	extern	char			handlerInfo[];		//	required, descriptive C string, ex "Jon's Sound Handler (shareware $5: user@mail.net) v1.00"
	extern	int32			handlerVersion;		//	required, integer, ex 100

	extern	Format			inputFormats[];		//	optional (else Identify is always called)
	extern	Format			outputFormats[];	//	optional (else Translate is called anyway)
												//	Handlers that don't export outputFormats 
												//	will not be considered by files looking for 
												//	specific output formats.

	//	Return DATA_NO_HANDLER if not handling this data.
	//	Even if inputFormats exists, may be called for data without hints
	//	Ff outType is not 0, must be able to export in wanted format

	extern	status_t		Identify(	//	required
								
								BPositionIO &			inSource,
								const Format *		inFormat,	//	can beNULL
								BMessage *			ioExtension,	//	can be NULL
								DATAInfo &			outInfo,
								uint32				outType);

	//	Return DATA_NO_HANDLER if not handling the output format
	//	If outputFormats exists, will only be called for those formats

	extern	status_t		Translate(	//	required
								BPositionIO &			inSource,
								const DATAInfo &	inInfo,
								BMessage *			ioExtension,	//	can be NULL
								uint32				outFormat,
								BPositionIO &			outDestination);

	//	The view will get resized to what the parent thinks is 
	//	reasonable. However, it will still receive MouseDowns etc.
	//	Your view should change settings in the translator immediately, 
	//	taking care not to change parameters for a translation that is 
	//	currently running. Typically, you'll have a global struct for 
	//	settings that is atomically copied into the translator function 
	//	as a local when translation starts.
	//	Store your settings wherever you feel like it.

	extern	status_t		MakeConfig(	//	optional
								BMessage *			ioExtension,	//	can be NULL
								BView * &			outView,
								BRect &				outExtent);

	//	Copy your current settings to a BMessage that may be passed 
	//	to DATATranslate at some later time when the user wants to 
	//	use whatever settings you're using right now.

	extern	status_t		GetConfigMessage(	//	optional
								BMessage *			ioExtension);

	//	Create a BView that previews any data capture the add-on may 
	//	be able to do. Datatypes that export this function and Capture 
	//	are supposed to be able to Capture any type among their output 
	//	formats.

	extern	status_t		MakeCapturePanel(	//	optional, experimental
								BMessage *			ioExtension,	//	can be NULL
								BView * &			outView,
								BRect &				outExtent);

	//	Start capture. The capture will run until "done", as far as 
	//	the Datatype decides.

	extern	status_t		Capture(			//	Optional, experimental
								BView *				configView,	//	can be NULL
								BMessage *			ioExtension,	//	can be NULL
								BPositionIO &		outStream,
								uint32				captureFormat);

}

#pragma export reset


#endif // _DATAHANDLER_H_


