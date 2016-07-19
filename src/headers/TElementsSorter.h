//---------------------------------------------------------------------
//
//	File:	TElementsSorter.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TElementsSorter__
#define __TElementsSorter__

// Constants
const int16 kSorterHeight = 17;

// Forward Declarations
class TSorterContainer;
class TSorterIconContainer;

// Class Declarations
class TElementsSorter: public BView
{
	public:
		// Member functions
		TElementsSorter(BRect bounds, TElementsView *parent);
		~TElementsSorter();
		void Draw(BRect updateRect);
		
		void MessageReceived(BMessage *theMessage);
		
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		//void WindowActivated(bool state);
		//void KeyDown(const char *bytes, int32 numBytes);
		//void KeyUp(const char *bytes, int32 numBytes);
		
		void 	DeleteItem( int32 theItem );
		
		BRect 	GetScrollArea();
		
		// Member variables	
		
		// Inlines
		BList *GetSorterList(){ return fSorterList; }
		
	private:				
		// Member functions
		void 	Init();
				
		// Sorter Routines
		void 	HandleSorterClick(BMessage *theMessage);
		void 	HandleSorterResize(BMessage *theMessage);
		void 	HandleSorterScrollV(BMessage *theMessage);
		void 	HandleSorterSelect(BMessage *theMessage);
		void 	HandleSorterInvoke(BMessage *theMessage);
				
		// Utility Routines
		void 	SynchronizeLists(TSorterContainer *syncSource);
		
		// Ref Handling
		void 		HandleRefsMessage(BMessage *theMessage);
		void 		HandleRefMessage(BMessage *theMessage);
		status_t	EvaluateRef(entry_ref &ref);
		status_t 	HandleLink(entry_ref &theRef, struct stat &st);
		status_t 	HandleFile(entry_ref &ref, struct stat &st);
		status_t 	HandleDirectory(entry_ref &ref, struct stat &st, BDirectory &dir);
		status_t 	HandleVolume(entry_ref &ref, struct stat &st, BDirectory &dir);
		
		// Member variables		
		TElementsView		*fParent;
		
		TSorterContainer	*fIconSorter;		
		TSorterContainer	*fNameSorter;
		TSorterContainer	*fDateSorter;
		TSorterContainer	*fPathSorter;
		TSorterContainer	*fTypeSorter;
		TSorterContainer	*fDurationSorter;
		TSorterContainer	*fSizeSorter;
		TSorterContainer	*fAudioInfoSorter;
		TSorterContainer	*fVideoInfoSorter;
		
		int16				fLastSorterClicked;
		BList 				*fSorterList;
};		

#endif
