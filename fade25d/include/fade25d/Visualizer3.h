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

/// @file Visualizer3.h
#pragma once



#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE

#include "Point2.h"
#include "Segment2.h"
#include "VertexPair2.h"
#include "Edge2.h"
namespace GEOM_FADE25D
{
/**
 * \brief Visualizer3 is a 3D scene writer for the Geomview viewer.
 *
 * @deprecated This class enables the visualization of 3D scenes by outputting various geometric
 * objects (such as points, segments, and triangles) to a file, which can then be displayed
 * using the Geomview viewer. This viewer is reliable, fast, and minimal. However, despite
 * these advantages, **Geomview is outdated and only available on Linux**. Therefore, it is recommended
 * to use the VtkWriter class instead, for creating visualizations in the widely supported VTK file format.
 * VTK files are compatible with many viewers, and if you're new to this format, try using Paraview.
 *
 * \see VtkWriter A class for creating VTK visualization files.
 */
class CLASS_DECLSPEC Visualizer3
{
public:
	static const char * const CLIGHTBLUE;
	static const char * const CDARKBLUE;
	static const char * const CYELLOW;
	static const char * const CPINK;
	static const char * const CBLACK;
	static const char * const CLIGHTBROWN;
	static const char * const CDARKBROWN;
	static const char * const CORANGE;
	static const char * const CPURPLE;
	static const char * const CGRAY;
	static const char * const CLIGHTGRAY;
	static const char * const CRED;
	static const char * const CGREEN;
	static const char * const CWHITE;
	static const char * const CRIMSON;
	static const char * const CDARKORANGE;
	static const char * const CGOLDENROD;
	static const char * const COLIVE;
	static const char * const CLAWNGREEN;
	static const char * const CGREENYELLOW;
	static const char * const CPALEGREEN;
	static const char * const CMEDSPRINGGREEN;
	static const char * const CLIGHTSEAGREAN;
	static const char * const CCYAN;
	static const char * const CSTEELBLUE;
	static const char * const MIDNIGHTBLUE;
	static const char * const CWHEAT;


	/**
	 * @brief Returns a color by index
	 * @param i The index of the color.
	 * @return The corresponding color as a string.
	 */
	static const char * getColor(int i);

	/**
	 * @brief Returns the next color in the sequence
	 * @return The next color as a string.
	 */
	static const char * getNextColor();

	/**
	 * @brief Returns the next color and its name
	 * @param colorName A reference to the color name to be updated.
	 * @return The next color as a string.
	 */
	static const char * getNextColorAndName(const char*& colorName);

	/**
	 * @brief Returns the color index for the next color
	 * @return The next color index.
	 */
	static int getNextColorNum();

	/**
	 * \brief Constructor
	 *
	 * Initializes the Visualizer3 with the specified output file name.
	 * The file is used to write the visualization data in a format readable
	 * by the Geomview viewer.
	 *
	 * @param name The name of the output file.
	 */
	explicit Visualizer3(const char* name);
	/** \brief Destructor */
	~Visualizer3();

	/**
	 * \brief Closes the output file
	 *
	 * This method finalizes the output and closes the file.
	 */
	void closeFile();

	/**
	 * \brief Write normals to the output file
	 *
	 * Writes normals of the specified triangles to the output file.
	 * Normals are scaled by the specified factor.
	 *
	 * @param vT The triangles whose normals are written.
	 * @param scale The scaling factor for the normals.
	 */
	void writeNormals(const std::vector<Triangle2*>& vT,double scale);

	/**
	 * \brief Write points to the output file
	 *
	 * Writes the specified points to the output file with the given line width
	 * and color.
	 *
	 * @param vPoints The points to be written.
	 * @param linewidth The width of the lines used to represent points.
	 * @param color The color of the points.
	 */
	void writePoints(const std::vector<Point2*>& vPoints,unsigned linewidth,const char* color) ;

	/**
	 * \brief Write points to the output file (overloaded)
	 *
	 * Writes the specified points to the output file with the given line width
	 * and color. This version uses a vector of Point2 objects directly.
	 *
	 * @param vPoints The points to be written.
	 * @param linewidth The width of the lines used to represent points.
	 * @param color The color of the points.
	 */
	void writePoints(const std::vector<Point2>& vPoints,unsigned linewidth,const char* color) ;

	/**
	 * \brief Write a single point to the output file
	 *
	 * Writes the specified point to the output file with the given line width
	 * and color.
	 *
	 * @param p The point to be written.
	 * @param linewidth The width of the line used to represent the point.
	 * @param color The color of the point.
	 */
	void writePoint(const Point2& p,unsigned linewidth,const char* color);

	/**
	 * \brief Write a segment to the output file
	 *
	 * Writes a segment defined by two endpoints to the output file, with the given
	 * color and optional endpoint visibility.
	 *
	 * @param src The source point.
	 * @param trg The target point.
	 * @param color The color of the segment.
	 * @param bWithEndPoints Flag indicating whether to show endpoints.
	 */
	void writeSegment(const Point2& src,const Point2& trg,const char* color,bool bWithEndPoints=false);

	/**
	 * \brief Write multiple segments to the output file
	 *
	 * Writes a collection of segments to the output file with the specified color
	 * and optional endpoint visibility.
	 *
	 * @param vSegments The segments to be written.
	 * @param color The color of the segments.
	 * @param bWithEndPoints Flag indicating whether to show endpoints.
	 */
	void writeSegments(const std::vector<Segment2>& vSegments,const char* color,bool bWithEndPoints=false);

	/**
	 * \brief Write multiple edge segments to the output file
	 *
	 * Writes a collection of Edge2 objects to the output file with the specified
	 * color and optional endpoint visibility.
	 *
	 * @param vEdge2 The edges to be written.
	 * @param color The color of the segments.
	 * @param bWithEndPoints Flag indicating whether to show endpoints.
	 */
	void writeSegments(const std::vector<Edge2>& vEdge2,const char* color,bool bWithEndPoints=false);

	/**
	 * \brief Write vertex pairs to the output file
	 *
	 * Writes a collection of segments, defined by VertexPair2 objects to the output file with the specified color.
	 *
	 * @param vVertexPairs The segments to be written.
	 * @param color The color of the segments.
	 */
	void writeVertexPairs(const std::vector<VertexPair2>& vVertexPairs,const char* color);

	/**
	 * \brief Write cubes to the output file
	 *
	 * Writes a collection of cubes defined by points to the output file with the specified color.
	 *
	 * @param vPoints The points that define the cubes.
	 * @param color The color of the cubes.
	 */
	void writeCubes(const std::vector<Point2>& vPoints,const char* color);

	/**
	 * \brief Write triangles to the output file
	 *
	 * Writes a collection of triangles to the output file with the specified color
	 * and optional normals.
	 *
	 * @param vT The triangles to be written.
	 * @param color The color of the triangles.
	 * @param bWithNormals Flag indicating whether to include normals.
	 */
	void writeTriangles(const std::vector<Triangle2*>& vT,const char* color,bool bWithNormals=false);

	/**
	 * \brief Write triangles using corner points to the output file
	 *
	 * Writes a collection of triangles defined by corner points to the output file
	 * with the specified color and optional normal visibility.
	 *
	 * @param vTriangleCorners The corner points of the triangles.
	 * @param color The color of the triangles.
	 * @param bWithNNV Flag indicating whether to include normals.
	 */
	void writeTriangles(const std::vector<Point2>& vTriangleCorners,const char* color,bool bWithNNV);

	/**
	 * \brief Write a single triangle to the output file
	 *
	 * Writes a single triangle to the output file with the specified color.
	 *
	 * @param t The triangle to be written.
	 * @param color The color of the triangle.
	 */
	void writeTriangle(const Triangle2& t,const char* color);

	/**
	 * \brief Write a triangle to the output file using 3 points
	 *
	 * Writes a triangle defined by 3 points to the output file with the specified color.
	 *
	 * @param p0 The first point of the triangle.
	 * @param p1 The second point of the triangle.
	 * @param p2 The third point of the triangle.
	 * @param color The color of the triangle.
	 */
	void writeTriangle(const Point2& p0,const Point2& p1,const Point2& p2,const char* color);

	/**
	 * \brief Write a ball (point with radius) to the output file
	 *
	 * Writes a ball defined by a point and radius to the output file.
	 *
	 * @param p The center point of the ball.
	 * @param radius The radius of the ball.
	 */
	void writeBall(const Point2& p,double radius);

	/**
	 * \brief Set whether to show backfaces in a different color.
	 *
	 * This method allows setting whether backfaces be shown in a different color.
	 *
	 * @param bWithBackfaces Flag indicating whether to show backfaces in a different color.
	 */
	void setBackfaces(bool bWithBackfaces);
private:
	Visualizer3(const Visualizer3&);
	Visualizer3& operator=(const Visualizer3&);
	void startList(size_t numPoints,size_t numTriangles,bool bWithEdges);
	void endList();
	std::ofstream* pOutFile;
	static int nextColor;
	bool bWithBackfaces;
};

} // NAMESPACE FADE25D

#elif GEOM_PSEUDO3D==GEOM_FALSE
#else
#error GEOM_PSEUDO3D is not defined
#endif

