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


/// @file Circle2.h
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

/** \brief Circle class for visualization
 *
 * Represents a circle used for visualization purposes. This class is primarily intended
 * for graphical representation.
 *
 * \see Visualizer2
 */

class Circle2
{
public:
/** \brief Constructor
*
* @param x x-coordinate of the center
* @param y y-coordinate of the center
* @param sqRadius_ squared radius of the circle
*/
	CLASS_DECLSPEC
	Circle2(double x,double y,double sqRadius_);

/** \brief Constructor
*
* @param center_ center of the circle
* @param sqRadius_ squared radius of the circle
*/
	CLASS_DECLSPEC
	Circle2(const Point2& center_,double sqRadius_);
/** \brief Destructor */
	CLASS_DECLSPEC
	~Circle2();
/** \brief Get the radius of the circle
*
* @return the radius
*/
	CLASS_DECLSPEC
	double getRadius();

/** \brief Get the squared radius of the circle
*
* @return the squared radius
*/
	CLASS_DECLSPEC
	double getSqRadius();

/** \brief Get the center of the circle
*
* @return the center as a Point2 object
*/
	CLASS_DECLSPEC
	Point2 getCenter();
	CLASS_DECLSPEC
	friend std::ostream &operator<<(std::ostream &stream, Circle2 b);
protected:
	Point2 center;
	double sqRadius;
};

} // (namespace)
