// Copyright (C) Geom Software e.U, Bernhard Kornberger, Graz/Austria
//
// This file is part of the Fade2D library. The student license is free
// of charge and covers personal, non-commercial research. Licensees
// holding a commercial license may use this file in accordance with
// the Commercial License Agreement.
//
// This software is provided AS IS with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Please contact the author if any conditions of this licensing are
// unclear to you.
//
// Support: https://www.geom.at/contact/
// Project: https://www.geom.at/fade2d/html/
/// \file TriangleAroundVertexIterator.h

#pragma once
#include "common.h"
#include "Point2.h"
#include "Triangle2.h"
//#include "tools.h"

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

/// @private
inline
int incBy1( int num)
{
	++num;
	if(num>2) return 0;
	return num;
}

/// @private
inline
int incBy2( int num)
{
	--num;
	if(num<0) return 2;
	return num;
}

/** \brief Iterator for all triangles around a given vertex
 *
 * The `TriangleAroundVertexIterator` iterates over all triangles incident to a given vertex
 * (Point2 pointer) of a Fade_2D instance. The iteration can be performed in counterclockwise
 * order using `operator++()` or in clockwise order using `operator--()`.
 *
 * \image html umbrella.jpg "Left: The iterator visits the triangles in CCW or CW order. Right: When the iterator at a border vertex points to NULL, another increment/decrement moves it to the next triangle." width=40%
 * \image latex umbrella.eps "Left: The iterator visits the triangles in CCW or CW order. Right: When the iterator at a border vertex points to NULL, another increment/decrement moves it to the next triangle." width=8cm
 *
 * A usage example is:
 *
\code
 	Point2* pVtx=...; // A vertex in the triangulation
	TriangleAroundVertexIterator tavi(pVtx); // Iterator to traverse the incident triangles
    TriangleAroundVertexIterator taviStop(tavi); // Make a copy to remember the stopping triangle
    do
    {
        Triangle2* pT(*tavi);
        if(pT==NULL)
        {
        	// The iterator points to NULL because pVtx is a border vertex. A subsequent
        	// increment will move the iterator to the next triangle.
            std::cout<<"pT=NULL"<<std::endl;
            continue;
        }
        std::cout<<"pT="<<pT<<std::endl;
    } while(++tavi!=taviStop);
 \endcode
 * \see `Fade_2D::getIncidentTriangles()` is a simple method to retrieve all incident triangles.
 */
class CLASS_DECLSPEC  TriangleAroundVertexIterator
{
public:
/** \brief Constructor
 *
* Constructor
*
* @param pPnt_ The vertex whose incident triangles are traversed by the iterator.
*
* The iterator is initialized to point to an arbitrary triangle (not NULL).
*/
	explicit TriangleAroundVertexIterator(const Point2* pPnt_):pPnt(pPnt_),pTr(pPnt_->getIncidentTriangle()),pSavedTr(NULL)
	{
		if(pTr==NULL)
		{
			//flog("TriangleAroundVertexIterator::TriangleAroundVertexIterator(), created from an invalid point");
			FadeException fadeEx;
			throw fadeEx;
		}
		assert(pTr!=NULL);

	}
/** \brief Constructor
*
* @param pPnt_ The vertex whose incident triangles are traversed by the iterator.
* @param pTr_ The triangle to which the iterator initially points.
*/
	TriangleAroundVertexIterator(Point2* pPnt_, Triangle2* pTr_):pPnt(pPnt_),pTr(pTr_),pSavedTr(NULL)
	{
		assert(pTr!=NULL);
	}
/** \brief Copy constructor
*
* @param it The iterator to be copied.
*
* This constructor creates a copy of the iterator \p it.
*/
	TriangleAroundVertexIterator(const TriangleAroundVertexIterator& it) : pPnt(it.pPnt),pTr(it.pTr),pSavedTr(NULL)
	{
		assert(pTr!=NULL);
	}
/** \brief Assignment operator
*
* @param other The iterator to be assigned.
*
* This operator assigns the state of the iterator \p other to the current iterator.
*
*/
	TriangleAroundVertexIterator &operator=(const TriangleAroundVertexIterator& other)
	{
		pPnt=other.pPnt;
		pTr=other.pTr;
		pSavedTr=other.pSavedTr;
		return *this;
	}

/** \brief Move the iterator to the next triangle (counterclockwise order)
*
* Advances the iterator to the next triangle in counterclockwise (CCW) order.
*
* @warning If the iterator is at the border of a triangulation, `operator++()` can move
* the iterator beyond the triangulation, causing it to point to NULL. A subsequent call
* to `operator++()` will move the iterator to the next triangle in counterclockwise order.
*/
	TriangleAroundVertexIterator& operator++()
	{
//GCOUT<<"tavi++"<<std::endl;
		if(pTr==NULL)
		{
//GCOUT<<"pTr==NULL, calling loop"<<std::endl;
			loop();
			return *this;
		}

		int ccwIdx=incBy1(pTr->getIntraTriangleIndex(pPnt));
//GCOUT<<"ccwIdx="<<ccwIdx<<"now swapping saved="<<pSavedTr<<", pTr="<<pTr<<std::endl;;
		std::swap(pSavedTr,pTr);
		pTr=pSavedTr->getOppositeTriangle(ccwIdx);
//GCOUT<<"and pTr is now the opposite triangle of pSavedTr="<<*pSavedTr<<", namely "<<pTr<<std::endl;

		return *this;
	}

/** \brief Move the iterator to the previous triangle (clockwise order)
*
* Advances the iterator to the previous triangle in clockwise (CW) order.
*
* @warning If the iterator is at the border of a triangulation, `operator--()` can move
* the iterator beyond the triangulation, causing it to point to NULL. A subsequent call
* to `operator--()` will move the iterator to the next triangle in clockwise order.
*/
	TriangleAroundVertexIterator& operator--()
	{
		if(pTr==NULL)
		{
			loop();
			return *this;
		}
		int cwIdx=incBy2(pTr->getIntraTriangleIndex(pPnt));
		std::swap(pSavedTr,pTr);
		pTr=pSavedTr->getOppositeTriangle(cwIdx);
		return *this;
	}
/** \brief Compare the current iterator with another iterator
*
* Compares the center and the current triangle of the iterator and the given iterator (\p rhs).
*
* @return `true` if the iterators point to the same vertex and triangle, `false` otherwise.
*/
	bool operator==(const TriangleAroundVertexIterator& rhs)
	{
		return (pPnt==rhs.pPnt && pTr==rhs.pTr);
	}
/** \brief Compare the current iterator with another iterator
*
* Compares the center and the current triangle of the iterator and the given iterator (\p rhs).
*
* @return `true` if the iterators point to different triangles or vertices, `false` otherwise.
*/
	bool operator!=(const TriangleAroundVertexIterator& rhs)
	{
		return (pPnt!=rhs.pPnt || pTr!=rhs.pTr);
	}
/** \brief Returns the current triangle (or NULL)
*
* Dereferencing the iterator returns a pointer to the triangle currently pointed to by the iterator.
 *
* @warning This method might return NULL if the iterator is at the border of a triangulation.
*/
	Triangle2* operator*()
	{
		return pTr;
	}
/** \brief Preview the next triangle (CCW direction).
*
* This method returns the triangle the iterator would point to if it were incremented.
*
* @return A pointer to the next triangle (in counterclockwise direction), without modifying the current position.
*
* @warning This method might return NULL if the iterator is at the border of a triangulation.
*/

	Triangle2* previewNextTriangle()
	{
		TriangleAroundVertexIterator tmp(*this);
		++tmp;
		return *tmp;
	}
/** \brief Preview the previous triangle (CW direction).
*
* This method returns the triangle the iterator would point to if it were decremented.
*
* @return A pointer to the previous triangle (in clockwise direction), without modifying the current position.
* @warning This method might return NULL if the iterator is at the border of a triangulation.
*/
	Triangle2* previewPrevTriangle()
	{
		TriangleAroundVertexIterator tmp(*this);
		--tmp;
		return *tmp;
	}



protected:
	const Point2* pPnt;
	Triangle2 *pTr,*pSavedTr;
	/** @private (internal use)*/
	void loop()
	{
		assert(pTr==NULL && pSavedTr!=NULL);

		enum DIRECTION{DIRECTION_NONE,DIRECTION_BACK,DIRECTION_FWD};
		DIRECTION direction(DIRECTION_NONE);

		int axisIndex=pSavedTr->getIntraTriangleIndex(pPnt);

		if(pSavedTr->getOppositeTriangle(incBy2(axisIndex))!=NULL)
		{
			direction=DIRECTION_BACK;
		}
		if(pSavedTr->getOppositeTriangle(incBy1(axisIndex))!=NULL)
		{
			assert(direction==DIRECTION_NONE);
			direction=DIRECTION_FWD;
		}
		pTr=pSavedTr;
		if(direction==DIRECTION_FWD) while(*operator++()!=NULL); // fast forward
		if(direction==DIRECTION_BACK) while(*operator--()!=NULL); // rewind
		pTr=pSavedTr;
	}
};


} // (namespace)
