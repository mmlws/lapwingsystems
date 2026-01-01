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

/// @file Vector2.h

#pragma once
#include "common.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


class CLASS_DECLSPEC Vector2;




/** \brief Vector
*
 * \if DOC2D
* This class represents a 2D vector.
 * \endif
 * \if DOC25D
* This class represents a 2.5D vector.
 * \endif
*/
class Vector2
{
public:

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Constructor
 *
 * Constructs a vector with the given x, y, and z components.
 *
 * @param x_,y_,z_ The coordinates of the vector.
 */
	Vector2(const double x_,const double y_,const double z_);

/** \brief Default constructor
 *
 * Initializes the vector to (0, 0, 0).
 */
	Vector2();

/** \brief Copy constructor
 *
 * Creates a copy of the given vector.
 *
 * @param v_ The vector to copy.
 */
	Vector2(const Vector2& v_);

#else

/** \brief Constructor
*
* Constructs a 2D vector with the given x and y components.
*
* @param x_,y_ The coordinates of the vector.
*/
	Vector2(const double x_,const double y_);

/** \brief Default constructor
 *
 * Initializes the vector to (0, 0).
 */
Vector2();


/** \brief Copy constructor
 *
 * Creates a copy of the given vector.
 *
 * @param v_ The vector to copy.
 */
Vector2(const Vector2& v_);
#endif

	/** \brief Assignment operator
	 *
	 * Assigns the values of one vector to another.
	 *
	 * @param other The vector to copy.
	 * @return The updated vector.
	 */
	Vector2& operator=(const Vector2& other);



/** \brief Get a normalized, orthogonal 2D vector (CCW direction)
 *
 * Computes a 2D Vector2 orthogonal to the current vector in a counterclockwise (CCW) direction.
 * \if DOC25D The z-coordinate is set to 0. \endif
 *
 * @return A Vector2 orthogonal to the current vector in CCW direction.
 */
	Vector2 orthogonalVector() const;

	/** \brief Check if the vector is degenerate.
	 *
	 * Checks if the vector is degenerate (i.e., has zero length).
	 *
	 * @return `true` if the vector length is 0, `false` otherwise.
	 */
	bool isDegenerate() const;


	/** \brief Get the x-value
	 *
	 * @return The x-coordinate of the vector.
	 */
	double x() const;

	/** \brief Get the y-value
	 *
	 * @return The y-coordinate of the vector.
	 */
	double y() const;

#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Get the z-value.
	 *
	 * @return The z-coordinate of the vector.
	 */

	double z() const;
#endif

#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Set the components
	 *
	 * Sets the x, y, and z components of the vector.
	 *
	 * @param x_,y_,z_ The components of the vector.
	 */
	void set(const double x_,const double y_,const double z_);

#else
	/** \brief Set the components
	 *
	 * Sets the x, and y components of the vector.
	 *
	 * @param x_,y_ The components of the vector.
	 */
	void set(const double x_,const double y_);
#endif

	/** \brief Get the squared length of the vector
	 *
	 * @return The squared length of the vector.
	 */
	double sqLength() const;


#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get the index of the largest absolute component.
 *
 * Returns the index of the largest absolute component (0 for x, 1 for y, or 2 for z).
 *
 * @return The index of the largest absolute component of the vector.
 */
	int getMaxIndex() const;
#endif

	/** \brief Get the length of the vector
	 *
	 * @return The length of the vector.
	 */
	 double length() const;

	/** \brief Scalar product
	 *
	 * Computes the scalar product of the current vector and another vector.
	 *
	 * @param other The other vector to compute the scalar product with.
	 * @return The scalar product of the two vectors.
	 */
#if GEOM_PSEUDO3D==GEOM_TRUE

	double operator*(const Vector2& other) const;
#else

	double operator*(const Vector2& other) const;
#endif


	/** \brief Multiplication by a scalar
	 *
	 * Multiplies the vector by a scalar value.
	 *
	 * @param val The scalar value to multiply by.
	 * @return A new vector resulting from the multiplication.
	 */
#if GEOM_PSEUDO3D==GEOM_TRUE

	Vector2 operator*(double val) const;
#else

	Vector2 operator*(double val) const;
#endif

	/** \brief Division by a scalar
	 *
	 * Divides the vector by a scalar value.
	 *
	 * @param val The scalar value to divide by.
	 * @return A new vector resulting from the division.
	 */
#if GEOM_PSEUDO3D==GEOM_TRUE
	Vector2 operator/(double val) const;
#else

	Vector2 operator/(double val) const;
#endif

protected:
	double valX;
	double valY;
#if GEOM_PSEUDO3D==GEOM_TRUE
	double valZ;
#endif
};




// Free functions
/** \brief Print a Vector2.
 *
 * Prints a Vector2 to an output stream.
 *
 * @param stream The output stream.
 * @param vec The vector to print.
 *
 * @return The output stream.
*/
inline std::ostream &operator<<(std::ostream &stream, const Vector2& vec)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	stream << "Vector2: "<<vec.x()<<", "<<vec.y()<<", "<<vec.z();
#else
	stream << "Vector2: "<<vec.x()<<", "<<vec.y();
#endif
	return stream;
}

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Cross product
 *
 * Computes the cross product of two vectors.
 *
 * @param vec0 The first vector.
 * @param vec1 The second vector.
 * @return A new vector resulting from the cross product.
 */
inline Vector2 crossProduct(const Vector2& vec0,const Vector2& vec1)
{
	double x=vec0.y() * vec1.z() - vec0.z() * vec1.y();
	double y=vec0.z() * vec1.x() - vec0.x() * vec1.z();
	double z=vec0.x() * vec1.y() - vec0.y() * vec1.x();
	return Vector2(x,y,z);
}
#endif


/** \brief Opposite vector
 *
 * Returns a vector pointing in the opposite direction.
 *
 * @return A new vector pointing in the opposite direction.
 */
inline Vector2 operator-(const Vector2& in)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Vector2(-in.x(),-in.y(),-in.z());
#else
	return Vector2(-in.x(),-in.y());
#endif
}

/** \brief Multiplication with a scalar
 *
 * Multiplies a scalar by a vector.
 *
 * @param d The scalar value.
 * @param vec The vector to multiply.
 * @return A new vector resulting from the multiplication.
 */
inline Vector2 operator*(double d,const Vector2& vec)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Vector2(d*vec.x(),d*vec.y(),d*vec.z());
#else
	return Vector2(d*vec.x(),d*vec.y());
#endif
}

/** \brief Addition
 *
 * Adds two vectors.
 *
 * @param vec0 The first vector.
 * @param vec1 The second vector.
 * @return A new vector resulting from the addition.
 */
inline Vector2 operator+(const Vector2& vec0,const Vector2& vec1)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Vector2(vec0.x()+vec1.x(), vec0.y()+vec1.y() , vec0.z()+vec1.z());
#else
	return Vector2(vec0.x()+vec1.x(), vec0.y()+vec1.y() );
#endif
}

/** \brief Subtraction
 *
 * Subtracts two vectors.
 *
 * @param vec0 The first vector.
 * @param vec1 The second vector.
 * @return A new vector resulting from the subtraction.
 */
inline Vector2 operator-(const Vector2& vec0,const Vector2& vec1)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Vector2(vec0.x()-vec1.x(), vec0.y()-vec1.y() , vec0.z()-vec1.z());
#else
	return Vector2(vec0.x()-vec1.x(), vec0.y()-vec1.y() );
#endif
}


} // (namespace)
