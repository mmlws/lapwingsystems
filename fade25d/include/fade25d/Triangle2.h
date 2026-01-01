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

/// @file Triangle2.h
#pragma once
#include "Point2.h"



#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

/** @brief Enumeration for the quality of the circumcenter computation. */
enum CircumcenterQuality
{
	CCQ_INIT, ///< Init value
	CCQ_INEXACT,  ///< Double precision computation, the result is accurate enough
	CCQ_EXACT, ///< Computation with multiple-precision arithmetic, the result is exact (apart from tiny quantization errors)
	CCQ_OUT_OF_BOUNDS ///< Computation with multiple-precision arithmetic, but the result is not representable with double precision coordinates
};


/** @brief Represents a triangle in a triangulation.
 *
 * Triangle2 represents a triangle in a Fade_2D triangulation.
 *
 * **Details:**
 * - A Triangle2 contains three pointers to its corners, each represented by a `Point2`. The corners are
 * counterclockwise ordered and can be retrieved using the intra-triangle indices 0, 1 and 2.
 * - A Triangle2 contains three pointers to its neighbor-triangles which are also retrieved through
 * the intra-triangle indices. The i-th neighbor triangle is the one opposite to the i-th corner.
 *
 * \image html  getNeig.png "Triangle2: The corners are counterclockwise; the i-th neighbor triangle is the one opposite to the i-th corner of a triangle" width=25%
 * \image latex getNeig.png "Triangle2: The corners are counterclockwise; the i-th neighbor triangle is the one opposite to the i-th corner of a triangle" width=6cm
 *
 * @see [A C++ example about accessing and drawing elements of a triangulation](https://www.geom.at/example2-traversing/)
  */
class CLASS_DECLSPEC  Triangle2
{
public:
	/** \brief Default constructor */
	Triangle2()
	{
		aVertexPointer[0]=NULL;
		aVertexPointer[1]=NULL;
		aVertexPointer[2]=NULL;
		aOppTriangles[0]=NULL;
		aOppTriangles[1]=NULL;
		aOppTriangles[2]=NULL;

	} // Never used!

	/** @brief Retrieves the specified corner of the triangle
	*
	*  Retrieves a `Point2`* corresponding to the triangle corner specified by the intra-triangle index \p idx.
	 *
	* \image html corners.jpg "The counterclockwise ordering of intra-triangle indices for the corners of a triangle" width=15%
	* \image latex corners.eps "The counterclockwise ordering of intra-triangle indices for the corners of a triangle" width=4cm
	 *
	* @param idx The intra-triangle index of the corner (0, 1, or 2).
	* @return A pointer to the corner at index \p idx of the triangle.
	*/
	Point2* getCorner(const int idx) const;

	/** @brief Retrieves all three triangle corners
	 *
	 * Retrieves the three corners of the Triangle2.
	 *
	 * @param[out] p0,p1,p2 Pointers to the three corners
	 */
	void getCorners(Point2*& p0,Point2*& p1,Point2*& p2) const;



/** @private (deprecated, kept for backward compatibility) */
std::pair<Point2,bool> getDual(bool bForceExact=false) const; // DEPRECATED, see getCircumcenter()

/**
 * @brief Get the circumcenter of the triangle.
 *
 * Returns the circumcenter (the center of the circumcircle) of the current triangle.
 *
 * @param [out] ccq Indicates the quality of the computed circumcenter and is one
 * of CCQ_INEXACT, CCQ_EXACT, and CCQ_OUT_OF_BOUNDS.
 * @param [in] bForceExact If `true`, forces exact computation with multiple-precision
 * arithmetic.
 *
 * \if DOC25D
 * @return The circumcenter of the Triangle2, with a z-coordinate of 0.0.
 * \else
 * @return The circumcenter of the Triangle2.
 * \endif
 *
* @attention The circumcenter of a nearly collinear triangle has extremely
 * large coordinates. While multiple-precision arithmetic is used for such cases,
 * the result might still exceed the precision limits of double-precision arithmetic.
 * In such scenarios, a finite point is returned, and \p ccq is set to CCQ_OUT_OF_BOUNDS.
 * To avoid these cases, insert four dummy vertices around the triangulation at coordinates
 * 10 times larger than the domain of the data points. This restricts the Voronoi cells
 * to a manageable range.
 */
Point2 getCircumcenter(CircumcenterQuality& ccq,bool bForceExact=false) const;



/**
 * @brief Get the barycenter of the triangle
 *
 * Computes and returns the barycenter the this triangle.
 *
 * @return The barycenter of the current triangle.
 */
	Point2 getBarycenter() const;


#if GEOM_PSEUDO3D==GEOM_TRUE
/**
 * @brief Get the normal vector of the triangle.
 *
 * Computes the normalized normal vector of the current triangle.
 *
 * @return The normalized normal vector.
 */
	Vector2 getNormalVector() const;



#endif
/**
 * @brief Get interior 2D angle
 *
 * Returns the interior 2D angle at the vertex specified by the intra-triangle index \p idx.
 *
 * @return The interior 2D angle at the vertex at the intra-triangle index \p idx
 *
 * \if DOC25D
 * @note This method ignores the z-coordinate.
 * \endif
 */
double getInteriorAngle2D(int idx) const;

#if GEOM_PSEUDO3D==GEOM_TRUE
/**
 * @brief Get the interior 2.5D angle.
 *
 * Returns the interior 2.5D angle at the vertex specified by the intra-triangle index \p idx.
 *
 * @return The interior 2.5D angle at the vertex at index \p idx.
 */
double getInteriorAngle25D(int idx) const;
#endif

/**
 * @brief Get the 2D area of the triangle.
 *
 * Computes the 2D area of the current triangle.
 *
 * @return The area of the current triangle in 2D.
 *
 * @note This method ignores the z-coordinate.
 */
	double getArea2D() const;




#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get the 2.5D area of the triangle.
 *
 * Computes the 2.5D area of the current triangle.
 *
 * @return the 2.5D area of the current triangle.
 *
 */
	double getArea25D() const;
#endif

/** \brief Get the neighbor triangle at the specified index.
 *
 * Retrieves the neighbor triangle opposite to the corner at the specified intra-triangle index \p idx.
 *
 * \image html getNeig.jpg "Neighbor triangles: The i-th neighbor triangle is the one opposite to the i-th corner of a triangle" width=25%
 * \image latex getNeig.eps "Neighbor triangles: The i-th neighbor triangle is the one opposite to the i-th corner of a triangle" width=6cm
 *
 *
 * @param idx The intra-triangle index specifying the opposite corner (0, 1 or 2)
 * @return The neighbor triangle at index \p idx, if any; `NULL` otherwise.
 */
	Triangle2* getOppositeTriangle(const int idx) const;


/** \brief Get the index of the specified corner
*
 * Retrieves the index of the triangle corner \p pCorner.
 *
* \image html getITI.png "The intra-triangle index of pCorner is 1." width=20%
* \image latex getITI.png "The intra-triangle index of pCorner is 1." width=5cm
*
* @param pCorner A pointer to a corner in the current triangle.
*
* @return The intra-triangle index (0,1 or 2) of @p pCorner in the current triangle.
*/

	int getIntraTriangleIndex(const Point2* pCorner) const;

/** \brief Get the index of the specified neighbor triangle
*
 * Retrieves the intra-triangle index of the neighbor triangle \p pNeig.
 *
* \image html getITI_t.png "The intra-triangle index of \p pNeig in the current triangle is 0." width=20%
* \image latex getITI_t.png "The intra-triangle index of \p pNeig in the current triangle is 0." width=5cm
*
* @param pNeig A neighbor triangle of the current triangle.
*
* @return The intra-triangle index of the triangle \p pNeig in the current triangle. This
* index corresponds to the intra-triangle index of the vertex opposite to \p pNeig in the current triangle.
*/
	int getIntraTriangleIndex(const Triangle2* pNeig) const;

/** \brief Get the index of the specified edge.
*
 * Retrieves the intra-triangle index of the edge defined by \p (p0,p1) in the current triangle.
 *
 * \image html getITI_e.png "Example: The edge defined by the vertex pair (p0, p1) corresponds to the intra-triangle index 0." width=15%
 * \image latex getITI_e.png "Example: The edge defined by the vertex pair (p0, p1) corresponds to the intra-triangle index 0." width=4cm
 *
 * @param p0,p1 Two corners specifying an edge of the current triangle. The order of \p p0 and \p p1 does not matter.
 *
 * @return The intra-triangle index of the edge \p (p0, p1) in the current triangle. This index corresponds
 * to the intra-triangle index of the vertex opposite to this edge.

*/

	int getIntraTriangleIndex(const Point2* p0,const Point2* p1) const;

/** \brief Get the squared 2D length of the specified edge.
 *
 * Computes the squared 2D length of the edge at index \p idx.
 *
 * @param idx The index of the edge for which the squared length is computed.
 * @return The squared 2D length of the edge at index \p idx.
 *
 * \if DOC25D
 * @note This method ignores the z-coordinate.
 * \endif
  */
	double getSquaredEdgeLength2D(int idx) const;

/** @private (deprecated name)
 */
	double getSquaredEdgeLength(int idx) const; // Deprecated, use getSquaredEdgeLength2D()

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get the squared 2.5D length of the specified edge.
 *
 * Computes the squared 2.5D length of the edge at index \p idx.
 *
 * @param idx The index of the edge for which the squared 2.5D length is computed.
 * @return The squared 2.5D length of the edge at index \p idx.
 *
  */
	double getSquaredEdgeLength25D(int idx) const;
#endif
/** \private (internal use only)
*
* @param idx is the index of the opposite corner in the current Triangle2.
* @param pTriangle is a pointer to the new neighbor triangle.
*/
	void setOppTriangle(const int idx, Triangle2* pTriangle);
/** \brief Set all corners
 *
 * Sets the three corner points. This method is mostly for internal use.
 *
 * @param pI,pJ,pK The corners to be assigned.
*/
	void setProperties( Point2* pI, Point2* pJ, Point2* pK);

/** \private (internal use only) */
	void clearProperties();

/** \private (internal use only) */
	void setPropertiesAndOppT(Point2* pI, Point2* pJ, Point2* pK,Triangle2* pNeig0,Triangle2* pNeig1,Triangle2* pNeig2);

/** \brief Set a corner.
 *
 * Sets the corner at the intra-triangle index \p idx. This method is mostly for internal use.
 *
 * @param idx The intra-triangle index (0, 1, or 2).
 * @param pVtx A pointer to a Point2 object
*/
	void setVertexPointer(const int idx, Point2* pVtx);

/** \brief Check if the specified vertex is a corner of the triangle.
 *
 * Checks if the specified vertex \p pVtx is a corner of the current triangle.
 *
 * @param pVtx A pointer to a Point2 object to search for
 * \return `true` if \p pVtx is a corner of the current Triangle2, `false` otherwise.
 */
	bool hasVertex(const Point2* pVtx) const;

/** \brief Check if the specified point coincides with a corner of the triangle.
 *
 * Checks if the specified point \p q coincides with any corner of the current triangle.
 *
 * @param q The Point2 object to search for.
 * \return `true` if \p q coincides with a corner of the current Triangle2, `false` otherwise.
 */
	bool hasVertex(const Point2& q) const;

/** \brief Check if an edge includes the specified point.
 *
 * Checks if the specified point \p q lies exactly on the edge at index \p idx of the current triangle.
 *
 * \return `true` if the point \p q lies exactly on the edge at index \p idx, `false` otherwise.
 *
 * @if DOC25D
 * @note This is a 2D test; it ignores the z coordinate.
 * @endif
 */
	bool hasOnEdge(int idx,const Point2& q) const;

/** \brief Check if the triangle includes the specified point.
 *
 * Checks if the specified point \p q is on the current triangle.
 *
 * @param q The point to test.
 * @return `true` if \p q lies on the current triangle, `false` otherwise.
 *
 * @if DOC25D
 * This is a 2D test; it ignores the z coordinate.
 * @endif
 */
	bool hasOn(const Point2& q) const;

/** \brief Get the index of the largest edge.
 *
 * Returns the index of edge with the largest 2D length.
 *
 * @return The index of the edge with the largest 2D length.
*/
	int getMaxIndex() const;

/** \brief Get the index of the smallest edge.
 *
 * Returns the intra-triangle index of the edge with the smallest 2D length.
 *
 * @return The intra-triangle index of the edge with the smallest 2D length.
*/
	int getMinIndex() const;

/** \brief Get the maximum squared 2D edge length.
 *
 * Computes and returns the largest squared 2D edge length among the triangle's edges.
 *
 * @return the largest squared 2D edge length.
 *
 * \if DOC25D
 * @note This method ignores the z-coordinate.
 * \endif
*/
	double getMaxSqEdgeLen2D() const;

	/// @private
	void getCommonOffset(double& x,double& y) const;

/** \brief Output operator for Triangle2.
 *
 * Prints a Triangle2 object to an output stream.
 *
 * @param stream The output stream to which the Triangle2 object is written.
 * @param c The Triangle2 object to be written to the stream.
 * @return A reference to the output stream after writing the Triangle2 object.
 */
	CLASS_DECLSPEC
	friend std::ostream &operator<<(std::ostream &stream, const Triangle2& c);
	/// @private
	friend inline void registerTriangles(Triangle2* fromTriangle,int ith,Triangle2*  toTriangle,int jth);

protected:
	/// @private
	double computeArea(double l0,double l1,double l2) const;
	/// @private
	bool getCC_inexact(double avgOffX,double avgOffY,Point2& cc) const;
	/// @private
	Point2* aVertexPointer[3];
	/// @private
	Triangle2* aOppTriangles[3];
	//bool bState;
};

namespace{
/// @private
inline bool checkRange(int ith)
{
	return (ith==0 || ith==1 || ith==2); // true if ith={0,1,2}
}

}
inline Triangle2* Triangle2::getOppositeTriangle(const int ith) const
{
	assert(checkRange(ith));
	return aOppTriangles[ith];
}

inline void Triangle2::setOppTriangle(const int ith, Triangle2* pNeig)
{
	assert(checkRange(ith));
	aOppTriangles[ith]=pNeig;
}


inline int Triangle2::getIntraTriangleIndex(const Point2* p0,const Point2* p1) const
{
	for(int i=0;i<3;++i)
	{
		int ici1((i+1)%3);
		int ici2((i+2)%3);
		if(	(aVertexPointer[ici1]==p0 && aVertexPointer[ici2]==p1) ||
			(aVertexPointer[ici1]==p1 && aVertexPointer[ici2]==p0) ) return i;
	}

	GCOUT<<"BUG: Triangle2::getIntraTriangleIndex failed for"<<std::endl;// COUTOK
	GCOUT<<*p0<<std::endl;// COUTOK
	GCOUT<<*p1<<std::endl;// COUTOK
	GCOUT<<*this<<std::endl;// COUTOK
	assert(false);
	return -1;
}

inline int Triangle2::getIntraTriangleIndex(const Point2* pVtx) const
{
#ifndef NDEBUG
	for(int i=0;i<3;++i)
	{
		if(aVertexPointer[i]==pVtx)
		{
			return i;
		}
	}
	assert(false);
#endif
	return ( (aVertexPointer[1]==pVtx) + 2*(aVertexPointer[2]==pVtx));
}


inline int Triangle2::getIntraTriangleIndex(const Triangle2* pTriangle) const
{
#ifndef NDEBUG
	// Just debug code
	for(int i=0;i<3;++i)
	{
		if(aOppTriangles[i]==pTriangle)
		{
			return i;
		}
	}
	assert(false);
#endif
	return ( (aOppTriangles[1]==pTriangle) + 2*(aOppTriangles[2]==pTriangle));
}



inline Point2* Triangle2::getCorner(const int ith) const
{
	assert(checkRange(ith));
	return aVertexPointer[ith];
}

inline void Triangle2::getCorners(Point2*& p0,Point2*& p1,Point2*& p2) const
{
	p0=aVertexPointer[0];
	p1=aVertexPointer[1];
	p2=aVertexPointer[2];
}

inline void Triangle2::setVertexPointer(const int ith, Point2* pp)
{
	aVertexPointer[ith]=pp;
}

inline void Triangle2::setProperties( Point2* pI, Point2* pJ, Point2* pK)
{
	assert((pI!=NULL && pJ!=NULL && pK!=NULL));
	aVertexPointer[0]=pI;
	aVertexPointer[1]=pJ;
	aVertexPointer[2]=pK;
	pI->setIncidentTriangle(this);
	pJ->setIncidentTriangle(this);
	pK->setIncidentTriangle(this);
	aOppTriangles[0]=NULL;
	aOppTriangles[1]=NULL;
	aOppTriangles[2]=NULL;
}

inline void Triangle2::clearProperties()
{
	for(int i=0;i<3;++i)
	{
		aVertexPointer[i]=NULL;
		aOppTriangles[i]=NULL;
	}
}

inline void Triangle2::setPropertiesAndOppT(
	Point2* pI, Point2* pJ, Point2* pK,
	Triangle2* pNeig0,Triangle2* pNeig1,Triangle2* pNeig2
	)
{
	assert((pI!=NULL && pJ!=NULL && pK!=NULL));
	aVertexPointer[0]=pI;
	aVertexPointer[1]=pJ;
	aVertexPointer[2]=pK;
	pI->setIncidentTriangle(this);
	pJ->setIncidentTriangle(this);
	pK->setIncidentTriangle(this);
	aOppTriangles[0]=pNeig0;
	aOppTriangles[1]=pNeig1;
	aOppTriangles[2]=pNeig2;
}

/// @private
inline void registerTriangles(Triangle2* pFromT,int ith,Triangle2* pToT,int jth)
{
	assert(checkRange(ith));
	assert(checkRange(jth));

	pFromT->aOppTriangles[ith]=pToT;
	pToT->aOppTriangles[jth]=pFromT;

}




} // (namespace)
