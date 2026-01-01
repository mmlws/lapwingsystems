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


/** @file Fade_2D.h is the main API file
 *
 */

#pragma once

#include "common.h"
#include "freeFunctions.h"
#ifndef FADE2D_EXPORT
	#include "License.h"
#endif
#include "Point2.h"
#include "Triangle2.h"
#include "TriangleAroundVertexIterator.h"
#include "Visualizer2.h"
#include "VtkWriter.h"
#include "Zone2.h"
#include "ConstraintGraph2.h"
#include "Performance.h"
#include "MeshGenParams.h"
#include "MsgBase.h"
#include "SegmentChecker.h"
#include "testDataGenerators.h"
#include "FadeExport.h"
#include "Voronoi2.h"
#include "PolygonClipper.h"


#if GEOM_PSEUDO3D==GEOM_TRUE
	#include "IsoContours.h"
	#include "EfficientModel.h"
	#include "CutAndFill.h"
	#include "Visualizer3.h"
	#include "CloudPrepare.h"
	#include "RayShooter.h"
#endif



#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif



class Dt2; // Forward
class Visualizer3; // Forward


/** \brief Fade_2D is a class that contains a Delaunay triangulation
 *
The library comes in two versions: 2D and 2.5D. The 2D version computes
classic Delaunay triangulations in the xy-plane, while the 2.5D version
includes a <b>z-coordinate</b> and provides additional algorithms for
<b>lifted</b> triangulations.

@if DOC25D
\htmlonly
<div style="display: flex; justify-content: space-between; max-width: 1500px; margin: 0 auto;">
    <div style="text-align: center;">
        <img src="terrain_25d.png" alt="A 2.5D Delaunay triangulation or lifted Delaunay triangulation, computed with the 2.5D version of Fade" width="85%" />
        <div><b>2.5D Delaunay triangulation</b></div>
    </div>
    <div style="text-align: center;">
        <img src="delaunay_with_constraints.png" alt="A 2D Delaunay triangulation with constraint segments" width="65%" />
        <div>2D Delaunay triangulation with constraint edges (Fade 2D)</div>
    </div>
</div>
\endhtmlonly
@endif
@if DOC2D
\htmlonly
<div style="display: flex; justify-content: space-between; max-width: 1500px; margin: 0 auto;">
    <div style="text-align: center;">
        <img src="delaunay_with_constraints.png" alt="A 2D Delaunay triangulation with constraint segments" width="85%" />
        <div><b>2D Delaunay triangulation with constraint edges.</b></div>
    </div>
    <div style="text-align: center;">
        <img src="terrain_25d.png" alt="A 2.5D Delaunay triangulation or lifted Delaunay triangulation, computed with the 2.5D version of Fade" width="65%" />
        <div>2.5D Delaunay triangulation (Fade 2.5D)</div>
    </div>
</div>
\endhtmlonly
@endif

\image latex delaunay_with_constraints.png "A 2D Delaunay triangulation with constraint segments" width=0.49\textwidth
\image latex terrain_25d.png "A 2.5D Delaunay triangulation (Fade 2.5D)" width=0.49\textwidth
@if DOC25D
 You are currently viewing the <b>2.5D</b> documentation. That's fine, because Fade2.5D
 includes all the features of the 2D version, plus additional capabilities. However, if
 you prefer, you can switch to the [2D documentation](https://www.geom.at/fade2d/html/).
@endif

@if DOC2D
You are currently viewing the <b>2D</b> documentation. If you are interested in lifted
 triangulations, you can switch to the [2.5D documentation](https://www.geom.at/fade25d/html/).
@endif


 */
class CLASS_DECLSPEC Fade_2D
{
public:
/** \brief Constructor of the triangulation class
 *
 * @param numExpectedVertices The number of points that will be
 * inserted. This is an optional parameter.
 */
	explicit Fade_2D(unsigned numExpectedVertices=3);

/** \brief Copy constructor
 *
 * This constructor creates a copy of the provided `Fade_2D` instance,
 * including the option to specify zones to copy into the new instance.
 *
 * @param [in] other The `Fade_2D` instance to copy from.
 * @param [in] vZoneIn A vector of `Zone2*` objects to copy.
 * @param [out] vZoneOut Used to return pointers of the copied Zone2* objects in the new triangulation.
 */
Fade_2D(Fade_2D& other,std::vector<Zone2*> vZoneIn=std::vector<Zone2*>(),std::vector<Zone2*> vZoneOut=std::vector<Zone2*>());

	/// Destructor
	~Fade_2D();

/** \brief Save a triangulation to a binary file
 *
 * The `saveTriangulation()` method saves all triangles of the current
 * triangulation to a binary file. It retains constraint edges and custom
 * vertex indices, if available. If `Zone2` pointers are provided, these
 * zones will also be saved, and their order will be preserved.
 *
 * @param [in] filename The name of the output file.
 * @param [in] vSaveZones A vector of `Zone2` pointers representing zones to be saved.
 *
 * \sa To store zones alone, use `Zone2::save()` or `Fade_2D::saveZones()`.
 * To reload data from saved files, use `Fade_2D::load()`.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 */
	bool saveTriangulation(const char* filename,std::vector<Zone2*>& vSaveZones);


/** \brief Save a triangulation to a binary file
 *
 * The `saveTriangulation()` method saves all triangles of the current
 * triangulation to a binary file. It retains constraint edges and custom
 * vertex indices, if available. If `Zone2*` pointers are provided, these
 * zones will also be saved, and their order will be preserved.
 *
 * @param stream is the output stream
 * @param [in] vSaveZones A vector of `Zone2` pointers representing zones to be saved.
 *
 * \sa To store zones alone, use `Zone2::save()` or `Fade_2D::saveZones()`.
 * To reload data from saved files, use `Fade_2D::load()`.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 */
	bool saveTriangulation(std::ostream& stream,std::vector<Zone2*>& vSaveZones);

/** \brief Save zones to a binary file
 *
 * The `saveZones()` method saves the triangles of the zones in
 * \p vSaveZones to a binary file. It retains the order of the
 * zones, and maintains the constraint edges and custom vertex
 * indices within the domain.
 *
 * @note A Delaunay triangulation is convex without holes, but this
 * may not be the case for the zones being saved. As a result, additional
 * triangles may be added to fill concavities. These extra triangles will
 * belong to the `Fade_2D` instance, when reloaded, but will not be
 * associated with any reloaded `Zone2`.
 *
 * @param [in] filename The name of the output file
 * @param [in] vSaveZones A non-empty vector of `Zone2` pointers specifying
 * the zones to be saved.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 *
 * \sa Use the `saveTriangulation()` method to store all triangles of
 * a triangulation along with any specified zones. The `Zone2::save()`
 * method can be used to save a single zone. To reload data from a saved
 * file, use `Fade_2D::load()`.
 */
	bool saveZones(const char* filename,std::vector<Zone2*>& vSaveZones);


/** \brief Save zones to a binary file
 *
 * The `saveZones()` method saves the triangles of the zones in
 * \p vSaveZones to a binary file. It retains the order of the
 * zones, and maintains the constraint edges and custom vertex
 * indices within the domain.
 *
 * @note A Delaunay triangulation is convex without holes, but this
 * may not be the case for the zones being saved. As a result, additional
 * triangles may be added to fill concavities. These extra triangles will
 * belong to the `Fade_2D` instance, when reloaded, but will not be
 * associated with any reloaded `Zone2`.
 *
 * @param [in] stream is the name of output stream
 * @param [in] vSaveZones A non-empty vector of `Zone2` pointers specifying
 * the zones to be saved.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 *
 * \sa Use the `saveTriangulation()` method to store all triangles of
 * a triangulation along with any specified zones. The `Zone2::save()`
 * method can be used to save a single zone. To reload data from a saved
 * file, use `Fade_2D::load()`.
 */
	bool saveZones(std::ostream& stream,std::vector<Zone2*>& vSaveZones);



/** \brief Load a triangulation from a binary file
 *
 * This method loads a triangulation from a binary file, including any
 * custom indices, constraint edges, and zones. The file must have been
 * saved using the `saveTriangulation()`, `saveZones()`, or `Zone2::save()`
 * methods.
 *
 * @param [in] filename The name of the input file.
 * @param [out] vZones A vector of `Zone2*` that will be populated with
 * pointers to the zones (if any) stored in the file. The order of the
 * pointers will match the order at the time of storage.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 */
	bool load(const char* filename,std::vector<Zone2*>& vZones);


/** \brief Load a triangulation from a binary file
 *
 * This method loads a triangulation from a binary file, including any
 * custom indices, constraint edges, and zones. The file must have been
 * saved using the `saveTriangulation()`, `saveZones()`, or `Zone2::save()`
 * methods.
 *
 * @param [in] stream  is an input stream
 * @param [out] vZones A vector of `Zone2*` that will be populated with
 * pointers to the zones (if any) stored in the file. The order of the
 * pointers will match the order at the time of storage.
 *
 * @return `true` if the operation was successful, `false` otherwise.
 */
	bool load(std::istream& stream,std::vector<Zone2*>& vZones);


/** \brief Export triangulation data
 *
 * This method exports triangulation data from the current Fade
 * instance to a simple `FadeExport` structure, which is defined
 * in a header and consists solely of arrays containing basic
 * data types. The method also offers the option to clear the
 * Fade object during the export to prevent memory usage peaks.
 *
 * @param [out] fadeExport A `FadeExport` structure that will hold the
 * requested triangulation data after the export operation.
 * @param [in] bWithCustomIndices A boolean flag that determines whether
 * custom indices for the points should be included in the export.
 * @param [in] bClear A boolean flag that specifies whether the Fade
 * instance should be cleared during the export to free memory. If `true`,
 * all memory associated with the Fade instance is released, and all existing
 * pointers to its objects will become invalid.
 */
	void exportTriangulation(FadeExport& fadeExport,bool bWithCustomIndices,bool bClear);

/** \brief Checks the validity of the triangulation
 *
 * @test This method is primarily intended for development and debugging purposes.
 * It is not recommended for use in production code unless you suspect an issue
 * with the triangulation or the algorithm's correctness.
 *
 * This method checks the validity of the triangulation's data structure.
 * It verifies various properties, and can optionally recheck the empty circle
 * property using slow multiprecision arithmetic for higher accuracy.
 *
 * @param [in] bCheckEmptyCircleProperty A boolean flag that specifies whether
 * multiprecision arithmetic should be used to recheck the empty circle property.
 * Enabling this option will slow down the check.
 * @param [in] msg A debug message that will be displayed in the terminal output
 * to indicate which call to `checkValidity` is currently running.
 *
 */
	bool checkValidity(bool bCheckEmptyCircleProperty,const char* msg) const;


/** \brief Set the number of CPU cores for multithreading (deprecated)
*
* @deprecated Use `setGlobalNumCPU()` instead. This method is kept for backward
* compatibility. It internally forwards the call to `setGlobalNumCPU()`.
*
* @param [in] numCPU The number of CPU cores to use for multithreading.
*/
	int setNumCPU(int numCPU);


/** \brief Get the Voronoi diagram of the triangulation
 *
 * This method returns the Voronoi diagram of the existing vertices,
 * which is the dual diagram of their Delaunay triangulation.
 *
 * @return A pointer to a `Voronoi2` instance, representing the dual
 * diagram of the triangulation.
 *
 * @note The Voronoi diagram is dynamically updated when the
 * triangulation changes.
 *
 * @warning While the Voronoi diagram is the dual diagram of a Delaunay triangulation, it
 * is **NOT** dual to a Constrained Delaunay triangulation, so do not create any constraints in it.
 *
 * \image html voronoi-diagram.png "Voronoi diagram" width=30%
 * \image latex voronoi-diagram.png "Voronoi diagram" width=0.3\textwidth
 */
	Voronoi2* getVoronoiDiagram();


/** \brief Set fast mode
 *
 * By default, multiple-precision arithmetic may be used (if required)
 * to compute a 100% perfect Delaunay triangulation. However, the quality
 * difference compared to using double precision arithmetic is hardly noticeable.
 * Use this method to skip computationally expensive calculations and improve
 * performance.
 *
 * @param bFast Set to true to avoid using computationally expensive multiple-precision
 * arithmetic in favor of better performance.
 *
 * Depending on the position of the input points, enabling `setFastMode(true)`
 * may have no effect if multiple-precision arithmetic wouldn't be used anyway,
 * or provide considerable acceleration if it would otherwise be needed.
 */
	void setFastMode(bool bFast);


/** \brief Statistics
 *
 * Prints mesh quality statistics.
 */
 	void statistics(const char*  s) const;

/** \brief Draws the triangulation as a PostScript or PDF file.
 *
 * `show()` is a convenience function for quickly generating outputs with a default
 * appearance. It is also possible to use the Visualizer2 class directly to
 * draw arbitrary circles, line segments, vertices, and labels with
 * custom colors.
 *
 * @param filename The output file name, e.g., "file.ps" for PostScript or "file.pdf" for PDF.
 * @param bWithConstraints Specifies whether constraint segments should be shown (default: true).
 *
 * @see https://www.geom.at/example2-traversing/ for examples of how to use the Visualizer2 class.
 */
 	void show(const char* filename,bool bWithConstraints=true) const;


/** \brief Draws the triangulation as a PostScript or PDF file.
 *
 * `show()` is a convenience function for quickly generating outputs with a default
 * appearance. It is also possible to use the Visualizer2 class directly to
 * draw arbitrary circles, line segments, vertices, and labels with
 * custom colors.
 *
 * @param pVis A pointer to a `Visualizer2` object, which may already contain geometric
 * elements or may later be used to draw additional elements.
 * @param bWithConstraints Specifies whether constraint segments should be shown (default: true).
 *
 * @note After adding the last elements, finalize the visualization by calling
 * `pVis->writeFile()`.
 *
 * @see https://www.geom.at/example2-traversing/ for examples on how to use the Visualizer2 class.
 */

 	void show(Visualizer2* pVis,bool bWithConstraints=true) const;


/** \brief Draws the triangulation using the VTK file format
 *
 * This method generates a .VTK file representing the triangulation, which can be viewed with
 * tools like Paraview.
 *
 * @param filename The output filename, e.g., "file.vtk".
 * @param color The color for the triangles
 * @param constraintColor The color for the constraint edges. `VTK_TRANSPARENT` hides them.
 *
 * @note It is also possible to use the `VtkWriter` class directly to visualize custom
 *       points, line segments, and triangles.
 */
	void showVtk(const char* filename,VtkColor color,VtkColor constraintColor=VTK_TRANSPARENT) const;

/** \brief Draws the triangulation using the VTK file format
 *
 * This method generates a .VTK file representing the triangulation, which can be viewed with
 * tools like Paraview.
 *
 * @param pVtkWriter A pointer to a `VtkWriter` object, which may already contain
 * geometric elements or can be used to add further elements.
 * @param color The color for the triangles.
 * @param constraintColor The color for the constraint edges. Use `VTK_TRANSPARENT` to hide them.
 *
 * @note Once all elements have been drawn, you need to call `pVtkWriter->writeFile()` to save the file.
 *
 * @note It is also possible to use the `VtkWriter` class directly to visualize custom
 *       points, line segments, and triangles.
 */
	void showVtk(VtkWriter* pVtkWriter,VtkColor color,VtkColor constraintColor=VTK_TRANSPARENT) const;


#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Draws the triangulation for the Geomview viewer
	*
	* Draws the triangulation for the Geomview viewer
	*
	* @param filename The name of the output file.
	* @param color The color of the triangulation, default is white (rgba = 1 1 1 0.5).
	*
	* @note The free viewer Geomview (available only on Linux) can be used to view such files.
	*       For 3D visualization on Windows, use the `showVtk()` function instead.
	*/
	void showGeomview(const  char* filename, const char* color="1 1 1 0.5") const;

	/** \brief Draws the triangulation for the Geomview viewer
	*
	* Draws the triangulation for the Geomview viewer
	*
	* @param pVis points to a Visualizer3 object
	* @param color The color of the triangulation, default is white (rgba = 1 1 1 0.5).
	*
	* @note The free viewer Geomview (available only on Linux) can be used to view such files.
	*       For 3D visualization on Windows, use the `showVtk()` function instead.
	*/
	void showGeomview(Visualizer3* pVis, const char* color="1 1 1 0.5") const;
#endif

/** \brief Remove a single vertex
 *
 * @param pVertex The vertex to be removed.
 *
 * \note \p pVertex must not belong to a constraint edge. Otherwise the
 * vertex will not be removed, and a warning will be issued.
 */
 void remove(Point2* pVertex);

/** \brief Remove vertices
*
* @param vPoints are the points to be removed
*
* \note \p vPoints must not be vertices of any constraint edge. Otherwise
* they will not be removed.
*/
	void remove(std::vector<Point2*>& vPoints);


/** \brief Reset the triangulation
 *
 * Resets the triangulation, making it reusable.
 */
    void reset();

/** \brief Compute the convex hull
 *
 * Computes the convex hull of the triangulation.
 *
 * @param [in] bAllVertices A boolean that determines whether all convex hull points
 * are returned or if collinear ones should be removed.
 * @param [out] vConvexHullPointsOut A vector used to return the convex hull
 * vertices in counterclockwise order.
 */
	void getConvexHull(bool bAllVertices,std::vector<Point2*>& vConvexHullPointsOut);

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Insert points from a CloudPrepare object
 *
 * This method inserts points from a `CloudPrepare` object into
 * the triangulation.
 *
 * \param [in] pCloudPrepare A `CloudPrepare` object that
 * contains the point cloud data.
 * \param [in] bClear A boolean flag that determines whether
 * `pCloudPrepare` should be cleared during the operation
 * to save memory. It is recommended to use `bClear=true` unless
 * you plan to reuse `pCloudPrepare` for other triangulations.
 *
 * @see https://www.geom.at/mesh-improvements This article contains
 * example code that demonstrates the use of CloudPrepare objects.
 *
 *
 * @note There is a difference in peak memory usage compared to other
 * `insert()` methods: You can provide the vertices to the `CloudPrepare`
 * object and remove them from your software's data structures before
 * any triangles are created. Only after calling this `insert()` function
 * with the `CloudPrepare` object will the vertices be triangulated.
 *
 * @note To activate multithreading, use `setGlobalNumCPU()` before using this method.
 */
	void insert(CloudPrepare* pCloudPrepare,bool bClear=true);
#endif


/** \brief Insert a single point
 *
 * This method inserts a single point into the triangulation.
 *
 * \param p The point to be inserted.
 * \return A pointer to the vertex in the triangulation.
 *
 * The triangulation keeps a copy of \p p, and the return value is a pointer to this copy.
 * If duplicate points are inserted, the triangulation does not create new copies but
 * instead returns a pointer to the copy of the first insertion.
 *
 * @note This method offers very good performance, but it is still faster to pass all
 * points at once, if possible.
 */

	Point2* insert(const Point2& p);

/** \brief Insert a vector of points
 *
 * This method inserts a vector of points into the triangulation.
 *
 * \param vInputPoints A vector containing the points to be inserted.
 *
 * @note To activate multithreading, use `setGlobalNumCPU()` before using this method.
 */
	void insert(const std::vector<Point2>& vInputPoints);



/** \brief Insert points from a std::vector<Point2> and store pointers in \p vHandles
 *
 * This method inserts points from \p vInputPoints and stores the corresponding
 * pointers to the inserted points in \p vHandles.
 *
 * \param vInputPoints A vector containing the points to be inserted.
 * \param vHandles An empty vector that will be populated with `Point2` pointers.
 *
 * Internally, the triangulation keeps copies of the inserted points, and these copies
 * are returned in \p vHandles (in the same order as in \p vInputPoints). If duplicate
 * points are found in \p vInputPoints, only one copy will be created, and a pointer
 * to this unique copy will be stored in \p vHandles for each occurrence of that point.
 *
 * @note To activate multithreading, use `setGlobalNumCPU()` before using this method.
 */
	void insert(const std::vector<Point2>& vInputPoints,std::vector<Point2*>& vHandles);

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Insert points from an array
 *
 * This method inserts points from an array of coordinates and stores the corresponding
 * pointers to the inserted points in \p aHandles.
 *
 * \param numPoints The number of points to be inserted.
 * \param aCoordinates An array of \e 3n double values, representing the coordinates of
 *        the points in the format: {x0, y0, z0, x1, y1, z1, ..., xn, yn, zn}.
 * \param aHandles An array of size \e n where pointers to the inserted points will be stored.
 *
 * @note To activate multithreading, use `setGlobalNumCPU()` before using this method.
*/
#else
/** \brief Insert points from an array
 *
 * This method inserts points from an array of coordinates and stores the corresponding
 * pointers to the inserted points in \p aHandles.
 *
 * \param numPoints The number of points to be inserted.
 * \param aCoordinates An array of \e 2n double values, representing the coordinates of
 *        the points in the format: {x0, y0, x1, y1, ..., xn, yn}.
 * \param aHandles An empty array of size \e n where pointers to the inserted points will be stored by Fade.
 *
 * \note To activate multithreading, use `Fade_2D::setNumCPU()` before using this method.
 */
#endif
	void insert(int numPoints,double * aCoordinates,Point2 ** aHandles);



/** \brief Measure the Delaunay triangulation time
 *
 * This method measures the time taken to create a Delaunay triangulation from a given set of points.
 *
 * \image html delaunay-benchmark.png "Benchmark of single- and multithreaded Delaunay triangulation (Intel i9 10980XE, Linux)" width=60%
 * \image latex delaunay-benchmark.png "Benchmark of single- and multithreaded Delaunay triangulation (Intel i9 10980XE, Linux)" width=0.9\textwidth
 *
 * \param [in] vPoints A vector of points to be inserted into the triangulation.
 *
 * \return The total wall time for point insertion in seconds.
 *
 * \note To enable multithreading, use `setGlobalNumCPU()`.
 */
	double measureTriangulationTime(const std::vector<Point2>& vPoints);




/** \brief Locate a triangle that contains \e p
 *
 * \image html locate.jpg "Point location"
 * \image latex locate.eps "Point location" width=7cm
 *
 *
 * \if DOC25D
 * This method searches for and returns a pointer to the `Triangle2` that contains the query point \p p.
 * The height (z-coordinate) of the query point is ignored in this method.
 * \endif
 *
 * \if DOC2D
 * This method searches for and returns a pointer to the `Triangle2` that contains the query point \p p.
 * \endif
 *
 * \param [in] p The query point whose containing `Triangle2` is to be found.
 *
 * \return A pointer to a `Triangle2` containing \p p, or `NULL` if \p p lies outside
 * the triangulation.
 */

	Triangle2* locate(const Point2& p);


/** \brief Get nearest neighbor
 *
 * This method returns the closest vertex to the point \p p.
 *
 * \image html getNearestNeighbor.png "Nearest neighbor query: The query point (red) and the result vertex (blue)" width=15%
 * \image latex getNearestNeighbor.png "Nearest neighbor query: The query point (red) and the result vertex (blue)" width=0.4\textwidth
 *
 * @param [in] p A pointer to the query point.
 * @return A pointer to the closest vertex to \p p.
 */
Point2* getNearestNeighbor(const Point2& p);

#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Compute the height of a certain point
 *
 * Computes the height (z) at the coordinates \p x and \p y, assigns it
 * to \p heightOut, and returns `true` if successful.
 *
 * @param [in] x, y The input coordinates.
 * @param [out] heightOut The computed height.
 * @param [in] pApproxT An optional parameter that can be set to a nearby
 * triangle to speed up the search. However, point location is very fast,
 * and if you're unsure, using `NULL` is likely faster.
 * @param [in] tolerance A tolerance value (default: 0). See below for details.
 *
 * @note Due to rounding errors, your query point may lie slightly outside
 * the convex hull of the triangulation. In this case, the method will correctly
 * return `false`. However, you can use the optional \p tolerance parameter
 * (default: 0). If your query point is within the \p tolerance of the convex
 * hull, the method will return the height of the closest point on the convex hull.
 */

	bool getHeight(double x,double y,double& heightOut,Triangle2* pApproxT=NULL,double tolerance=0) const;
#endif

/** \brief Delaunay refinement
 *
 * Creates a mesh inside the area given by a Zone2 object.
 *
 * @param pZone is the zone whose triangles are refined. Allowed zoneLocation values are ZL_INSIDE and ZL_BOUNDED.
 * @param minAngleDegree (up to 30) is the minimum interior triangle angle
 * @param minEdgeLength is a lower threshold on the edge length. Triangles with smaller edges are not refined.
 * @param maxEdgeLength is an upper threshold on the edge length. Triangles with larger edges are always refined.
 * @param bAllowConstraintSplitting specifies if constraint edges may be splitted
 *
 * @note You can convert any Zone2 into a bounded zone using
 * @ref convertToBoundedZone "Zone2::convertToBoundedZone()".
 *
 * @see void refineAdvanced() This method allows more control over the refinement process
 */

	void refine(	Zone2* pZone,
					double minAngleDegree,
					double minEdgeLength,
					double maxEdgeLength,
					bool bAllowConstraintSplitting
					);

/** \brief Delaunay refinement and grid meshing
 *
 * This method calls an advanced Delaunay mesh generator and grid mesher.
 * The parameters are encapsulated in the `MeshGenParams` class, which provides
 * default parameters that can be used as is. Alternatively, client code can derive
 * from `MeshGenParams` and override the methods and parameters to gain full control
 * over the mesh generation process.
 *
 * \see https://www.geom.at/advanced-mesh-generation/ This article provides examples on advanced mesh generation.
 *
 *
\htmlonly
<div class="sbs4image">
<a href="http://www.geom.at/wp-content/uploads/2021/01/delaunay_triangulation.jpg">
 <img style="width: 95%" src="http://localhost/geom_homepage_www/fade25d/html/mesh-generator-customParameters.png" alt="Control over the local mesh density in a Delaunay Mesh" /></a>
  <p>Control over the local mesh density</p>
</div>
<div class="sbs4image">
<a href="http://www.geom.at/wp-content/uploads/2021/01/constrained_delaunay.jpg">
 <img style="width: 95%" src="http://www.geom.at/wp-content/uploads/2021/01/mesh-generator-maxLength.png" alt="Delaunay meshing with maximum edge length" /></a>
  <p>Delaunay meshing with maximum edge length</p>
</div>
<div class="sbs4image">
<a href="http://www.geom.at/wp-content/uploads/2021/01/delaunay_zones.png">
 <img style="width: 95%" src="http://localhost/geom_homepage_www/fade25d/html/grid-mesh.png" alt="Axis-aligned grid mesh" /></a>
  <p>Grid mesh, axis-aligned</p>
</div>
<div class="sbs4image">
<a href="http://www.geom.at/wp-content/uploads/2021/01/delaunay-mesher-fade_l.png">
 <img style="width: 95%" src="http://localhost/geom_homepage_www/fade25d/html/grid-mesh-angle.png" alt="Grid mesh aligned to a certain direction" /></a>
  <p>Grid mesh, aligned to a certain direction</p>
</div>
<div class="floatStop"></div>
\endhtmlonly
 *
*/
	void refineAdvanced(MeshGenParams* pParameters);






/** \brief Number of points
 *
 * Computes the number of points in the triangulation.
 *
 * @return the number of points in the triangulation.
 *
 * @note Due to possibly duplicate input points the number of points is
 * not stored somewhere but freshly computed in O(n) time. This is fast
 * but you are adviced to avoid calling this method over-frequently in
 * a loop. Duplicate point insertions count only once.
 */
	size_t numberOfPoints() const;

/** \brief Number of triangles
 *
 * Computes the number of points in the triangulation
 *
 * @return the number of triangles in the triangulation.
 */
 	size_t numberOfTriangles() const;


/** \brief Get pointers to all triangles
 *
 * This method retrieves pointers to all existing triangles in the triangulation.
 *
 * @param [out] vAllTriangles A vector used to return the pointers to all triangles.
 *
 */
	void getTrianglePointers(std::vector<Triangle2*>& vAllTriangles) const;

/** \brief Get pointers to all vertices
 *
 * Returns all vertex pointers of the triangulation. The order in which the points
 * are returned is arbitrary.
 *
 * @param [out] vAllPoints A vector to return the vertex pointers.
 *
 * @note For duplicate point insertions, only one vertex pointer exists and it
 * is stored only once. Thus, the number of returned points is smaller than the
 * number of inserted points if duplicate points have been inserted.
 */
void getVertexPointers(std::vector<Point2*>& vAllPoints) const;

/** \brief Get vertex pointer
 *
 * Checks if the given point \p is a vertex of the triangulation.
 * If \p is a vertex, the function returns a pointer to that vertex.
 * Otherwise, it returns NULL.
 *
 * @param [in] p The query point.
 * @return A pointer to the vertex if \p is a vertex of the triangulation, or NULL if \p is not a vertex.
 */
Point2* getVertexPointer(const Point2& p);

/** \brief Get the adjacent triangle of an oriented edge
 *
 * A method to retrieve the counterclockwise adjacent triangle of an oriented edge of the triangulation.
 *
 * \image html getAdjacentTriangle.png "The adjacent triangle of edge (p0,p1)" width=15%
 * \image latex getAdjacentTriangle.png "The adjacent triangle of edge (p0,p1)" width=0.3\textwidth
 *
 * @param [in] p0, p1 Pointers to the vertices of the triangulation that form the endpoints
 * of the oriented edge.
 * @return A pointer to the `Triangle2` containing the oriented edge (p0, p1),
 * or NULL if no such triangle is present.
 *
 * @note Recall the counter-clockwise enumeration of vertices in a `Triangle2`:
 * While an unoriented edge (p0, p1) can have two adjacent triangles, only one
 * of them contains the <b>oriented</b> edge (p0, p1), while the other contains
 * the <b>oriented</b> edge (p1, p0).
*/
Triangle2* getAdjacentTriangle(Point2* p0,Point2* p1) const;


/** \brief Check if the triangulation contains any triangles
 *
 * A triangulation only contains triangles if at least three non-collinear points
 * exist. If all inserted points are collinear, no triangles can be formed,
 * regardless of how many points have been inserted. This method checks whether
 * the triangulation currently contains any triangles, i.e., if an area exists.
 *
 * \image html coll.jpg "Collinear points (black), triangles are generated as soon as the first non-collinear point (red) is inserted."
 * \image latex coll.eps "Collinear points (black), triangles are generated as soon as the first non-collinear point (red) is inserted." width=7cm
 *
 * @return true if at least one triangle exists, false otherwise
 */
	bool hasArea() const;

/** @private
 *  The function name is2D() was misleading. Thus it has been replaced
 *  by hasArea(). The deprecated name is2D() is kept for backwards
 *  compatibility
*/
	bool is2D() const;

/** \brief Get the median 2D edge length
 *
 * Computes the median edge length of all edges in the triangulation in 2D.
 * \if DOC25D
 * This method considers the projection of the edges onto the XY plane,
 * ignoring the z-coordinate. To compute the 2.5D median edge length, use
 * getMedianEdgeLength25D().
 * \endif
 *
 * @return The median 2D edge length of all edges in the triangulation.
 */
	double getMedianEdgeLength2D();

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get the median 2.5D edge length
*
* Computes the median 2.5D edge length of all edges in the triangulation.
*
* @return The median 2.5D edge length.
*/
	double getMedianEdgeLength25D();
#endif


/**
 * @brief Insert constraint edges (edges, polyline or polygon)
 * @anchor createConstraint
 *
 * @param vSegments Segments to be inserted as enforced edges of the triangulation.
 *
 * @param cis Use CIS_CONSTRAINED_DELAUNAY, the supported constraint-insertion-strategy.
 *
 * @param bOrientedSegments Specifies whether the input segments in @p vSegments are
 * oriented with a counterclockwise direction around an enclosed area. This property
 * is required if the returned ConstraintGraph2 object shall be used for later Zone2
 * creation. If @p bOrientedSegments is false and @p vSegments represents a simple
 * polygon, the edges are automatically reoriented to establish this property. For
 * complex or uncertain polygon orientations, use the PolygonClipper to repair and
 * orient @p vSegments upfront.
 *
 * @if DOC25D
 * @param bUseHeightOfLatest Determines height priority in conflict cases (e.g., when
 * crossing an existing vertex or constraint segment). If true, assigns the height (z)
 * of the last inserted segment to the intersection point. If false, the height of
 * the crossed, pre-existing element is used for the intersection point.
 * @endif
 *
 * @return A pointer to the created ConstraintGraph2 object
 *
 * The present method subdivides the segments only if required, which is the case if existing
 * vertices or constraint edges are crossed. If you want to subdivide the created
 * constraint edges subsequently to achieve better triangle shapes (see the third image),
 * use ConstraintGraph2::makeDelaunay() on the returned ConstraintGraph2.
 *
 * @if DOC25D
 * The segments are inserted at their original elevation, i.e., no projection onto an
 * existing triangulation takes place. If this is desired, you can use Fade_2D::drape()
 * to project the segments onto a surface upfront.
 *
 * @see Fade_2D::drape() is a method that projects segments onto an existing triangulation
 * @endif
 *
 * @see ConstraintGraph2::makeDelaunay() subdivides a ConstraintGraph2 to
 * achieve better triangle shapes.
 *
 * @see https://www.geom.at/example4-zones-defined-areas-in-triangulations/ provides
 * examples of Zone2 creation.
 *
 * @see https://www.geom.at/polygon-clipper-repairing-polygons/ contains examples of repairing polygons.
 *
 * @image html bare-delaunay.png "Delaunay triangulation without constraints" width=40%
 * @image latex bare-delaunay.eps "Delaunay triangulation without constraints" width=6cm
 *
 * @image html constrained_delaunay.png "Constrained Delaunay triangulation" width=40%
 * @image latex constrained_delaunay.eps "Constrained Delaunay triangulation" width=6cm
 *
 * @image html conforming-delaunay-makeDelaunay.png "Conforming Delaunay triangulation using ConstraintGraph::makeDelaunay()" width=40%
 * @image latex conforming-delaunay-makeDelaunay.png "Conforming Delaunay triangulation using ConstraintGraph::makeDelaunay()" width=6cm
 */
#if GEOM_PSEUDO3D==GEOM_TRUE
	ConstraintGraph2* createConstraint(
		std::vector<Segment2>& vSegments,
		ConstraintInsertionStrategy cis=CIS_CONSTRAINED_DELAUNAY,
		bool bOrientedSegments=false,
		bool bUseHeightOfLatest=false
		);
#else
	ConstraintGraph2* createConstraint(
		std::vector<Segment2>& vSegments,
		ConstraintInsertionStrategy cis,
		bool bOrientedSegments=false
		);
#endif





/** \brief Create a Zone2
* \anchor createZone
*
* Creates a Zone2 object which represents an area of a triangulation.
*
* @param [in] zoneLoc can be ZL_INSIDE, ZL_OUTSIDE or ZL_GLOBAL.
* @param [in] pConstraintGraph points to an existing ConstraintGraph2
* object (which must be oriented) or is NULL if zoneLoc is ZL_GLOBAL.
* @param [in] bVerbose is `true` by default and causes a warning if NULL is returned.
*
* @return A pointer to the created `Zone2` object, or `NULL` if no triangles
* exist, or if `pConstraintGraph->isOriented()` returns `false`. Make sure to
* check for `NULL` before proceeding. @n
*
\htmlonly <div class="center"> \endhtmlonly
	<div style="text-align: center;">
	    <img src="bikeZones.png" alt="A zone in a 2D Delaunay triangulation" width="40%" />
	    <div><b>A Zone in a 2D Delaunay triangulation</b></div>
	</div>
\htmlonly </div> \endhtmlonly
\image latex bikeZones.png "A zone in a 2D Delaunay triangulation" width=0.6\textwidth
*/
	Zone2* createZone(ConstraintGraph2* pConstraintGraph,ZoneLocation zoneLoc,bool bVerbose=true);

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get a Zone2 excluding nearly vertical border triangles
 *
 * Triangulations often have narrow, nearly vertical triangles at their
 * borders. This method returns a `Zone2` that includes all triangles of the
 * present triangulation, excluding these nearly vertical border triangles.
 * If no triangles remain, the method returns NULL.
 *
 * \image html peeled-border-triangles.png "Left: A triangulation containing nearly vertical border triangles. Right: Vertical border triangles removed." width=50%
 * \image latex peeled-border-triangles.png "Left: A triangulation containing nearly vertical border triangles. Right: Vertical border triangles removed." width=0.8\textwidth
 *
 * @param [in] angleThreshold The maximum allowed angle between the triangle's normal
 * and the vertical vector (0,0,1). For example, use 89.9 degrees to exclude steep
 * boundary triangles.
 * @param [in] bAvoidSplit Prevents the zone from splitting into disconnected
 * components. If true, keeps triangles that cannot be removed without splitting
 * the zone into separate parts.
 * @return A pointer to the `Zone2` object, or NULL if no triangles remain.
 *
 * \see Zone2::peelOffIf() This function provides more flexibility when peeling off triangles.
 * \see https://www.geom.at/mesh-improvements/ for example code that removes unwanted border triangles.
 */
	Zone2* getPeeledZone(double angleThreshold,bool bAvoidSplit);
#endif


/**
 * @brief Create a Zone2 bounded by multiple ConstraintGraph2 objects by growing from a seed point
 *
 * Creates a `Zone2` object bounded by multiple `ConstraintGraph2` objects, grown from a
 * seed point.
 *
 * \image html zone_from_seed.png "Zone2, grown from a seed point until the constraint edges limit the growing" width=50%
 * \image latex zone_from_seed.png "Zone2, grown from a seed point until the constraint edges limit the growing" width=0.5\textwidth
 *
 * @param vConstraintGraphs A vector of `ConstraintGraph2` objects.
 * @param zoneLoc The `ZoneLocation` must be `ZL_GROW`.
 * @param startPoint The point from which the area grows until the borders defined in `vConstraintGraphs` are reached.
 * @param bVerbose Is `true` by default and triggers a warning if `NULL` is returned.
 *
 * @return A pointer to the created `Zone2` object, or `NULL` if `zoneLoc != ZL_GROW` or no triangles exist.
 */
	Zone2* createZone(const std::vector<ConstraintGraph2*>& vConstraintGraphs,ZoneLocation zoneLoc,const Point2& startPoint,bool bVerbose=true);


/**
 * @brief Create a Zone2 bounded by a ConstraintGraph2 object by growing from a seed point
 *
 * Creates a `Zone2` object bounded by a `ConstraintGraph2` object, grown from a
 * seed point.
 *
 * \image html zone_from_seed.png "Zone2, grown from a seed point until the constraint edges limit the growing" width=50%
 * \image latex zone_from_seed.png "Zone2, grown from a seed point until the constraint edges limit the growing" width=0.5\textwidth
 *
 * @param pConstraintGraph A pointer to the `ConstraintGraph2` that defines the boundary.
 * @param zoneLoc The `ZoneLocation` must be `ZL_GROW`.
 * @param startPoint The point from which the area grows until the borders defined in @p pConstraintGraph are reached.
 * @param bVerbose Is `true` by default and triggers a warning if `NULL` is returned.
 *
 * @return A pointer to the created `Zone2` object, or `NULL` if `zoneLoc != ZL_GROW` or no triangles exist.
 */
	Zone2* createZone(ConstraintGraph2* pConstraintGraph,ZoneLocation zoneLoc,const Point2& startPoint,bool bVerbose=true);


/**
 * @brief Create a Zone2 defined by a vector of triangles
 *
 * This method creates a `Zone2` defined by the triangles in `@p vTriangles`.
 *
 * @param vTriangles A vector of `Triangle2*` pointers defining the area.
 * @param bVerbose Triggers a warning if `NULL` is returned. Default: `true`.
 * @return A pointer to the created `Zone2` object, or `NULL` if `vTriangles` is empty.
 *
 * \image html zoneFromTriangles.png "Zone2, defined by triangles" width=50%
 * \image latex zoneFromTriangles.png "Zone2, defined by triangles" width=0.5\textwidth
 */
	Zone2* createZone( std::vector<Triangle2*>& vTriangles,bool bVerbose=true );



/** \brief Delete a Zone2 object
*
* Zone2 objects are automatically destroyed with their Fade_2D objects.
* In addition this method provides the possibility to eliminate Zone2
* objects earlier.
*
* \note Zones are designed transparently: When two zones Z1 and Z2 are
* combined to a new one Z3 (for example through a boolean operation)
* then Z1, Z2 and Z3 form a tree datastructure such that changes in the
 * leaf nodes Z1 and Z2 can propagate up to the root node Z3. For this
 * reason Z1 and Z2 must be alive as long as Z3 is used.
*/
	void deleteZone(Zone2* pZone);


/** \brief Apply conforming constraints and zones (deprecated!)
* @deprecated The present function applyConstraintsAndZones() as well as the
* two constraint insertion strategies CIS_CONFORMING_DELAUNAY and
* CIS_CONFORMING_DELAUNAY_SEGMENT_LEVEL are deprecated. These are
* only kept for backwards compatibilty. The replacement is
* CIS_CONSTRAINED_DELAUNAY along with the methods Fade_2D::drape()
* and/or ConstraintGraph2::makeDelaunay(). See the example code in
* examples_25D/terrain.cpp
*
* This method establishes conforming constraint segments and zones
* which depend on them. For technical reasons conforming constraint
* segments are not immediately established but inserted at the end
* of the triangulation process. This step must be triggered manually
* i.e., it is up to the user to call applyConstraintsAndZones() before
* the resulting triangulation is used. If afterwards the triangulation
* is changed in any way, applyConstraintsAndZones() must be called again.

*/

	void applyConstraintsAndZones();

/**
 * @brief Computes the axis-aligned bounding box of the triangulation
 *
 * This method calculates the smallest axis-aligned bounding box (AABB)
 * that encloses all points currently present in the triangulation.
 *
 * @return Bbox2 An object representing the computed bounding box.
 *
 * @note If no points have been inserted into the triangulation yet,
 * the returned Bbox2 object is invalid. In this case, calling
 * Bbox2::isValid() on the returned object will return false.
 */
	Bbox2 computeBoundingBox() const;


/** \brief Check if an edge is a constraint edge
 *
 * This method checks whether the edge opposite to the \p ith vertex in the triangle \p pT
 * is a constraint edge.
 *
 * \param pT The triangle to check.
 * \param ith The index of the vertex in the triangle.
 *
 * \return A boolean value indicating whether the edge opposite to the \p ith vertex is a
 *         constraint edge (true) or not (false).
 */

	bool isConstraint(Triangle2* pT,int ith) const;

/** \brief Get active (alive) constraint segments
 *
 * This method retrieves the active (alive) constraint segments and stores them in the
 * provided vector.
 *
 * \param vAliveConstraintSegments A reference to a vector that will be populated
 *        with pointers to the active constraint segments.
 */
void getAliveConstraintSegments(std::vector<ConstraintSegment2*>& vAliveConstraintSegments) const;

/** \brief Get all (alive and dead) constraint segments
 */
	void getAliveAndDeadConstraintSegments(std::vector<ConstraintSegment2*>& vAllConstraintSegments) const;

/** \brief Retrieve a ConstraintSegment2
 *
 * @param [in] p0,p1 The endpoints of the ConstraintSegment2
 *
 * @return A pointer to the `ConstraintSegment2` between p0 and p1, or NULL
 * if the segment is not a constraint edge of the triangulation (or is dead
 * because it has been split).
 */
	ConstraintSegment2* getConstraintSegment(Point2* p0,Point2* p1) const;

/** \brief Get incident triangles
 *
 * Stores pointers to all triangles around the vertex \p pVtx into the vector \p vIncidentT.
 *
 * @param [in] pVtx A pointer to a vertex of the triangulation.
 * @param [out] vIncidentT A vector used to return pointers to the incident triangles of \p pVtx.
 *
 * \image html incidentTriangles.png "Incident triangles of Point2* pVtx" width=20%
 * \image latex incidentTriangles.png "Incident triangles of Point2* pVtx" width=0.35\textwidth
 */
	void getIncidentTriangles(Point2* pVtx,std::vector<Triangle2*>& vIncidentT) const;


/** \brief Get adjacent vertices
 *
 * Stores pointers to all vertices around the vertex \p pVtx into the vector \p vAdjVtx
 *
 * @param [in] pVtx A pointer to a vertex of the triangulation.
 * @param [out] vAdjVtx A vector used to return pointers to all vertices adjacent to \p pVtx.
 *
 * \image html incidentVertices.png "Adjacent vertices of Point2* pVtx" width=20%
 * \image latex incidentVertices.png "Adjacent vertices of Point2* pVtx" width=0.35\textwidth
 */
	void getIncidentVertices(Point2* pVtx,std::vector<Point2*>& vAdjVtx) const;


/** \brief Write the current triangulation to an .obj file
 *
 * This method writes the current triangulation data to an .obj file,
 * which can be used for visualization and further processing in tools
 * like Meshlab.
 *
 * @param filename The output filename, e.g., "triangulation.obj".
 */
	void writeObj(const char*  filename) const;

#ifndef SKIPTHREADS
#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Write the current triangulation to a *.ply file
 *
 * This method writes the current triangulation data to a *.ply file.
 * The file can be saved in either ASCII or binary format.
 *
 * @param filename The output filename, e.g., "triangulation.ply".
 * @param bASCII Specifies whether to write the *.ply file in ASCII format
 *               (true) or binary format (false). Default is false (binary).
 *
 * @note This method is available only on platforms that support C++11 or higher.
 */
	bool writePly(const char*  filename,bool bASCII=false) const;


/** \brief Write the current triangulation to a *.ply file
 *
 * This method writes the current triangulation data to a *.ply file.
 * The file can be saved in either ASCII or binary format.
 *
 * @param os The output stream (e.g., std::ofstream) where the data will be written.
 * @param bASCII Specifies whether to write the *.ply file in ASCII format
 *               (true) or binary format (false). Default is false (binary).
 *
 * @note This method is available only on platforms that support C++11 or higher.
 */
	bool writePly(std::ostream& os,bool bASCII=false) const;
#endif
#endif


/** \brief Write a Zone2 an .obj file
 *
* This method writes a Zone2 to an .obj file, which can be used for
 * visualization and further processing in tools like Meshlab.
 *
 * @param filename The output filename, e.g., "triangulation.obj".
 * @param pZone A pointer to the pZone to be written.
 */
	void writeObj(const char*  filename,Zone2* pZone) const;

/** \brief Create a scene of a triangulation that can be viewed in a web browser
 *
 * This method generates a scene of the triangulation, which can be viewed in a web browser.
 *
 * @param path The output filename or file path where the scene will be saved.
 */
	void writeWebScene(const char* path) const;

/** \brief Create a scene of a Zone2 that can be viewed in a web browser
 *
 * This method generates a scene of a `Zone2` object, which can be viewed in a web browser.
 *
 * @param path The output filename or file path where the scene will be saved.
 * @param pZone A pointer to the `Zone2` object to be visualized.
 *
 */
void writeWebScene(const char* path,Zone2* pZone) const;


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

/** \brief Check if an edge is a constraint edge
 *
 * This method checks whether the edge formed by the points \p p0 and \p p1 is a
 * constraint edge.
 *
 * \param p0,p1 Pointers to endpoints of the edge
 *
 * \return A boolean value indicating whether the edge (p0, p1) is a constraint edge
 *         (true) or not (false).
 */
	bool isConstraint(Point2* p0,Point2* p1) const;

/** \brief Check if a vertex is a constraint vertex
 *
 * This method checks whether the vertex \p pVtx belongs to a constraint edge.
 *
 * \param pVtx Pointer to the vertex to check.
 *
 * \return A boolean value indicating whether the vertex \p pVtx belongs to a constraint edge
 *         (true) or not (false).
 */
	bool isConstraint(Point2* pVtx) const;

	/// Prints license information
	void printLicense() const;

	/// @private
	bool checkZoneQuality(Zone2* pZone,double minAngle,const char*  name);
	/// @private
	void setName(const char* s);
	/// @private
	std::string getName() const;


	/**
	 * @brief Deletes all constraints and zones.
	 *
	 * This method deletes all `ConstraintGraph2` objects, all `ConstraintSegment2` objects, and all `Zone2` objects.
	 * It does not modify existing triangles; existing edges that have been created as constraint segments before are
	 * not flipped back to satisfy the 'empty circle property' of a Delaunay triangulation. However, these edges become
	 * unprotected, and any subsequent modifications to the triangulation may cause them to be flipped.
	 *
	 * @warning This method is dangerous. It violates the 'empty circle property', and it invalidates the pointers of
	 * the deleted objects. Use it only if you are fully aware of the consequences.
	 */
	void deleteConstraintsAndZones();


	/** \brief Import Triangles (deprecated in favor of the robust version)
*
* This method imports triangles into an empty triangulation.
*
* @deprecated
* Using this older version of the method is only recommended if you are certain that
* your input triangulation is 100% correct. If there is any possibility that the input
* points have been rounded (e.g., due to conversion from 64-bit to 32-bit precision or
* simply because they were parsed from an ASCII file), you should use the robust version,
* importTriangles_robust(), instead.
*
* \param vPoints A vector containing the input triangulation, with 3 subsequent points
* per triangle.
* \param bReorientIfNeeded Specifies whether the orientations of the point triples
* should be checked and corrected. If you are confident that the point triples are
* already oriented in counterclockwise order, you can skip the orientation test.
* \param bCreateExtendedBoundingBox If true, the four corners of an extended bounding
* box will be inserted as dummy points. This can be useful in some cases. If unsure,
* set this to false.
*
* \return A pointer to a Zone2 object, or NULL if the input data is invalid.
*
*/
	Zone2* importTriangles(	std::vector<Point2>& vPoints,
							bool bReorientIfNeeded,
							bool bCreateExtendedBoundingBox
						);


/** \brief Import Triangles - Robust (Error-tolerant version)
*
* This method imports triangles into an empty Fade object. Unlike the classic
* `importTriangles()` method, this robust version handles errors in the input
* data and automatically corrects overlapping triangles. This robust version
* is very fast, and due to the additional checks, it is also very safe. Thus,
* it is highly recommended to use it always rather than the classic importTriangles()
* method.
*
* \param vPoints A vector containing the input triangulation, with 3 subsequent
* points per triangle.
*
* \return A pointer to a Zone2 object, or NULL if the input data is invalid.
*
* @note A Delaunay triangulation is always <b>convex</b>, and thus additional
* fill-triangles are created, if necessary, to complete the convex hull.
* Consequently, when you later call Fade_2D::getTrianglePointers(), you will
* also receive the <b>additional triangles</b>. If you are only interested
* in the triangles that you have imported, use the Zone2* returned by the
* present command, and call Zone2::getTriangles() instead.
*/
	Zone2* importTriangles_robust(std::vector<Point2>& vPoints);

#ifndef SKIPTHREADS
#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Import Triangles from *.PLY
*
* This method imports triangles from a *.PLY file into an empty Fade
* object. This method is robust i.e., it works also in presence of 
* errors in the input data, and corrects overlapping triangles
* automatically. 
*
* \param filename contains the input *.PLY file (ASCII or binary)
* \return a pointer to a Zone2 object or NULL if the input data is invalid.
 *
 * @note Method available for platforms with C++11
*/
	Zone2* importTrianglesFromPly(const char* filename);

/** \brief Import Triangles from *.PLY
*
* This method imports triangles from a *.PLY file into an empty Fade
* object. This method is robust i.e., it works also in presence of 
* errors in the input data, and corrects overlapping triangles
* automatically. 
*
* \param is is the input *.PLY file (ASCII or binary)
* \return a pointer to a Zone2 object or NULL if the input data is invalid.
 *
 * @note Method available for platforms with C++11
*/
	Zone2* importTrianglesFromPly(std::istream& is);
#endif
#endif

/** \brief Compute the orientation of 3 points
 *
 * Determines if the ordered points \p p0, \p p1, and \p p2 are collinear,
 * clockwise, or counterclockwise.
 *
 * @param [in] p0,p1,p2 The points whose orientation is computed.
 *
 * @return `ORIENTATION2_COLLINEAR`, `ORIENTATION2_CW`, or `ORIENTATION2_CCW`.
 */
Orientation2 getOrientation(const Point2& p0,const Point2& p1,const Point2& p2);

/** \brief Cut through a triangulation
*
* \param knifeStart is one point of the knife segment
* \param knifeEnd is the second point of the knife segment
* \param bTurnEdgesIntoConstraints turns all 3 edges of each intersected
* triangle into constraint segments.
*
* This method inserts a constraint edge \e knife(\e knifeStart,\e knifeEnd).
* If existing edges \e E are intersected by \e knife, then \e knife is
* subdivided at the intersection points \e P.
*
* In any case \e knife will exist (in a possibly subdivided form) in
* the result. But a consequence of the insertion of the points \e P
* is that the edges \e E and even edges which are not intersected by
* \e knife may be flipped. Use bTurnEdgesIntoConstraints=true to avoid
* that.
*
* \note The intersection point of two line segments may not be exactly
* representable in double precision floating point arithmetic and thus
* tiny rounding errors may occur. As a consequence two very close
* intersection points may be rounded to the same coordinates. \n
*
* \note When more than one knife segment is inserted then the method
* void cutTriangles(std::vector<Segment2>& vSegments,bool bTurnEdgesIntoConstraints)
* should be used. The reason is that each individual cut operation
* changes the triangulation and thus iterative calls to the present
* version of the method can lead to a different result. \n
*
*
*/
	void cutTriangles(	const Point2& knifeStart,
						const Point2& knifeEnd,
						bool bTurnEdgesIntoConstraints);

/** \brief Cut through a triangulation
*
* \param vSegments are the knife segments
* \param bTurnEdgesIntoConstraints specifies if intersected edges shall automatically be turned into constraints
*
* Same method as void cutTriangles(const Point2& knifeStart,const Point2& knifeEnd,bool bTurnEdgesIntoConstraints)
* but it takes a vector of segments instead of a single segment. This
* is the recommended method to cut through a triangulation when more
* than one knife segment exists.
*
*/

	void cutTriangles(	std::vector<Segment2>& vSegments,
						bool bTurnEdgesIntoConstraints);


/** \brief Cookie Cutter (deprecated)
 * @deprecated This method is deprecated. It works as expected, but does not support
 * zones with holes. To avoid behavioral changes, this method is left unchanged and
 * has been deprecated in favor of the new method Fade_2D::createZone_cookieCutter2(),
 * which allows for zones with holes, is faster, and provides improved numerical
 * accuracy.
 */
Zone2* createZone_cookieCutter(std::vector<Segment2>& vSegments,bool bProtectEdges);

/** \brief Replicate the boundaries of a zone in the current Fade_2D instance.
 *
 * This method replicates only the boundaries of the given `Zone2` `pZoneIn` in the current
 * `Fade_2D` instance. It is primarily intended to facilitate boolean operations with zones,
 * as these operations require the zones to belong to the same Fade_2D instance.
 *
 * @param pZoneIn The input zone whose boundaries are to be replicated.
 * \if DOC25D
 * @param bKeepHeight If `true`, the boundaries are inserted at their original height (z-coordinates)
 *                    from `pZoneIn`. If `false`, the boundaries are projected onto the current
 *                    triangulation, if any.
 * \endif
 * @return A new zone, constrained by the replicated boundaries.
 */
#if GEOM_PSEUDO3D==GEOM_TRUE
	Zone2* replicateZoneBoundary(Zone2* pZoneIn,double bKeepHeight);
#else
	Zone2* replicateZoneBoundary(Zone2* pZoneIn);
#endif

/** \brief Retrieves all zones of the triangulation.
 *
 * This method retrieves all the zones in the triangulation and stores their
 * pointers in the provided vector.
 *
 * @param [out] vZones A vector used to return pointers to all existing `Zone2` objects.
 *
 */	void getZones(std::vector<Zone2*>& vZones) const;


/** \brief Cookie Cutter (deprecated)
 * @deprecated This method is deprecated. It works as expected, but does not support
 * zones with holes. To avoid behavioral changes, this method is left unchanged and
 * has been deprecated in favor of the new method Fade_2D::createZone_cookieCutter2(),
 * which allows for zones with holes, is faster, and provides improved numerical
 * accuracy.
 */
	Zone2* createZone_cookieCutter( std::vector<Segment2>& vSegments,
									bool bProtectEdges,
									ConstraintGraph2*& pProtectedEdgesCG,
									ConstraintGraph2*& pBoundaryCG);


/** \brief Cookie Cutter2
 *
 * The Cookie Cutter cuts out a part of a triangulation and returns it as a Zone2 object.
 *
 * \image html cookie_cutter.png "Cookie Cutter: Cuts a part of the triangulation, with the red polygon (showing @p vSegments) defining the shape." width=65%
 * \image latex cookie_cutter.png "Cookie Cutter: Cuts a part of the triangulation, with the red polygon (showing @p vSegments) defining the shape." width=0.8\textwidth
 *
 * @param [in] vSegments specifies the input polygon. @if DOC25D The height values of the 
	input polygon (\p vSegments) are ignored, and the z-coordinates are calculated automatically.@endif
 * @param [in] bProtectEdges specifies if existing edges shall be locked by turning
 * them into constraint segments
 * @param [out] pProtectedEdgesCG is used to return the ConstraintGraph2* of edges that have been protected
 * @param [out] pBoundaryCG is used to return the ConstraintGraph2* of the boundary
 * @return a Zone2 object consisting of all triangles inside the polygon or NULL when
 * the operation has failed due to unmet preconditions.
 *
 */
	Zone2* createZone_cookieCutter2( std::vector<Segment2>& vSegments,
	                                bool bProtectEdges,
	                                ConstraintGraph2*& pProtectedEdgesCG,
	                                ConstraintGraph2*& pBoundaryCG);
/** \brief Cookie Cutter2
 *
 * The Cookie Cutter cuts out a part of a 2D or 2.5D triangulation and returns it as a Zone2 object.
 *
 * \image html cookie_cutter.png "Cookie Cutter: Cuts a part of the triangulation, with the red polygon (showing @p vSegments) defining the shape." width=65%
 * \image latex cookie_cutter.png "Cookie Cutter: Cuts a part of the triangulation, with the red polygon (showing @p vSegments) defining the shape." width=0.8\textwidth

 * @param [in] vSegments specifies the input polygon. @if DOC25D The height values 
	of the input polygon (\p vSegments) are ignored, and the z-coordinates are 
	calculated automatically. @endif
 * @param [in] bProtectEdges specifies if existing edges shall be locked by turning
 * them into constraint segments
 * @return a Zone2 object consisting of all triangles inside the polygon or NULL when
 * the operation has failed due to unmet preconditions.
 *
 */
	Zone2* createZone_cookieCutter2( std::vector<Segment2>& vSegments,
	                                 bool bProtectEdges);

/** \brief Drape segments onto a surface
 *
 * Projects the segments from \p vSegmentsIn onto the triangulation.
 * Degenerate input segments are ignored.
 * \if DOC2D
 * Segments are subdivided at intersections with triangulation edges.
 * \endif
 *\if DOC25D
 * The heights (z-values) of the resulting segments are adapted to the surface.
 * @param [in] zTolerance Used to control segment subdivision. If `zTolerance = -1.0`,
 * segments are split at every intersection with triangulation edges. A positive
 * value defines the maximum allowed height error; segments are only subdivided
 * to keep the error below this threshold.
 * \endif
 * @param [in] vSegmentsIn Input segments.
 * @param [out] vSegmentsOut Output segments.
 * @param [in] offTolerance Used to prevent truncation of segments that
 * are slightly outside the convex hull due to tiny rounding errors. If
 * this value is greater than 0, segment endpoints slightly outside the
 * convex hull of the triangulation will be inserted into the triangulation.
 * @param [in] bKeepOffSegments Specifies whether segments outside the
 * triangulation should be kept. Default: `false`.
 *
 * @return `true` if all input segments are inside the convex hull of the
 * triangulation. Otherwise, `false` is returned, and the result is still
 * valid but only contains the segment parts inside the convex hull unless
 * \p bKeepOffSegments is `true`.
 *
 * \if DOC25D
 * \htmlonly <style>div.image img[src="drape.png"]{width:100%;}</style> \endhtmlonly
 * * \image html drape.png "Drape-Method: Input segments (blue) are draped (red) onto a triangulation. Left with @p tolerance 1.0, right without tolerance"
 * \image latex drape.eps "Drape-Method: Input segments (blue) are draped (red) onto a triangulation. Left with @p tolerance 1.0, right without tolerance" width=0.9\textwidth
 * \else
 * \htmlonly <style>div.image img[src="drape2d.png"]{width:50%;}</style> \endhtmlonly
 * * \image html drape2d.png "Drape: Input segments are draped (red) onto a TIN. They are subdivided (blue points) at intersections with triangulation edges"
 * \image latex drape2d.eps "Drape: Input segments are draped (red) onto a TIN. They are subdivided (blue points) at intersections with triangulation edges" width=6cm
 * \endif
 *
 * @note Draping segments onto a triangulation does not insert them. Use
 * Fade_2D::createConstraint() for that purpose.
 */

#if GEOM_PSEUDO3D==GEOM_TRUE
	bool drape(	std::vector<Segment2>& vSegmentsIn,
				std::vector<Segment2>& vSegmentsOut,
				double zTolerance,
				double offTolerance=0.0,
				bool bKeepOffSegments=false) const;
#else
	bool drape(	std::vector<Segment2>& vSegmentsIn,
				std::vector<Segment2>& vSegmentsOut,
				double offTolerance=0.0,
				bool bKeepOffSegments=false) const;
#endif




/** \brief Get directed edges
* Edges are counterclockwise oriented around their triangle. The
* present method returns directed edges. That means each edge(a,b)
* is returend twice, as edge(a,b) and as edge(b,a).
*/
void getDirectedEdges(std::vector<Edge2>& vDirectedEdgesOut) const;

/** \brief Get undirected edges
 *
 * Retrieves all undirected edges from the triangulation.
 *
 * @param [out] vUndirectedEdgesOut A vector used to return a unique set of undirected edges.
 */
void getUndirectedEdges(std::vector<Edge2>& vUndirectedEdgesOut) const;

/// @private
	/* Deprecated: Use setNumCPU() instead. This method is kept for
	* compatibility with existing applications. Internally it calls
	* setNumCPU(0) to automatically determine and use the number of
	* available CPU cores.
	*/
	void enableMultithreading();
	// Development functions, not for public use
/// @private
	void internal(int au,int fu,const char*  s="");
/// @private
	Dt2* getImpl();
/// @private
	void setDev(const char* s,int ival,double dval);

protected:
/// @private
	void initFade(unsigned numExpectedVertices);
/// @private
	Fade_2D& operator=(const Fade_2D&); // No assignment allowed
/// @private
	Dt2* pImpl;
}; // end of class


/** \brief Copy a Fade_2D object and selected Zone2 objects
 *
 * @param [in] dt0 is the source triangulation
 * @param [out] dt1 is the target triangulation
 * @param [in] vZones0 (possibly empty) contains the source zones
 * @param [out] vZones1 returns the target zones in the same order
*/
inline void copyFade(Fade_2D& dt0,std::vector<Zone2*>& vZones0,Fade_2D& dt1,std::vector<Zone2*>& vZones1)
{
	std::stringstream ss;
	dt0.saveTriangulation(ss,vZones0);
	dt1.load(ss,vZones1);
}




} // (namespace)

