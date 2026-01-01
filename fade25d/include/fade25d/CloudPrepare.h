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

/// \file CloudPrepare.h

#pragma once

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE

#include "Point2.h"


namespace GEOM_FADE25D {


/** \brief Enumeration for point aggregation strategies used in CloudPrepare
 *
 * Defines strategies for how multiple points in a given cell are combined during point cloud simplification.
 */
enum SumStrategy
{
	SMS_MINIMUM, ///< Assign the minimum height. Corresponds to ground filtering by prioritizing the lower ground points over those in vegetation.
	SMS_MAXIMUM, ///< Assign the maximum height
	SMS_MEDIAN, ///< Assign the median height. Reduces outliers and stabilizes the cloud by taking the median height of similar points.
	SMS_AVERAGE ///< Assign the average height. Averaging reduces noise in the point cloud.
};

/** \brief Enumeration for strategies to treat convex hull points in CloudPrepare
 *
 * Defines how convex hull points are handled during point cloud simplification.
 */
enum ConvexHullStrategy
{
	CHS_NOHULL, ///< No special treatment for convex hull points
	CHS_MAXHULL, ///< Keep all points from the convex hull
	CHS_MINHULL  ///< Keep only non-collinear points of the convex hull
};
class CloudPrepareImpl; // FWD


/** \brief CloudPrepare simplifies dense point clouds and helps prevent
 * memory usage peaks during data transfer.
 *
 * \image html pointCloudReduction.png "Point Cloud Reduction: Left original, right reduced" width=75%
 * \image latex pointCloudReduction.eps "Point Cloud Reduction: Left original, right reduced" width=0.9\textwidth
 *
 * This class serves two main purposes:
 * 1. **Point cloud simplification**: The reduction of dense point clouds can be performed
 * either by applying a tolerance threshold for vertical error, which adapts the output density
 * to the local level of detail, or using a grid-based approach. For the latter, four aggregation
 * strategies are available to group similar points:
 *   - **SMS_MINIMUM**: Corresponds to ground filtering by prioritizing the lower ground points over those in vegetation.
 *   - **SMS_MAXIMUM**: Keeps the highest points.
 *   - **SMS_MEDIAN**: Reduces outliers and stabilizes the cloud by taking the median height of similar points
 *   - **SMS_AVERAGE**: Averages similar points, reducing noise in the point cloud.
 *
 * 2. **Avoiding memory peaks during triangulation**: This is important when working with
 * large point clouds. The point cloud is stored in your own data structures. If you directly
 * insert it into Fade_2D, the point data is copied and triangles are created immediately,
 * leading to a temporary memory peak. @n
 * To avoid this, you can pass the points to `CloudPrepare` first, remove them from your
 * own data structures, and only then call `Fade_2D::insert(&CloudPrepare)`. This two-step
 * approach prevents the temporary memory peak caused by duplicating the point cloud data.
 *
 * @see [Example with CloudPrepare](https://www.geom.at/terrain-triangulation/#point-cloud-simplification) for practical C++ usage.
 */

class CLASS_DECLSPEC CloudPrepare
{
public:
	CloudPrepare();
	~CloudPrepare();


	/** \brief Add a point to the CloudPrepare object
	*
	* This method adds a single point to the CloudPrepare object, specified by its
	* 3D coordinates (x, y, z). An optional \p customIndex can be provided to link
	* the point to your own data structures.
	*
	* @param[in] x,y,z  The coordinates of the point.
	* @param[in] customIndex  An optional index that can be used to link the point
	*                         with your own data structures. If you later call
	 *                         `Point2::getCustomIndex()` on this point, the same
	 *                         custom index will be returned.
	*/
	void add(double x,double y,double z,int customIndex=-1);

	/** \brief Add a set of points to the CloudPrepare object (vector version)
	*
	* This method allows you to add multiple points to the CloudPrepare object at once,
	* using a vector of Point2 objects.
	*
	* @param[in] vPoints A vector containing the points to be added.
	*/
	void add(const std::vector<Point2>& vPoints);

	/** \brief Add points to the CloudPrepare object (array-version)
	*
	* This method adds multiple points to the CloudPrepare object. The points are provided
	* as an array of coordinates, where each group of three consecutive values represents
	* the x, y, and z coordinates of a point.
	*
	* @param[in] numPoints  The number of points to be added.
	* @param[in] aCoordinates An array containing the coordinates of the points.
	*                          The array holds `3*numPoints` elements, where
	*                          each set of three consecutive values represents the x, y,
	*                          and z coordinates of a point (e.g., x0, y0, z0, x1, y1, z1, ...).
	*/
	void add(size_t numPoints,double * aCoordinates);


	/** \brief Simplify the point cloud according to grid resolution
	*
	* This method simplifies the point cloud by dividing the xy-plane into a grid and
	* combining all points within each grid cell into a single point. The resulting
	* point's position is determined according to the specified aggregation strategy.
	*
	* @param[in] gridLength The grid resolution, which defines the cell spacing.
	* Points within each cell are aggregated into a single point.
	* @param[in] sms The aggregation strategy used to combine the points within each cell.
	* Possible values are:
	*   - **SMS_MINIMUM**: Uses the minimum value in each cell.
	*   - **SMS_MAXIMUM**: Uses the maximum value in each cell.
	*   - **SMS_MEDIAN**: Uses the median value of points in each cell.
	*   - **SMS_AVERAGE**: Uses the average of points in each cell.
	* @param[in] chs The ConvexHullStrategy that determines how to handle convex hull points:
	*   - **CHS_MAXHULL**: Keeps all points on the convex hull unchanged.
	*   - **CHS_MINHULL**: Preserves only the convex corner points of the convex hull,
	*                      excluding any points lying in the middle of straight line segments
	*                      that form the hull.
	*   - **CHS_NOHULL**:  Treats all points, including convex hull points, the same.
	* @param[in] bDryRun If set to `true`, the point cloud simplification is not performed.
	* This option is used to calculate how many points would result from the simplification
	* with the specified parameters without actually modifying the point cloud.
	*
	* @return The number of points in the simplified point cloud after the operation.
	*/
	size_t uniformSimplifyGrid(double gridLength,SumStrategy sms,ConvexHullStrategy chs,bool bDryRun=false);


	/** \brief Simplify the point cloud using a grid resolution determined by the desired number of output points
	*
	* This method simplifies the point cloud by dividing the xy-plane into a grid and
	* combining all points within each grid cell into a single point. The resolution
	* of the grid is automatically adjusted to achieve the desired number of output points.
	* The resulting point's position is determined according to the specified aggregation strategy.
	*
	* @param approxNumPoints [in] The desired number of points in the simplified point cloud.
	* The algorithm will adjust the grid resolution to reduce the point cloud to approximately
	* this number of points.
	*
	* @param sms [in] The aggregation strategy used to combine the points within each cell.
	* Possible values are:
	*   - **SMS_MINIMUM**: Uses the minimum value of the points in each cell.
	*   - **SMS_MAXIMUM**: Uses the maximum value of the points in each cell.
	*   - **SMS_MEDIAN**: Uses the median value of the points in each cell.
	*   - **SMS_AVERAGE**: Uses the average value of the points in each cell.
	*
	* @param chs [in] The ConvexHullStrategy that determines how to handle convex hull points:
	*   - **CHS_MAXHULL**: Keeps all points on the convex hull unchanged.
	*   - **CHS_MINHULL**: Preserves only the convex corner points of the convex hull,
	*     excluding points lying within straight line segments that form the convex hull.
	*   - **CHS_NOHULL**: Treats all points, including convex hull points, equally.
	*
	* @return The number of points in the simplified point cloud after the operation.
	*/
	size_t uniformSimplifyNum(int approxNumPoints,SumStrategy sms,ConvexHullStrategy chs);

	/** \brief Simplifies the point cloud based on a tolerance threshold for the z-values
	*
	* This method simplifies the point cloud adaptively according to the height values.
	* This means that adjoining points with similar z-values (within the given tolerance
	* @p maxDiffZ) are combined into one.
	*
	* @param[in] maxDiffZ The maximum difference in height (z-coordinate) between points.
	* Points with a difference up to this value will be combined into one.
	* @param[in] sms The strategy used to aggregate similar points into one.
	* Possible values are SMS_MINIMUM, SMS_MAXIMUM, SMS_MEDIAN, and SMS_AVERAGE.
	* @param[in] chs The ConvexHullStrategy:
	*   - Use CHS_MAXHULL to preserve all convex hull points.
	*   - Use CHS_MINHULL to preserve only the convex corner points of the convex hull,
 	*     excluding any points lying in the middle of straight line segments that form the hull.
	*   - Use CHS_NOHULL to treat convex hull points like all other points.
	* @param[in] bDryRun If set to true, the method will simulate the simplification
	* process without modifying the point cloud. This can be used to determine the
	* number of points that would result from the simplification with the given
	* parameters. The default value is false.
	*
	* @return The resulting number of points after simplification.
	*/
	size_t adaptiveSimplify(double maxDiffZ,SumStrategy sms,ConvexHullStrategy chs,bool bDryRun=false);

	/** @brief Get the range of the x-coordinates
	*
	* This method returns the range (maximum - minimum) of the x-coordinates in the point cloud.
	*
	* @return The x-range.
	*/
	double getRangeX();

	/** @brief Get the range of the y-coordinates
	*
	* This method returns the range (maximum - minimum) of the y-coordinates in the point cloud.
	*
	* @return The y-range.
	*/
	double getRangeY();

	/** @brief Get the range of the z-coordinates
	*
	* This method returns the range (maximum - minimum) of the z-coordinates in the point cloud.
	*
	* @return The z-range.
	*/
	double getRangeZ();

	/** @brief Get the minimum and maximum bounds of the point cloud
	*
	* This method retrieves the minimum and maximum coordinates of all points
	* in the point cloud.
	*
	* @param[out] minX,minY,minZ,maxX,maxY,maxZ
	*/
	void getBounds(double& minX,double& minY,double& minZ,double& maxX,double& maxY,double& maxZ);



	/** @brief Compute the Convex Hull
	*
	* This method computes the convex hull for a point cloud.
	*
	* @param[in] bAllPoints If set to `true`, all points on the convex hull
	* are returned, including those that do not contribute to the minimal convex
	* shape. If set to `false`, only the essential convex points are returned,
	* excluding those that lie on straight line segments between convex hull
	* points.
	* @param[out] vConvexHull A vector that will be filled with the points
	* forming the convex hull.
	*/
	bool computeConvexHull(bool bAllPoints,std::vector<Point2>& vConvexHull);

	// Points
	/** @brief Get the total number of points in the CloudPrepare object
	*
	* This method returns the number of points currently stored in the CloudPrepare object.
	*
	* @return The total number of points in the CloudPrepare object.
	*/
	size_t getNumPoints() const;

	/** @brief Retrieve the point cloud
	*
	* This method retrieves the current point cloud and stores
	* them in the provided vector.
	*
	* @param[out] vPointsOut The vector where the simplified points will be stored.
	*
	* @note For better memory efficiency, it is recommended to insert the points
	* from the CloudPrepare object using `Fade_2D::insert(CloudPrepare)`. This
	* avoids the need to copy the points first, reducing memory overhead.
	*
	*/
	void getPoints(std::vector<Point2>& vPointsOut) const;
	/// Clear all stored data
	void clear();
protected:

private:
	/// @private
	CloudPrepareImpl* pImpl;
private:
	CloudPrepare(const CloudPrepare&);
	CloudPrepare& operator=(const CloudPrepare&);
};

} // NAMESPACE

// up to here: if GEOM_PSEUDO3D==GEOM_TRUE
#elif GEOM_PSEUDO3D==GEOM_FALSE
	//namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif
