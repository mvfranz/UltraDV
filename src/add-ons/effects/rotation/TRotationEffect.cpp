//---------------------------------------------------------------------
//
//	File:	TRotationEffect.cpp
//
//	Author:	Gene Ragan (and Mike Ost)
//
//	Date:	10.12.98
//
//	Desc:
//
//	This is the effect module which can rotate a bitmap some number
//	of degrees.
//
//---------------------------------------------------------------------

// declarations
#include "TRotationEffect.h"

#include <support/Debug.h>		// for ASSERT()
#include "TCuePosition.h"		// for that class
#include "TRotateDialog.h"		// for that one

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TRotationEffect::TRotationEffect() :
	TTransformEffect(),
	m_dialogTarget(0),
	m_keyFramesDialog(0)
{
}

TRotationEffect::~TRotationEffect()
{
	if (m_keyFramesDialog) {
		m_keyFramesDialog->Quit();
		m_keyFramesDialog = 0;
	}
}

#pragma mark -
#pragma mark === File Support ===
	
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//

TRotationEffect::TRotationEffect(BMessage* msg) : 
	TTransformEffect(msg)
{	
}

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TRotationEffect::Instantiate(BMessage* archive) 
{ 	
	if (validate_instantiation(archive, "TRotationEffect"))
		return new TRotationEffect(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TRotationEffect::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = TTransformEffect::Archive(data, deep);
	if (err != B_OK)
		return err;
	
	// Add our class name to the archive
	return data->AddString("class", class_name(this));
}

#pragma mark -
#pragma mark === Static Class Behavior ===
	
//---------------------------------------------------------------------
//	Name
//---------------------------------------------------------------------
//

string TRotationEffect::Name() const
{
	// TODO: get from resource
	return "Rotation";
}

//---------------------------------------------------------------------
//	CueIcon
//---------------------------------------------------------------------
//

const BBitmap* TRotationEffect::CueIcon(bool up) const
{
	// TODO: get from resource
	return 0;
}

#pragma mark -
#pragma mark === Transformations ===

//---------------------------------------------------------------------
//	TransformBitmap
//---------------------------------------------------------------------
//

BBitmap* TRotationEffect::TransformBitmap(int32 time, const BBitmap* source,
		const TCuePosition& pos, EffectQuality quality)
{
	//	Get rotation value at 'time'
	float 	rotateDegrees = RotationDegrees(time);
	//	Make sure we even need to rotate
	if (rotateDegrees == 0)
		return 0;
	
	//	Calculate bounds rect to composite into
	BRect drawRect = source->Bounds();
			
	//	Calculate source dimensions
	const int32 srcColumns  = drawRect.Width();
	const int32 srcRows 	= drawRect.Height();

	//	Calculate rotation angle
    double angle 	= ((double) 3.14159265 / (double) 180.0) * (double) rotateDegrees;	// convert to RADs
	
	//	Calculate angle minus 90 degrees
	double	angle2 	= 1.570796327 - angle;
    
    //	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine	= sin(angle);

	// 	Calculate destination dimensions
	const int32 dstColumns 	= srcRows * fabs( cos(angle2) ) + srcColumns * fabs(cosine); 
	const int32 dstRows 	= srcRows * fabs( cosine ) + srcColumns * fabs( cos(angle2)); 

	//	Calculate new bitmap bounds
	BRect newArea(0, 0, dstColumns, dstRows);	
	
	//	Calculate draw pt based on center of bitmap
	int32 areaCenterX = newArea.Width()  / 2;
	int32 areaCenterY = newArea.Height() / 2;

	//	Get center point of drawRect
	int32 drawCenterX = drawRect.Width()  / 2;
	int32 drawCenterY = drawRect.Height() / 2;

	//	Calculate difference between center points
	int32 diffX = areaCenterX - drawCenterX;
	int32 diffY = areaCenterY - drawCenterY;

	//	Offset drawRect and set up drawPt
	drawRect.OffsetBy(-diffX, -diffY);
			
	//	Offset rect to proper drawing coordinates
	newArea.OffsetTo(drawRect.left, drawRect.top);		
	
	BBitmap* rotated = new BBitmap( newArea, B_RGB32, true, false);
				
	//	Do rotation
	switch(quality) 
	{
		case kWireframeQuality:
		case kPreviewQuality:
		case kBetterQuality:
		case kBestQuality: 
		{
			//	Create composite bitmap
			RotateBitmapNN(source, rotated, rotateDegrees, pos.Registration());
			break;
		}
		
		default:
			TRESPASS();
			delete rotated;
			rotated = 0;
			break;
	}		

	return rotated;
}

//---------------------------------------------------------------------
//	TransformPosition
//---------------------------------------------------------------------
//

bool TRotationEffect::TransformPosition(int32 time, TCuePosition* pos)
{
	//	Get rotation value at 'time'
	float rotateDegrees = RotationDegrees(time);
	//	Make sure we even need to rotate
	if (rotateDegrees == 0)
		return false;

    //	Calculate rotation angle in radians
   	double	angle 	= ((double) 3.14159265 / (double) 180.0) * (double) rotateDegrees;
   	
	//	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine 	= sin(angle);

	//	Precalculate points
	// TODO: doesn't this assume square corners? Is that OK?
	float cosLeft 	= pos->Corner1().x * cosine;
	float sinLeft 	= pos->Corner1().x * sine;
	float cosRight	= pos->Corner4().x * cosine;
	float sinRight	= pos->Corner4().x * sine;
	float cosTop	= pos->Corner1().y * cosine;
	float sinTop	= pos->Corner1().y * sine;
	float cosBottom	= pos->Corner4().y * cosine;
	float sinBottom	= pos->Corner4().y * sine;
	
	BPoint reg;
	reg.x = pos->Registration().x * (1 - cosine) + pos->Registration().y * sine;		
	reg.y = pos->Registration().x * sine + pos->Registration().y * (1 - cosine);
	
	// TODO: check for actual movement of the corners for the return value
	// TODO: make these change Corner() values instead of 'rect' values
	//	Top left
	BPoint thePoint;
	thePoint.x = cosLeft - sinTop + reg.x;
	thePoint.y = sinLeft + cosTop + reg.y;
	pos->Corner1(thePoint);
	
	//	Top right
	thePoint.x = cosRight - sinTop + reg.x;
	thePoint.y = sinRight + cosTop + reg.y;		
	pos->Corner2(thePoint);

	//	Bottom right
	thePoint.x = cosRight - sinBottom + reg.x;
	thePoint.y = sinRight + cosBottom + reg.y;		
	pos->Corner3(thePoint);

	//	Bottom left		
	thePoint.x = cosLeft - sinBottom + reg.x;
	thePoint.y = sinLeft + cosBottom + reg.y;		
	pos->Corner4(thePoint);

	return true;
}

#pragma mark -
#pragma mark === Edit Methods ===

//---------------------------------------------------------------------
//	MouseMovedPosition
//---------------------------------------------------------------------
//

bool TRotationEffect::MouseMovedPosition(BPoint pt, int32 time)
{
/* TODO:
	figure what the new rotation is based on mouse movement
	if time == -1
		update the state of both KFa and KFz
	else 
		if there is no key frame at 'time'
			create one
		set m_rotation
	if the edit dialog is up for the key frame at 'time'
		update its value as well
	return true if there was a change
*/
	return false;
}

//---------------------------------------------------------------------
//	EditKeyFrame
//---------------------------------------------------------------------
//

bool TRotationEffect::EditDefaults(BLooper* target)
{
	return EditKeyFramesDialog(target, FirstKeyFrame(), true);
}

bool TRotationEffect::EditKeyFrame(BLooper* target, TKeyFrameIterator kf)
{
	return EditKeyFramesDialog(target, kf, false);
}

bool TRotationEffect::EditKeyFramesDialog(BLooper* target, TKeyFrameIterator 
		kf, bool doingDefaults)
{
	// See if the dialog is already open.  If so, bring it to front
	if (m_keyFramesDialog) {
		m_keyFramesDialog->Activate(true);	
		return false;
	}

	// Hang on to the stuff we keep around while the dialog is up
	m_dialogTarget = target;
	m_keyFrameTarget = kf;
	m_editingDefaults = doingDefaults;
	const TRotationState* rs = RotationState(kf);
	m_keyFramesDialog = new TRotateDialog(this, rs->m_rotation);

	// Center it
	BScreen theScreen(B_MAIN_SCREEN_ID);
	BRect screenBounds = theScreen.Frame();
	BRect windBounds = m_keyFramesDialog->Frame();
	m_keyFramesDialog->MoveTo((screenBounds.Width() - windBounds.Width())/2, 
			(screenBounds.Height() - windBounds.Height())/2);

	// Show it
	m_keyFramesDialog->Show();
	return true;
	
	// The dialog will now call DialogValueChanged(), DialogIsClosing()
}

//---------------------------------------------------------------------
//	DialogValueChanged
//---------------------------------------------------------------------
//

void TRotationEffect::DialogValueChanged(bool update)
{
	ASSERT(m_keyFramesDialog);
	
	bool change = false;
		
	// update the state for the target key frame(s)
	TRotationState newState(m_keyFramesDialog->Rotation());
	if (KeyFrameState(m_keyFrameTarget, &newState)) 
		change = true;

	// Include the end frame if we are editing defaults
	if (m_editingDefaults) {
		TKeyFrameIterator kf = EndOfKeyFrames();
		kf--;
		if (KeyFrameState(kf, &newState))
			change = true;
	}

	if (change && update) {
		// tell the cue effect to update itself
		BMessage msg(CUEFX_UPDATE_MSG);
		msg.AddPointer("effect", &this);
		m_dialogTarget->PostMessage(&msg, m_dialogTarget);
	}
}


//---------------------------------------------------------------------
//	DialogIsClosing
//---------------------------------------------------------------------
//

void TRotationEffect::DialogIsClosing(bool okd)
{
	// clear out our variables. NOTE: the dialog is presumed deleted
	// by the BWindow::Quit method.
	m_dialogTarget = 0;
	m_keyFramesDialog = 0; 

	// If they cancelled out of an initialization, have the cue
	// destroy us.
	if (!okd && m_editingDefaults) {
		BMessage msg(CUEFX_CANCEL_MSG);
		msg.AddPointer("effect", &this);
		m_dialogTarget->PostMessage(&msg, m_dialogTarget);
	}
}

#pragma mark -
#pragma mark === Support Methods ===

//---------------------------------------------------------------------
//	RotationState
//---------------------------------------------------------------------
//

const TRotationEffect::TRotationState* TRotationEffect::RotationState(
		TCueEffect::TKeyFrameIterator kf) const
{
	return dynamic_cast<const TRotationState*>(KeyFrameState(kf));
}

//---------------------------------------------------------------------
//	NewEffectState
//---------------------------------------------------------------------
//

TEffectState* TRotationEffect::NewEffectState(void) const
{
	return dynamic_cast<TEffectState*>(new TRotationState());
}

//---------------------------------------------------------------------
//	Interpolate
//---------------------------------------------------------------------
//

void TRotationEffect::Interpolate(TKeyFrameIterator kf) const
{
	// Find the key frames which go before and after
	TKeyFrameIterator before = FirstKeyFrame();
	TKeyFrameIterator after = before;
	for ( ; after != EndOfKeyFrames(); after++) {
		if (after == kf)
			break;
		before++;
	}
	after++;
	
	ASSERT(before != kf);
	ASSERT(after != kf);
	ASSERT(after != EndOfKeyFrames());
	
	// Get the values for the neighbor key frames
	float prevRotation = RotationState(before)->m_rotation;
	float nextRotation = RotationState(after)->m_rotation;

	// Calculate the rotation and time differences
	float rDiff = (nextRotation - prevRotation);
// I thought STL implemented operator->...?
//	float tDiff = (kf->Time() - before->Time()) / (after->Time() - before->Time());
	float tDiff = ((*kf).Time() - (*before).Time()) / ((*after).Time() - (*before).Time());

	// Set the new state
	TRotationState rs(rDiff * tDiff + prevRotation);
	KeyFrameState(kf, &rs);
}


//---------------------------------------------------------------------
//	RotationDegrees
//---------------------------------------------------------------------
//

float TRotationEffect::RotationDegrees(int32 time) const
{
	int32 lastTime = 0;
	float lastRotation = 0.0;
	
	// Work through all the key frames, looking for 'time'
	for (TKeyFrameIterator kf = FirstKeyFrame(); kf != EndOfKeyFrames(); kf++) {
		const TRotationState* rs = RotationState(kf);

		// We hit right on a key frame
		if ((*kf).Time() == time)
			return rs->m_rotation;

		// We need to interpolate a value between two key frames
		if ((*kf).Time() > time) {
			float rDiff = (rs->m_rotation - lastRotation);
			float tDiff = (time - lastTime) / ((*kf).Time() - lastTime);
			return rDiff * tDiff + lastRotation;
		}
		
		lastTime = (*kf).Time();
		lastRotation = rs->m_rotation;
	}
	
	// Couldn't find 'time'
	ASSERT(false);
	return 0;
}

//---------------------------------------------------------------------
//	RotateBitmapNN
//---------------------------------------------------------------------
//

void TRotationEffect::RotateBitmapNN(const BBitmap *srcBitmap, BBitmap *dstBitmap, 
		float degrees, BPoint originPt)
{
    //	Calculate rotation angle
    double	angle 	= ((double) 3.14159265 / (double) 180.0) * (double) degrees;	// convert to RADs
	
	//	Calculate angle minus 90 degrees
	double	angle2 	= 1.570796327 - angle;
    
    //	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine	= sin(angle);
	float 	cosf 	= (float)cosine;
	float 	sinf 	= (float)sine;
    	
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();
	
	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows 	= dstBitmap->Bounds().Height();
	
	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();
	
	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();
	
	//	Calculate center points
	const int32 centerX = srcColumns / 2;
	const int32 centerY = srcRows / 2;
	
	const int32 xDiff = (dstColumns - srcColumns) / 2;
	const int32 yDiff = (dstRows - srcRows) / 2;
					
	//
	//	Inverse map the destination into the source
	//
	
	float floatX, floatY;
	int32 intX, intY;	
	
	uchar	*dstRowPtr;
	
	dstRowPtr = dstBits;
	
	//	Handle all rows in image
	for (int32 y = -1 * yDiff; y < (dstRows - yDiff); y++)
	{
		//	Handle all columns in image
		for( int32 x = -1 * xDiff; x < (dstColumns - xDiff); x++)
		{
			
			floatX = (x - centerX) * cosine + (y - centerY) * sine;
			floatX += centerX;
			
			floatY = (y - centerY) * cosine - (x - centerX) * sine;
			floatY += centerY;
			
			intX = (int32) floatX;
			intY = (int32) floatY;
			
			//	Clip the source pixel
			if ( (intX >= 0) && (intX < srcColumns) && (intY >= 0) && (intY < srcRows) )			
			{
				*(uint32 *)dstRowPtr = *(uint32 *)(srcBits + (intY * srcBPR) + (intX * srcIncrement) );
			}
			
			//	Increment
			dstRowPtr += dstIncrement;

		}
		
		dstRowPtr = (dstBits += dstBPR);
	}
}


//---------------------------------------------------------------------
//	RotateBitmapBilinear
//---------------------------------------------------------------------
//
//	Rotate using bilinear interpolation
//

void TRotationEffect::RotateBitmapBilinear(const BBitmap *srcBitmap, 
		BBitmap *dstBitmap, float degrees, BPoint originPt)
{

    //	Calculate rotation angle
    double	angle 	= ((double) 3.14159265 / (double) 180.0) * (double) degrees;	// convert to RADs
	
	//	Calculate angle minus 90 degrees
	double	angle2 	= 1.570796327 - angle;
    
    //	Calculate sine and cosine of rotation angle
	double 	cosine 	= cos(angle);
	double 	sine	= sin(angle);
	float 	cosf 	= (float)cosine;
	float 	sinf 	= (float)sine;
    	
	//	Calculate source dimensions
	const int32 srcColumns  = srcBitmap->Bounds().Width();
	const int32 srcRows 	= srcBitmap->Bounds().Height();
	
	// 	Calculate destination dimensions
	const int32 dstColumns 	= dstBitmap->Bounds().Width();
	const int32 dstRows 	= dstBitmap->Bounds().Height();
	
	//	Increment pointers to proper offset within the bitmap
	const int32 srcIncrement = GetBitmapPixelSize(srcBitmap->ColorSpace());
	const int32 dstIncrement = GetBitmapPixelSize(dstBitmap->ColorSpace());

	// 	Get pointer to bitmap bits for pointer arithmatic
	uchar *srcBits = (uchar *)srcBitmap->Bits();
	uchar *dstBits = (uchar *)dstBitmap->Bits();
	
	// 	Set up bytes per row constants
	const int32 srcBPR  = srcBitmap->BytesPerRow();
	const int32 dstBPR	= dstBitmap->BytesPerRow();
	
	//	Calculate center points
	const int32 centerX = srcColumns / 2;
	const int32 centerY = srcRows / 2;
	
	const int32 xDiff = (dstColumns - srcColumns) / 2;
	const int32 yDiff = (dstRows - srcRows) / 2;
					
	//
	//	Inverse map the destination into the source
	//
	
	float floatX, floatY;
	int32 intX, intY;	
	
	uchar	*dstRowPtr;
	
	dstRowPtr = dstBits;
	
	//	Handle all rows in image
	for (int32 y = -1 * yDiff; y < (dstRows - yDiff); y++)
	{
		//	Handle all columns in image
		for( int32 x = -1 * xDiff; x < (dstColumns - xDiff); x++)
		{
			
			floatX = (x - centerX) * cosine + (y - centerY) * sine;
			floatX += centerX;
			
			floatY = (y - centerY) * cosine - (x - centerX) * sine;
			floatY += centerY;
			
			intX = (int32) floatX;
			intY = (int32) floatY;
			
			//	Clip the source pixel
			if ( (intX >= 0) && (intX < srcColumns) && (intY >= 0) && (intY < srcRows) )			
			{
				*(uint32 *)dstRowPtr = *(uint32 *)(srcBits + (intY * srcBPR) + (intX * srcIncrement) );
			}
						
			//	Increment
			dstRowPtr += dstIncrement;

		}
		
		dstRowPtr = (dstBits += dstBPR);
	}
}

#pragma mark -
#pragma mark === TRotateState Methods ===

//---------------------------------------------------------------------
//	TRotateState Methods
//---------------------------------------------------------------------
//

// Constants
	// Names used in a message
const char* TRotationEffect::TRotationState::kRotationLabel = "rotation";

TRotationEffect::TRotationState::TRotationState() :
	m_rotation(0)
{
}

TRotationEffect::TRotationState::TRotationState(float rotation) :
	m_rotation(rotation)
{
}

TRotationEffect::TRotationState::TRotationState(BMessage* msg)
{
	msg->FindFloat(kRotationLabel, &m_rotation);
}

TRotationEffect::TRotationState::~TRotationState()
{}

BArchivable* TRotationEffect::TRotationState::Instantiate(BMessage* data)
{
	if (validate_instantiation(data, "TRotationState"))
		return new TRotationState(data); 
		
	return NULL; 
}

status_t TRotationEffect::TRotationState::Archive(BMessage* data, bool deep) const
{
	status_t err;
	err = data->AddString("class", class_name(this));
	if (err == B_OK)
		err = data->AddFloat(kRotationLabel, m_rotation);
	return err;
}

bool TRotationEffect::TRotationState::Equals(const TEffectState* s) const
{
	const TRotationState* r = dynamic_cast<const TRotationState*>(s);
	if (r == 0)
		return false;
	return m_rotation == r->m_rotation;
}

bool TRotationEffect::TRotationState::LessThan(const TEffectState* s) const
{
	const TRotationState* r = dynamic_cast<const TRotationState*>(s);
	if (r == 0)
		return false;
	return m_rotation < r->m_rotation;
}

TEffectState* TRotationEffect::TRotationState::NewCopy() const
{
	TRotationState* r = new TRotationState();
	r->m_rotation = m_rotation;
	return dynamic_cast<TEffectState*>(r);
}

bool TRotationEffect::TRotationState::Assign(const TEffectState* s)
{
	const TRotationState* r = dynamic_cast<const TRotationState*>(s);
	if (r != 0 && m_rotation != r->m_rotation) {
		m_rotation = r->m_rotation;
		return true;
	}

	return false;
}
