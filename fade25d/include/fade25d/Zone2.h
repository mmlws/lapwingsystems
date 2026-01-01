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

/// @file Zone2.h

#pragma once

#include "common.h"
#include "freeFunctions.h"
#include "FadeExport.h"
#include "Bbox2.h"
#include "Edge2.h"
#include "Segment2.h"
#include "UserPredicates.h"
#include "MsgBase.h"
#include "VtkWriter.h"
#include "CompPolygon.h"
#include "PolygonClipper.h"


#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class ZoneShooter2; // FWD
class ConstraintSegment2; // FWD
class Fade_2D; // FWD

/** \enum OptimizationMode
*
* Enumerates the possible modes for Valley/Ridge optimization
* through Zone2::slopeValleyRidgeOptimization().
*
*/

enum OptimizationMode
{
	OPTMODE_STANDARD, ///< Fastest optimization mode
	OPTMODE_BETTER, ///< Considerably better quality and still fast
	OPTMODE_BEST ///< Best quality but quite time consuming
};



/**
* @private Specifies which inner segments to return when the method getZoneReplicationData() is called.
*
* This enum is used to define the criteria for selecting the segments
* that should be returned by the getZoneReplicationData() method.
*/
enum SegmentSelectionType
{
	SST_CONSTRAINT, ///< Return only segments corresponding to inner ConstraintSegment2 objects.
	SST_INCIRCLE, ///<Return inner constraints or segments where the opposite point of one adjacent triangle lies inside the circumcircle of the other triangle.
	SST_IN_OR_ONCIRCLE ///<Return inner constraints or segments where the opposite point of one adjacent triangle lies inside or on the circumference of the circumcircle of the other triangle.
};



class Progress; // FWD
class Dt2; // Fwd
class ConstraintGraph2; // Fwd
class Triangle2; // Fwd
class Point2; // Fwd
class Visualizer2; // Fwd
class Visualizer3; // Fwd



/** \brief Represents a defined area within a triangulation
 *
 * A `Zone2` object represents a defined area within a triangulation.
 *
 * @image html constrained-delaunay-zones.png "A Zone2 is a defined area within a triangulation" width=35%
 * @image latex constrained-delaunay-zones.png "A Zone2 is a defined area within a triangulation" width=0.6\textwidth
 *
 * \see http://www.geom.at/example4-zones-defined-areas-in-triangulations/ for examples of how to create and use `Zone2` objects.
 * \see http://www.geom.at/boolean-operations/ for a demonstration of boolean operations on zones.
 */
class CLASS_DECLSPEC Zone2
{
public:
	/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_);
	/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_,ConstraintGraph2* pConstraintGraph_);
	/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_,const std::vector<ConstraintGraph2*>& vConstraintGraphs_);
	/// @private
	Zone2(Dt2* pDt_,const std::vector<ConstraintGraph2*>& vConstraintGraphs_,ZoneLocation zoneLoc_,std::vector<Point2>& vStartPoints);
	/// @private
	~Zone2();
	/**
	* @brief Checks if a point lies on the zone.
	*
	* This method checks if the query point \p p lies on the Zone2 or its boundary.
	*
	* @param p The point to check.
	* @return `true` if the point is on the Zone2 or its boundary, `false` otherwise.
	*
	* The first call to this method initializes a search structure. If boundary
	* edges of the Zone2 are not constraint edges yet, constraint edges are created.
	*/
	bool hasOn(const Point2& p);


	/** \brief Get the median 2D edge length
	*
	* Computes the median edge length of all edges in the current zone in 2D.
	* \if DOC25D
	* This method considers the projection of the edges onto the XY plane,
	* ignoring the z-coordinate. To compute the 2.5D median edge length, use
	* getMedianEdgeLength25D().
	* \endif
	*
	* @return The median 2D edge length of all edges in the zone.
	*/
	double getMedianEdgeLength2D();

#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Get the median 2.5D edge length
	*
	* Computes the median 2.5D edge length of all edges in the current zone.
	*
	* @return The median 2.5D edge length of all edges in the zone.
	*/
	double getMedianEdgeLength25D();
#endif

	/**
	* @brief Checks if a point lies on the boundary of the zone.
	*
	* This method checks if the query point \p p lies exactly on the boundary of
	 * the present Zone2.
	*
	* @param p The point to check.
	* @return `true` if the point is on the zone's boundary, `false` otherwise.
	*
	* The first call to this method initializes a search structure. If boundary
	* edges of the Zone2 are not constraint edges yet, constraint edges are created.
	*/
	bool hasOnBoundary(const Point2& p);

	/**
	* @brief Locates the triangle containing the given point within the zone.
	 *
	 * Locates the triangle containing the query point within the Zone2.
	*
	* @param p The point to locate.
	* @return A pointer to the Triangle2 if the point is inside the Zone2, otherwise `NULL`.
	*
	* The first call to this method initializes a search structure. If boundary
	* edges of the Zone2 are not constraint edges yet, constraint edges are created.
	*/
	Triangle2* locate(const Point2& p);

/**
 * @brief Locates the nearest boundary `ConstraintSegment2` of the Zone2
 *
 * This method locates the nearest `ConstraintSegment2` on the boundary of the current Zone2
 * within the specified distance, but only if the point \p p has an orthogonal projection
 * onto this `ConstraintSegment2`.
 *
 * @image html orthogonal_projection.png "The red query point has an orthogonal projection onto the boundary, while the blue one does not." width=35%
 * @image latex orthogonal_projection.png "The red query point has an orthogonal projection onto the boundary, while the blue one does not." width=0.5\textwidth

 * @param p The point from which the search is initiated.
 * @param tolerance The maximum allowed 2D distance between the point \p p and the `ConstraintSegment2`.
 *
 * @return A pointer to the closest alive `ConstraintSegment2` within the tolerance distance,
 * where \p p has an orthogonal projection, or NULL if no such `ConstraintSegment2` is found.
 *
 * @note The first call to any of the following methods initializes a search structure. For
 * zones not of type `ZL_INSIDE`, `ZL_BOUNDED`, or `ZL_GLOBAL`, constraint edges are inserted
 * around the zone.
 */
	ConstraintSegment2* getNearbyBoundaryConstraint(Point2& p, double tolerance);
	/**
	 * @brief Finds a point close to the query that lies inside the zone.
	 *
	 * This method finds a point close to the query point \p q that lies
	 * inside the Zone2 or on its boundary.
	 *
	 * @param [in] q A point outside the Zone2.
	 * @param [in] tolerance The maximum allowed 2D distance to find a point on the Zone2.
	 * @param [out] result A reference to store the point found within the Zone2, close to \p q.
	 * @return `true` if a point within the zone is successfully found and returned as \p result;
	 * `false` otherwise.
	 *
	 * The first call to this method initializes a search structure. If boundary edges of the
	 * Zone2 are not constraint edges yet, constraint edges are created.
	 */
	bool shiftToZone(const Point2& q, double tolerance, Point2& result);


	/** \brief Save the zone
	 *
	 * This command saves the present Zone2 to a binary file. Any
	 * constraint edges and custom indices in the domain are retained.
	 *
	 * @param [in] filename is the output filename
	 * @return whether the operation was successful

	 * @note A Delaunay triangulation is convex without holes but this
	 * may not hold for the Zone2 to be saved. Thus extra triangles may
	 * be saved to fill concavities. These extra-triangles will belong
	 * to the Fade_2D instance but not to the Zone2 object when reloaded.
	 *
	 * @see save(std::ostream& stream) is a similar command taking an ostream.
	 *
	 * @see Fade_2D::saveZones(const char* file, std::vector<Zone2*>& vZones) is a
	 * similar command to store multiple Zone2 objects at once.
	 *
	 * @see Fade_2D::saveTriangulation() stores the whole triangulation plus any
	 * specified Zone2 objects.
	 *
	 * @see Fade_2D::load() can be used to reload the data from a file.
	 *
	 *
	*/
	bool save(const char* filename);

	/** \brief Save the zone
	 *
	 * This command saves the present Zone2 to an ostream. Any
	 * constraint edges and custom indices in the domain are retained.
	 *
	 * @param stream is the output stream
	 * @return whether the operation was successful

	 * @note A Delaunay triangulation is convex without holes but this
	 * may not hold for the Zone2 to be saved. Thus extra triangles may
	 * be saved to fill concavities. These extra-triangles will belong
	 * to the Fade_2D instance but not to the Zone2 object when reloaded.
	 *
	 * @see save(const char* filename) is a similar command taking an filename.
	 *
	 * @see Fade_2D::saveZones(const char* file, std::vector<Zone2*>& vZones) is a
	 * similar command to store multiple Zone2 objects at once.
	 *
	 * @see Fade_2D::saveTriangulation() stores the whole triangulation plus any
	 * specified Zone2 objects.
	 *
	 * @see Fade_2D::load() can be used to reload the data from a file.
	*/
	bool save(std::ostream& stream);


/** \brief Export triangles from a zone
 *
 * Exports the Zone2 to a FadeExport struct.
 *
 * @param fadeExport is a simple struct that will be filled with data
 * @param bWithCustomIndices determines whether the custom indices of the points are also stored
 *
 * \see https://www.geom.at/triangulation-export/ This article explains the FadeExport struct in detail.
 */
	void exportZone(FadeExport& fadeExport,bool bWithCustomIndices) const;



/** \brief Get the offset boundary of a shape.
 *
 * This function computes an offset boundary of a Zone2. The result is returned as
 * a vector of counterclockwise-oriented `Segment2` objects.
 *
 * @image html positive-offset-boundary.png "The positive offset boundary of a Zone2" width=35%
 * @image latex positive-offset-boundary.png "The positive offset boundary of a Zone2" width=0.5\textwidth
 *
 * @param [in] offset The positive or negative offset distance.
 * @param [out] vOffsetBoundary A vector that will contain the output segments, in arbitrary order,
 * oriented counterclockwise around the resulting shape.
 * @param [in] mergeAngleDeg The angle threshold (in degrees) used to merge circular arcs in
 * the offset boundary. Arcs with an angle less than this value are collapsed to a single
 * point. Default: 10.0, valid range: greater than 0 and up to 135.
 * @param [in] angleStepDeg Specifies the angle interval (in degrees) at which circular arcs
 * are sampled using line segments. Default: 20.0, valid range: greater than 0 and up to 135.
 *
 * @see https://www.geom.at/offset-polygon-example/ This article contains examples on polygon
 * offsets.
 */
	void getOffsetBoundary(double offset,std::vector<Segment2>& vOffsetBoundary,double mergeAngleDeg=10.0,double angleStepDeg=20.0) const;

/**
 * \brief Creates an offset zone.
 *
 * Creates a positive or negative offset zone based on the current Zone2
 * and stores the result in a new `Fade_2D` object.
 *
 * @image html create-offset-zone.png "A Zone2 and a positive offset of it" width=35%
 * @image latex create-offset-zone.png "A Zone2 and a positive offset of it" width=0.6\textwidth

 * \param [in] pFade An empty `Fade_2D` object where the result zone will be created.
 * \param [in] offset The positive or negative offset distance.
* \param [in] mergeAngleDeg is used as an angle threshold (in degrees) to merge small
 * circular arcs in the offset shape into a single point. Default: 10.0, valid
 * range: (>0,...,135)
* \param [in] angleStep Specifies the angle interval (in degrees) at which circular
 * arcs in the offset shape are sampled using line segments. Default: 20.0,
 * valid range: (>0,...,135)
 * \return A pointer to a new `Zone2` object, created within `pFade` or `NULL` if
 * the result is empty, which may be the case if a large negative offset value is used.
 * Be sure to check this case before proceeding.
 */
	Zone2* createOffsetZone(Fade_2D* pFade,double offset,double mergeAngleDeg=10,double angleStep=20.0) const;



/**
 * \brief Morphological opening to remove small details
 *
 * Morphological opening removes small details from a shape by applying erosion
 * (negative offset) followed by dilation (positive offset) using a disk-shaped
 * structuring element whose radius can be chosen. The result is stored in a new
 * Zone2 object, which is created in the provided `Fade_2D` object.
 *
 * \image html morphological-opening.png "Left: The input zone, Right: The result of morphological opening - small details removed." width=60%
 * \image latex morphological-opening.png "Left: The input zone, Right: The result of morphological opening - small details removed." width=0.8\textwidth
 *
 * \param pFade An empty `Fade_2D` object in which the result will be created as a Zone2.
 * \param offset The offset distance, which defines the radius of the disk-shaped structuring
 * element. This value must be positive.
 * \param mergeAngleDeg An angle threshold (in degrees). Circular arcs in the result
 * which have an angle smaller than this value are collapsed into a single point.
 * Recommended value: 91 degrees, to avoid most arcs.
 * \param angleStep The angle interval (in degrees) at which circular arcs in the
 * offset boundary, if any, are sampled using line segments. Default value: 20.0.
 * Valid range: greater than 0 and up to 135.
 *
 * \return A pointer to a new `Zone2` object, created in `pFade`, representing the result of the
 * morphological opening operation.
 *
 * @see https://www.geom.at/offset-polygon-example/#morphological_operations
 *
 * \warning The result of morphological opening may be empty! In such cases,
 * this method returns `NULL`. Be sure to check for this condition before proceeding.
 */

	Zone2* morphOpen(Fade_2D* pFade,double offset,double mergeAngleDeg=10.0,double angleStep=20.0) const;


/**
 * \brief Morphological closing to bridge small gaps in the Zone2.
 *
 * Morphological closing bridges small gaps between shapes by applying dilation (positive offset)
 * followed by erosion (negative offset) using a disk-shaped structuring element whose radius can
 * be chosen. The result is stored in a new Zone2 object, created within the provided `Fade_2D` object.
 *
 * \image html morphological-closing.png "Left: The input zone, Right: The result of morphological closing - small gaps bridged." width=60%
 * \image latex morphological-closing.png "Left: The input zone, Right: The result of morphological closing - small gaps bridged." width=0.8\textwidth
 *
 * \param pFade An empty `Fade_2D` object that will hold the result of the operation (the closed zone).
 * \param offset The offset distance. It defines the radius of the disk-shaped structuring element.
 * This value must be positive.
 * \param mergeAngleDeg An angle threshold (in degrees). Circular arcs in the result
 * which have an angle smaller than this value are collapsed into a single point.
 * Recommended value: 91 degrees, to avoid most arcs.
 * \param angleStep The angle interval (in degrees) at which circular arcs in the offset boundary, if
 * any, are sampled using line segments. Default value: 20.0. Valid range: greater than 0 and up to 135.
 *
 * \return A pointer to a new `Zone2` object, created in `pFade`, representing the result of the morphological closing.
 *
 * @see https://www.geom.at/offset-polygon-example/#morphological_operations
 */
	Zone2* morphClose(Fade_2D* pFade,double offset,double mergeAngleDeg=10.0,double angleStep=20.0) const;


	/** \brief Registers a message receiver
	*
	* This method registers a subscriber to receive messages of a specified type.
	*
	* @param msgType The type of message the subscriber will receive, e.g., `MSG_PROGRESS` or `MSG_WARNING`.
	* @param pMsg A pointer to a custom class derived from `MsgBase` that will handle the messages.
	*/
	void subscribe(MsgType msgType,MsgBase* pMsg);


	/** \brief Unregisters a message receiver
	*
	* This method removes a subscriber from receiving messages of a specified type.
	*
	* @param msgType The type of message that the subscriber will no longer receive.
	* @param pMsg A pointer to a custom class derived from `MsgBase` representing the subscriber to be removed.
	*/
	void unsubscribe(MsgType msgType,MsgBase* pMsg);

	/** \brief Get the zone location
	*
	 * This method is used to retrieve the ZoneLocation value of the present Zone2.
	 *
	 * @return The ZoneLocation value.
	*/
	ZoneLocation getZoneLocation() const;


/** \brief Convert a zone to a bounded zone
 *
 * \anchor convertToBoundedZone
 *
 * This method creates a bounded zone from a possibly unbounded one.
 *
 * Algorithms like `refine()` and `refineAdvanced()` require a `Zone2` object as
 * input that is bounded by constraint edges. This is always the case for zones
 * with `zoneLocation` `ZL_INSIDE`, but other types of zones may be unbounded.
 *
 * @return A pointer to a new `Zone2` object with `zoneLocation` `ZL_RESULT_BOUNDED`
 * or a pointer to the present `Zone2` if `this->getZoneLocation()` returns `ZL_INSIDE`.
 */
	Zone2* convertToBoundedZone();


	/** \brief Postscript- and PDF visualization
	*
	* This method visualizes and saves the Zone2 as a PDF or Postscript file.
	*
	* @param filename The name of the output file. Use `.ps` or `.pdf` as the file extension.
	* \param zoneFillColor The fill color for the zone triangles.
	* \param zoneEdgeColor The edge color for the zone triangles.
	* \param nonZoneFillColor The fill color for the non-zone triangles.
	* \param nonZoneEdgeColor The edge color for the non-zone triangles.
	* \param constraintEdgeColor The color for constraints.
	*
	* Any of the colors can be chosen as CTRANSPARENT to avoid drawing the corresponding element.
	*/
	void show(const char* filename,const Colorname& zoneFillColor=CTRANSPARENT,const Colorname& zoneEdgeColor=CBLACK,const Colorname& nonZoneFillColor=CTRANSPARENT,const Colorname& nonZoneEdgeColor=CTRANSPARENT,const Colorname& constraintEdgeColor=CTRANSPARENT) const;

	/** \brief Postscript- and PDF visualization
	*
	* This method visualizes and saves the Zone2 as a PDF or Postscript file.
	*
	* \param pVisualizer A pointer to an existing Visualizer2 object.
	* \param zoneFillColor The fill color for the zone triangles.
	* \param zoneEdgeColor The edge color for the zone triangles.
	* \param nonZoneFillColor The fill color for the non-zone triangles.
	* \param nonZoneEdgeColor The edge color for the non-zone triangles.
	* \param constraintEdgeColor The color for constraints.
	*
	* Any of the colors can be chosen as CTRANSPARENT to avoid drawing the corresponding element.
	*/
	void show(Visualizer2* pVisualizer,const Colorname& zoneFillColor=CTRANSPARENT,const Colorname& zoneEdgeColor=CBLACK,const Colorname& nonZoneFillColor=CTRANSPARENT,const Colorname& nonZoneEdgeColor=CTRANSPARENT,const Colorname& constraintEdgeColor=CTRANSPARENT) const;


	/** \brief Postscript- and PDF-visualization
	 *
	 * This method visualizes and saves the Zone2 as a PDF or Postscript file.
	 *
	 * @param filename The name of the output file. Use `.ps` or `.pdf` as the file extension.
	 * @param bShowFull Specifies whether only the Zone2 or the full triangulation should be drawn.
	 * @param bWithConstraints Specifies whether constraint edges should be drawn.
	 */
 	void show(const char* filename,bool bShowFull,bool bWithConstraints) const;

	/** \brief Postscript- and PDF-visualization
	*
	* This method visualizes and saves the Zone2 as a PDF or Postscript file.
	*
	* @param pVisualizer A pointer to an existing Visualizer2 object.
	* @param bShowFull Specifies whether only the Zone2 or the full triangulation should be drawn.
	* @param bWithConstraints Specifies whether constraint edges should be drawn.
	*
	* @note You must call pVisualizer->writeFile() before program end
	*/
	void show(Visualizer2* pVisualizer,bool bShowFull,bool bWithConstraints) const;

	/** \brief VTK visualization
	*
	* This method visualizes and saves the Zone2 as a VTK file, which can be viewed in tools like Paraview.
	*
	* @param filename The name of the output file.
	* @param zoneColor The color for the zone's triangles.
	* @param nonZoneColor The color for the non-zone triangles. Use VTK_TRANSPARENT to prevent them from being drawn.
	* @param constraintColor The color of the constraint edges. Use VTK_TRANSPARENT to prevent them from being drawn.
	*/
	void showVtk(const char* filename,VtkColor zoneColor,VtkColor nonZoneColor=VTK_TRANSPARENT,VtkColor constraintColor=VTK_TRANSPARENT) const;

	/** \brief VTK visualization
	*
	* This method visualizes and saves the Zone2 as a VTK file, which can be viewed in tools like Paraview.
	*
	* @param pVtk A VtkWriter object that may already contain other geometric objects
	* @param zoneColor The color for the zone's triangles.
	* @param nonZoneColor The color for the non-zone triangles. Use VTK_TRANSPARENT to prevent them from being drawn.
	* @param constraintColor The color of the constraint edges. Use VTK_TRANSPARENT to prevent them from being drawn.
	*
	* @note Call pVtk->writeFile() to finally write the .vtk file.
	*/
	void showVtk(VtkWriter* pVtk,VtkColor zoneColor,VtkColor nonZoneColor=VTK_TRANSPARENT,VtkColor constraintColor=VTK_TRANSPARENT) const;





#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Geomview visualization
	*
	* This method visualizes the Zone2 by saving it as a file for the Geomview viewer (available for Linux only).
	*
	* @param filename The name of the output file.
	* @param color A string representing the color in the format "r g b a", for example, "1.0 0.0 0.0 1.0" for red.
	*
	* @see showVtk() for 3D visualizations under Windows.
	*/
	void showGeomview(const char* filename,const char* color) const;
	/** \brief Geomview visualization
	*
	* This method visualizes the Zone2 by saving it as a file for the Geomview viewer (available for Linux only).
	*
	* @param pVis points to a Visualizer3 object
	* @param color A string representing the color in the format "r g b a", for example, "1.0 0.0 0.0 1.0" for red.
	*
	* @see showVtk() for 3D visualizations under Windows.
	*/
	void showGeomview(Visualizer3* pVis,const char* color) const;

	/// @private
	void analyzeAngles(const char* name="");

	/** @private
	 *
	 * This test function retrieves all triangles whose normal vector deviates by more than
	 * the specified angle (in degrees) from the up-vector (0, 0, 1). These triangles are
	 * considered as almost vertical walls and are added to the output vector.
	 *
	 * @param aboveAngleDegree The threshold angle in degrees. Triangles with a normal vector
	 *        deviating more than this angle from the up-vector will be selected.
	 * @param[out] vVerticalWalls A vector that will be filled with pointers to the selected Triangle2 objects.
	 * @return The maximum deviation angle (in degrees) found among all evaluated triangles.
	 */
	double getVerticalWalls(double aboveAngleDegree,std::vector<Triangle2*>& vVerticalWalls);

	/**
	 * \brief Smoothing
	 *
	 * @deprecated This method is deprecated but retained for backwards
	 * compatibility. Use the new method @ref smoothing2() instead for
	 * better results.
	 * @param numIterations is the number of smoothing passes.
	 * @param bWithXY specifies if the x and y coordinates are also adapted.
	 *
	*/
	void smoothing(int numIterations=2,bool bWithXY=true);


	/** \brief Smooths the vertices of the zone.
	*
	* @image html mesh-smoothing.png "A Zone2 before and after the smoothing operation." width=65%
	* @image latex mesh-smoothing.png "A Zone2 before and after the smoothing operation." width=0.8\textwidth
	*
	* This method performs weighted Laplacian smoothing on the vertices of the Zone2. It
	* distinguishes two types of vertices:
	*  - **Static vertices**: These belong to constraint edges or border edges of
	*    the Zone2. Their x- and y-coordinates are **never** changed, while
	*    their z-coordinate may be modified depending on the value of the parameter \p bWithConstraintZ.
	*  - **Dynamic vertices**: These are all other vertices of the Zone2. Their
	*    z-coordinates can always be altered, while their x- and y-coordinates
	*    can only be changed if the value \p bWithXY is set to `true`.
	*
	* @param numIterations The number of smoothing iterations to perform. Higher
	* values yield smoother results; 2-3 iterations are recommended.
	* @param bWithXY If set to `false`, only the z-coordinates of dynamic vertices
	* are modified. If set to `true`, both the x- and y-coordinates of dynamic vertices
	* can also be changed.
	* @param bWithConstraintZ Controls whether the z-coordinates of static vertices
	* can be modified by the smoothing process.
	 *
	 * see https://www.geom.at/mesh-improvements/ This article discusses various mesh improvements.
	*/
	void smoothing2(int numIterations,bool bWithXY,bool bWithConstraintZ);
	/** \brief Optimize Slopes, Valleys, and Ridges
	 *
	 * Creating an elevated triangulation simply by lifting the points
	 * of a standard Delaunay triangulation ignores the z-coordinate of
	 * the vertices. As a result, valleys, ridges, and water bodies can
	 * appear unnatural.
	 * This method optimizes a Zone2 by keeping the points fixed while
	 * performing edge flips to alter the connectivity of the triangulation,
	 * resulting in a smoother surface overall.
	 *
	 * \image html valley-ridge-optimization.png "Left: A lifted Delaunay triangulation. Right: Connectivity modified for smoother appearance" width=60%
	 * \image latex valley-ridge-optimization.png "Left: A lifted Delaunay triangulation. Right: Connectivity modified for smoother appearance" width=0.8\textwidth
	 * @param om The optimization mode:
	 * - OPTMODE_NORMAL is the fastest option.
	 * - OPTMODE_BETTER provides significantly better results while still being relatively fast.
	 * - OPTMODE_BEST delivers the best results but with a significantly higher time requirement.
	 *
	 * This method supports the progress-bar mechanism to track the process.
	 *
	 * see https://www.geom.at/mesh-improvements/ This article discusses various mesh improvements.
	 */
	void slopeValleyRidgeOptimization(OptimizationMode om=OPTMODE_BETTER);

	/// @private
	/*
	 * This function is deprecated but kept for backwards compatibility.
	 * Better use slopeValleyRidgeOptimization() (see above)
	 *
	 * Optimize Valleys and Ridges
	 *
	 * A Delaunay triangulation is not unique when when 2 or more triangles
	 * share a common circumcircle. As a consequence the four corners of
	 * a rectangle can be triangulated in two different ways: Either the
	 * diagonal proceeds from the lower left to the upper right corner
	 * or it connects the other two corners. Both solutions are valid and
	 * an arbitrary one is applied when points are triangulated. To improve
	 * the repeatability and for reasons of visual appearance this method
	 * unifies such diagonals such that they point from the lower left to
	 * the upper right corner (or in horizontal direction).\n
	 *
	 * Moreover a Delaunay triangulation does not take the z-value into
	 * account and thus valleys and ridges may be disturbed. The present
	 * method flips diagonals such that they point from the lower left to
	 * the upper right corner of a quad. And if the 2.5D lengths of the
	 * diagonals are significantly different, then the shorter one is
	 * applied.
	 *
	 * @param tolerance2D is 0 when only exact cases of more than 3 points
	 * on a common circumcircle shall be changed. But in practice input
	 * data can be disturbed by noise and tiny rounding errors such that
	 * grid points are not exactly on a grid. The numeric error is computed
	 * as \f$error=\frac{abs(diagonalA-diagonalB)}{max(diagonalA,diagonalB)}\f$.
	 * and \p tolerance2D is an upper threshold to allow modification despite
	 * such tiny inaccuracies.
	 * @param lowerThreshold25D is used to take also the heights of the
	 * involved points into account. For example, the points\n
	 * \n
	 * Point_2 a(0,0,0);\n
	 * Point_2 b(10,0,0);\n
	 * Point_2 c(10,10,0);\n
	 * Point_2 d(0,10,1000);\n
	 * \n
	 * can form the triangles (a,b,c) and (a,c,d) or the triangles (a,b,d)
	 * and (d,b,c) but (a,c) is obviousy the better diagonal because the
	 * points a,b,c share the same elevation while d is at z=1000.
	 * Technically spoken, the diagonal with the smaller 2.5D-length is
	 * applied if the both, the 2D error is below \p tolerance2D and the
	 * 2.5D error is above \p lowerThreshold25D. The 2.5D
	 * criterion has priority over the 2D criterion.
	 *
	 */
	void optimizeValleysAndRidges(double tolerance2D,double lowerThreshold25D);
#endif


	/** \brief Unify Grid
	*
	* \image html unified-triangulation.png "Left: A valid Delaunay triangulation. Right: Diagonals unified" width=50%
	* \image latex unified-triangulation.png "Left: A valid Delaunay triangulation. Right: Diagonals unified" width=0.5\textwidth
	*
	* A Delaunay triangulation is not unique when two or more triangles
	* share a common circumcircle. For example, the four corners of a
	* rectangle can be triangulated in two ways. This method unifies
	* such cases by ensuring diagonals always go from the lower left to
	* the upper right corner (or horizontally) for consistency and visual
	* appearance.
	*
	* @param tolerance Defines the maximum allowed error in diagonal
	* lengths due to noise or rounding errors. A value of 0 restricts
	* changes to exact cases.  The error is computed as:
	*   error = abs(diagonalA - diagonalB) / max(diagonalA, diagonalB)
	*/
	void unifyGrid(double tolerance);

	/// @private
	bool assignDt2(Dt2* pDt_);

	/** \brief Get the triangles of the zone.
	*
	* This method retrieves the triangles of the zone.
	*
	* @param [out] vTriangles A vector of `Triangle2` pointers that will be populated with
	* the triangles of the present `Zone2`.
	*/
	void getTriangles(std::vector<Triangle2*>& vTriangles) const;

	/** \brief Get the vertices of the zone.
	*
	*/
	void getVertices(std::vector<Point2*>& vVertices_) const;


	/** \brief Prints statistics
	 *
	 * Prints statistics.
	 */
	void statistics(const char* s) const;


	/** \brief Get the associated constraint
	*
	* Retrieves the associated ConstraintGraph2, if any.
	*
	* @return A pointer to the `ConstraintGraph2` object defining the Zone2, or `NULL` if no ConstraintGraph2 is associated.
	*/
	ConstraintGraph2* getConstraintGraph() const;

	/** \brief Get the number of triangles
	 *
	 * This method computes and returns the number of triangles in the Zone2.
	 *
	 * @return The number of triangles.
	*
	*/
	size_t getNumberOfTriangles() const;


	/** \brief Get the associated constraint graphs
	*
	*/
	void getConstraintGraphs(std::vector<ConstraintGraph2*>& vConstraintGraphs_) const;

	/// @private
	Dt2* getDelaunayTriangulation() const;

	/** \brief Get the number of ConstraintGraph2 objects
	 *
	 * Retrieves the number of ConstraintGraph2 objects.
	 *
	 * @return The number of ConstraintGraph2 objects associated with the current Zone2.
	 */
	size_t numberOfConstraintGraphs() const;

	/** \private */
	void debug(const char* name="");

	/** \brief Compute the bounding box
	 */
	Bbox2 getBoundingBox() const;

	// Deprecated, replaced by getBorderEdges() but kept for
	// backwards-compatibility.
	/** @private
	 */
	void getBoundaryEdges(std::vector<Edge2>& vEdges) const;

/** \brief Get connected components
 *
 * This method finds connected components of the Zone2. For each
 * connected component, it returns a `CompPolygon` struct consisting of
 * `Triangle2` pointers of that component, the outer boundary polygon,
 * and any hole polygons within the component, represented as `Edge2`
 * vectors.
 *
 * @param [out] vCompPolygons A vector that will hold the `CompPolygon`
 * structs for each connected component.
 *
 * @note `Edge2` objects consist of a `Triangle2` and an index, and are always
 * oriented counterclockwise around the associated `Triangle2`. As a result,
 * the outer boundary polygon of a component is always oriented counterclockwise,
 * while the hole polygons are oriented clockwise.
 */
	void getComponentPolygons(std::vector<CompPolygon>& vCompPolygons) const;


/** \brief Get the boundaries as segments
 *
 * Returns the boundaries of the Zone2 as Segment2 objects.
 *
 * @param [out] vSegments A vector of `Segment2` objects that will hold
 * the counterclockwise oriented border segments of the Zone2 in arbitrary
 * order.
 *
 */
	void getBoundarySegments(std::vector<Segment2>& vSegments) const;
/** \brief Get 2D Area
 *
 * \if DOC2D
 * This method computes and returns the 2D area of the Zone2.
 * \endif
 * \if DOC25D
 * This method computes and returns the 2D area of the Zone2, ignoring its elevation (the z-coordinate).
 * \endif
 *
 * @return The 2D area of the Zone2.
 */
	double getArea2D() const;


#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Get 2.5D Area
	 *
	 * This method computes and returns the 2.5D area of the Zone2.
	 *
	 * @return The 2.5D area of the Zone2.
	 *
	 */
	double getArea25D() const;
#endif

/** \brief Get border edges
 *
 * Returns the boundaries of the Zone2.
 *
 * @param [out] vBorderEdgesOut A vector of `Edge2` objects that will hold the
 * counterclockwise oriented border edges of the Zone2 in arbitrary order.
 *
 *
 */
void getBorderEdges(std::vector<Edge2>& vBorderEdgesOut) const;

/** \brief Write the Zone2 to an .obj file
 *
 * This method writes the current Zone2 data to an .obj file,
 * which can be used for visualization and further processing in tools
 * like Meshlab.
 *
 * @param filename The output filename.
 */
	void writeObj(const char* filename) const;

#ifndef SKIPTHREADS
#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Write the Zone2 to a *.ply file
 *
 * This method writes the current Zone2 data to a *.ply file.
 * The file can be saved in either ASCII or binary format.
 *
 * @param filename The output filename.
 * @param bASCII Specifies whether to write the *.ply file in ASCII format
 *               (true) or binary format (false). Default is false (binary).
 *
 * @note This method is available only on platforms that support C++11 or higher.
 */
	bool writePly(const char*  filename,bool bASCII=false) const;

	/** \brief Write the Zone2 to a *.ply file
	*
	* This method writes the current Zone2 data to a *.ply file.
	* The file can be saved in either ASCII or binary format.
	*
	* @param os is the output stream
	* @param bASCII Specifies whether to write the *.ply file in ASCII format
	*               (true) or binary format (false). Default is false (binary).
	*
	* @note This method is available only on platforms that support C++11 or higher.
	*/
	bool writePly(std::ostream& os,bool bASCII=false) const;
#endif
#endif


	/**
	* @private Retrieves essential zone data for replication in another triangulation.
	*
	* This method collects all the necessary data from a zone that is required to replicate
	* the zone in a different triangulation.
	*
	* @param[out] vBoundarySegments A vector to store the boundary segments of the zone.
	* @param[out] vInnerSegments A vector to store relevant inner segments of the zone.
	* @param[out] vPoints A vector to store the points that define the zone.
	* @param[in] segmentSelectionType Specifies which type of segments should be included as vInnerSegments. The possible values are:
	- `SST_CONSTRAINT`: Only return segments corresponding to real inner `ConstraintSegment2` objects.
	- `SST_INCIRCLE`: Return inner constraints or segments where the opposite point of one adjacent triangle lies inside the circumcircle of the other one.
	- `SST_IN_OR_ONCIRCLE`: Return inner constraints or segments where the opposite point of one adjacent triangle lies inside
	 or on the circumference of the circumcircle of the other one.
	*/
	void getReplicationData(    SegmentSelectionType segmentSelectionType,
	                            std::vector<Point2>& vPoints,
			                    std::vector<Segment2>& vInnerSegments,
		                        std::vector<Segment2>& vBoundarySegments
							   ) const;


protected:
	Zone2& operator=(const Zone2&);
	// Optimization techniques
	/// @private
	void optMode_standard_sub(std::vector<Triangle2*>& vT,std::vector<Triangle2*>& vChangedT);
	/// @private
	void optMode_standard();
	/// @private
	double optMode_prioq(double noEdgeBelowDih,bool bWithProgress);
	/// @private
	void getEdgesForOptimization(double noEdgeBelowDegree,std::vector<Edge2>& vEdges);
	/// @private
	void optMode_simulatedAnnealing();
	/// @private
	void optMode_simulatedAnnealing_sub(std::vector<Edge2>& vUniqueEdges,double temperature);
	/// @private
	void removeConstraintEdges(std::vector<Edge2>& vEdges) const;
/// @private
	Zone2(const Zone2&);
/// @private
	void getTriangles_RESULT(std::vector<Triangle2*>& vTriangles) const;
/// @private
	void initWorkspace(bool bInside,std::set<std::pair<Point2*,Point2*> >& sNoGrowEdges,std::vector<Triangle2*>& vWorkspace) const;
/// @private
	void bfsFromWorkspace(std::vector<Triangle2*>& vWorkspace,std::set<std::pair<Point2*,Point2*> >& sNoGrowEdges,std::vector<Triangle2*>& vTriangles) const;
/// @private
	Zone2* ctbz_treatCC(std::vector<Triangle2*>& vOneCC);
	// Data
/// @private
	Dt2* pDt;
/// @private
	Progress* pZoneProgress;
/// @private
	ZoneShooter2* pZoneShooter;
/// @private
	ZoneLocation zoneLoc;
	CLASS_DECLSPEC
	friend Zone2* zoneUnion(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneIntersection(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneDifference(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneSymmetricDifference(Zone2* pZone0,Zone2* pZone1);

	/** \brief Peel off undesired border triangles (deprecated)
	*
	* @deprecated This function is DEPRECATED but kept for backwards compatibility.
	* Use the new and better function peelOffIf(Zone2* pZone, bool bAvoidSplit,PeelPredicateTS* pPredicate)
	*
	*/
	CLASS_DECLSPEC
	friend Zone2* peelOffIf(Zone2* pZone, UserPredicateT* pPredicate,bool bVerbose); // Depricated!


	/** \brief Peel off undesired border triangles
	*
	* Triangulations often contain narrow, nearly vertical triangles at their borders.
	* This function returns a `Zone2` that includes all triangles from \p pZone, except
	* the undesired ones. If no triangles remain, the method returns `NULL`.
	* A user-defined predicate distinguishes between desired and undesired
	* triangles.
	*
	* \image html peeled-border-triangles.png "Left: A Zone2 containing nearly vertical border triangles. Right: Vertical border triangles removed." width=50%
	* \image latex peeled-border-triangles.png "Left: A Zone2 containing nearly vertical border triangles. Right: Vertical border triangles removed." width=0.8\textwidth
	*
	* @param pZone The input zone.
	* @param bAvoidSplit If `true`, the algorithm removes a triangle only if it does not split the zone into independent components.
	* @param pPredicate A user-defined predicate to decide whether a triangle should be removed.
	*
	* @return A new Zone2 containing a subset of the triangles from \p pZone, or `NULL` if no triangles remain.
	 *
	 * \sa [C++ Example: Removing unwanted border triangles](https://www.geom.at/mesh-improvements/#removing-unwanted-triangles)
	*/
	CLASS_DECLSPEC
	friend Zone2* peelOffIf(Zone2* pZone, bool bAvoidSplit,PeelPredicateTS* pPredicate);



private:
#ifndef __MINGW32__
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
#endif
	std::vector<ConstraintGraph2*> vConstraintGraphs;
	std::vector<Point2> vStartPoints;
	std::vector<Zone2*> vInputZones;

#ifndef __MINGW32__
#ifdef _WIN32
#pragma warning(pop)
#endif
#endif
};

// Free functions
/**
 * @brief Computes the difference between two Zone2 objects.
 *
 * @image html zone-difference.png "Left: pZone0, Middle: pZone1, Right: difference" width=75%
 * @image latex zone-difference.png "Left: pZone0, Middle: pZone1, Right: difference" width=0.8\textwidth
 *
 * This function returns a pointer to a new Zone2 object representing the difference
 * between \p pZone0 and \p pZone1 (i.e., \p pZone0 minus \p pZone1).
 * The input zones must belong to the same Fade_2D object.
 *
 * @param pZone0 Pointer to the first Zone2 object.
 * @param pZone1 Pointer to the second Zone2 object.
 *
 * @return A pointer to a new Zone2 object representing the difference.
 *         If the result is empty, the function returns `NULL`. Be sure to
 *         check this case before proceeding.
 */
CLASS_DECLSPEC
Zone2* zoneDifference(Zone2* pZone0,Zone2* pZone1);

/**
 * @brief Computes the intersection of two Zone2 objects.
 *
 * @image html zone-intersection.png "Left: pZone0, Middle: pZone1, Right: intersection" width=75%
 * @image latex zone-intersection.png "Left: pZone0, Middle: pZone1, Right: intersection" width=0.8\textwidth
 *
 * This function returns a pointer to a new Zone2 object representing the intersection
 * of the two input zones. The intersection is the area covered by both input zones.
 * The input zones must belong to the same Fade_2D object.
 *
 * @param pZone0 Pointer to the first Zone2 object.
 * @param pZone1 Pointer to the second Zone2 object.
 *
 * @return A pointer to a new Zone2 object representing the intersection.
 *         If the result is empty, the function returns `NULL`. Be sure to
 *         check for this case before proceeding.
 */

CLASS_DECLSPEC
Zone2* zoneIntersection(Zone2* pZone0,Zone2* pZone1);


/**
 * @brief Computes the symmetric difference between two Zone2 objects.
 *
 * @image html zone-symmetric-difference.png "Left: pZone0, Middle: pZone1, Right: symmetric difference" width=75%
 * @image latex zone-symmetric-difference.png "Left: pZone0, Middle: pZone1, Right: symmetric difference" width=0.8\textwidth
 *
 * This function returns a pointer to a new Zone2 object representing the symmetric
 * difference between the two input zones. The symmetric difference is the area covered
 * by either of the input zones, but not by both. The input zones must belong to the
 * same Fade_2D object.
 *
 * @param pZone0 Pointer to the first Zone2 object.
 * @param pZone1 Pointer to the second Zone2 object.
 *
 * @return A pointer to the resulting Zone2 object representing the symmetric difference.
 *         If the result is empty, the function returns `NULL`. Be sure to check for this case
 *         before proceeding.
 */
CLASS_DECLSPEC
Zone2* zoneSymmetricDifference(Zone2* pZone0,Zone2* pZone1);


/**
 * @brief Computes the union of two Zone2 objects.
 *
 * @image html zone-union.png "Left: pZone0, Middle: pZone1, Right: union" width=75%
 * @image latex zone-union.png "Left: pZone0, Middle: pZone1, Right: union" width=0.8\textwidth
 *
 * This function returns a pointer to a new Zone2 object representing the union
 * of the two input zones. The union is the area covered by either of the input
 * zones. Both input zones must belong to the same Fade_2D object.
 *
 * @param pZone0 Pointer to the first Zone2 object.
 * @param pZone1 Pointer to the second Zone2 object.
 * @return A pointer to the resulting Zone2 object representing the union of the two zones.
 */
CLASS_DECLSPEC
Zone2* zoneUnion(Zone2* pZone0,Zone2* pZone1);






} // (namespace)
