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

#pragma once

#include "common.h"
#include "Point2.h"
#include <ostream>

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


/**
 * \brief An edge consisting of two vertex pointers.
 *
 * The `VertexPair2` struct can be thought of as an **oriented edge**, where the
 * `operator<()` compares it like an **unordered edge**, based on the pointer
 * values. It consists of two different `Point2` pointers, `p0` and `p1`, ensuring
 * `p0 < p1`. Despite this reordering, the original edge orientation is preserved
 * internally by the \c bReverse flag.
 */
struct CLASS_DECLSPEC VertexPair2
{
public:
/**
 * \brief Constructor that initializes the vertex pair with two different vertex pointers.
 *
 * This constructor takes two **different** `Point2` pointers, \p pSource and \p pTarget,
 * to initialize the members \c p0 and \c p1. If \c p0 > \c p1, the pointers are swapped,
 * and \c bReverse is set to \c true to indicate that the orientation is reversed
 * compared to the original order.
 *
 * \param[in] pSource Pointer to the source vertex.
 * \param[in] pTarget Pointer to the target vertex.
 */
	VertexPair2(Point2* pSource,Point2* pTarget):p0(pSource),p1(pTarget)
	{
		if(p0==p1)
		{
			GCOUT<<std::endl;
			GCOUT<<"VertexPair2::VertexPair2(),pSource==pTarget"<<std::endl;
			GCOUT<<"p0="<<p0<<", p1="<<p1<<std::endl;
			GCOUT<<"*p0="<<*p0<<", *p1="<<*p1<<std::endl;
			assert(p0!=p1);
		}
		if(p0>p1)
		{
			bReverse=true;
			std::swap(p0, p1);
		}
		else
		{
			bReverse=false;
		}
	}
	/** \brief Default constructor. */
	VertexPair2():p0(NULL),p1(NULL),bReverse(false)
	{
	}

	/**
	 * \brief Get the squared 2D length of the edge.
	 *
	 * Computes the squared 2D length of the edge. @if DOC25D The z-coordinate is ignored. @endif
	 *
	 * \return The squared 2D distance between *`p0` and *`p1`.
	 */
	double getSqLen2D() const
	{
		return sqDistance2D(*p0,*p1);
	}
#if GEOM_PSEUDO3D==GEOM_TRUE
/**
	 * \brief Get the squared 2.5D length of the edge.
	 *
	 * Computes the squared 2.5D length of the edge.
	 *
	 * \return The squared 2.5D distance between *`p0` and *`p1`.
	 */
	double getSqLen25D() const
	{
		return sqDistance25D(*p0,*p1);
	}
#endif

	/** \brief Equality operator.
	*
	* Checks if the vertex pointers of the other vertex pair are identical.
	*
	* \param other Another `VertexPair2` instance to compare against.
	* \return `true` if both pairs are identical, `false` otherwise.
	*/
	bool operator==(const VertexPair2& other) const
	{
		return (p0==other.p0 && p1==other.p1);
	}

	/**
	 * \brief Less-than operator.
	 *
	 * Compares two `VertexPair2` instances based on their vertex pointers.
	 *
	 * \param other Another `VertexPair2` instance to compare against.
	 * \return `true` if this pair of pointers is lexicographically less than the other pair.
	 */
	bool operator<(const VertexPair2& other) const
	{
		if(p0<other.p0) return true;
		if(p0>other.p0) return false;
		return (p1<other.p1);
	}


	/**
	* \brief Stream output operator for `VertexPair2`.
	*
	* Prints the VertexPair2.
	*
	* \param stream Output stream.
	* \param pr The `VertexPair2` instance to print.
	* \return The output stream.
	*/
	CLASS_DECLSPEC
	friend std::ostream &operator<<(std::ostream &stream, const VertexPair2& pr);

	/**
	* \brief Get the oriented source vertex pointer.
	*
	* Returns the source vertex pointer, determined by the \c bReverse flag.
	*
	* \return \c p0 if \c bReverse is \c false; \c p1 otherwise.
	*/
	Point2* getSrc() const
	{
		if(bReverse) return p1;
		else return p0;
	}

	/**
	* \brief Get the oriented target vertex pointer.
	*
	* Returns the target vertex pointer, determined by the \c bReverse flag.
	*
	* \return \c p1 if \c bReverse is \c false; \c p0 otherwise.
	*/
	Point2* getTrg() const
	{
		if(bReverse) return p0;
		else return p1;
	}
	// Data
	Point2* p0;///< The smaller vertex pointer
	Point2* p1;///< The larger vertex pointer
	bool bReverse;///< Flag indicating if the pointers were reversed.
};


inline std::ostream &operator<<(std::ostream &stream, const VertexPair2& pr)
{
	stream << "("<<pr.p0<<","<<pr.p1<<"), "<<*pr.getSrc()<<" -> "<<*pr.getTrg();
	return stream;
}

} // (namespace)
