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


/** @file VoroVertex2.h
 *
 * Voronoi vertex
 */
#pragma once
#include "common.h"
#include "Triangle2.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class Voronoi2Impl; // FWD

/** @brief Voronoi vertex
 *
 * This class represents a Voronoi vertex, which is the circumcenter of
 * a triangle from the dual Delaunay triangulation.
 */
class CLASS_DECLSPEC VoroVertex2
{
public:
/// @private
	VoroVertex2(Voronoi2Impl* pImpl_,Triangle2* pT_);


	/** @brief Check if the Voronoi vertex is still valid
	 *
	 * As the Voronoi diagram evolves dynamically when points are inserted
	 * or removed from the dual Delaunay triangulation, the validity of a
	 * Voronoi vertex may change.
	 *
	 * @return `true` if the Voronoi vertex is still valid, `false` otherwise
	 */
	bool isAlive() const;


	/** @brief Get the Voronoi vertex as a Point2
	 *
	 * Retrieves the Voronoi vertex as a Point2 object.
	 *
	 * @return the Voronoi vertex represented as a Point2
	 */
	Point2 getPoint();

	/** @brief Retrieve the corresponding dual Delaunay triangle
	*
	* The Voronoi vertex is the circumcenter of a triangle in the dual
	* Delaunay triangulation. This method retrieves the Delaunay triangle
	* that corresponds to the current Voronoi vertex.
	*
	* @return A pointer to the Delaunay triangle corresponding to the current Voronoi vertex.
	*/
	Triangle2* getDualTriangle() const;
protected:
/// @private
	bool isSaved() const;
/// @private
	Voronoi2Impl* pImpl;
/// @private
	CircumcenterQuality ccq;
/// @private
	void computeCC(bool bForceExact);
/// @private
	Point2 voroPoint;
/// @private
	Triangle2* pT;
/// @private
	Point2* aDelVtx[3];

};


} // (namespace)


