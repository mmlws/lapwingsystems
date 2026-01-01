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


/// @file CompPolygon.h
#pragma once
#include "Edge2.h"
#include "common.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


/**
 * \brief Connected component of triangles with boundary and hole polygons.
 *
 * @image html connected-component.png "Polygon with a hole: The outer boundary is counterclockwise oriented, the hole's boundary clockwise." width=40%
 * @image latex connected-component.png "Polygon with a hole: The outer boundary is counterclockwise oriented, the hole's boundary clockwise." width=0.8\textwidth

 * The `CompPolygon` structure holds a collection of triangles that form a
 * connected component. Two triangles are considered connected if they share
 * an edge. The structure also stores ordered polygons representing the
 * boundary (outer polygon) and any holes within the connected component.
 *
 * @note Good to know: An Edge2 is represented by a Triangle2 and an opposite index, and
 * it is directed, with the orientation always being counterclockwise (CCW) around the
 * Triangle2. As a consequence, outer polygon edges are oriented counterclockwise
 * (CCW), while edges representing hole polygons (if present) are clockwise (CW).
 *
 */
struct CLASS_DECLSPEC CompPolygon
{
	/**
	 * \brief Default constructor.
	 *
	 * Default constructor
	 */
	CompPolygon();

	/**
	 * \brief Copy constructor.
	 *
	 * Creates a copy of another `CompPolygon` object.
	 *
	 * \param other The `CompPolygon` object to copy.
	 */
	CompPolygon(const CompPolygon& other);

	/**
	 * \brief Assignment operator.
	 *
	 * Assigns the contents of another `CompPolygon` object to this one.
	 *
	 * \param other The `CompPolygon` object to assign.
	 * \return Reference to this `CompPolygon` object.
	 */
	CompPolygon& operator=(const CompPolygon& other);

	/**
	 * \brief Destructor.
	 *
	 * Destructor.
	 */
	~CompPolygon();
	// Data

	/**
	 * \brief Connected component of triangles.
	 *
	 * A vector of pointers to Triangle2 objects that form the connected component.
	 */
	std::vector<Triangle2*>* pvCC;

	/**
	 * \brief Ordered outer polygon edges.
	 *
	 * A vector of `Edge2` objects representing the outer boundary of the connected component.
	 */
	std::vector<Edge2>* pvOuterPolygon;

	/**
	 * \brief Ordered hole polygons.
	 * A vector of vectors of `Edge2` objects representing holes within the connected component.
	 * Each hole is represented by a separate vector of `Edge2`'s.
	 */
	std::vector<std::vector<Edge2> >* pvvHolePolygons;
};


} // (namespace)
