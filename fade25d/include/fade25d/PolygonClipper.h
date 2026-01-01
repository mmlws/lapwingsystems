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
#include <vector>
#include "Segment2.h"
#include "PolygonTree.h"
#include "Visualizer2.h"
#include "common.h"

/// \file PolygonClipper.h

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif
class PolygonClipperImpl; // FWD
class Zone2; // FWD

/**
 * @brief Limits an input polygon to a specified zone.
 *
 * @image html polygon-clipping.png "The original polygons on the left are clipped by a rectangular window (box) on the right." width=75%
 * @image latex polygon-clipping.png "The original polygons on the left are clipped by a rectangular window (box) on the right." width=0.8\textwidth
 *
 * This function trims an input polygon so that its area is constrained to the given Zone2.
 *
 * @param pBaseZoneIn The Zone2 used to clip the polygon.
 * @param vPolygonInput A vector of unordered and unoriented `Segment2`s. The segments
 * may self-intersect, and the polygon can have holes. All such issues are automatically repaired.
\if GEOM_PSEUDO3D==GEOM_TRUE
 * @param bWithDrape Specifies whether the output polygon should also be draped onto \p pBaseZoneIn
\else
 * @param bWithDrape A dummy parameter with no effect for the 2D version; use false.
\endif
 * @param vPolygonSegmentsOut A vector to store the resulting polygon segments. The output
 * segments are free of self-intersections and are oriented counterclockwise around the
 * polygon area.
 *
 * @return `true` if successful; `false` if the operation fails due to invalid input or empty output.
 */
bool CLASS_DECLSPEC clipPolygon(Zone2* pBaseZoneIn,std::vector<Segment2>& vPolygonInput,bool bWithDrape,std::vector<Segment2>& vPolygonSegmentsOut);

/**
 * @brief The PolygonClipper class handles polygon repair operations.
 *
 * The PolygonClipper class handles polygon repair operations.
 *
 * \image html polygon-repair.png "Left: A damaged poloyon. right: It's repaired result." width=50%
 * \image latex polygon-repair.png "Left: A damaged poloyon. right: It's repaired result." width=0.8\textwidth
 *
 * This class processes a set of polygon segments. After optionally removing
 * ultra-short segments, it resolves self-intersections and provides
 * methods to retrieve the layers of the repaired polygon.
 *
 * \see clipPolygon() A free function to trim an input polygon to a specified domain.
 */
class CLASS_DECLSPEC PolygonClipper
{
public:

    /**
     * @brief Constructs a PolygonClipper object.
     *
     * Constructor: Initializes the clipper with a set of segments representing polygon edges
     * and a collapse distance used to eliminate near-degenerate edges.
     *
     * @param vSegments A vector containing possibly intersecting polygon edges with no specific order or orientation.
	 * @param collapseDist A threshold distance: Use a positive value to collapse edges shorter than this distance,
	 * -1 to automatically determine a tiny value for \p collapseDist based on the numeric uncertainty of the given
     * coordinates, or 0 to retain all edges.
     */
	PolygonClipper(const std::vector<Segment2>& vSegments, double collapseDist);
    /** @brief Destructor for the PolygonClipper class. */
	~PolygonClipper();

	/**
	* @brief Visualizes the polygon regions as a .PDF or PostScript file.
	*
	* Visualizes the polygon regions using a Visualizer2 object (PDF or PostScript file).
	*
	* @param pVis A pointer to a Visualizer2 object.
	* @param matColor The color used for the 'material' regions. Default is yellow.
	* @param airColor The color used for the 'air' regions. Default is white (transparent).
	*/
	 void show(Visualizer2* pVis,const Color& matColor=Color(CYELLOW,0,true),const Color airColor=Color(CWHITE,0,false)) const;
	/**
	* @brief Visualizes the polygon regions as a .PDF or PostScript file.
	*
	* Generates a visualization of the polygon regions and saves it as a .PDF or
	* PostScript file, depending on the file extension provided.
	*
	* @param name The output filename. Use `.pdf` or `.ps` as the file extension.
	* @param matColor The color used for the 'material' regions. Default is yellow.
	* @param airColor The color used for the 'air' regions. Default is white (transparent).
	*/
	void show(const std::string& name,const Color& matColor=Color(CYELLOW,0,true),const Color airColor=Color(CWHITE,0,false)) const;


	/**
	* @brief Returnes the PolygonTree structure.
	*
	* @image html nested-polygon-layers.png "A nested polygon on the left, and the corresponding \c PolygonTree on the right" width=50%
	* @image latex nested-polygon-layers.png "A nested polygon on the left, and the corresponding \c PolygonTree on the right" width=0.8\textwidth

	* This method returns a pointer to a `PolygonTree` object. This object
	* represents the root node of the hierarchical PolygonTree structure for
	* the repaired polygon. The structure can be analyzed layer by layer.
	*
	* @return A pointer to the root node of the PolygonTree.
	*/
	PolygonTree* getPolygonTree();
	/**
	* @brief Retrieves the outermost segments of the polygon in counter-clockwise direction.
	*
	* This method fills the provided vector with the polygon's outer boundary
	* segments, ordered and oriented in counter-clockwise (CCW) direction around
	* the enclosed area. Segments inside this outer polygon, if any, are ignored.
	*
	* @image html extract-polygon-hull.png "An input polygon on the left and its outer hull on the right side" width=50%
	* @image latex extract-polygon-hull.png "An input polygon on the left and its outer hull on the right side" width=0.8\textwidth
	*
	* **Details:**
	* - The returned polygon is "traversed from the outside", meaning the largest
	* possible polygon is returned, rather than splitting a non-simple polygon
	* into multiple parts.
	* - The input polygon may consist of multiple connected components, and in
	* this case, more than one polygon is stored in the output vector. Due to
	* the counterclockwise orientation of the segments, this output is still
	* suitable as input for Fade_2D::createZone(ConstraintGraph2*,ZoneLocation,bool).
	* - If you need individual polygons for each connected component, you may
	* retrieve the root node of the PolygonTree using getPolygonTree(), and
	* then query its first-layer children (layer 0), which represent the individual
	* connected components of the outermost layer.
	*
	* @param vOuterSegments_CCW A vector to be filled with the CCW-oriented outer boundary segments.
	 *
	 * \see [C++ example: Polygon repair](https://www.geom.at/polygon-clipper-repairing-polygons/#polygon-repair)
	 * \see [C++ example: Analyzing polygon layers](https://www.geom.at/polygon-clipper-repairing-polygons/#analyzing-polygon-layers)
	*/
	void getOuterSegments_CCW(std::vector<Segment2>& vOuterSegments_CCW) const;


/**
 * @brief Retrieves the inner and outer polygon segments oriented by region.
 *
 * This method returns all layers of the polygon, oriented in a counter-clockwise
 * direction around their adjacent inside regions. This way, the result is suitable
 * as input for `Fade_2D::createConstraint()` and for subsequent creation of a Zone2.
 *
 * @image html polygon-repair-full.png "An input polygon on the left, and the regions of the repaired polygon on the right" width=50%
 * @image latex polygon-repair-full.png "An input polygon on the left, and the regions of the repaired polygon on the right" width=0.8\textwidth
 *
 * @param vSegments_regionOriented A vector to be filled with the region-oriented segments.
 *
 * **Key details**
 * - **Even-odd rule:** The outermost layer (layer 0) of the polygon defines a transition
 *   from empty space to filled area (from 'air' to 'material'), while the subsequent
 *   layer (layer 1) indicates holes (a transition back to 'air'). More generally, as
 *   we move from outside to inside, the polygon layers signify alternating transitions between
 *   air and material. Even layers represent borders from air to material, while odd
 *   layers represent borders to holes within that material. Consequently, even layers
 *   are oriented counter-clockwise, while odd layers are clockwise.
 * - **Largest possible shapes:** The polygons form the largest possible shapes instead
 *   of splitting non-simple polygons into multiple parts.
 * - **Single output vector:** All polygons are stored in the same output vector. Due
 *   to their counterclockwise orientation around inside regions, they are
 *   suited for Zone2 creation.
 * - **Individual polygons** If you need individual polygons, you can retrieve the root
 *   node of the PolygonTree using `getPolygonTree()` and query it layer-by-layer. Each
 *   layer corresponds to one layer of the polygon, from outside to inside.
 *
 * \see [C++ Example: Correctly creating a Zone2](https://www.geom.at/polygon-clipper-repairing-polygons/#zone-creation)
 * \see [C++ Example: Repairing a polygon](https://www.geom.at/polygon-clipper-repairing-polygons/#polygon-repair)
 */
	void getSegments_regionOriented(std::vector<Segment2>& vSegments_regionOriented) const;

private:
	/// @private
	PolygonClipper(const PolygonClipper &);
	/// @private
	PolygonClipper &operator=(const PolygonClipper &);


	// Data
	PolygonClipperImpl* pImpl;
};





} // Namespace

