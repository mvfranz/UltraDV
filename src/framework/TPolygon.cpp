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
	fPointList = new BList();
		
	//	Copy data over
	for (int32 index = 0; index < numPoints; index++)
	{
		BPoint tmpPt = ptArray[index];
		fPointList->AddItem(&tmpPt);
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
	fPointList = new BList();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a rect
//

TPolygon::TPolygon( BRect rect )
{	
	fPointList = new BList();
	
	//	Copy data over
	BPoint tmpPt;
	
	tmpPt.Set(rect.left, rect.top);
	fPointList->AddItem(&tmpPt);
	tmpPt.Set(rect.right, rect.top);
	fPointList->AddItem(&tmpPt);
	tmpPt.Set(rect.right, rect.bottom);
	fPointList->AddItem(&tmpPt);
	tmpPt.Set(rect.left, rect.bottom);
	fPointList->AddItem(&tmpPt);
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct form another polygon
//

TPolygon::TPolygon( TPolygon *polygon )
{
	fPointList = new BList();
	
	BList *ptList = polygon->GetPointList();
	
	//	Copy data over
	for (int32 index = 0; index < ptList->CountItems(); index++)
	{
		fPointList->AddItem( ptList->ItemAt(index) );
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
	for (int32 index = 0; index < fPointList->CountItems(); index++)
	{
		BPoint *thePt = (BPoint *)fPointList->ItemAt(index);
		delete thePt;		
	}
	
	delete fPointList;
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
	fPointList->AddItem(newPt);	
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
		fPointList->AddItem(&tmpPt);	
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
		
	if ( fPointList->CountItems() >= 4)
	{
		//	Copy data over
		BPoint topLeft 		= *(BPoint *)fPointList->ItemAt(0);
		BPoint topRight 	= *(BPoint *)fPointList->ItemAt(1);
		BPoint bottomRight 	= *(BPoint *)fPointList->ItemAt(2);
		BPoint bottomLeft 	= *(BPoint *)fPointList->ItemAt(3);
		
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
	
	for (int32 index = 0; index < fPointList->CountItems(); index++)
	{
		tmpPt = (BPoint *)fPointList->ItemAt(index);
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
	int32 numVerts = fPointList->CountItems();

	vertex0 = (BPoint *)fPointList->ItemAt(numVerts - 1);
	vertex1 = (BPoint *)fPointList->ItemAt(0); 
	
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
		vertex1 = (BPoint *)fPointList->ItemAt(index); 
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
	if (fPointList->CountItems() > 0)
	{
		delete fPointList;
		fPointList = new BList();
	}
	
	//	Assign data
	BList *srcList = from.GetPointList();

	for (int32 index = 0; index < srcList->CountItems(); index++)
	{
		BPoint *thePt = (BPoint *)srcList->ItemAt(index);
		fPointList->AddItem(thePt);				
	}	
	
	return *this;
}