#if !defined(__TTRANSITIONEFFECT_H__)
#define __TTRANSITIONEFFECT_H__

//---------------------------------------------------------------------
//
//	File:	TTransitionEffect.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.11.98
//
//	Desc:
//
//---------------------------------------------------------------------

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations
#include "TVisualEffect.h"
#include "ResourceManager.h"
#include "TMuseumIcons.h"

class TCuePosition;

class TTransitionEffect : public TVisualEffect 
{
	public:
		TTransitionEffect(bool transitionIn);
		
		// Construct self from a message
		TTransitionEffect(BMessage* msg);
		
		// delete
		virtual ~TTransitionEffect();

		//----------------------
		// BArchivable overrides (file support)
		static BArchivable* Instantiate(BMessage* data);
		virtual status_t Archive(BMessage* data, bool deep) const;
		
		// Return a name to identify the effect by. This is used in 
		// the tool palette, in menus and within the cue view.
		string Name() const;
		
		const BBitmap* CueIcon(bool up) const;
	
		virtual BBitmap *TransformBitmap(uint32 time, const BBitmap* source,
								 const TCuePosition& registration, DisplayQuality quality);
		
		virtual BBitmap *Icon();
		
		//	Inlines
		inline bool TransitionIn(){ return m_TransitionIn; }

	protected:				
		// Have the subclass create a new effect state 
		TEffectState* NewEffectState() const;
		
		// Have the subclass interpolate a reasonable value given 
		// a key frame iterator. Use neighboring key frames to
		// figure out a good value. 
		void Interpolate(TKeyFrameIterator kf) const;
		
		void	InitOffscreen(BRect offRect);

		//	Member Variables
		bool	m_TransitionIn;
		BBitmap *m_OffscreenBitmap;
		BView	*m_OffscreenView;
		bool	m_Inited;			
};

#endif
