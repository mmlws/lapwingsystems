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


/// @file Segment2.h
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



/**
 * \brief Represents a line segment between two points.
 *
 * The `Segment2` class defines a line segment between two points (`Point2` objects).
 */
class Segment2
{
protected:
	Point2 src; ///< The source point of the segment.
	Point2 trg; ///< The target point of the segment.
public:
	/**
	* \brief Create a Segment2 object with specified source and target points.
	*
	* This constructor initializes the segment using two given `Point2` objects as the source and target.
	*
	* \param src_ The first endpoint (source) of the segment.
	* \param trg_ The second endpoint (target) of the segment.
	*/
	CLASS_DECLSPEC
	Segment2(const Point2& src_,const Point2& trg_);
	/**
	 * \brief Default constructor for a Segment2 object.
	 *
	 * This constructor creates a default `Segment2` object with the coordinates of both the source and target
	 * points initialized to -DBL_MAX.
	 */
	CLASS_DECLSPEC
	Segment2();

	/**
	 * \brief Get the source point of the segment.
	 *
	 * This function returns the source point of the segment.
	 *
	 * \return The source point as a `Point2` object.
	 */
	CLASS_DECLSPEC
	Point2 getSrc() const;

	/**
	 * \brief Get the target point of the segment.
	 *
	 * This function returns the target point of the segment.
	 *
	 * \return The target point as a `Point2` object.
	 */
	CLASS_DECLSPEC
	Point2 getTrg() const;

	/**
	 * \brief Calculate the squared 2D length of the segment.
	 *
	 * This function calculates the squared 2D length of the segment.
	 *
	 * \return The squared 2D length of the segment.
	 *
	 * \if DOC25D
	 * This function does not consider the z-coordinate.
	 * \see getSqLen25D()
	 * \endif
	 */
	CLASS_DECLSPEC
	double getSqLen2D() const;

#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	 * \brief Calculate the squared 2.5D length of the segment
	 *
	 * This function calculates the squared 2.5D length of the segment.
	 *
	 * \return The squared length of the segment in 2.5D.
	 */
	CLASS_DECLSPEC
	double getSqLen25D() const;
#endif
	/**
	 * \brief Swap the source and target points of the segment.
	 *
	 * This function swaps the source and target points of the segment.
	 * After the operation, the previous target point becomes the source,
	 * and the previous source becomes the target.
	 */
	CLASS_DECLSPEC
	void swapSrcTrg();


	/**
	 * \brief Stream output operator for the Segment2 class.
	 *
	 * This operator prints a segment to an output stream.
	 *
	 * \param stream The output stream to which the segment is written.
	 * \param seg The `Segment2` object to output.
	 *
	 * \return The output stream with the segment data written to it.
	 */
	CLASS_DECLSPEC
	friend std::ostream &operator<<(std::ostream &stream, Segment2 seg);

	/**
	 * \brief Equality operator for the Segment2 class.
	 *
	 * This operator checks whether two segments are equal. This check is
	 * undirected, it compares the points regardless of their order.
	 *
	 * \param other The other `Segment2` object to compare.
	 * \return `true` if the two segments are equal, `false` otherwise.
	 */
	CLASS_DECLSPEC
	bool operator==(const Segment2& other) const;

	/**
	 * \brief Less-than operator for comparing two segments lexicographically.
	 *
	 * This operator does not compare the lenghts but the source and target points for ordering purposes.
	 *
	 * \param other The other `Segment2` object to compare.
	 * \return `true` if the current segment is considered less than the other segment, `false` otherwise.
	 */
	bool operator<(const Segment2& other) const
	{
		if(src<other.src) return true;
		if(src>other.src) return false;
		return trg<other.trg;
	}


};



/** private */
// Comparator class
struct CLASS_DECLSPEC Func_compareSegment
{
	bool operator()(const Segment2* seg0,const Segment2* seg1) const
	{
		if(seg0->getSrc()<seg1->getSrc()) return true;
		if(seg0->getSrc()>seg1->getSrc()) return false;
		if(seg0->getTrg()<seg1->getTrg()) return true;
		if(seg0->getTrg()>seg1->getTrg()) return false;
		return false;
	}
};

} // (namespace)
