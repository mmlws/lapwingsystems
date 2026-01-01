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


/// @file PShape.h
#pragma once
#include "Point2.h"
#include "common.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif
/**
 * @brief Polygonal Shape for Visualization.
 *
 * PShape represents a polygonal shape used for visualization with
 * the PDF and Postscript Writer Visualizer2.
 */

class CLASS_DECLSPEC PShape
{
public:
	/**
 * \brief Constructor
 *
 * Initializes a PShape object with a vector of Point2 objects.
 * The points in \p vP_ define the vertices of the polygonal shape.
 *
 * @param vP_ A vector of Point2 objects representing the vertices of the shape.
 */
	explicit PShape(std::vector<Point2>& vP_);

/**
 * \brief Copy Constructor
 *
 * Creates a new PShape object as a copy of another PShape object.
 *
 * @param other The PShape object to copy.
 */
	PShape(const PShape& other);

/**
 * \brief Assignment Operator
 *
 * Assigns the values of one PShape object to another.
 *
 * @param other The PShape object to assign from.
 * @return A reference to the assigned PShape object.
 */
	PShape& operator=(const PShape& other);
/** Destructor */
	~PShape();

/**
 * \brief Prints a PShape
 *
 * Prints the values of a PShape object to the specified output stream.
 *
 * @param stream The output stream where the PShape will be printed.
 * @param shape The PShape object to be printed.
 * @return The output stream after printing the PShape.
 */
	friend std::ostream &operator<<(std::ostream &stream, PShape shape);
protected:
	friend class Visualizer2;
	std::vector<Point2>* pVP;
};

} // (namespace)
