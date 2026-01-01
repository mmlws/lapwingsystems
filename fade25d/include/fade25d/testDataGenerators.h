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

/// @file testDataGenerators.h
#pragma once
#include "Point2.h"
#include "Segment2.h"
#include <vector>




#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


/** \defgroup dataGenerators Test Data Generators
 *
 * @brief Random object generators for automated software stress tests
 *
 * \image html random_objects.png "Random objects (points, lines, polygons, polylines)" width=75%
 *
 * Theory, careful programming, and automated stress tests are essential. None of these
 * can replace the others. Testing with random data helps to uncover unexpected bugs early.
 * Fade provides random object generators to assist with automated stress tests, including:
 *
 * - Random simple polygons
 * - Random segments
 * - Random point clouds
 * - Random numbers
 * - Polylines based on sine functions
 *
 * These tools allow you to reproduce specific error conditions. The random object generators
 * accept a seed value to initialize the random number generator. Using the same seed value
 * will always generate the same sequence of objects. Only the seed value 0 will produce
 * different sequences with each execution.
 *
 *
 *  @{
 */


/** \brief Generate (reproducible) random numbers
 *
 * This function generates a specified number of random numbers within a given range
 * and stores them in the output vector.
 *
 * @param [in] num The number of random numbers to generate.
 * @param [in] min The lower bound for the random numbers.
 * @param [in] max The upper bound for the random numbers.
 * @param [out] vRandomNumbersOut The output vector that will store the generated random numbers.
 * @param [in] seed Initializes the random number generator (RNG).
 *             - `0` (default): Uses a random seed, resulting in non-reproducible sequences.
 *             - Otherwise: Uses the given value for reproducible sequences.
 *
 * @note Reproducible random numbers are useful for testing software with random geometric constructions.
 * Providing a seed value **other than `0`** ensures the output sequence can be reproduced. Conversely, a seed
 * value of `0` maps to random initialization, leading to a unique output sequence for each function call.
 */
CLASS_DECLSPEC
void generateRandomNumbers(	size_t num,
							double min,
							double max,
							std::vector<double>& vRandomNumbersOut,
							unsigned int seed=0
							);

/** \brief Generate (reproducible) random points
 *
 * This function generates a specified number of random points within a given range
 * and stores them in the output vector.
 *
 * @param numRandomPoints The number of random points to generate.
 * @param min The lower bound for both x and y coordinates.
 * @param max The upper bound for both x and y coordinates.
 * @param [out] vRandomPointsOut The output vector to store the generated random points.
 * @param seed Initializes the random number generator (RNG).
 *             - `0` (default): Uses a random seed, resulting in non-reproducible sequences.
 *             - Non-zero: Uses the given value for reproducible sequences.
 *
 * @note Reproducible random points are useful for testing applications involving
 * random geometric constructions. Providing a seed value other than `0` ensures the
 * output sequence can be reproduced. Conversely, a seed value of `0` maps to random
 * initialization, leading to a unique output sequence for each function call.
 *
 * \image html randomPoints.png "Point generator" width=25%
 * \image latex randomPoints.eps "Point generator" width=5cm
 */
CLASS_DECLSPEC
void generateRandomPoints(	size_t numRandomPoints,
							double min,
							double max,
							std::vector<Point2>& vRandomPointsOut,
							unsigned int seed=0
						);

#if GEOM_PSEUDO3D==GEOM_TRUE

/** @brief Generate (reproducible) random 3D points
 *
 * @param numRandomPoints Number of points to be generated.
 * @param min Lower bound (x, y, z) coordinates.
 * @param max Upper bound (x, y, z) coordinates.
 * @param [out] vRandomPointsOut The output vector to store the generated random points.
 * @param seed Initializes the random number generator (RNG).
 *             - Default: `0` (mapped to a random seed for unique sequences).
 *             - Other values: Use for constant initialization and reproducible sequences.
 *
 *
 */
CLASS_DECLSPEC
void generateRandomPoints3D(size_t numRandomPoints,
							double min,
							double max,
							std::vector<Point2>& vRandomPointsOut,
							unsigned int seed=0
						);

#endif


/** @brief Generate a (reproducible) random polygon
 *
 * This function generates a random simple polygon.
 *
 * @param numSegments The number of segments to be generated.
 * @param min Lower bound for (x, y) coordinates.
 * @param max Upper bound for (x, y) coordinates.
 * @param [out] vPolygonOut The output vector that stores the generated polygon's segments.
 * @param seed Initializes the random number generator (RNG).
 *             - Default: `0` (mapped to a random seed for unique sequences).
 *             - Other values: Use for constant initialization and reproducible sequences.
 *
 * \image html randomPolygon.png "Polygon generator: Random simple polygon" width=25%
 * \image latex randomPolygon.eps "Polygon generator: Random simple polygon" width=5cm
 */
CLASS_DECLSPEC
void generateRandomPolygon(	size_t numSegments,
							double min,
							double max,
							std::vector<Segment2>& vPolygonOut,
							unsigned int seed=0
						);


/** @brief Generate (reproducible) random line segments
*
* @param numSegments The number of segments to be generated.
* @param min Lower bound for (x, y) coordinates.
* @param max Upper bound for (x, y) coordinates.
* @param maxLen The maximum length of each segment.
* @param [out] vSegmentsOut The output vector storing the generated segments.
* @param seed Initializes the random number generator (RNG):
*             - Default: `0` (mapped to a random seed for unique sequences).
*             - Other values: Use for constant initialization and reproducible sequences.
*
* \image html randomSegments.png "Segment generator: Random line segments" width=25%
* \image latex randomSegments.eps "Segment generator: Random line segments" width=5cm
*/
CLASS_DECLSPEC
void generateRandomSegments(size_t numSegments,
							double min,
							double max,
							double maxLen,
							std::vector<Segment2>& vSegmentsOut,
							unsigned int seed);

/** @brief Generate segments from a sine function
 *
 * This function generates segments from a sine function.
 *
 * @param numSegments Number of segments to be generated
 * @param numPeriods Number of periods of the sine function
 * @param xOffset Offset of the output x-coordinates
 * @param yOffset Offset of the output y-coordinates
 * @param xFactor Factor to scale the sine function in x direction
 * @param yFactor Factor to scale the sine function in y direction
 * @param bSwapXY Swap the x and y coordinate of the function
 * @param [out] vSineSegmentsOut is the output vector
 *
 * \image html sinePolyline.png "Polyline generator: Polylines from sine functions" width=25%
 * \image latex sinePolyline.eps "Polyline generator: Polylines from sine functions" width=5cm
 */
CLASS_DECLSPEC
void generateSineSegments(
							int numSegments,
							int numPeriods,
							double xOffset,
							double yOffset,
							double xFactor,
							double yFactor, // 0 causes absolute values
							bool bSwapXY,
							std::vector<Segment2>& vSineSegmentsOut
							);


/** \brief Generate points on a circle
 *
 * This function generates points evenly distributed on the circumference
 * of an ellipse or circle, centered at the specified coordinates.
 *
 * @param numPoints The number of points to generate along the circle's circumference.
 * @param x The x-coordinate of the circle's center.
 * @param y The y-coordinate of the circle's center.
 * \if DOC25D
 * @param z The z-coordinate of the circle's center.
 * \endif
 * @param radiusX The radius along the x-axis.
 * @param radiusY The radius along the y-axis.
 * @param vCirclePointsOut [out] A vector to store the generated points.
 *
 * @note If `radiusX` equals `radiusY`, the points will form a circle.
 * For other values, the points form an ellipse.
 */
#if GEOM_PSEUDO3D==GEOM_TRUE
CLASS_DECLSPEC
void generateCircle(
					int numPoints,
					double x,
					double y,
					double z,
					double radiusX,
					double radiusY,
					std::vector<Point2>& vCirclePointsOut
					);
#else
CLASS_DECLSPEC
void generateCircle(
					int numPoints,
					double x,
					double y,
					double radiusX,
					double radiusY,
					std::vector<Point2>& vCirclePointsOut
					);
#endif



#if GEOM_PSEUDO3D==GEOM_TRUE

/** @brief Generate a (reproducible) random surface
 *
 * @param numX,numY Specifies the grid dimensions. Both must be >1, resulting in numX*numY points.
 * @param numCenters The number of extreme points
 * @param xmin,ymin,zmin,xmax,ymax,zmax Defines the geometric bounds for the surface.
 * @param [out] vSurfacePointsOut The output vector storing the generated surface points.
 * @param seed Initializes the random number generator:
 *             - Default: `0` (mapped to a random seed for unique sequences).
 *             - Other values: Use for constant initialization and reproducible sequences.
 * @param bGrid Specifies whether points are placed exactly on a grid or slightly randomized.
 *              - Default: `true` (grid-aligned points).
 *
 */
CLASS_DECLSPEC
void generateRandomSurfacePoints(
							size_t numX,
							size_t numY,
							size_t numCenters,
							double xmin,double ymin,double zmin,double xmax,double ymax,double zmax,
							std::vector<Point2>& vSurfacePointsOut,
							unsigned int seed,
							bool bGrid=true
						);

#endif

/** @brief Apply shear transformation to points
 *
 * Applies a shear transformation to a set of points, modifying their coordinates based on the specified shear factors.
 *
 * @param [in,out] vPointsInOut The input points to be sheared. These points are modified in place.
 * @param [in] shearX, shearY The shear factors in x and y direction.
 *
 * @note The shear transformation modifies the original points in place. The result will be stored in the same vector.
 */
CLASS_DECLSPEC
void shear(	std::vector<Point2>& vPointsInOut,double shearX,double shearY );


/** @}*/
} // Namespace
