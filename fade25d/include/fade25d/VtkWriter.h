// Copyright (C) Geom Software e.U, Bernhard Kornberger, Graz/Austria
//
// This file is part of the Fade2D library. The student license is free
// of charge and covers personal non-commercial research. Licensees
// holding a commercial license may use this file in accordance with
// the Commercial License Agreement.
//
// This software is provided AS IS with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Please contact the author if any conditions of this licensing are
// not clear to you.
//
// Author: Bernhard Kornberger, bkorn (at) geom.at
// http://www.geom.at

/// @file VtkWriter.h
#pragma once
#include "common.h"
#include "Point2.h"
#include "Segment2.h"
#include "Edge2.h"
#include "Triangle2.h"

#if GEOM_PSEUDO3D == GEOM_TRUE
namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D == GEOM_FALSE
namespace GEOM_FADE2D {
#else
#error GEOM_PSEUDO3D is not defined
#endif

struct Dat3; // FWD

/// @enum VtkColor
/// @brief Enumeration of colors used by the VTKWriter class to represent various colors in the VTK file format.
enum VtkColor
{
	VTK_RED, ///< Red color.
	VTK_GREEN, ///< Green color.
	VTK_BLUE, ///< Blue color.
	VTK_YELLOW, ///< Yellow color.
	VTK_CYAN, ///< Cyan color.
	VTK_MAGENTA, ///< Magenta color.
	VTK_BLACK, ///< Black color.
	VTK_WHITE, ///< White color.
	VTK_GRAY, ///< Gray color.
	VTK_ORANGE, ///< Orange color.
	VTK_PINK, ///< Pink color.
	VTK_LIGHTBLUE, ///< Light blue color.
	VTK_DARKBLUE, ///< Dark blue color.
	VTK_LIGHTGREEN, ///< Light green color.
	VTK_BROWN, ///< Brown color.
	VTK_PURPLE, ///< Purple color.
	VTK_TRANSPARENT ///< Transparent color, meaning: Don't draw.
};

/** @brief A class for writing data to the VTK file format for visualization purposes.
 *
 * \image html paraview.png "Example visualization using the VTK file format and the Paraview Viewer" width=50%
 * \image latex paraview.png "Example visualization using the VTK file format and the Paraview Viewer" width=0.8\textwidth
 *
 * This class visualizes 3D scenes by outputting various geometric
 * objects (such as points, segments, and triangles) to a VTK file. VTK files are
 * compatible with many viewers, and if you're new to this format, you may want to try **Paraview** for visualization.
 *
 *
 * You can use this class to create custom 2D and 3D scenes, or use
 * pre-made VTK visualizations available in classes like Fade_2D and Zone2.
 *
 * \see Fade_2D::showVtk()
 * \see Zone2::showVtk()
 */
class CLASS_DECLSPEC VtkWriter
{
public:

	/**
	 *  @brief Constructor for the VtkWriter class.
	 *  @param name The name of the output VTK file.
	 */
	explicit VtkWriter(const char* name);

	/**
	 *  @brief Destructor for the VtkWriter class.
	 */
	~VtkWriter();

	/**
	 *  @brief Get the next color in the predefined sequence of colors.
	 *  @return The next VtkColor in the sequence.
	 */
	VtkColor getNextColor();

	/**
	 *  @brief Add a single point
	 *
	 *  Adds a single point to the VTK file with a specified color.
	 *
	 *  @param p The point to add.
	 *  @param color The color of the point.
	 */
	void addPoint(const Point2& p, VtkColor color);

	/**
	 *  @brief Add multiple points
	 *
	 *  Adds multiple points with a specified color to the VTK file.
	 *
	 *  @param vPoints A vector of points to add.
	 *  @param color The color of the points.
	 */
	void addPoints(const std::vector<Point2*>& vPoints, VtkColor color);
	/**
	 *  @brief Add multiple points
	 *
	 *  Adds multiple points with a specified color to the VTK file.
	 *
	 *  @param vPoints A vector of points to add.
	 *  @param color The color of the points.
	 */
	void addPoints(const std::vector<Point2>& vPoints, VtkColor color);

	/**
	 *  @brief Add a segment
	 *
	 *  Adds a segment with a specified color to the VTK file.
	 *
	 *  @param segment The segment to add.
	 *  @param color The color of the segment.
	 *  @param bWithEndPoints Whether to include the end points in the visualization.
	 */
	void addSegment(const Segment2& segment, VtkColor color, bool bWithEndPoints = false);

	/**
	 *  @brief Add a segment
	 *
	 *  Adds a segment defined by source and target points with a specified color to the VTK file.
	 *
	 *  @param source The source point of the segment.
	 *  @param target The target point of the segment.
	 *  @param color The color of the segment.
	 *  @param bWithEndPoints Whether to include the end points in the visualization.
	 */
	void addSegment(const Point2& source, const Point2& target, VtkColor color, bool bWithEndPoints = false);

	/**
	 *  @brief Add multiple segments
	 *
	 *  Adds multiple segments with a specified color to the VTK file.
	 *
	 *  @param vSegments A vector of segments to add.
	 *  @param color The color of the segments.
	 *  @param bWithEndPoints Whether to include the end points in the visualization.
	 */
	void addSegments(const std::vector<Segment2>& vSegments, VtkColor color, bool bWithEndPoints = false);

	/**
	 *  @brief Add multiple Edge2 objects
	 *
	 *  Adds multiple Edge2 objects with a specified color to the VTK file.
	 *
	 *  @param vEdges A vector of Edge2 objects to add.
	 *  @param color The color of the edges.
	 *  @param bWithEndPoints Whether to include the end points in the visualization.
	 */
	void addSegments(const std::vector<Edge2>& vEdges, VtkColor color, bool bWithEndPoints = false);

	/**
	 *  @brief Add multiple segments
	 *
	 *  Adds multiple segments defined by pairs of endpoints with a specified color to the VTK file.
	 *
	 *  @param vSegmentEndPoints A vector of point pairs defining the endpoints of the segments.
	 *  @param color The color of the segments.
	 *  @param bWithEndPoints Whether to include the end points in the visualization.
	 */
	void addSegments(const std::vector<Point2>& vSegmentEndPoints, VtkColor color, bool bWithEndPoints = false);

	/**
	 *  @brief Add multiple triangles
	 *
	 *  Adds multiple triangles with a specified color to the VTK file.
	 *
	 *  @param vT A vector of triangle pointers to add.
	 *  @param color The color of the triangles.
	 */
	void addTriangles(const std::vector<Triangle2*>& vT, VtkColor color);

	/**
	 *  @brief Add multiple triangles
	 *
	 *  Adds multiple triangles defined by their corner points with a specified color to the VTK file.
	 *
	 *  @param vTriangleCorners A vector of points defining the corners of the triangles.
	 *  @param color The color of the triangles.
	 */
	void addTriangles(const std::vector<Point2>& vTriangleCorners, VtkColor color);

	/**
	 *  @brief Add a single triangle
	 *
	 *  Adds a single triangle with a specified color to the VTK file.
	 *
	 *  @param t The triangle to add.
	 *  @param color The color of the triangle.
	 */
	void addTriangle(const Triangle2& t, VtkColor color);

	/**
	 *  @brief Add a triangle defined by three points
	 *
	 *  Adds a triangle defined by three points with a specified color to the VTK file.
	 *
	 *  @param p0 The first point defining the triangle.
	 *  @param p1 The second point defining the triangle.
	 *  @param p2 The third point defining the triangle.
	 *  @param color The color of the triangle.
	 */
	void addTriangle(const Point2& p0, const Point2& p1, const Point2& p2, VtkColor color);

	/**
	 *  @brief Finalize the visualization
	 *
	 *  Writes all accumulated data to the specified VTK file.
	 *
	 * @note This function finalizes the VTK file. **It must be called after all geometric elements have been added.**
	 */
	void writeFile();

private:
	/// \brief Copy constructor (deleted)
	VtkWriter(const VtkWriter&);

	/// \brief Assignment operator (deleted)
	VtkWriter& operator=(const VtkWriter&);

	Dat3* pDat; ///< Pointer to the data structure
};

} // namespace GEOM_FADE25D or GEOM_FADE2D

