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

/// @file ConstraintGraph2.h

#pragma once
#include "Segment2.h"
#include "ConstraintSegment2.h"
#include "Edge2.h"
#include <map>


#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

// FWD
class Dt2;
class ConstraintSegment2;
class GeomTest;
class Visualizer2;
class Color;

/**
 * \brief A constraint graph is a collection of constraint edges (ConstraintSegment2).
 *
 * ConstraintGraph2 represents a graph formed by a collection of ConstraintSegment2
 * objects. It is used to represent fixed (constrained) edges in a Delaunay
 * triangulation.
 *
 * \see \ref Fade_2D::createConstraint()
 *
 * @image html constrained_delaunay.png "Constrained Delaunay triangulation"
 * @image latex constrained_delaunay.eps "Constrained Delaunay triangulation" width=10cm
 */
class ConstraintGraph2
{
public:
/// @internal Constructor
	CLASS_DECLSPEC
	ConstraintGraph2(
		Dt2* pDt2_,
		std::vector<ConstraintSegment2*>& vCSegments,
		ConstraintInsertionStrategy
		);

/// @internal Constructor
	CLASS_DECLSPEC
	ConstraintGraph2(
		Dt2* pDt2_,
		std::vector<ConstraintSegment2*>& vCSegments_,
		std::map<std::pair<Point2*,Point2*>,bool > mPPReverse,
		ConstraintInsertionStrategy cis_
	);

/// @internal
	void init(std::vector<ConstraintSegment2*>& vCSegments_); // Internal use

/// @internal
	bool isDualOwned(ConstraintSegment2* pCSeg) ; // Internal use


/**
 * \brief Checks if the constraint graph forms a closed polygon.
 *
 * This method checks if the current constraint graph forms a closed polygon.
 *
 * \return `true` if the constraint graph forms a closed polygon, `false` otherwise.
 *
 * \note This method does not check if the polygon is simple (i.e., has no self-intersections).
 */
	CLASS_DECLSPEC
	bool isPolygon() const;


/**
* \brief Checks if the segments of the ConstraintGraph2 are oriented.
*
* This method checks if the ConstraintGraph2 has been created with
* the parameter \p bOrientedSegments set to \c true or if it was
* automatically reoriented (which happens only for simple polygons).
*
* \return `true` if the segments are oriented, `false` otherwise.
*/
	CLASS_DECLSPEC
	bool isOriented() const;


/**
 * \brief Retrieves the vertices of the constraint segments.
 *
 * This method populates the provided vector with the vertices of the constraint
 * segments in the current `ConstraintGraph2`. The ConstraintGraph2 may be discontinuous,
 * and thus the vertices are returned in the format (a, b, b, c, c, d, ...), i.e.,
 * they may appear multiple times. The order of the vertices depends on
 * how the `ConstraintGraph2` was created:
 * - If the ConstraintGraph2 was created with the `bOrientedSegments` parameter set to `true`,
 *   the vertices will be returned in their original order.
 * - Otherwise, the segments may have been reordered as part of Fade_2D::createConstraint().
 *
 * If any constraint segments have been split, the split points will also be included
 * in the result. For example, if the `ConstraintSegment2(a, b)` was split
 * at vertex `x`, the resulting order of vertices would be (a, x, x, b, b, c, c, d, ...).
 *
 * \param vVertices A vector of `Point2*` that will be populated with the vertices
 * of the constraint segments.
 *
 * \see Zone2::getBorderEdges()
 */
	CLASS_DECLSPEC
	void getPolygonVertices(std::vector<Point2*>& vVertices) ;


/**
* \brief Gets the constraint insertion strategy.
*
* This method returns the `ConstraintInsertionStrategy` used for constructing the `ConstraintGraph2`.
*
* \return The `ConstraintInsertionStrategy` used for constructing the `ConstraintGraph2`.
*
* \note The only supported `ConstraintInsertionStrategy` is `CIS_CONSTRAINED_DELAUNAY`.
* All other possible values are deprecated in favor of newer techniques.
*/
	CLASS_DECLSPEC
	ConstraintInsertionStrategy getInsertionStrategy() const;

/** \brief Check if an edge is a ConstraintSegment2 of the current ConstraintGraph2
*
* This method checks whether the edge defined by the points `(p0, p1)` is a
 * ConstraintSegment2 in the current `ConstraintGraph2`. The order of the points
 * `(p0, p1)` does not matter.
 *
 * @return `true` if the edge `(p0, p1)` is a ConstraintSegment2 of the present
 * ConstraintGraph2; `false` otherwise.
*/
	CLASS_DECLSPEC
	bool isConstraint(Point2* p0,Point2* p1) const;

/**
 * \brief Checks if a `ConstraintSegment2` is part of the current `ConstraintGraph2`.
 *
 * This method checks if the given `ConstraintSegment2` is part of the current
 * `ConstraintGraph2`.
 *
 * \param pCSeg A pointer to the ConstraintSegment2 to check.
 * \return \c true if the ConstraintSegment2 is part of the graph, \c false otherwise.
 *
 * \note If a `ConstraintSegment2` has been split, it is no longer alive and
 * is not a member of the `ConstraintGraph2` anymore. However, its child segments, if
 * alive, are part of the `ConstraintGraph2`.
 */
 	CLASS_DECLSPEC
	bool isConstraint(ConstraintSegment2* pCSeg) const;

/**
 * \brief Visualizes the ConstraintGraph2 as a PostScript or PDF file.
 *
 * This method visualizes and saves the `ConstraintGraph2` to a PostScript
 * or PDF file, depending on the file extension.
 *
 * \param name The name of the output file.
 */
	CLASS_DECLSPEC
	void show(const char*  name);

/**
 * \brief Visualizes the `ConstraintGraph2` as a PostScript or PDF file.
 *
 * This method takes a pointer to a `Visualizer2` object and renders
 * the current `ConstraintGraph2`.
 *
 * \param pVis A pointer to a `Visualizer2` object.
 * \param color The color for the `ConstraintGraph2`.
 *
 * @note To finalize and save the visualization, you must call `pVis->writeFile()`
 * after using this method.
 */
	CLASS_DECLSPEC
	void show(Visualizer2* pVis,const Color& color);

/**
 * \brief Gets the original `ConstraintSegment2` objects.
 *
 * This method retrieves the original `ConstraintSegment2` objects used to construct
 * the `ConstraintGraph2`. As a result of subdivision, these may no longer be alive
 * and may have child segments recursively, i.e., the child segments may also have
 * their own child segments, and so on.
 *
 * \param vConstraintSegments_ A vector that will be populated with pointers to the
 * original `ConstraintSegment2` objects.
 */
 	CLASS_DECLSPEC
	void getOriginalConstraintSegments(std::vector<ConstraintSegment2*>& vConstraintSegments_) const;


/**
 * \brief Get the current `ConstraintSegment2` objects.
 *
 * This method returns the `ConstraintSegment2` objects in the current `ConstraintGraph2`,
 * which may include the original segments or their child segments if they have been split.
 *
 * @param[out] vConstraintSegments_ A vector of `ConstraintSegment2` pointers.
 */
	CLASS_DECLSPEC
	void getChildConstraintSegments(std::vector<ConstraintSegment2*>& vConstraintSegments_) const;

/// @private
bool dbg_hasDirection(ConstraintSegment2* pCSeg) const;

/// @private
void updateSplittedConstraintSegment(
		ConstraintSegment2* pCSeg,
		bool bUpdateCMGR);
/// @private
Dt2* getDt2();

/**
 * \brief Get the direct children of a ConstraintSegment2
 *
 * Retrieves the direct children of the `ConstraintSegment2` \p pParent,
 * if it has been subdivided. If \p pParent has not been split, both
 * \p pChild0 and \p pChild1 will be set to \c NULL.
 *
 * \param [in] pParent A `ConstraintSegment2` that may have been split
 * into child segments.
 * \param [out] pChild0, pChild1 Used to return the direct children of \p pParent
 * in the order they appear in the current ConstraintGraph2. These will be set
 * to \c NULL if \p pParent has not been split.
 */
	CLASS_DECLSPEC
	void getDirectChildren(ConstraintSegment2* pParent,ConstraintSegment2*& pChild0,ConstraintSegment2*& pChild1);
/// @private
	void getAliveConstraintChain(std::vector<ConstraintSegment2*>& vAliveCSeg) ; // For debugging
/// @private
	void setDirectionsRecursive(const std::vector<ConstraintSegment2*>& vCSegments);
/**
* \brief Get the orientation of a `ConstraintSegment2`.
*
* A `ConstraintSegment2` `pCSeg` is unoriented because it may participate
* (with different orientations) in more than one `ConstraintGraph2` objects
* and thus the vertices returned by `pCSeg->getSrc()` and `pCSeg->getTrg()`
* do not carry any orientation information. However, the orientation of
* `pCSeg` is stored in the `ConstraintGraph2` objects where `pCSeg` is
* a member, and this method returns whether the source and target vertices
* need to be exchanged to match the present graph's direction.
*/
	CLASS_DECLSPEC
	bool isReverse(ConstraintSegment2* pCSeg) const;

/**
 * \brief Improves the triangle quality (makes Delaunay).
 *
 * Insertion of constraint segments can make a triangulation locally
 * non-Delaunay, meaning that the empty-circumcircle property does not
 * hold for some triangles. This method subdivides the `ConstraintSegment2`
 * objects of the current `ConstraintGraph2` so that they appear naturally
 * as part of the Delaunay triangulation. This leads to visually more
 * appealing triangles with better aspect ratios.
 *
 * \param [in] minLength A lower bound for `ConstraintSegment2` lengths.
 * `ConstraintSegment2`s shorter than `minLength` will not be subdivided further.
 * This parameter prevents excessive subdivision in geometrically narrow settings.
 *
 * \return `true` if all required subdivisions have been carried out to restore
 * the empty-circle property; `false` if further subdivision was prevented due
 * to the `minLength` parameter.
 */
	CLASS_DECLSPEC
	bool makeDelaunay(double minLength);

protected:
/// @private
	bool checkAndSortPolygon(std::vector<ConstraintSegment2*>& vCSegments_);
/// @private
	bool checkAndSortPolygonSub(std::vector<ConstraintSegment2*>& vCSegments);
/// @private
	void makeSelfOwner(std::vector<ConstraintSegment2*>& vCSeg);

	// Data
	Dt2* pDt2;
	GeomTest* pGeomPredicates;
	ConstraintInsertionStrategy cis;
	std::vector<ConstraintSegment2*> vCSegParents;
	bool bIsPolygon;
	std::map<ConstraintSegment2*,bool,func_ltDerefPtr<ConstraintSegment2*> > mCSegReverse;
	std::map<Point2*,size_t> mSplitPointNum;
	bool bIsOriented;

private:
	/// @private
	ConstraintGraph2& operator=(const ConstraintGraph2&);

};

} // (namespace)
