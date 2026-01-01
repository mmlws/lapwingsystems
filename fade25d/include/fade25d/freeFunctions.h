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


/// @file freeFunctions.h
#pragma once
#include "Point2.h"
#include "Segment2.h"
#include "Edge2.h"
#include <vector>


#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class Mesh3; // FWD
class Zone2; // FWD



/** \defgroup tools Tools
 *  @{
 */



/**
 * \brief Simplify a polygon.
 *
 * This function simplifies the input polygon based on the specified geometric error.
 * Does not automatically check for self-intersections.
 *
 * @param [in] vIn specifies the input polygon to be simplified.
 * @param [in] errBound bounds the maximum geometric error.
 * @param [out] vOut the simplified output polygon.
 */
CLASS_DECLSPEC
void simplifyPolygon(const std::vector<Point2>& vIn,double errBound,std::vector<Point2>& vOut);



/** @brief Get offset points (deprecated)
 *
 * @deprecated This function is deprecated in favor of Zone2::getOffsetBoundary()
 * which will also resolve self-intersections. Do not use this function anymore!
 *
 * @param offset specifies the offset distance (positive or negative)
 * @param [in] vOrgPoints are ordered points of a <em>counterclockwise</em>
 * polygon (no point repeated)
 * @param [out] vOffsetPoints returns points moved outside/inside by a
 * positive/negative offset distance (times the angle correction factor)
 * @param [in] maxCorrectionFactor is the maximum angle correction
 * factor that may be applied to the \p offset value. It's default
 * value is 1.0 (no angle correction), lower values are ignored.
 * The angle that can be corrected with a certain maxCorrectionFactor
 * is computed according to \f$ 2*asin(1.0/maxCorrectionFactor) \f$. This means,
 * a maxCorrectionFactor=1.4143 can account for angles down to 90 degrees
 * while 2.6132 is required to correct angles as low as 45 degrees.
 *
 * @note You may want to simplify and smooth the input polygon using
 * `simplifyPolygon()` before using this function.
 *
 * @warning The resuling offset polygon may self-intersect!
 */
CLASS_DECLSPEC
void offsetPolygonPoints(
	double offset,
	const std::vector<Point2>& vOrgPoints,
	std::vector<Point2>& vOffsetPoints,
	double maxCorrectionFactor=1.0
	);

/** \brief Repair a selfintersecting polygon (deprecated)
 *
 * @deprecated This function is deprecated but retained for backward compatibility.
 * It is recommended to use the PolygonClipper class instead, which offers greater
 * versatility and robustness.
 *
 * @param vPolygon [in] The input polygon, no specific order and orientation
 * @param vvPolygon [out] One or more output polygons, free of selfintersections, counterclockwise oriented around the enclosed area
 */
CLASS_DECLSPEC
void untwistPolygon(std::vector<Segment2>& vPolygon,std::vector<std::vector<Segment2> >& vvPolygon);



/** \brief Set the global number of threads to be used
 *
 * Sets the number of CPU cores that Fade should use for parallel operations.
 *
 * @param numCPU The number of CPU cores to be used. A value of `0` means autodetection.
 * @return The number of CPU cores that will be used.
 *
 * @note Fade is by default <em>single-threaded</em> to avoid nested
 * multithreading when Fade runs in multiple threads of your application.
 *
 * Characteristics:
 * - When triangulating both a point cloud and constraint segments, you should
 *   insert the points first. This allows the algorithm to fully leverage multithreading.
 * - Fade continues to support older platforms, but multithreading requires C++11
 *   and is not available for very old compilers.
 */
CLASS_DECLSPEC
int setGlobalNumCPU(int numCPU);

/// \brief Enable or disable high-precision intersection calculations.
///
/// This function enables or disables the use of high-precision arithmetic
/// when computing intersections between constraint segments. High-precision
/// mode is more robust in degenerate cases but comes at a performance cost.
/// In virtually all scenarios, the default precision is safe and sufficient.
/// Therefore, high-precision intersection is disabled by default.
///
/// Enable high-precision mode only if you are dealing with problematic
/// input data, such as when constraint segments are extremely close to
/// each other. This situation can occur, for example, with nearly vertical
/// triangles.
///
/// \param bUseHighPrecisionIntersections
///        Pass `true` to enable high-precision intersection computations,
///        or `false` to use the faster default method.
///
/// \note This is a global setting that affects all subsequent intersection tests.
/// \warning This function is experimental and may change or be removed in future versions.
CLASS_DECLSPEC
void setHighPrecisionIntersections(bool bUseHighPrecisionIntersections);

/** \brief Set a global time limit for constraint insertion.
 *
 * @param ms Time limit in milliseconds. Use `DBL_MAX` to disable the limit.
 *
 * This function is only needed in special cases: When multiple constraint
 * segments intersect and lie very close to each other, there may not be
 * enough space to insert intersection points without creating new overlaps.
 * Such situations are extremely rare and have only been observed when
 * almost vertical walls were created. Avoid such constructions whenever
 * possible, as they may lead to infinite geometric loops.
 *
 * If such constructions cannot be avoided, use this function to set a time limit.
 * If the specified time is exceeded during constraint insertion, an exception is thrown.
 * Consider also using setHighPrecisionIntersections(true) for more accurate handling
 * of constraint segment intersections.
 *
 * \warning This function is experimental and may change or be removed in future versions.
 */

CLASS_DECLSPEC
void setGlobalTimeLimit_constraints(double ms);

/** \brief Get the number of cpu cores to be used
 *
 * @return The global number of cpu cores that can be used by Fade2D.
 *
 * @note Fade is by default <em>single-threaded</em> to avoid nested
 * multithreading when Fade runs within multiple threads of your application.
 * However, you can modify the number of threads using setGlobalNumCPU().
 */
CLASS_DECLSPEC
int getGlobalNumCPU();

/** \brief Get sorted boundary polygons
 *
 * This function processes a **single** edge-connected component of triangles
 * and returns the outer polygon, as well as any inner hole polygons, if present.
 *
 * @param [in] vConnectedComponent A connected component of triangles, where
 * the term "connected" means adjacency on edges. It is important that only
 * one connected component is passed!
 * @param [out] vOutsidePolygon The outside boundary of the connected component.
 * @param [out] vHolePolygons A vector that will contain any hole polygons,
 * if they exist.
 *
 * @note The edges in the output polygons are sorted and oriented counterclockwise (CCW)
 * **with respect to their triangle**. Thus, the outer polygon is always CCW-oriented,
 * and hole polygons are always clockwise (CW)-oriented. This property is sometimes useful.
 *
 * @attention Ensure that only one connected component is passed to this function.
 */

CLASS_DECLSPEC
void getSortedBoundaryPolygons(	std::vector<Triangle2*>& vConnectedComponent,
								std::vector<Edge2>& vOutsidePolygon,
								std::vector<std::vector<Edge2> >& vHolePolygons
								);




/** \brief Create segments from a set of points
 *
 * Turns a vector of points `(p0, p1, p2, ..., pm, pn)` into a vector of
 * segments `((p0, p1), (p1, p2), ..., (pm, pn))`. In case that bClose is
 * `true` and `pn!=p0`, an additional segment `(pn, p0)` is constructed.
 * Degenerate segments are ignored. Self-intersections of the polyline are
 * not checked.
 *
 * @param [in] vInPoints The input vector of points.
 * @param [in] bClose Specifies whether a closing segment shall be constructed.
 * @param [out] vOutSegments The output vector where the segments are stored.
 */
CLASS_DECLSPEC
void pointsToPolyline(std::vector<Point2>& vInPoints,bool bClose,std::vector<Segment2>& vOutSegments);




/** \brief Check if a polygon is simple
 *
 * This function checks whether the given set of segments forms a simple polygon.
 * A polygon is considered simple if it is closed (the first and last vertices are
 * connected) and does not have any self-intersections. Degenerate segments (i.e.,
 * segments with zero length) are ignored during the check.
 *
 * @param [in] vSegments A vector of Segment2 objects forming the input.
 * @return `true` if \p vSegments forms a closed polygon without self-intersections;
 *         `false` otherwise.
 */
CLASS_DECLSPEC
bool isSimplePolygon(std::vector<Segment2>& vSegments);

#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Get the normal vector of a triangle
 *
 * Computes the normalized normal vector of the triangle defined by the
 * three input points \p p0, \p p1, and \p p2.
 *
 * @param [in] p0, p1, p2 The corners of the triangle. If these points are
 * counterclockwise (CCW) oriented, the resulting normal vector points
 * towards the viewer.
 * @param [out] bOK A boolean value that returns `true` for valid results
 * or `false` if the input points are collinear.
 *
 * @return The normalized normal vector of the triangle.
 */
CLASS_DECLSPEC
Vector2 getNormalVector(	const Point2& p0,
							const Point2& p1,
							const Point2& p2,
							bool& bOK);
#endif




#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Get 2.5D area of a triangle
 *
 * Computes the area of the triangle defined by the three input points \p p0, \p p1, and \p p2.
 *
 * @param [in] p0, p1, p2 The corners of the triangle.
 * @return The 2.5D area of the triangle.
 */
CLASS_DECLSPEC
double getArea25D(const Point2* p0,const Point2* p1,const Point2* p2);
#endif


/** \brief Get 2D area of a triangle
 *
* Computes the 2D area of the triangle defined by the three input points \p p0, \p p1, \p p2.
*
* \if DOC25D
* @note This function ignores the z-coordinate.
* \see getArea25D()
* \endif
*
* @param [in] p0,p1,p2 The corners of the triangle.
* @return The 2D area of the triangle.
*/
CLASS_DECLSPEC
double getArea2D(const Point2* p0,const Point2* p1,const Point2* p2);


/** \brief Get directed edges
 *
* The directed edges of \p vT are returned in \p vDirectedEdgesOut.
* Directed means that each edge (a,b) with two adjacent triangles
* in vT is returned twice, as edge(a,b) and edge(b,a).
*/
CLASS_DECLSPEC
void getDirectedEdges(const std::vector<Triangle2*>& vT,std::vector<Edge2>& vDirectedEdgesOut);

/** \brief Get undirected edges
 *
 * This function computes the edges of the input triangles. The term "undirected"
 * means that edges which appear twice with different orientations (i.e., reversed
 * directions) are only returned once, with an arbitrary orientation.
 *
 * @param [in] vT A vector containing the input triangles.
 * @param [out] vUndirectedEdgesOut A vector that will contain the output edges.
 */
CLASS_DECLSPEC
void getUndirectedEdges(const std::vector<Triangle2*>& vT,std::vector<Edge2>& vUndirectedEdgesOut);

/** \brief Get connected components
 *
 * This function partitions the triangles in \p vT into connected components,
 * which are sets of triangles that are connected by shared edges.
 *
 * @param [in] vT A vector containing the input triangles
 * @param [out] vvT A vector of vectors that will contain the connected components of triangles
 */

CLASS_DECLSPEC
void getConnectedComponents(	const std::vector<Triangle2*>& vT,
								std::vector<std::vector<Triangle2*> >& vvT
								);


/** \brief Fill a hole in a 3D mesh with triangles (deprecated)
 *
 * @deprecated This function was experimental and is now deprecated because 3D
 * point cloud meshing has been moved to the WOF library.
 *
 * @param [in] vPolygonSegments contains the segments of a closed,
 * simple input polygon along with normal vectors. The segments are
 * counterclockwise oriented and ordered with respect to the surface
 * to be created. Check twice, the orientation is very important.
 * The normal vectors point in the direction of the thought surface
 * at the segment i.e., if a hole is filled, the normal vector of
 * an adjecent triangle is taken but if a T-joint is filled the
 * normal vector should be the average normal of the two triangles
 * at the edge.
 * @param [in] bWithRefine specifies if additional vertices shall
 * be created. (bWithRefine=true is experimental, don't use currently)
 * @param [in] bVerbose specifies if warnings shall be printed to stdout
 * @param [out] vCornersOut contains the created fill triangles, 3
 * corners per triangle, counterclockwise oriented.
 *
 * @note This function is deprecated because all related functionality
 * has been moved to the WOF project.
 */
CLASS_DECLSPEC
bool fillHole(	const std::vector<std::pair<Segment2,Vector2> >& vPolygonSegments,
				bool bWithRefine,
				bool bVerbose,
				std::vector<Point2>& vCornersOut
				);




#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Fill a hole in a 3D mesh with triangles (deprecated)
 *
 * @deprecated This function was experimental and is now deprecated because 3D
 * point cloud meshing has been moved to the WOF library.

 * @param [in] vMeshCorners specifies the input mesh, 3 points per
 * triangle in counterclockwise order.
 * @param [in] vPolygonSegments are the edges of the *closed* polygon
 * to be triangulated.
 * @param [in] bWithRefine specifies if additional vertices shall
 * be created (bWithRefine=true is experimental, don't use currently)
 * @param [in] bVerbose specifies if warnings shall be printed to stdout
 * @param [out] vCornersOut contains the created fill triangles, 3
 * corners per triangle, counterclockwise oriented.
 *
 * @note This function is deprecated because all related functionality
 * has been moved to the WOF project.
 */
CLASS_DECLSPEC
bool fillHole(	const std::vector<Point2>& vMeshCorners,
				const std::vector<Segment2>& vPolygonSegments,
				bool bWithRefine,
				bool bVerbose,
				std::vector<Point2>& vCornersOut
				);

/** \brief Fill a hole in a 3D mesh with triangles (deprecated)
 *
 * @deprecated This function was experimental and is now deprecated because 3D
 * point cloud meshing has been moved to the WOF library.
 *
 * @param [in] pMesh
 * @param [in] vPolygonEdges are edges of the polygon to be
 * triangulated. They must form a closed polygon in the mesh.
 * @param [in] bWithRefine specifies if additional vertices shall
 * be created (Note: bWithRefine=true is experimental, don't use currently)
 * @param [in] bVerbose specifies if warnings shall be printed to stdout
 * @param [out] vCornersOut contains the created fill triangles, 3
 * corners per triangle, counterclockwise oriented.
 *
 * @note This function is deprecated because all related functionality
 * has been moved to the WOF project.
 */
CLASS_DECLSPEC
bool fillHole(	Mesh3* pMesh,
				const std::vector<Edge2>& vPolygonEdges,
				bool bWithRefine,
				bool bVerbose,
				std::vector<Point2>& vCornersOut
				);
#endif

/** \brief Organize a set of edges into polygons
 *
 * This method processes a set of unorganized edges, grouping them into one or more polygons
 * when possible.
 *
 * @param [in] vEdgesIn A vector of Edge2 objects forming the input set.
 * @param [out] vvPolygonsOut A vector of output polygons, where each polygon is represented as
 *                            a vector<Edge2>.
 * @param [out] vRemainingOut A vector containing edges that could not form a closed polygon.
 *
 * The method adds one vector<Edge2> to \p vvPolygonsOut for each polygon found in \p vEdgesIn.
 * Any edges that do not form a closed polygon are stored in \p vRemainingOut.
 *
 * \image html edges-to-polygons.png "Polygons created by \p edgesToPolygons()" width=35%
 * \image latex edges-to-polygons.eps "Polygons created by \p edgesToPolygons()" width=8cm
 *
 * @note An Edge2 object represents an edge of a Triangle2. The corners of a Triangle2
 * are always counterclockwise (CCW) oriented. Consequently, polygons that enclose a
 * set of triangles are CCW-oriented while hole polygons are clockwise (CW) oriented.
 * See the figure for an illustration.

 */

CLASS_DECLSPEC
void edgesToPolygons(
	const std::vector<Edge2>& vEdgesIn,
	std::vector<std::vector<Edge2> >& vvPolygonsOut,
	std::vector<Edge2>& vRemainingOut
	);

/** \brief Get the border segments of a set of triangles
 *
 * This function computes the borders of the triangles in \p vT, which consist
 * of all edges that have exactly one adjacent triangle in \p vT.
 *
 * @param [in] vT A vector containing the input triangles
 * @param [out] vBorderSegmentsOut A vector that will contain the border segments as Segment2 objects
 */
CLASS_DECLSPEC
void getBorders(const std::vector<Triangle2*>& vT,std::vector<Segment2>& vBorderSegmentsOut);

/** \brief Sort a vector of segments
 *
 * This function reorients and sorts the segments in \p vRing such that
 * each segment joins at the endpoint of the previous one.
 *
 * @param [in, out] vRing A vector of segments to be sorted and reoriented.
 * @return Returns `true` if the sorting was successful; otherwise, `false`.
 */
CLASS_DECLSPEC
bool sortRing(std::vector<Segment2>& vRing);

/** \brief Order and orient a vector of segments (CCW)
 *
 * This function reorients and sorts the segments in \p vRing such that the
 * resulting polygon is counterclockwise oriented. It ensures that subsequent
 * segments join at their endpoints to form a continuous polygon.
 *
 * @param [in, out] vRing A vector of segments that will be sorted and reoriented
 * to form a CCW polygon.
 *
 * @return Returns `true` if the sorting was successful; otherwise, `false`.
 */
CLASS_DECLSPEC
bool sortRingCCW(std::vector<Segment2>& vRing);



/** \brief Get the orientation of three points
 *
 * This function returns the exact orientation of the points \p p0, \p p1, \p p2.
 *
 * @param [in] p0, p1, p2 The three input points.
 *
 * @return The orientation of the points:
 * - `ORIENTATION2_COLLINEAR` if \p p0, \p p1, \p p2 are collinear.
 * - `ORIENTATION2_CCW` if \p p0, \p p1, \p p2 are counterclockwise (CCW) oriented.
 * - `ORIENTATION2_CW` if \p p0, \p p1, \p p2 are clockwise (CW) oriented.
 *
 * @note This function is not thread-safe but is faster than the thread-safe version.
 *
 * \see getOrientation2_mt() The thread-safe version of this function.
 */
CLASS_DECLSPEC
Orientation2 getOrientation2(const Point2* p0,const Point2* p1,const Point2* p2);

/** \brief Get the orientation of three points (MT)
 *
 * This function returns the exact orientation of the points \p p0, \p p1, \p p2.
 *
 * @param [in] p0, p1, p2 The three input points.
 *
 * @return The orientation of the points:
 * - `ORIENTATION2_COLLINEAR` if \p p0, \p p1, \p p2 are collinear.
 * - `ORIENTATION2_CCW` if \p p0, \p p1, \p p2 are counterclockwise (CCW) oriented.
 * - `ORIENTATION2_CW` if \p p0, \p p1, \p p2 are clockwise (CW) oriented.
 *
 * @note This function is thread-safe.
 */

CLASS_DECLSPEC
Orientation2 getOrientation2_mt(const Point2* p0,const Point2* p1,const Point2* p2);

/**
 * @brief Check the position of a point relative to the circumcircle of a triangle.
 *
 * This function determines the position of the query point `q` in relation to the
 * circumcircle of the given triangle `pT`.
 *
 * @param pT The triangle whose circumcircle is used for the test.
 * @param [in] q The point whose position relative to the circumcircle is evaluated.
 *
 * @return The position of `q` relative to the circumcircle:
 * - `CINC2_ZERO`: `q` is exactly on the boundary of the circumcircle of `pT`.
 * - `CINC2_INCIRCLE`: `q` is inside the circumcircle of `pT`.
 * - `CINC2_OUTCIRCLE`: `q` is outside the circumcircle of `pT`.
 */
CLASS_DECLSPEC
CINC2 getPointCirclePosition(Triangle2* pT, const Point2& q);



/// @private
CLASS_DECLSPEC
const char* getString(const Orientation2 ori);

/** @}*/


/** \defgroup codeInfo Version Information
 *  @{
 */



/** \brief Get the ID string
 *
 * This function returns an ID string that provides versioning and build information.
 * The string format is:
 * @if DOC2D Fade2D @endif @if DOC25D Fade25D @endif `BUILDTYPE` `VER`
 * where:
 *   - `BUILDTYPE` is either `Release` or `Debug`
 *   - `VER` is the version string in the format `MajorVersion.MinorVersion.Revision`.
 *
 * @return The Fade ID string as a `const char*`.
 */
CLASS_DECLSPEC
const char* getFade2DVersion();
/** \brief Get the major version number
 *
 * This function returns the major version number of Fade.
 *
 * @return The major version number as an integer.
 */
CLASS_DECLSPEC
int getMajorVersionNumber();
/** \brief Get the minor version number
 *
 * This function returns the minor version number of Fade.
 *
 * @return The minor version number as an integer.
 */
CLASS_DECLSPEC
int getMinorVersionNumber();
/** \brief Get the revision version number
 *
 * This function returns the revision number of Fade.
 *
 * @return The revision number as an integer.
 */
CLASS_DECLSPEC
int getRevisionNumber();

/** \brief Check if a release of debug version is used.
 *
 * This function checks whether the current Fade binary is built as a release
 * or debug version.
 *
 * @return `true` if the used Fade binary is a release build, `false` if it is a debug build.
 */
CLASS_DECLSPEC
bool isRelease();

/// @private
CLASS_DECLSPEC
void setLic(
	const char* l1,
	const char* l2,
	const char* dt,
	const char* s1,
	const char* s2_
	);
/// @private
class Lic;
/// @private
Lic* getLic();

/** @}*/








/** \defgroup fileIO File I/O
 *  @{
 */


//////////////////////////
// READ AND WRITE, ASCII
//////////////////////////

/** \brief Write points to an ASCII file
 *
 * This function writes points to an ASCII file. The format is:
 * - \if DOC25D
 *   Three coordinates (x, y, z) per line.
 *   \else
 *   Two coordinates (x, y) per line.
 *   \endif
 * The coordinates are separated by whitespace.
 *
 * @param [in] filename The name of the output file where points will be written.
 * @param [in] vPointsIn A vector containing pointers to the points to be written to the file.
 *
 * @return `true` if the points were successfully written, `false` otherwise.
 *
 * @note Data exchange through ASCII files is easy and convenient, but floating point coordinates
 *       are not necessarily exact when represented as decimal numbers. If tiny rounding errors
 *       are not acceptable in your application, you are advised to write binary files using
 *       writePointsBIN() or writePointsPLY().
 *
 *
 */

CLASS_DECLSPEC
bool writePointsASCII(const char* filename,const std::vector<Point2*>& vPointsIn);

/** \brief Write points to an ASCII file
 *
 * This function writes points to an ASCII file. The file format consists of two coordinates (x, y) per line,
 * whitespace separated.
 *
 * @param [in] filename The name of the output file where the points will be written.
 * @param [in] vPointsIn A vector containing the points to be written to the file.
 *
 * @return `true` if the points were successfully written, `false` otherwise.
 *
 * \see readPointsASCII()
 */
CLASS_DECLSPEC
bool writePointsASCII(const char* filename,const std::vector<Point2>& vPointsIn);





/** \brief Read (x y) points from an ASCII file
 *
 * This function reads points from an ASCII file. Each line in the file
 * contains two coordinates (x, y), separated by whitespace.
 *
 * \cond DOC25D
 * The z coordinate of the output points is set to 0.
 * \endcond
 *
 * @param [in] filename The name of the ASCII file containing the points.
 * @param [out] vPointsOut A vector where the read points will be stored.
 *
 * @return `true` if the points were successfully read, `false` otherwise.
 */
CLASS_DECLSPEC
bool readXY(const char* filename,std::vector<Point2>& vPointsOut);

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Read (x y z) points from an ASCII file
 *
 * This function reads points from an ASCII file. The expected file format is three
 * coordinates (x, y, z) per line, with whitespace separating the values.
 *
 * @param [in] filename The input file containing the points.
 * @param [out] vPointsOut A vector that will store the read points.
 *
 * @return `true` if the points were successfully read, `false` otherwise.
 */
CLASS_DECLSPEC
bool readXYZ(const char* filename,std::vector<Point2>& vPointsOut);
#endif



//////////////////////////
// READ AND WRITE, PLY
//////////////////////////
#ifndef SKIPTHREADS
/** \brief Read points from a *.PLY file
 *
 * This function reads points from a *.PLY file and stores them in the provided vector.
 * Optionally, the function can also read associated vertex colors if specified.
 *
 * @param[out] vPoints A vector where the read points will be stored.
 * @param[in] filename The name of the input *.PLY file to read from.
 * @param[in] bUniquePoints Specifies whether to read a unique set of points or point triples (3 points per triangle):
 *   - `true` to read a unique set of points.
 *   - `false` to read point triples (3 points per triangle).
 * @param [in] pVertexColors A pointer to a vector where vertex colors will be stored. Default is `NULL`.
 *
 * @note If `pVertexColors` is not `NULL`, the function will store colors from the *.PLY file in `*pVertexColors`.
 *       Additionally, indices to these colors will be stored in the points, which can be retrieved using the
 *       `Point2::getCustomIndex()` function.
 *
 * @attention This function requires at least C++11 and is not available on very old platforms.
 */
CLASS_DECLSPEC
bool readPointsPLY(const char* filename,bool bUniquePoints,std::vector<Point2>& vPoints,std::vector<VtxColor>* pVertexColors=NULL);


/** \brief Read points from a *.PLY file
 *
 * This function reads points from a *.PLY file and stores them in the provided vector.
 * Optionally, the function can also read associated vertex colors if specified.
 *
 * @param [out] vPoints A vector where the read points will be stored.
 * @param [in] is The input stream to read from.
 * @param [in] bUniquePoints Specifies whether to read a unique set of points or point triples (3 points per triangle):
 *   - `true` to read a unique set of points.
 *   - `false` to read point triples (3 points per triangle).
 * @param [in] pVertexColors A pointer to a vector where vertex colors will be stored. Default is `NULL`.
 *
 * @note If `pVertexColors` is not `NULL`, the function will store colors from the *.PLY file in `*pVertexColors`.
 *       Additionally, indices to these colors will be stored in the points, which can be retrieved using the
 *       `Point2::getCustomIndex()` function.
 *
 * @attention This function requires at least C++11 and is not available on very old platforms.
 */
CLASS_DECLSPEC
bool readPointsPLY(std::istream& is,bool bUniquePoints,std::vector<Point2>& vPoints,std::vector<VtxColor>* pVertexColors=NULL);

/** \brief Write points to a *.PLY file
 *
 * This function writes points to a *.PLY file in either ASCII or binary format.
 *
 * @param [in] vPoints A vector of points to be written to the *.PLY file.
 * @param [in] bASCII Specifies whether to write the file in ASCII format (`true`) or binary format (`false`).
 * @param [in] os The output stream where the *.PLY file will be written.
 *
 * @attention This function requires at least C++11. It is not available on very old platforms.
 */
CLASS_DECLSPEC
bool writePointsPLY(std::ostream& os,std::vector<Point2*>& vPoints,bool bASCII);


/** \brief Write points to a *.PLY file
 *
 * This function writes points to a *.PLY file in either ASCII or binary format.
 *
 * @param [in] vPoints A vector of pointers to points to be written to the *.PLY file.
 * @param [in] bASCII Specifies whether to write the file in ASCII format (`true`) or binary format (`false`).
 * @param [in] filename The output file where the *.PLY file will be written.
 *
 * @attention This function requires at least C++11. It is not available on very old platforms.
 */
CLASS_DECLSPEC
bool writePointsPLY(const char* filename,std::vector<Point2*>& vPoints,bool bASCII);

#endif



//////////////////////////
// READ AND WRITE, BINARY
//////////////////////////


/** \brief Write points to a binary file
 *
 * This function writes points to a binary file. The file format consists of:
 *
 * - An integer `filetype`:
 * \if DOC25D
 *   - 30 for 2.5D data
 * \endif
 * \if DOC2D
 *   - 20 for 2D data
 * \endif
 * - A `size_t` indicating the number of points.
 * - For each point, three `double` values representing the coordinates:
 *   - `x`, `y`, and `z`.
 *
 * @param [in] filename The name of the output binary file.
 * @param [in] vPoints A vector containing the points to be written to the file.
 *
 * @return `true` if the points were successfully written, `false` otherwise.
 *
 * @attention The `size_t` type is 8 bytes on 64-bit platforms but only 4 bytes on 32-bit platforms.
 * This format always uses 8 bytes for compatibility across architectures.
 *
*/
CLASS_DECLSPEC
bool writePointsBIN(const char* filename,std::vector<Point2>& vPoints);

/** \brief Write points to a binary file
 *
 * This function writes points to a binary file. The file format consists of:
 *
 * - An integer `filetype`:
 * \if DOC25D
 *   - 30 for 2.5D data
 * \endif
 * \if DOC2D
 *   - 20 for 2D data
 * \endif
 * - A `size_t` indicating the number of points.
 * - For each point, three `double` values representing the coordinates:
 *   - `x`, `y`, and `z`.
 *
 * @param [in] filename The name of the output binary file.
 * @param [in] vPoints A vector of pointers to points to be written to the file.
 *
 * @return `true` if the points were successfully written, `false` otherwise.
 *
 * @attention The `size_t` type is 8 bytes on 64-bit platforms but only 4 bytes on 32-bit platforms.
 * This format always uses 8 bytes for compatibility across architectures.


*/
CLASS_DECLSPEC
bool writePointsBIN(const char* filename,std::vector<Point2*>& vPoints);


/** \brief Read points from a binary file
 *
 * This function reads points from a binary file into the provided vector.
 * The format of the file can either include a header (written by `writePointsBIN()`)
 * or it can contain just a sequence of double precision coordinates without a header.
 *
 * @param [in] filename The name of the input binary file to read from.
 * @param [out] vPoints A vector where the read points will be stored.
 * @param [in] bWithHeader Specifies if the file contains a header:
 *   - `true` if the file includes a header (written by `writePointsBIN()`).
 *   - `false` if the file contains just a sequence of double precision coordinates, with no header.
 *
 * \see writePointsBIN()
 */
CLASS_DECLSPEC
bool readPointsBIN(const char* filename, std::vector<Point2>& vPoints,bool bWithHeader=true);

/** \brief Write segments to a binary file
 *
 * Writes segments to a binary file with the following format:
 *
 * Binary file format:
 * \if DOC25D
 * - int filetype (31)
 * \else
 * - int filetype (21)
 * \endif
 * - size_t numSegments (\p vSegmentsIn.size())
 * - double x0_source
 * - double y0_source
 * \if DOC25D
 * - double z0_source
 * \endif
 * - double x0_target
 * - double y0_target
 * \if DOC25D
 * - double z0_target
 * \endif
 * ...
 * - double xn_source
 * - double yn_source
 * \if DOC25D
 * - double zn_source
 * \endif
 * - double xn_target
 * - double yn_target
 * \if DOC25D
 * - double zn_target
 * \endif
 *
 * \see readSegmentsBIN()
 */
CLASS_DECLSPEC
bool writeSegmentsBIN(const char* filename,std::vector<Segment2>& vSegmentsIn);

/** \brief Read segments from a binary file
 *
 * This function reads segments from a binary file and
 * stores them in the provided vector.
 *
 * @param [in] filename The name of the binary file to read.
 * @param [out] vSegmentsOut A vector where the read segments will be stored.
 *
 * @return `true` if the segments were successfully read, `false` otherwise.
 *
 * \see writeSegmentsBIN()
 */
CLASS_DECLSPEC
bool readSegmentsBIN(const char* filename,std::vector<Segment2>& vSegmentsOut);






/** @}*/
} // NAMESPACE
