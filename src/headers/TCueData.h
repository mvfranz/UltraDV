#if !defined(TCUEDATA_H)
#define TCUEDATA_H
//---------------------------------------------------------------------
//
//	File:	TCueData.h
//
//	Author:	Mike Ost
//
//	Date:	10.09.98
//
//	Desc:
//
//	TCueData is a class which handles the data which supports an 
//	ultraDV cue. The primary function of the class is to stream 
//	data out to the application for	display and/or playback. The 
//	cue will be asked to respond in real-time as well as during 
//	non-real-time operations like scrubbing, or editing.
//	
//	The class has other supporting behaviors regarding the MIME types 
//	it supports, sample icons, and the like. But its main responsibilities 
//	are for pulling data from a file and streaming it out to ultraDV.
//
//---------------------------------------------------------------------

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations
#include <string>
#include <vector>

using namespace std;

class TCueData : public BArchivable {
protected:
	typedef vector<string> TTypeVector;

public:
		// Default ctor. NOTE: to assign a specific file call SetTo
	TCueData();
		// Construct self from a message
	TCueData(BMessage* msg);
		// delete
	virtual ~TCueData();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	// TODO: to instantiate we will need to look in an add-on image.
	// Add that to the message somehow?
	virtual status_t Archive(BMessage* data, bool deep) const;

	//----------------------
	// Static Class Behavior - always available
		
		// supported MIME types. NOTE: Force the subclasses to
		// fill in these functions...
	typedef TTypeVector::const_iterator TTypeIterator;
	TTypeIterator FirstType() const		{ return sftypes.begin(); }
	TTypeIterator EndOfTypes() const	{ return sftypes.end(); }
		// A good default file name for the cue type. Default
		// is ""
	virtual string DefaultFileName() const { return sfdefaultName; };

		// Return a pointer to a bitmap which is to be used in the media
		// palette. Either the 'up' or 'down' version can be chosen.
		// Default is '0', for no icon is available.
	virtual const BBitmap* CueIcon(bool up) const 
			{ return up? sfupIcon : sfdownIcon; }
		// Return a brief descriptive string for the cue type. Used in
		// the media palette.
	virtual string Description() const;
	
		// These return information about the cue: does it generate visible
		// data to display on the stage? Does it produce sound during 
		// playback? Defaults are all 'false'
	virtual bool IsVisual()	const	{ return sfisVisual; }
	virtual bool IsAudible() const	{ return sfisAudible; }
		// Can it be widened?
	virtual bool CanStretch() const	{ return sfcanStretch; }
	
		// Attach to the indicated file. Returns the expected Be error codes.
	status_t SetTo(const entry_ref& ref);
		// Detach from the file.
	void Unset(void);
		// Is 'ref' a supported file?
	bool IsValidFile(const entry_ref& ref) const;
	
	//----------------------
	// Specific Behavior - only available after SetTo

	// Display
		// Draw the cue 'contents', if that makes sense. This is the
		// background cue data displayed in the cue view. By default,
		// do nothing.
		// TODO: if the contents are cached, watch for changes in
		// 'contents' to see if they need to be regenerated
	virtual void DrawContents(BView* view, const BRect& contents) const {}
		// Return the raw bitmap at a given time. Return zero if
		// there is no bitmap to display.
	virtual const BBitmap* GetBitmap(int32 time) const { return 0; }
	
	// File
		// The supporting entry_ref
	const entry_ref& EntryRef() const;
		// The cue's natural size, if applicable. This won't be called
		// for cues that aren't naturally visible.
	virtual BRect OriginalArea() const;
		// The cue's duration in milliseconds, derived from the file.
		// If a cue has no natural duration, this is a default value for it.
	virtual int32 OriginalDuration() const;
		// Return any type-specific information for the file; e.g., 
		// audio sample rate. Returns a string which can be used
		// in the browser window. The base class implementation
		// returns a huh? string: "-". 
	virtual string Info() const;
		// Respond to a node monitor message. Return true if the
		// change requires an update or resizing of the cue.
	virtual bool NodeChangedMessage(BMessage* msg);

protected:
	// Class data
		// Contains all the MIME types supported by this class
	static TTypeVector sftypes;
		// The default file name
	static string sfdefaultName;
		// The 'up' and 'down' cue icons
	static BBitmap* sfupIcon;
	static BBitmap* sfdownIcon;
		// Type information
	static bool sfisVisual;
	static bool sfisAudible;
	static bool sfcanStretch;
		
	// Specific Object Data
		// The supporting file
	entry_ref fentryRef;

	// Methods
		// All subclasses must define and call this function from 
		// their constructors. The method must define all the 
		// constant TCueData class members; they all start with 'sf'
	virtual void InitializeStaticMembers() = 0;
	
private:
	// Name used for fentryRef in a message
	static const char* kRefTagName;
	
	// Hide the copy ctor and assignment op.
	TCueData(const TCueData& rhs) {}
	const TCueData& operator==(const TCueData& rhs) {return *this;}
};

#endif		// !TCUEDATA_H
