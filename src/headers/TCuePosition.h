#ifndef TCUEPOSITION_H
#define TCUEPOSITION_H
//---------------------------------------------------------------------
//
//	File:	TCuePosition.h
//
//	Author:	Mike Ost
//
//	Date:	10.13.98
//
//	Desc:
//
//	This class describes a cue's position, as it gets transformed
//	over time by the stage tool effects. It contains the cue's
//	registration point (which is rotated around), and its four
//	corner points, which may get skewed and stretched around, but
//	start off as a rectangle.
//
//---------------------------------------------------------------------

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations
#include "BuildApp.h"

#include <vector>

class TCuePosition {
public:
		// Construct and empty position, destroy
	TCuePosition();
	~TCuePosition();
		// Set the outline; registration is set in the center
	TCuePosition(BRect outline);
		// NOTE: use the compiler default bitwise copy ctor and assign op

	// INFO METHODS
	BPoint Registration(bool transformed = false) const	
		{ return transformed? TransformPt(m_registration, true) : m_registration; }
	BPoint Corner1(bool transformed = false) const
		{ return transformed? TransformPt(m_corners[0]) : m_corners[0]; }
	BPoint Corner2(bool transformed = false) const
		{ return transformed? TransformPt(m_corners[1]) : m_corners[1]; }
	BPoint Corner3(bool transformed = false) const
		{ return transformed? TransformPt(m_corners[2]) : m_corners[2]; }
	BPoint Corner4(bool transformed = false) const
		{ return transformed? TransformPt(m_corners[3]) : m_corners[3]; }
		// The smallest rectangle which contains all of the corners
	BRect Enclosure(bool transformed = false) const;
		// 'true' if a point is within the corners. If 'transformed'
		// test for containment in the transformed shape.
	bool Contains(BPoint pt, bool transformed = false) const;

	// SETTING METHODS
		// Set the indicated values. NOTE: values are not transformed.
	void Registration(BPoint pt);
	void Corner1(BPoint pt);		
	void Corner2(BPoint pt);		
	void Corner3(BPoint pt);		
	void Corner4(BPoint pt);		
		// Set the outline; registration is set in the center
	void Outline(BRect outline);

	// MATRIX METHODS
		// Transform the internal matrix according to the passed
		// in matrix.
	void TransformMatrix(float m[][3], bool doRegistration = true);

private:
	// Data
	BPoint m_registration;
	BPoint m_corners[4];
		// The transformation matrix to apply to m_corners
	float m_CornerMatrix[3][3];
		// The matrix for m_registration. This allows for 
		// a transformation to skip changing the reg pt. 
	float m_RegistrationMatrix[3][3];
	
	// Methods
		// Transform the point according to the current matrix
	BPoint TransformPt(BPoint pt, bool useRegistrationMatrix = false) const;
	float TransformX(float x, bool useRegistrationMatrix = false) const;
	float TransformY(float x, bool useRegistrationMatrix = false) const;
};
	
#endif		// !TCUEPOSITION_H
