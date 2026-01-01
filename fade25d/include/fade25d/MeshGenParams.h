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

/// @file MeshGenParams.h
#pragma once
#include "common.h"
#include "Vector2.h"
#include "Point2.h"


#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


class Triangle2; // Forward
class Zone2; // Forward
class Fade_2D; // Forward
class ConstraintSegment2; // Forward
/** @internal Unused parameter: Empty template to avoid compiler warnings about unused function parameters */
template <typename T> inline void unusedParameter(const T&){}

/**
 * \brief Parameters for the mesh generator.
 *
 * This class serves as a container for the parameters used by the mesh
 * generator, i.e., for the `Fade_2D::refineAdvanced()` method. Client
 * code can derive a custom class from `MeshGenParams` and provide
 * specific implementations for methods such as `getMaxTriangleArea(Triangle* pT)`
 * or `getMaxEdgeLength(Triangle* pT)` to control the local density
 * of the generated mesh. The meshing algorithm calls these functions
 * during refinement to check whether a triangle meets the conditions
 * for further subdivision.
 *
 * \image html meshing-strategies.png "Quality meshing with different parameter configurations leading to varying results" width=100%
 * \image latex meshing-strategies.png "Quality meshing with different parameter configurations leading to varying results" width=0.9\textwidth
 *
 * \sa [C++ Examples on advanced mesh generation.](http://www.geom.at/advanced-mesh-generation/)
 */
class CLASS_DECLSPEC MeshGenParams
{
public:
	/** \brief Constructor for mesh generator parameters
	 *
	 * Initializes the mesh generator parameters with default values. These
	 * can be overridden as needed.
	 *
	 * \param pZone_ A pointer to the Zone2 to be remeshed.
	 */
	explicit MeshGenParams(Zone2* pZone_):
#if GEOM_PSEUDO3D==GEOM_TRUE
		// *** These two parameters exist only in Fade2.5D ***
		pHeightGuideTriangulation(NULL),
		maxHeightError(DBL_MAX),
#endif
		pZone(pZone_),
		minAngleDegree(20.0),
		minEdgeLength(1e-3),
		maxEdgeLength(DBL_MAX),
		maxTriangleArea(DBL_MAX),
		bAllowConstraintSplitting(true),
		growFactor(DBL_MAX),
		growFactorMinArea(1e-3),
		capAspectLimit(10.0),
		gridLength(0.0),
		bKeepExistingSteinerPoints(true),
		command(0), // command is for development purposes
		bReproducibleResults(true), // use false for better performance
		bGridMeshingLQ(false), // use true (only together with grid-meshing) for better performance
		psLockedConstraintSegments(NULL)
	{
		verify();
#if GEOM_PSEUDO3D==GEOM_TRUE
		gridVector=Vector2(1.0,0.0,0.0);
		gridPoint=Point2(DBL_MAX,DBL_MAX,DBL_MAX);
#else
		gridVector=Vector2(1.0,0.0);
		gridPoint=Point2(DBL_MAX,DBL_MAX);
#endif
	}

	virtual ~MeshGenParams();




	/** \brief Get the maximum allowed triangle area for mesh refinement
	 *
	 * This method is called by the mesh generation algorithm. It returns
	* for a Triangle2 \p pT the maximum allowed area. If the area of
	 * \p pT exceeds this value, then \p pT will be subdivided. The
	 * default implementation returns the value `maxTriangleArea` for all triangles,
	* which is `DBL_MAX` by default (indicating no limit). You may change
	* this value or derive a class from MeshGenParams and override the
	* method.
	 *
	 * \param pT A pointer to the Triangle2 being evaluated.
	 * \return The maximum area allowed for the Triangle2.
	 *
	 * \image html mesh-generator-customParameters.png "User Controlled Mesh Density, the area is limited in the lower left corner" width=25%
	 * \image latex mesh-generator-customParameters.eps "User Controlled Mesh Density, the area is limited in the lower left corner" width=7cm
	 */
	virtual double getMaxTriangleArea(Triangle2* pT)
	{
		unusedParameter(pT); // avoids useless compiler warnings
		return maxTriangleArea;
	}

	/** \brief Get the maximum allowed edge length for a Triangle2
	*
	* This method is called by the mesh generation algorithm. It returns
	* for a Triangle2 \p pT the maximum allowed edge length. If any of the
	* edges of \p pT are larger, then \p pT will be subdivided. The default
	* implementation returns the value `maxEdgeLength` for all triangles,
	* which is `DBL_MAX` by default (indicating no limit). You may change
	* this value or derive a class from MeshGenParams and override the
	* method.
	*
	* \param pT A pointer to the Triangle2 being evaluated.
	* \return The maximum edge length allowed for the Triangle2.
	*
	* \image html mesh-generator-customParameters.png "User Controlled Mesh Density, the edge length is limited in the lower left corner" width=25%
	* \image latex mesh-generator-customParameters.eps "User Controlled Mesh Density, the edge length is limited in the lower left corner" width=7cm
	*/
	virtual double getMaxEdgeLength(Triangle2* pT)
	{
		unusedParameter(pT); // avoids useless compiler warnings
		return maxEdgeLength;
	}


	// *** The following two parameters exist only in Fade2.5D ***
#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Height guide triangulation
	*
	* This parameter is used in Fade2.5D for more accurate height calculations during mesh refinement.
	* If set, the algorithm will use the provided triangulation for height queries instead of the
	* default computation.
	*/
	Fade_2D* pHeightGuideTriangulation;
	/** \brief Maximum height error for mesh refinement
	 *
	 * When `pHeightGuideTriangulation` is set, the algorithm will refine the mesh if the height error
	 * between the calculated and guide triangulation exceeds `maxHeightError`.
	 */
	double maxHeightError;
#endif
	/** \brief The Zone2 to be meshed
	 *
	 * This parameter specifies the Zone2 that the meshing algorithm will operate on.
	 */
	Zone2* pZone;

	/** \brief Minimum interior angle for triangles
	 *
	 * This parameter sets the minimum allowable interior angle for triangles in the
	 * mesh. Triangles with angles smaller than this value will be refined. The default
	 * value is 20.0 degrees, the maximum is 30.0.
	 */
	double minAngleDegree;
	/** \brief Minimum edge length for triangle refinement
	 *
	 * This parameter sets the minimum edge length for triangles in the mesh.
	 * Triangles with edges shorter than this value will not be subdivided.
	 * The default value is 1e-3.
	 */
	double minEdgeLength;

	/** \brief Maximum edge length
	*
	* The \p maxEdgeLength value is returned by the default implementation of
	* MeshGenParams::getMaxEdgeLength(). Larger edges are automatically subdivided.
	* Default value: `DBL_MAX` (no limit)
	*/
	double maxEdgeLength;

	/** \brief maxTriangleArea
	*
	* The \p maxTriangleArea value is returned by the default implementation of
	* MeshGenParams::getMaxTriangleArea(). Larger triangles are automatically
	* subdivided. Default value: `DBL_MAX` (no limit)
	*/
	double maxTriangleArea;


	/** \brief Allow constraint segment splitting
	 *
	 * The \p bAllowConstraintSplitting flag determines whether constraint segments
	 * can be split during mesh refinement. If set to `true`, constraint segments are
	 * allowed to be divided, otherwise not. Default: `true`.
	 */
	bool bAllowConstraintSplitting;
	/** \brief Grow factor for adjacent triangles
	 *
	 * The \p growFactor parameter controls how much adjacent triangles can grow in area.
	 * For two adjacent triangles (t0, t1), the ratio of their areas must be
	 * less than the \p growFactor. Recommendation: `growFactor>5.0`, Default: `DBL_MAX` (no limit).
	 */
	double growFactor;

	/** \brief growFactorMinArea
	*
	* The \c growFactor value is ignored for triangles with a smaller area
	* than \p growFactorMinArea. This value prevents generation of hundreds
	* of tiny triangles around one that is unusually small. Default: 0.001
	*/
	double growFactorMinArea;
	/** \brief Aspect ratio limit for triangle refinement
	 *
	 * The \p capAspectLimit parameter limits the ratio of edge length to height for triangles.
	 * Triangles with a higher aspect ratio than this value will be subdivided.
	 * Default: 10.0.
	 */
	double capAspectLimit;
	/** \brief gridVector
	*
	* This vector determines the alignment of the grid when grid-based meshing
	* is used. By default, the vector is aligned with the x-axis (1.0, 0.0).
	*
	* \image html grid-mesh-angle.png  "Grid meshing with custom alignment, not axis-aligned." width=25%
	* \image latex grid-mesh-angle.eps  "Grid meshing with custom alignment, not axis-aligned." width=7cm
	*/
	Vector2 gridVector;
	/** \brief Grid length for meshing
	 *
	 * The \p gridLength parameter determines the length of the grid cells
	 * for grid-based meshing. A value greater than 0 enables grid meshing.
	 * By default, \p `gridLength=0` (disabled).
	 *
	 * \image html grid-mesh.png "Grid Meshing axis aligned"
	 * \image latex grid-mesh.eps "Grid Meshing axis aligned" width=7cm
	 *
	 * @note The algorithm may automatically adapt the \p gridLength a bit
	 * such that the grid fits better into the shape.
	*/
	double gridLength;
	/** \brief Steiner points from previous refinements.
	*
	* A previous call to `refine()` or `refineAdvanced()` may have created
	* Steiner points. These points may be partially or entirely removed
	* during a later refinement call, even if the later refinement occurs
	* in a different zone. Whether this behavior is desired depends on your
	* application. Typically, you want to preserve the points, so the default
	* value of \p bKeepExistingSteinerPoints is true.
	*/
	bool bKeepExistingSteinerPoints;

/** \private A command for development, not for public use. */
	int command;
	/** \brief Locked constraint segments that should not be split
	 *
	 * This method allows the user to specify which ConstraintSegment2 objects
	 * should remain intact and not be split during mesh refinement. To
	 * use, set `bAllowConstraintSplitting` to `true` and then add locked
	 * segments using this method.
	 */
	void addLockedConstraint(ConstraintSegment2* pConstraintSegment);
	/** \brief Flag for reproducible results
	 *
	 * If `true`, the algorithm will generate reproducible triangulations.
	 * This is useful for debugging and consistency across runs. Set to
	 * false for better performance if reproducibility is not required.
	 * Default: `true`.
	 *
	 * @note Reproducible results are created inside `pZone`. For reproducibility
	 * across different zones use `Zone2::unifyGrid(0)` on the global Zone2.
	 *
	 */
	bool bReproducibleResults;

	/** \brief Low-quality grid meshing for high performance
	 *
	 * This flag allows for faster grid-based meshing at the cost of producing
	 * triangles with potentially bad shapes. Set this flag to `true` to improve
	 * performance when grid meshing is enabled. Default: `false`.
	 */
	bool bGridMeshingLQ;

	/// @private
	std::set<ConstraintSegment2*>* psLockedConstraintSegments;
	/** \brief Grid point through which the grid mesh must pass
	*
	* This optional point can be used to force the grid mesh to pass
	* through a specific point. By default, the coordinates of \p gridPoint
	* are initialized to DBL_MAX, which disables this feature. When set,
	* the grid will be aligned such that it passes through the specified
	* point.
	*/
	Point2 gridPoint;

/// @private
	void verify();
};


} // NAMESPACE
