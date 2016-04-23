//---------------------------------------------------------------------
//
//	File:	TPolygon.h
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


#ifndef __TPOLYGON_H__
#define __TPOLYGON_H__

 //	Class Definition
class TPolygon
{
	public:
		TPolygon(const BPoint *ptList, int32 numPoints);
		TPolygon();
		TPolygon(BRect rect);
		TPolygon( TPolygon *polygon);
		~TPolygon();
				
		//	Utility Routines
		void 		AddPoint(BPoint thePt);
		void 		AddPoints(BPoint *ptList, int32 numPoints);
		void		GetRect(BRect *theRect);
		BPolygon 	*GetBPolygon();
		bool 		Contains( BPoint mousePt);
		
		//	Operators
		TPolygon& operator = (TPolygon &frpm);

		//	Inlines
		inline BList *GetPointList(){ return m_PointList; }
		
	private:
	
	protected:
		BList	*m_PointList;		
};

#endif
