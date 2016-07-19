//---------------------------------------------------------------------
//
//	File:	TCuePosition.cpp
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

#include "TCuePosition.h"

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TCuePosition::TCuePosition()
{
}

TCuePosition::~TCuePosition()
{
}

TCuePosition::TCuePosition(BRect outline)
{
	Outline(outline);
}

//---------------------------------------------------------------------
//	Enclosure
//---------------------------------------------------------------------
//

BRect TCuePosition::Enclosure(bool transformed) const
{
	BRect r(fcorners[0], fcorners[0]);

	for (int i = 1; i < 4; i++) {
		if (fcorners[i].x < r.left)
			r.left = fcorners[i].x;
		if (fcorners[i].x > r.right)
			r.right = fcorners[i].x;
		if (fcorners[i].y < r.top)
			r.top = fcorners[i].y;
		if (fcorners[i].y > r.bottom)
			r.bottom = fcorners[i].y;
	}
	
	if (transformed) {
		r.left = TransformX(r.left);
		r.right = TransformX(r.right);
		r.top = TransformY(r.top);
		r.bottom = TransformY(r.bottom);
	}
	
	return r;
}


//---------------------------------------------------------------------
//	Contains
//---------------------------------------------------------------------
//

bool TCuePosition::Contains(BPoint pt, bool transformed) const
{
//	NOTE: this was lifted by gzr from some *unknown* source... mo 
//
// 	Shoot a test ray along +X axis.  The strategy is to compare vertex Y values 
//	to the testing point's Y and quickly discard edges which are entirely to one 
//	side of the test ray.  Note that CONVEX and WINDING code can be added 
//	as for the CrossingsTest() code; it is left out here for clarity. 
//
//	Input 2D polygon _pgon_ with _numverts_ number of vertices and test point 
//	_point_, returns 1 if inside, 0 if outside. 

	//	Get number of points in polygon
	int32 numVerts = 4;
	const BPoint* vertex0;
	const BPoint* vertex1;
	
	// Use transformed or non-transformed data, as appropriate
	BPoint xCorners[4];
	if (transformed) {
		xCorners[0] = TransformPt(fcorners[0]);
		xCorners[1] = TransformPt(fcorners[1]);
		xCorners[2] = TransformPt(fcorners[2]);
		xCorners[3] = TransformPt(fcorners[3]);

		vertex0 = &xCorners[numVerts - 1];
		vertex1 = &xCorners[0];
	}
	else {
		vertex0 = &fcorners[numVerts - 1];
		vertex1 = &fcorners[0];
	}
		
	//	Get test bit for above/below X axis
	bool yflag0 = ( vertex0->y >= pt.y ); 
    bool inside_flag = 0;
    for (int j = numVerts + 1; --j; ) 
	{ 
		bool yflag1 = ( vertex1->y >= pt.y );
		
		//	Check if endpoints straddle (are on opposite sides) of X axis 
		//	(i.e. the Y's differ); if so, +X ray could intersect this edge. 
		//	The old test also checked whether the endpoints are both to the 
		//	right or to the left of the test point.  However, given the faster 
		//	intersection point computation used below, this test was found to 
		//	be a break-even proposition for most polygons and a loser for 
		//	triangles (where 50% or more of the edges which survive this test 
		//	will cross quadrants and so have to have the X intersection computed 
		//	anyway).  I credit Joseph Samosky with inspiring me to try dropping 
		//	the "both left or both right" part of my code. 
        if ( yflag0 != yflag1 ) 
		{ 
			//	Check intersection of pgon segment with +X ray. 
			//	Note if >= point's X; if so, the ray hits it. 
			//	The division operation is avoided for the ">=" test by checking 
			//	the sign of the first vertex wrto the test point; idea inspired 
			//	by Joseph Samosky's and Mark Haigh-Hutchinson's different 
			//	polygon inclusion tests. 
            if (((vertex1->y - pt.y) * (vertex0->x - vertex1->x) >= 
            		(vertex1->x- pt.x ) * (vertex0->y-vertex1->y)) == yflag1) 
			{ 
                inside_flag = !inside_flag ; 
            } 
        } 

		//	Move to the next pair of vertices, retaining info as possible
		yflag0 = yflag1; 
		
		vertex0 = vertex1; 
		vertex1++;
    } 

	return( inside_flag ) ; 
}

//---------------------------------------------------------------------
//	Setting methods
//	TODO: sanity checks?
//---------------------------------------------------------------------
//

void TCuePosition::Registration(BPoint pt)
{
	fregistration = pt;
}

void TCuePosition::Corner1(BPoint pt)
{
	fcorners[0] = pt;
}

void TCuePosition::Corner2(BPoint pt)
{
	fcorners[1] = pt;
}

void TCuePosition::Corner3(BPoint pt)
{
	fcorners[2] = pt;
}

void TCuePosition::Corner4(BPoint pt)
{
	fcorners[3] = pt;
}

//---------------------------------------------------------------------
//	Outline
//---------------------------------------------------------------------
//

void TCuePosition::Outline(BRect outline)
{
	// Use the rect for the four corners
	Corner1(outline.LeftTop());
	Corner2(outline.RightTop());
	Corner3(outline.RightBottom());
	Corner4(outline.LeftBottom());

	// Set the registration in the center
	BPoint center(outline.left + outline.Width() / 2, outline.top +
			outline.Height() / 2);
	Registration(center);
}

//---------------------------------------------------------------------
//	TransformMatrix
//---------------------------------------------------------------------
//

void TCuePosition::TransformMatrix(float m[][3], bool doRegistration)
{
	// TODO: do this
}

//---------------------------------------------------------------------
//	Transform Point
//---------------------------------------------------------------------
//

BPoint TCuePosition::TransformPt(BPoint pt, bool useRegistrationMatrix) const
{
	// TODO: do this
	return pt;
}

float TCuePosition::TransformX(float x, bool useRegistrationMatrix) const
{
	// TODO: do this
	return x;
}

float TCuePosition::TransformY(float y, bool useRegistrationMatrix) const
{
	// TODO: do this
	return y;
}

