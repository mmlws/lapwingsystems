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



/** @file Voronoi2.h
 *
 * Voronoi diagram
 */

#pragma once
#include "common.h"
#include "VoroVertex2.h"
#include "VoroCell2.h"


#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


class Dt2; // FWD
class Voronoi2Impl; // FWD
class Visualizer2; // FWD

/** @brief Voronoi diagram
 *
 * \image html voronoi-diagram.png "Voronoi diagram" width=50%
 * \image latex voronoi-diagram.png "Voronoi diagram" width=0.5\textwidth

 * This class represents a Voronoi diagram which is the dual graph of a
 * Delaunay triangulation i.e.,
 *
 * - Each Voronoi cell contains the area closest to its site which is a Delaunay vertex.
 * - Each Voronoi edge has a dual edge in the Delaunay triangulation at an angle of 90 degrees to it.
 * - Each Voronoi vertex is the circumcenter of a Delaunay triangle.
 *
 * @note While the Voronoi diagram is the dual diagram of a Delaunay triangulation, it
 * is **NOT** dual to a Constrained Delaunay triangulation.
 *
 * \see https://en.wikipedia.org/wiki/Voronoi_diagram
 * \see [A C++ example on Voronoi diagrams](https://www.geom.at/example12-voronoi-diagram/)
*/
class CLASS_DECLSPEC Voronoi2
{
public:
	/// @private
	explicit Voronoi2(Dt2* pDt);
	/// @private
	~Voronoi2();
	/** \brief Get the Voronoi cells adjacent to a Voronoi edge
	*
	* This method retrieves the Voronoi cells adjacent to a Voronoi edge defined by two Voronoi vertices.
	*
	* @param[in] pVoroVtx0, pVoroVtx1 The Voronoi vertices that define the Voronoi edge.
	* @param[out] pCell0, pCell1 Pointers to store the adjacent Voronoi cells. If the command fails, they will be set to NULL.
	*
	* @return `true` if the command succeeds, i.e., if (pVoroVtx0, pVoroVtx1) represents a valid Voronoi edge;
	*         `false` otherwise.
	*/
	bool getVCellsAtVEdge(VoroVertex2* pVoroVtx0,VoroVertex2* pVoroVtx1,VoroCell2*& pCell0,VoroCell2*& pCell1);
	/** \brief Check if the Voronoi diagram is valid
	*
	* This method checks whether the Voronoi diagram is ready for use. The diagram is considered valid
	* as soon as there are at least 3 sites that are not collinear.
	*
	* @return `true` if the Voronoi diagram is valid, i.e., if there are at least 3 non-collinear sites;
	*         `false` otherwise.
	*/
	bool isValid() const;
	/** \brief Retrieve all Voronoi cells
	*
	* This method retrieves both finite and infinite Voronoi cells in the diagram.
	* The cells are returned in the provided vector.
	*
	* @param [out] vVoronoiCells A vector that will contain the Voronoi cells
	*                             after the method completes. Both finite and
	*                             infinite cells will be included.
	*/
	void getVoronoiCells(std::vector<VoroCell2*>& vVoronoiCells);

	/** \brief Retrieve the Voronoi cell of a specific site
	*
	* This method retrieves the Voronoi cell corresponding to a given site (a vertex in
	* the dual Delaunay triangulation).
	*
	* @param [in] pSite A `Point2` pointer to the Delaunay vertex for which the Voronoi cell is to be retrieved.
	*
	* @return A pointer to the Voronoi cell (\c VoroCell2) that contains the site \p pSite.
	*/

	VoroCell2* getVoronoiCell(Point2* pSite);

	/** \brief Draw the Voronoi diagram
	*
	* This method saves a graphical representation of the Voronoi diagram to
	* a PDF or PostScript file. The diagram can be customized by controlling
	* various options, such as whether to display Voronoi edges, cell colors,
	* sites, Delaunay triangles, and cell labels.
	*
	* @param [in] filename The output filename (must have a *.pdf or *.ps extension)
	* @param [in] bVoronoi Whether to draw the edges of the Voronoi diagram (default: true).
	* @param [in] bCellColors Whether to use background colors for the Voronoi cells (default: true).
	* @param [in] bSites Whether to draw the sites (default: true).
	* @param [in] bDelaunay Whether to draw the Delaunay triangles (default: true).
	* @param [in] bCellLabels Whether to display cell labels (default: false).
	*
	* This method automatically crops the viewport to twice the range of the sites.
	* Consequently, very large or infinite cells will appear clipped.
	*/
	void show(
				const char* filename,
				bool bVoronoi=true,
				bool bCellColors=true,
				bool bSites=true,
				bool bDelaunay=true,
				bool bCellLabels=false
				);


	/** \brief Draw the Voronoi diagram
	*
	* This method saves a graphical representation of the Voronoi diagram to
	* a PDF or PostScript file. The diagram can be customized by controlling
	* various options, such as whether to display Voronoi edges, cell colors,
	* sites, Delaunay triangles, and cell labels.
	*
	* @param [in] pVisualizer A pointer to a Visualizer2 object, which will be used to
	 * handle the graphical output.
	* @param [in] bVoronoi Whether to draw the edges of the Voronoi diagram (default: true).
	* @param [in] bCellColors Whether to use background colors for the Voronoi cells (default: true).
	* @param [in] bSites Whether to draw the sites (default: true).
	* @param [in] bDelaunay Whether to draw the Delaunay triangles (default: true).
	* @param [in] bCellLabels Whether to display cell labels (default: false).
	*
	* This method automatically crops the viewport to twice the range of the sites.
	* Consequently, very large or infinite cells will appear clipped. You must
	 * call \p pVisualizer->writeFile() to finalize and write the output file.
	*/
	void show(
				Visualizer2* pVisualizer,
				bool bVoronoi=true,
				bool bCellColors=true,
				bool bSites=true,
				bool bDelaunay=true,
				bool bCellLabels=false
				);
	/** \brief Locate a Voronoi Cell
	*
	* This high-performance method locates the Voronoi cell that contains
	* the given query point.
	*
	* @param [in] queryPoint The query point to locate within the Voronoi diagram.
	*
	* @return The Voronoi cell that contains the \p queryPoint, or NULL if
	*         the Voronoi diagram is invalid.
	*/
	VoroCell2* locateVoronoiCell(const Point2& queryPoint);
	/** \brief Get the Voronoi vertex of a triangle
	*
	* This method returns the Voronoi vertex corresponding to the
	* Delaunay triangle \p pT. The Voronoi vertex is located at the
	* circumcenter of the triangle.
	*
	* @param [in] pT The Delaunay triangle whose Voronoi vertex is to be retrieved.
	*
	* @return The Voronoi vertex corresponding to \p pT.
	*/
	VoroVertex2* getVoronoiVertex(Triangle2* pT) ;
protected:
	Voronoi2Impl* pImpl;
private:
	Voronoi2(Voronoi2&);
	Voronoi2& operator=(Voronoi2&);

};


} // (namespace)
