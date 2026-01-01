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
/// @file Edge2.h
#pragma once

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class Triangle2; // FWD
class Point2; // FWD

/**
 * \brief Represents a directed edge in a triangulation.
 *
 * An `Edge2` defines a directed edge, represented by a
 * - `Triangle2` object and
 * - an intra-triangle index specifying the edge within `pT`.
 *
 * The orientation of the `Edge2` is always counterclockwise (CCW) with respect to the `Triangle2`.
 *
 * @image html Edge2.png "An Edge2, defined by a Triangle2 and an opposite index 0; the orientation is counterclockwise with respect to the the Triangle2." width=30%
 * @image latex Edge2.png "An Edge2, defined by a Triangle2 and an opposite index 0; the orientation is counterclockwise with respect to the the Triangle2." width=0.45\textwidth

 */
class CLASS_DECLSPEC  Edge2
{
public:
	/**
	 * \brief Default constructor.
	 */
	Edge2();
	/**
	 * \brief Copy constructor.
	 *
	 * \param e The edge to copy.
	 */
	Edge2(const Edge2& e);
	/**
	 * \brief Constructs an Edge2 from a Triangle2 and an intra-triangle-index
	 *
	 * \param pT The Triangle2 from which the edge is constructed.
	 * \param oppIdx_ The index of the opposite vertex in the triangle.
	 *
	 * The edge is oriented counterclockwise (CCW) with respect to \p pT.
	 * For example, `Edge2(pT, 0)` constructs an edge from `pT->getCorner(1)` to `pT->getCorner(2)`.
	 */
	Edge2(Triangle2* pT,int oppIdx_);

	/**
	 * \brief Assignment operator.
	 *
	 * Assignment operator.
	 *
	 * \param other The edge to assign from.
	 * \return A reference to this edge.
	 */
	Edge2& operator=(const Edge2& other);

	/**
	 * \brief Destructor.
	 */
	~Edge2();
	/**
	 * \brief Comparison operator for ordering edges (does not compare the lengths)
	 *
	 * The comparison not based on the lengths but on the associated Triangle2 pointers
	 * and intra-triangle indices.
	 *
	 * \param e The Edge2 to compare to.
	 * \return `true` if this Edge2 is considered "less" than the other edge.
	 */
	bool operator<(const Edge2& e) const
	{
		if(pT<e.pT) return true;
		if(pT>e.pT) return false;
		if(oppIdx<e.oppIdx) return true;
		return false;
	}
	/**
	 * \brief Equality operator for edges.
	 *
	 * The edges are considered equal if they have the same Triangle2 and
	 * intra-triangle index, i.e., they are compared as directed edges.
	 *
	 * \param e The edge to compare to.
	 * \return `true` if the edges are equal.
	 */
	bool operator==(const Edge2& e) const
	{
		return(pT==e.pT && oppIdx==e.oppIdx);
	}
	/**
	 * \brief Inequality operator for edges.
	 *
	 * The `Edge2` class represents a directed edge. This operator returns `true`
	 * if two `Edge2` objects are unequal, meaning they have different `Triangle2`
	 * objects or different intra-triangle indices.
	 *
	 * \param e The edge to compare to.
	 * \return `true` if the edges are different, `false` otherwise.
	 */
	bool operator!=(const Edge2& e) const
	{
		return((pT!=e.pT || oppIdx!=e.oppIdx));
	}

	/**
	 * \brief Get the source point of the edge.
	 *
	 * \return The source point of the edge, which is `pT->getCorner((oppIdx + 1) % 3)`.
	 */
	Point2* getSrc() const;

	/**
	 * \brief Get the target point of the edge.
	 *
	 * \return The target point of the edge, which is `pT->getCorner((oppIdx + 2) % 3)`.
	 */
	Point2* getTrg() const;
	/**
	 * \brief Get both endpoints of the edge.
	 *
	 * \param p1 The source point of the edge.
	 * \param p2 The target point of the edge.
	 */
	void getPoints(Point2*& p1,Point2*& p2) const;
	/**
	 * \brief Get the 2D length of the edge.
	 *
	 * @if DOC25D
	 * \return The 2D length of the edge, ignoring the z-coordinate.
	 * @endif
	 *
	 * @if DOC2D
	 * \return The 2D length of the edge.
	 * @endif
	 */
	double getLength2D() const;

#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	 * \brief Get the 2.5D length of the edge.
	 *
	 * \return The 2.5D length of the edge, considering also the z-coordinate.
	 */
	double getLength25D() const;
#endif

	/**
	 * \brief Get the Triangle2 associated with the edge.
	 *
	 * \return Get the Triangle2 associated with the edge.
	 */
	Triangle2* getTriangle() const;
	/**
	 * \brief Get the intra-triangle-index of the edge.
	 *
	 * \return The intra-triangle index of the opposite vertex in the triangle.
	 */
	int getIndex() const;
	/**
	 * \brief Get the two adjacent triangles of the edge.
	 *
	 * \param pT0 The first adjacent triangle.
	 * \param idx0 The intra-triangle index in the first adjacent triangle.
	 * \param pT1 The second adjacent triangle (or `NULL` if not present).
	 * \param idx1 The intra-triangle index in the second adjacent triangle (or -1 if not present).
	 *
	 */
	void getTriangles(Triangle2*& pT0,Triangle2*& pT1,int& idx0,int& idx1) const;

	/**
	 * \brief Stream output for `Edge2`.
	 *
	 * \param stream The output stream to write to.
	 * \param e The edge to output.
	 * \return The output stream.
	 */
	friend std::ostream &operator<<(std::ostream &stream, const Edge2& e);

protected:
	Triangle2* pT;
	int oppIdx;
};

/// @private
struct Func_ltUndirected
{
	bool operator()(const Edge2& eA,const Edge2& eB) const
	{
		Point2 *pA1,*pA2,*pB1,*pB2;
		eA.getPoints(pA1,pA2);
		eB.getPoints(pB1,pB2);
		if(pA1>pA2) std::swap(pA1,pA2);
		if(pB1>pB2) std::swap(pB1,pB2);

		if(pA1<pB1) return true;
		if(pA1>pB1) return false;
		return pA2<pB2;
	}
};
/// @private
inline bool sameEdge(const Edge2& eA,const Edge2& eB)
{
	return (    (eA.getSrc()==eB.getSrc() && eA.getTrg()==eB.getTrg()) ||
	            (eA.getSrc()==eB.getTrg() && eA.getTrg()==eB.getSrc()) );
}

/** @private Functor to sort edges by 2d length (ascending)
*/
struct Func_ltEdge2D
{
	bool operator()(const Edge2& e0,const Edge2& e1) const
	{
		if(e0.getLength2D()<e1.getLength2D()) return true;
			else return false;
	}
};
/** @private Functor to sort edges by 2d length (descending)
*/
struct Func_gtEdge2D
{
	bool operator()(const Edge2& e0,const Edge2& e1) const
	{
		if(e0.getLength2D()>e1.getLength2D()) return true;
			else return false;
	}
};


#if GEOM_PSEUDO3D==GEOM_TRUE
/** @private Functor to sort edges by 2.5d length (ascending)
*/
struct Func_ltEdge25D
{
	bool operator()(const Edge2& e0,const Edge2& e1) const
	{
		if(e0.getLength25D()<e1.getLength25D()) return true;
			else return false;
	}
};
#endif




} // (namespace)
