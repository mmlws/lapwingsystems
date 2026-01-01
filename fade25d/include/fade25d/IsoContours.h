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

/// @file IsoContours.h
#pragma once
#if GEOM_PSEUDO3D==GEOM_TRUE

#include "Segment2.h"
#include <map>

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


struct TriNode; // FWD
class Triangle2; // FWD
class GeomTest; // FWD
class Segment2; // FWD
class Func_Rot3D; // FWD

/// @private
typedef std::multimap<Point2,Segment2*> MMPS;
/// @private
typedef MMPS::iterator MMPSIt;

/**
 * \brief The IsoContours class computes intersections of planes
 * with a set of triangles, generating iso-contours (profiles or slices) in
 * 3D space.
 *
 * The class computes iso-contours by slicing with horizontal planes at
 * specific heights, and it enables generating profiles by slicing along
 * **arbitrary** directions. It uses fast spatial data structures to
 * efficiently generate the corresponding contour lines.
 *
 * @image html terrain_triangulation_25d_iso_contours.png "A terrain with ISO contours at multiple heights" width=55%
 * @image latex terrain_triangulation_25d_iso_contours.png "A terrain with ISO contours at multiple heights" width=0.8\textwidth
 *
 * @note This class is not limited to 2.5D but accepts **real 3D** models as input.
 *
 * \sa [This article contains example code.](https://www.geom.at/breakline-insertion/#iso-contours)
 */
class IsoContours
{

public:
	/**
	* \brief Constructor
	*
	* This constructor initializes the IsoContours object with a set of triangles
	* and enables the generation of iso-contours by cutting the Triangle2 objects
	* with **horizontal** planes along the z-axis.
	*
	* @param vTriangles A reference to a vector containing Triangle2 pointers.
	*/
	CLASS_DECLSPEC
	explicit IsoContours(std::vector<Triangle2*>& vTriangles);
	/**
	* \brief Constructor to initialize IsoContours with a set of triangle corners
	* and slice direction.
	*
	* This constructor initializes the IsoContours object using triangle corners
	* and allows generating iso-contours (slices or profiles) based on the provided
	* direction vector. The direction vector defines the cutting direction, enabling
	* slices to be created in **arbitrary directions**.
	*
	* @param vCorners A vector containing 3*n points, specifying n triangles. Each
	* triangle is defined by 3 consecutive points.
	* @param dirVec A vector specifying the slice direction for computing the iso-contours (profiles).
	*/
	CLASS_DECLSPEC
	IsoContours(std::vector<Point2>& vCorners,const Vector2& dirVec);
	/// Destructor
	CLASS_DECLSPEC
	~IsoContours();

	/**
	 * \brief Get the minimum height (smallest z-coordinate).
	 *
	 * @return The smallest z-coordinate of the data
	 */
	CLASS_DECLSPEC
	double getMinHeight();
	/**
	* \brief Get the maximum height (largest z-coordinate).
	*
	* @return The largest z-coordinate of the data
	*/
	CLASS_DECLSPEC
	double getMaxHeight();


	/**
	 * \brief Get the contours at a specified height.
	 *
	 * This method computes the intersection of a horizontal plane at a specific height
	 * with all triangles and returns the resulting contours. The method requires height
	 * values that are not equal to the z-coordinate of any triangle vertices. If the
	 * provided height value coincides with a vertex, the method returns false except
	 * \p `autoPerturbate=true`. In this case a tiny offset is automatically added to
	 * \p height.
	 *
	 * @param height The height (z-coordinate) at which to compute the contours.
	 * @param vvContours A reference to a vector of vectors, where each vector contains
	 * the segments of a contour (polygon or polyline).
	 * @param bVerbose A flag to enable verbose output.
	 * @param bAutoPerturbate A flag to automatically perturb the height slightly
	 * if it coincides with a vertex (default is true).
	 *
	 * @return True if the contours were successfully computed, otherwise false.
	 */
	CLASS_DECLSPEC
	bool getContours(double height,std::vector<std::vector<Segment2> >& vvContours,bool bVerbose,bool bAutoPerturbate=true);

	/**
	 * \brief Get the profile at a specified point.
	 *
	 * This method computes the slice orthogonal to a direction defined in the constructor.
	 *
	 * @param p A point specifying the location of the slice.
	 * @param vSegmentsOut A reference to a vector that will be filled with the segments of the profile.
	 */
	CLASS_DECLSPEC
	void getProfile(const Point2& p,std::vector<Segment2>& vSegmentsOut);

protected:
	IsoContours(const IsoContours& other);
	IsoContours& operator=(const IsoContours& other);

/// @private
	void init();
/// @private
	CLASS_DECLSPEC
	bool getIntersectionSegments(double height,std::vector<Segment2*>& vIntersectionSegments);
/// @private
	void createContours(MMPS& mmPS,std::vector<std::vector<Segment2> >& vvContours);
/// @private
	void getIntersection(Triangle2* pT,double height,int i,std::vector<Point2>& vIntersectionPoints);
/// @private
	TriNode* pTree;
	std::vector<Triangle2*> vTriangles;
/// @private
	GeomTest* pGeomPredicates;
/// @private
	std::set<double> sForbiddenHeights;
/// @private
	Func_Rot3D* pFRot;
/// @private
	std::vector<Point2*> vVtx; // Rotation version
};


} // NAMESPACE FADE2D


#endif


