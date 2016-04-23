//---------------------------------------------------------------------
//
//	File:	TSorterContainer.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSORTERCONTAINER_H__
#define __TSORTERCONTAINER_H__ 

// Constants
const int16 kMinSorterWidth 	= 40;
const int16 kSorterMiniHeight 	= 16;
const int16 kSorterLargeHeight 	= 32;
const int16 kSorterRowSpacer	= 5;
const int16 kSorterIconOffset 	= 15;
const int16 kSorterFontSize 	= 10;
const int16 kSorterTextOffset 	= 5;


// Sorter Types
typedef enum
{
	kNameSorter,
	kDateSorter,
	kPathSorter,
	kTypeSorter,
	kDurationSorter,
	kSizeSorter,
	kAudioInfoSorter,
	kVideoInfoSorter,
	kIconSorter
} SorterType;

typedef enum
{
	kUnknownType,
	kAudioType,
	kMIDIType,	
	kPictureType,
	kTextType,
	kVideoType
} DataType;

// Structures
typedef enum
{
	kSorterMiniMode,
	kSorterLargeMode
} SorterIconMode;


typedef struct
{
	BBitmap *m_MiniIcon;
	BBitmap *m_LargeIcon;
} TSorterIcons;

// Forward declarations
class TSorterList;
class TSorterBar;

// Class Declarations
class TSorterContainer: public BView
{
	public:
		// Member functions
		TSorterContainer(BRect bounds, char *title, SorterType theType, int16 theID);
		~TSorterContainer();		
		
		void 	Draw(BRect updateRect);				
		void 	MessageReceived(BMessage *theMessage);		
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		
		void 	DeleteItem( int32 theItem );
			
		void 	MakeActive(bool theState);
		
		void 	Synchronize(TSorterContainer *syncSource);
		void 	Sort();
		
		// Inlines
		inline TSorterBar	*GetSorterBar(){ return m_SorterBar; }
		inline TSorterList	*GetSorterList(){ return m_SorterList; }
		inline bool			IsActive(){ return m_IsActive; }
		inline int16		GetID(){ return m_ID; }
		inline SorterType	GetType(){ return m_Type; }
		
		// Member variables	
		
		
	private:				
		// Member functions
		void 	Init();
		
		void 	HandleScrollVMessage(BMessage *theMessage);
				
				
		// Member variables			
		char 		*m_Title;
		int16		m_ID;	
		bool		m_IsActive;	
		BView		*m_ScrollArea;		
		TSorterBar	*m_SorterBar;
		TSorterList	*m_SorterList;
		SorterType	m_Type;	
};		

// Message
#define	SORTER_CLICK_MSG	'sCmG'
#define SORTER_RESIZE_MSG	'sRmG'
#define SORTER_SCROLL_V_MSG	'sCRv'
#define SORTER_SCROLL_H_MSG	'sCRh'
#define SORTER_INVOKE_MSG	'sINm'
#define SORTER_SELECT_MSG	'sSEm'
#define SORTER_REFS_MSG		'sRRm'

#endif
