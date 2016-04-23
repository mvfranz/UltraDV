//---------------------------------------------------------------------
//
//	File:	TPolygon.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.24.98
//
//	Desc:	General purpose polygon
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <support/Debug.h>

#include "TPolygon.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct form list of points
//

TPolygon::TPolygon(const BPoint *ptArray, int32 numPoints)
{
	m_PointList = new BList();
		
	//	Copy data over
	for (int32 index = 0; index < numPoints; index++)
	{
		BPoint tmpPt = ptArray[index];
		m_PointList->AddItem(&tmpPt);
	}
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct an empty polygon
//

TPolygon::TPolygon()
{
	m_PointList = new BList();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a rect
//

TPolygon::TPolygon( BRect rect )
{	
	m_PointList = new BList();
	
	//	Copy data over
	BPoint tmpPt;
	
	tmpPt.Set(rect.left, rect.top);
	m_PointList->AddItem(&tmpPt);
	tmpPt.Set(rect.right, rect.top);
	m_PointList->AddItem(&tmpPt);
	tmpPt.Set(rect.right, rect.bottom);
	m_PointList->AddItem(&tmpPt);
	tmpPt.Set(rect.left, rect.bottom);
	m_PointList->AddItem(&tmpPt);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct form another polygon
//

TPolygon::TPolygon( TPolygon *polygon )
{
	m_PointList = new BList();
	
	BList *ptList = polygon->GetPointList();
	
	//	Copy data over
	for (int32 index = 0; index < ptList->CountItems(); index++)
	{
		m_PointList->AddItem( ptList->ItemAt(index) );
	}
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TPolygon::~TPolygon()
{
	// Clean up point array
	for (int32 index = 0; index < m_PointList->CountItems(); index++)
	{
		BPoint *thePt = (BPoint *)m_PointList->ItemAt(index);
		delete thePt;		
	}
	
	delete m_PointList;
}


#pragma mark -
#pragma mark === Utility Routines ===


//---------------------------------------------------------------------
//	AddPoint
//---------------------------------------------------------------------
//
//	Add point to Polygon point list
//

void TPolygon::AddPoint(BPoint thePt)
{	
	//	Make a copy of the point
	BPoint *newPt = new BPoint(thePt);
	
	//	Add it to the list
	m_PointList->AddItem(newPt);	
}


//---------------------------------------------------------------------
//	AddPoints
//---------------------------------------------------------------------
//
//	Add points in point array to Polygon point list
//

void TPolygon::AddPoints(BPoint *ptList, int32 numPoints)
{	
	for( int32 index = 0; index < numPoints; index++)
	{
		BPoint tmpPt = ptList[index];		
		m_PointList->AddItem(&tmpPt);	
	}		
}


//---------------------------------------------------------------------
//	GetRect
//---------------------------------------------------------------------
//
//	Return Polygon as defined by a BRect
//

void TPolygon::GetRect(BRect *theRect)
{	
		
	if ( m_PointList->CountItems() >= 4)
	{
		//	Copy data over
		BPoint topLeft 		= *(BPoint *)m_PointList->ItemAt(0);
		BPoint topRight 	= *(BPoint *)m_PointList->ItemAt(1);
		BPoint bottomRight 	= *(BPoint *)m_PointList->ItemAt(2);
		BPoint bottomLeft 	= *(BPoint *)m_PointList->ItemAt(3);
		
		theRect->left 	= topLeft.x;
		theRect->top 	= topLeft.y;
		theRect->right 	= bottomRight.x;
		theRect->bottom = bottomLeft.y;
	}	
}


//---------------------------------------------------------------------
//	GetBPolygon
//---------------------------------------------------------------------
//
//	Return BPolygon
//

BPolygon *TPolygon::GetBPolygon()
{			
	//	Create new BPolygon
	BPolygon *thePolygon = new BPolygon();
	
	BPoint *tmpPt;
	
	for (int32 index = 0; index < m_PointList->CountItems(); index++)
	{
		tmpPt = (BPoint *)m_PointList->ItemAt(index);
		thePolygon->AddPoints(tmpPt, 1);
	}
	
	return thePolygon;
}


//---------------------------------------------------------------------
//	Contains
//---------------------------------------------------------------------
//
//	Determine if point lies within polygon
//

bool TPolygon::Contains( BPoint mousePt)
{

// 	Shoot a test ray along +X axis.  The strategy is to compare vertex Y values 
//	to the testing point's Y and quickly discard edges which are entirely to one 
//	side of the test ray.  Note that CONVEX and WINDING code can be added 
//	as for the CrossingsTest() code; it is left out here for clarity. 
//
//	Input 2D polygon _pgon_ with _numverts_ number of vertices and test point 
//	_point_, returns 1 if inside, 0 if outside. 

	register int j, yflag0, yflag1, inside_flag; 
	BPoint		 *vertex0, *vertex1; 
	
	//	Get number of points in polygon
	int32 numVerts = m_PointList->CountItems();

	vertex0 = (BPoint *)m_PointList->ItemAt(numVerts - 1);
	vertex1 = (BPoint *)m_PointList->ItemAt(0); 
	
	//	Get test bit for above/below X axis
	yflag0 	= ( vertex0->y >= mousePt.y ); 
	
    inside_flag = 0;
    
    int32 index = 0;
    
    for (j = numVerts+1; --j; ) 
	{ 
		index++;
		
		yflag1 = ( vertex1->y >= mousePt.y );
		
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
            if ( ( (vertex1->y - mousePt.y) * (vertex0->x - vertex1->x) >= (vertex1->x- mousePt.x ) * (vertex0->y-vertex1->y)) == yflag1 ) 
			{ 
                inside_flag = !inside_flag ; 
            } 
        } 

		//	Move to the next pair of vertices, retaining info as possible
		yflag0 	= yflag1; 
		
		vertex0 = vertex1; 
		vertex1 = (BPoint *)m_PointList->ItemAt(index); 
    } 

	return( inside_flag ) ; 
} 

#pragma mark -
#pragma mark === Operators ===

//---------------------------------------------------------------------
//	operator =
//---------------------------------------------------------------------
//
//	Assign one polygon to the other
//

TPolygon &TPolygon::operator = (TPolygon &from)
{
	//	Free assignment polygon of any data
	if (m_PointList->CountItems() > 0)
	{
		delete m_PointList;
		m_PointList = new BList();
	}
	
	//	Assign data
	BList *srcList = from.GetPointList();

	for (int32 index = 0; index < srcList->CountItems(); index++)
	{
		BPoint *thePt = (BPoint *)srcList->ItemAt(index);
		m_PointList->AddItem(thePt);				
	}	
	
	return *this;
}