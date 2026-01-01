// Copyright (C) Geom Software e.U, Bernhard Kornberger, Graz/Austria
//
// This file is part of the Fade2D library. The student license is free
// of charge and covers personal non-commercial research. Licensees
// holding a commercial license may use this file in accordance with
// the Commercial License Agreement.
//
// This software is provided AS IS with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Please contact the author if any conditions of this licensing are
// not clear to you.
//
// Author: Bernhard Kornberger, bkorn (at) geom.at
// http://www.geom.at

#if GEOM_PSEUDO3D==GEOM_TRUE
#pragma once
#include "common.h"
#include "Segment2.h"
#include "Zone2.h"

namespace GEOM_LOCATE3
{
	class RTree; // FWD
}

namespace GEOM_FADE25D {


/**
 * @enum RSQuality
 * @brief Defines the precision/correctness levels for ray shooting.
 */
enum RSQuality
{
	/// Uses double precision arithmetic which is very fast and usually accurate.
	/// However, intersections may be missed and numerical errors can occur in edge cases such as:
	/// - intersections very close to or exactly on triangle edges,
	/// - nearly collinear triangles,
	/// - or when the ray and triangle are almost coplanar.
	RSQ_DOUBLE_PRECISION,
	/// Computes the result exactly: no intersections are missed, and the computed
	/// intersections are as precise as representable with double precision numbers.
	/// This mode is slower than RSQ_DOUBLE_PRECISION but still performs efficiently.
	///
	/// Recommended if exact correctness is required and minor additional computation time is acceptable.
	RSQ_EXACT
};

/**
 * @class RayShooter
 * @brief Provides fast queries for segment-triangle intersections.
 *
 * The RayShooter class provides fast intersection tests between query
 * segments and a set of triangles.
 */
class CLASS_DECLSPEC RayShooter
{
public:
	/**
	 * @brief Constructs a RayShooter with the given set of triangles.
	 * @param vT Vector of triangle pointers.
	 *
	 * The RayShooter does not take ownership of the triangles.
	 * The caller must ensure that the triangles remain valid as
	 * long as the RayShooter instance is used.
	 */
	explicit RayShooter(const std::vector<Triangle2*>& vT);
	/**
	 * @brief Destructor.
	 */
	~RayShooter();


	/**
	 * @brief Finds triangles that are intersected by the query segment.
	 * @param seg Query segment.
	 * @param vTriangles Output vector of intersected triangles.
	 */
	void getIntersectedTriangles(const Segment2& seg,RSQuality quality,std::vector<Triangle2*>& vTriangles);
	/**
	 * @brief Computes the intersection points between the query segment and the triangles.
	 * @param seg Query segment.
	 * @param vTrianglesAndIntersections Output vector of pairs (triangle, intersection point).
	 *
	 * @see getIntersections_mt()
	 */
	void getIntersections(const Segment2& seg,RSQuality quality,std::vector<std::pair<Triangle2*,Point2> >& vTrianglesAndIntersections);

#ifndef SKIPTHREADS
	/**
	 * @brief Multi-threaded version of getIntersections() for multiple query segments.
	 * @param vSeg Vector of query segments.
	 * @param vvTrianglesAndIntersections Output vector of results,
	 * one entry per query segment in the same order.
	 *
	 * @attention: Use setGlobalNumCPU(0) to enable multi-threading before using the method.
	 * @note: This method is available on platforms with support for C++11 or later.
	 */
	void getIntersections_mt(std::vector<Segment2>& vSeg,RSQuality quality,std::vector<std::vector<std::pair<Triangle2*,Point2>>>& vvTrianglesAndIntersections);
#endif

protected:
	/// @private Clip a segment to the bounding box of the data
	bool clipSegment(const Point2& pA, const Point2& pB,Point2& pOutA, Point2& pOutB);
	/// @private
	void getBoxIntersectors(const Segment2& seg,std::vector<Triangle2*>& vBoxIntersectors);

	/// @private
	GEOM_LOCATE3::RTree* pRT3;
	/// @private Numeric uncertainty
	double unc,unc3,unc6;
	/// @private Bounding box of the data
	double xmin,ymin,zmin,xmax,ymax,zmax,rangex,rangey,rangez;
	/// @private Step widths
	double xStep,yStep,zStep;
};

} // Namespace

#endif