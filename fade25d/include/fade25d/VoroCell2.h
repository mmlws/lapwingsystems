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

/** @file VoroCell2.h
 *
 * Voronoi cell
 */

#pragma once
#include "common.h"
#include "Point2.h"
#include "Bbox2.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class Voronoi2Impl; // FWD
class VoroVertex2; // FWD

/** @brief Voronoi cell
 *
 * This class represents a Voronoi cell. A Voronoi cell corresponds to a certain site,
 * which is a vertex of the dual Delaunay triangulation.
 */
class CLASS_DECLSPEC VoroCell2
{
public:
	/// @private
	VoroCell2(Point2* pSite_,Voronoi2Impl* pOwner_);
	/** @brief Set custom cell-index
	 *
	 * Use this method to associate Voronoi cells with your own
	 * data structures or to assign labels for visualization.
	 *
	 * @param customCellIndex_ an arbitrary integer that will be assigned
	 *                         to the Voronoi cell.
	 *
	 * \see getCustomCellIndex()
	 */
	void setCustomCellIndex(int customCellIndex_);
	/** @brief Get custom cell-index
	 *
	 * Retrieves the custom cell index that was set previously.
	 * Returns -1 if no custom cell index has been set.
	 *
	 * @return the custom cell index.
	 *
	 * \see setCustomCellIndex()
	 */
	 int getCustomCellIndex() const;

	/** Get Voronoi vertices
	 *
	 * Used to retrieve the Voronoi vertices of the cell.
	 *
	 * @param [out] vVoroVertices contains VoroVertex2 objects of the current cell in counterclockwise order.
	 *
	 * @return  `true` if the cell is finite, `false` otherwise.
	 *
	 * \see getBoundaryPoints()
	 */
	bool getVoronoiVertices(std::vector<VoroVertex2*>& vVoroVertices) const;

	/** @brief Get boundary points
	 *
	 * This method retrieves the boundary points of the current Voronoi cell
	 * as `Point2` objects. The points are returned in counterclockwise order.
	 *
	 * @param [out] vPoints contains the boundary points in counterclockwise order.
	 * @param [out] pvInfiniteDirections an optional vector to store the directions
	 *                                    of the infinite Voronoi edges.
	 *                                    If the cell is finite, this parameter
	 *                                    is ignored. Default value: `NULL`
	 *
	 * @return `true` if the cell is finite, `false` otherwise.
	 *
	 * \see getVoronoiVertices()
	 */
	bool getBoundaryPoints(
							std::vector<Point2>& vPoints,
							std::vector<Vector2>* pvInfiniteDirections=NULL
							) const;

	/** @brief Get incident triangles
	*
	* Retrieves the Delaunay triangles that are incident to the site of the Voronoi cell.
	* These triangles are returned in counterclockwise order.
	*
	* @param [out] vIncTriangles a vector to store the Delaunay triangles incident to the site of the cell.
	*
	* @return `true` if the cell is finite, `false` otherwise.
	*/
	bool getIncidentTriangles(std::vector<Triangle2*>& vIncTriangles) const;
	/** @brief Get neighbor sites
	 *
	 * Retrieves the sites of the Voronoi cells adjacent to the current cell in
	 * counterclockwise order.
	 *
	 * @param [out] vSites a vector to store the sites of the neighbor cells.
	 *
	 * @return `true` if the cell is finite, `false` otherwise.
	 */
	bool getNeighborSites(std::vector<Point2*>& vSites) const;

	/** @brief Get adjacent Voronoi cells
	 *
	 * Retrieves the Voronoi cells adjacent to the current cell.
	 * The cells are returned in counterclockwise order.
	 *
	 * @param [out] vAdjacentCells a vector to store the adjacent Voronoi cells.
	 *
	 * @return `true` if the cell is finite, `false` otherwise.
	 */
	bool getAdjacentVCells(std::vector<VoroCell2*>& vAdjacentCells) const;

	/** @brief Is finite cell
	 *
	 * Checks if the current Voronoi cell is finite.
	 *
	 * @return `true` if the cell is finite, `false` otherwise.
	 */
	bool isFinite() const;

	/** @brief Get site
	 *
	 * Retrieves the site associated with the current Voronoi cell.
	 * The site is also a vertex of the dual Delaunay triangulation.
	 *
	 * @return a pointer to the site `Point2`.
	 */
	Point2* getSite() const;

	/** @brief Get the centroid and area
	 *
	 * Retrieves the centroid of the Voronoi cell and its area.
	 * If the cell is finite, the area is computed and returned.
	 * If the cell is infinite, -1.0 is returned for the area.
	 *
	 * @param [out] centroid the centroid of the Voronoi cell.
	 *
	 * @return the area of the Voronoi cell if it is finite, or -1.0 if it is infinite.
	 */
	double getCentroid(Point2& centroid) const;

	/** @brief Get the area
	 *
	 * Retrieves the area of the Voronoi cell. If the cell is finite,
	 * the area is computed and returned. If the cell is infinite, -1.0 is returned.
	 *
	 * @return the area of the Voronoi cell if it is finite, or -1.0 if it is infinite.
	 */
	double getArea() const;
	/// @private
	Voronoi2Impl* getOwner() const;

protected:
	/// @private
	Point2* pSite;
	/// @private
	Voronoi2Impl* pOwner;
	/// @private
	int customCellIndex;

};


} // (namespace)


