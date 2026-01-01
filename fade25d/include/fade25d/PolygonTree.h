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


/// @file PolygonTree.h
#pragma once

#include "common.h"
#include "Segment2.h"
#include "Edge2.h"
#include "VertexPair2.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif



/**
 * @brief Represents the possibly nested layers of a polygon as a hierarchical structure.
 *
 * The PolygonTree organizes nested polygon layers as a hierarchical, tree-like structure.
 *
 * @image html nested-polygon-layers.png "A nested polygon on the left, and the corresponding \c PolygonTree on the right" width=50%
 * @image latex nested-polygon-layers.png "A nested polygon on the left, and the corresponding \c PolygonTree on the right" width=0.8\textwidth
 *
 * **Details**
 * - **Hierarchical structure:** Nested polygons are represented in a tree format, where each layer
 * corresponds to a level in the hierarchy, progressing from the outermost to the innermost regions.
 * - **Layers:** The direct children of the root node represent layer 0, which corresponds to the
 * outermost polygons (potentially multiple in case of disjoint components). The leaf nodes of
 * the tree correspond to the innermost polygons.
 */
class CLASS_DECLSPEC PolygonTree
{
public:
	/**
     * @private Constructs a PolygonTree object
     *
     * Constructor
     *
     * @param layer_ The layer number this tree node represents.
     */
	explicit PolygonTree(int layer_);

    /** @private Destructor for the PolygonTree class.  */
	~PolygonTree();

	/**
	* @brief Retrieves the layer number of this PolygonTree node.
	*
	* Returns the layer number associated with the current PolygonTree node.
	*
	* @return The layer number of this node.
	*
	* **Details**
	* - The root node has a layer number of -1 and does not contain any segments.
	* - Direct children of the root (layer 0) represent the outermost segments.
	*   Multiple layer-0 nodes can exist for disjoint components of the polygon.
	* - Child nodes of layer-0 nodes correspond to layer 1, representing the next inner layer,
	*   continuing inward for subsequent layers.
	*/    int getLayer() const;
	/**
	* @brief Retrieves the direct child nodes of this PolygonTree node.
	*
	* Returns the direct children of the current PolygonTree node as a vector of pointers.
	*
	* @return A vector of pointers to the direct child PolygonTree nodes.
	*/
	std::vector<PolygonTree*>& getChildren();

	/**
	* @brief Retrieves all child nodes recursively.
	*
	* Populates the provided vector with all descendant nodes of this PolygonTree node,
	* traversing the hierarchy recursively.
	*
	* @param vChildNodesRec A vector to be filled with pointers to all descendant nodes.
	*/
	void getChildrenRecursive(std::vector<PolygonTree*>& vChildNodesRec);

	/**
	* @brief Retrieves region-oriented segments.
	*
	* Retrieves the segments of this node, oriented counter-clockwise around
	* the inside regions of the polygon.
	*
	* @image html polygon-with-hole.png  "Polygon with hole: Layer 0 (outer hull) returns a counterclockwise polygon, while Layer 1 (hole) returns a globally clockwise oriented polygon." width=30%
	* @image latex polygon-with-hole.png "Polygon with hole: Layer 0 (outer hull) returns a counterclockwise polygon, while Layer 1 (hole) returns a globally clockwise oriented polygon." width=0.5\textwidth
	*
	* **Observe:**
	* - If the layer number is even, the layer represents a transition from 'air' to 'material', and its segments
	*   are returned in a globally counter-clockwise orientation.
	* - If the layer number is odd, the layer represents a hole (a transition from 'material' back to 'air').
	*   Consequently, its segments are globally clockwise oriented (to be counter-clockwise around the inside region).
	*
	* @param vSegments A vector to be filled with the region-oriented segments.
	*/
	void getSegments_regionOriented(std::vector<Segment2>& vSegments) const;

	/**
   * @brief Retrieves globally counter-clockwise ordered and oriented segments.
   *
   * This method fills the provided vector with segments that are ordered and oriented
   * in a globally counter-clockwise (CCW) direction.
   *
   * @param vSegments A vector to be filled with CCW-oriented segments.
   *
    * @note For hole-layers, globally counter-clockwise oriented segments
    * differ from segments that are counter-clockwise oriented around inside-regions.
	* Double-check your requirements: If you need segments oriented counter-clockwise
	* around the inside-regions, call getSegments_regionOriented() instead.
   */
	void getSegments_CCW(std::vector<Segment2>& vSegments) const;

    /**
     * @brief Retrieves clockwise ordered and oriented segments.
     *
     * This method fills the provided vector with segments ordered
     * and oriented in a globally clockwise direction.
     *
     * @param vSegments A vector to be filled with CW-oriented segments.
     */
	void getSegments_CW(std::vector<Segment2>& vSegments) const;

	/// @private
	void setGeometrySortedPairs(std::vector<VertexPair2>& vGeometrySortedCW);
	/// @private
	void getPointerSortedPairs(std::vector<VertexPair2>& vFromCW_sorted) const;
private:
	/// @private
	//	const std::vector<VertexPair2>& getGeometrySortedPairsCW();
	explicit PolygonTree(const PolygonTree&);
	PolygonTree& operator=(const PolygonTree&);
	// DATA
	struct Impl;
	Impl* pImpl;///< Pointer to the implementation details of the PolygonTree.

};



} // (namespace)
