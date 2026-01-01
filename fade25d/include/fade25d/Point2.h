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

/// @file Point2.h

#pragma once

#include "common.h"
#include "Vector2.h"
#include <cstring>

/**
 * @enum CINC2
 * @brief Enum for the position of a point relative to a circle.
 *
 * This enumeration describes the possible positions of a point
 * in relation to a circle.
 */
enum CINC2
{
	CINC2_UNKNOWN, 	///< Invalid value (undefined)
	CINC2_ZERO,    	///< Point lies exactly on the circumference of the circle
	CINC2_INCIRCLE,	///< Point lies inside the circle
	CINC2_OUTCIRCLE	///< Point lies outside the circle
};

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


class Triangle2; // FWD

/// @brief Structure to store vertex color (RGBA).
struct VtxColor
{
	/// @brief Constructor
    /// @param r_,g_,b_,a_ Red, green, blue, alpha (0-255).
	VtxColor(unsigned char r_,unsigned char g_,unsigned char b_,unsigned char a_):r(r_),g(g_),b(b_),a(a_)
	{}
	float r,g,b,a;
};


/**
 * @if DOC2D
 * @brief Represents a 2D point.
 *
 * The Point2 class represents a 2D point with x- and y-coordinates.
 * @endif
 * @if DOC25D
 * @brief Represents a 2.5D point.
 *
 * The Point2 class represents the a 2.5D point with x-, y-, and z-coordinates.
 * @endif
 */
class CLASS_DECLSPEC Point2
{
#ifdef GEOM_DBG
	/// \private Debugging function (internal use)
	int getSeqNum()
	{
		static int seqNum(0);
		return seqNum++;
	}
#endif

public:

#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Constructor
	 *
	 * Constructor
	*
	* @param[in] x_,y_,z_ x-, y- and z-coordinates
	*/
	Point2(const double x_,const double y_,const double z_):
		coordX(x_),
		coordY(y_),
		coordZ(z_),
		pAssociatedTriangle(NULL),
		customIndex(-1)
	{
#ifdef GEOM_DBG
		customIndex=getSeqNum();
#endif
	}


	/** \brief Default constructor
	*
	 * Default constructor
	 *
	* Initializes the coordinates to `-DBL_MAX`, the associated Triangle2 to NULL, and the `customIndex` to -1.
	*/
	Point2():coordX(-DBL_MAX),coordY(-DBL_MAX),coordZ(-DBL_MAX),pAssociatedTriangle(NULL),customIndex(-1)
	{
#ifdef GEOM_DBG
		customIndex=getSeqNum();
#endif
	}
	/** \brief Copy constructor
	 *
	 * Copy constructor. Copies the coordinates and the custom index from \p p.
	*
	* @param[in] p_ The point to be copied.
	*/
	Point2(const Point2& p_):
		coordX(p_.x()),
		coordY(p_.y()),
		coordZ(p_.z()),
		pAssociatedTriangle(NULL),
		customIndex(p_.customIndex)
	{
	}
	/** \brief Assignment operator.
	 *
	 * Assignment operator. Will assign the coordinates and the custom index of the other Point2.
	 *
	 * @param other The Point2 to be assigned to the current one.
	*/
	Point2 &operator=(const Point2& other)
	{
		coordX=other.x();
		coordY=other.y();
		coordZ=other.z();
		pAssociatedTriangle=NULL;
		customIndex=other.customIndex;
		return *this;
	}

	/** @private Deprecated, use setHeight() instead. Kept for backward compatibility */
	void setZ(double z)
	{
		setHeight(z);
	}

	/** @private (internal only) */
	void print()
	{
		GCOUT<<coordX<<" "<<coordY<<" "<<coordZ<<std::endl;
	}
#else
	/** \brief Constructor
	*
	* Constructor for a 2D point. Initializes the \c customIndex to -1.
	*
	* @param x_,y_ The coordinates of the point
	*/
	Point2(const double x_,const double y_):
		coordX(x_),
		coordY(y_),
		pAssociatedTriangle(NULL),
		customIndex(-1)
	{
		//BC("Point2 Constructor default");
	}
	/** \brief Default constructor
	*
	* Initializes the coordinates to `-DBL_MAX`, the associated Triangle2 to NULL, and the `customIndex` to -1.
	*/
	Point2():
		coordX(-DBL_MAX),
		coordY(-DBL_MAX),
		pAssociatedTriangle(NULL),
		customIndex(-1)
	{
	}
	/** \brief Copy constructor
	 *
	 * Copy constructor. Copies the coordinates and the custom index.
	*
	* @param[in] p_ The point to be copied.
	*/
	Point2(const Point2& p_):
		coordX(p_.x()),
		coordY(p_.y()),
		pAssociatedTriangle(NULL),
		customIndex(p_.customIndex)
	{
	}
	/** \brief Assignment operator
	 *
	 * Assignment operator.
	 *
	 * @param other The point whose coordinates and the custom index are assigned
	*/
	Point2 &operator=(const Point2& other)
	{
		coordX=other.x();
		coordY=other.y();
		pAssociatedTriangle=NULL;
		customIndex=other.customIndex;
		return *this;
	}

	/** @private (internal use) */
	void print()
	{
		GCOUT<<coordX<<" "<<coordY<<std::endl;
	}
#endif

	/**
	 * @brief Destructor.
	 *
	 */
	~Point2()
	{
	}


	/**
	 * @brief Get the x-coordinate of the Point2.
	 *
	 * Get the x-coordinate of the current Point2.
	 *
	 * @return The x-coordinate
	 */

	double x() const
	{
		return coordX;
	}
	/**
	  * @brief Get the y-coordinate of the Point2.
	  *
	  * Get the y-coordinate of the current Point2.
	  *
	  * @return The y-coordinate
	  */
	double y() const
	{
		return coordY;
	}

#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
     * @brief Get the z-coordinate of the Point2.
     *
     * Get the z-coordinate of the current Point2.
     *
     * @return The z-coordinate
     */
	double z() const
	{
		return coordZ;
	}
#endif

#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	 * @brief Get all three coordinates (x, y, z) of the Point2.
	 *
	 * Retrieves all three coordinates (x, y, z) of the current Point2.
	 *
	 * @param x_,y_,z_ Used to return the coordinate values.
	 */
	void xyz(double& x_,double& y_,double& z_) const
	{
		x_=coordX;
		y_=coordY;
		z_=coordZ;
	}
#endif
	/**
	 * @brief Get the x- and y- coordinates of the Point2.
	 *
	 * Retrieves the x- and y- coordinates of the current Point2.
	 *
	 * @param x_,y_ Used to return the coordinate values.
	 */
	void xy(double& x_,double& y_) const
	{
		x_=coordX;
		y_=coordY;
	}


#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	* @brief Set the z-coordinate of the current Point2.
	*
	* Sets the z-coordinate of the current Point2.
	*
	* @param z The z-coordinate value to be assigned.
	*/
	void setHeight(double z)
	{
		coordZ=z;
	}
#endif
	/**
	 * @brief Get the larger of the absolute values of x and y.
	 *
	 * Get the maximum of the absolute values of the x- and y-coordinates.
	 *
	 * @return The larger of the absolute values of x and y
	 */
	double getMaxAbs() const
	{
		double a(fabs(coordX));
		double b(fabs(coordY));
		return (std::max)(a,b);
	}
	/**
	 * @brief Compare if the current Point2 is less than another one.
	 *
	 * Less-than comparison, based on x and y coordinates.
	 *
	 * @param p The Point2 to compare with
	 *
	 * @return `true` if the current point is less, `false` otherwise
	 *
	 * @if DOC25D
	 * @warning The z-coordinate is not compared!
	 * @endif
	 */
	bool operator<(const Point2& p) const
	{
		if(coordX<p.coordX) return true;
		if(coordX>p.coordX) return false;
		if(coordY<p.coordY) return true;
		return false;
	}
	/**
	 * @brief Compare if the current Point2 is greater than another one.
	 *
	 * Greater-than comparison, based on x and y coordinates.
	 *
	 * @param p The Point2 to compare with.
	 *
	 * @return `true` if the current Point2 is greater, `false` otherwise.
	 *
	 * @if DOC25D
	 * @warning The z-coordinate is not compared.
	 * @endif
	 */
	bool operator>(const Point2& p) const
	{
		if(coordX>p.coordX) return true;
		if(coordX<p.coordX) return false;
		if(coordY>p.coordY) return true;
		return false;
	}
	/**
	* @brief Compares if the current Point2 is equal to another one.
	*
	* This function compares the current Point2 with another one based on the `x` and `y` coordinates.
	*
	* @param p The point to compare with.
	* @return `true` if the points are equal (same `x` and `y`), `false` otherwise.
	*
	* @if DOC25D
	* @warning The `z` coordinate is not compared! As a result, two points with identical `x` and `y` coordinates but different `z` coordinates will be considered equal.
	* @endif
	*/
	bool operator==(const Point2& p) const
	{
		return (coordX==p.coordX && coordY==p.coordY);
	}
#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	* @brief Compares if the current Point2 is equal to another one, based on x,y and z.
	*
	 * Compares if the current Point2 is equal to another one, based on the
	 * the `x`, `y`, and `z` coordinates. Unlike the `operator==()`, which only
	* compares the `x` and `y` coordinates, this function takes the `z` coordinate
	* into account as well.
	*
	* @param p The point to compare with.
	* @return `true` if the `x`, `y`, and `z` coordinates are identical, `false` otherwise.
	*/
	bool samePoint(const Point2& p) const
	{
		return(coordX==p.coordX && coordY==p.coordY && coordZ==p.coordZ);
	}
#endif
/**
	* @brief Compares if the current Point2 is different from another one.
	*
	* This function checks if the current Point2 and \p p are different, based on the `x` and `y` coordinates.
	*
	* @param p The point to compare with.
	* @return `true` if the points are different, `false` otherwise
	*
	* @if DOC25D
	* @warning The `z` coordinate is not compared! As a result, two points with identical `x` and `y` coordinates
 	* are always considered equal, even with different `z` coordinates.
	* @endif
	*/
	bool operator!=(const Point2& p) const
	{
		if(coordX!=p.coordX || coordY!=p.coordY) return true;
		return false;
	}

	/**
	 * @private Get the associated triangle of the point.
	 *
	 * @return The associated triangle
	 */
	Triangle2* getIncidentTriangle() const
	{
		return pAssociatedTriangle;
	}


#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \private Set the coordinates
	*
	* @param x_,y_,z_ coordinates to assign
	*
	* @note Does not adapt customIndex and pAssociatedTriangle.
	*/
	void setCoords(const double x_,const double y_,const double z_)
	{
		coordX=x_;
		coordY=y_;
		coordZ=z_;
	}
/** \private Set the coordinates
*
* @param x_,y_,z_ coordinates to assign
* @param customIndex_ Arbitrary index, use -1 if not required
*/
	void set(const double x_,const double y_,const double z_,int customIndex_)
	{
		coordX=x_;
		coordY=y_;
		coordZ=z_;
		pAssociatedTriangle=NULL;
		customIndex=customIndex_;
#ifdef GEOM_DBG
		customIndex=getSeqNum();
#endif
	}
#else
	/** \private Set the coordinates and customIndex
	*
	* Internal method
	*
	* @param x_,y_ coordinates to assign
	* @param customIndex_ Arbitrary index, use -1 if not required
	*/
	void set(const double x_,const double y_,int customIndex_)
	{
		coordX=x_;
		coordY=y_;
		pAssociatedTriangle=NULL;
		customIndex=customIndex_;
#ifdef GEOM_DBG
		customIndex=getSeqNum();
#endif
	}
    /**
     * @private Exchange the coordinates of the point.
     *
     * @param x_,y_ coordinates to assign
     */
	void change(const double x_,const double y_)
	{
		coordX=x_;
		coordY=y_;
	}
#endif

	/** \private Set the coordiantes
	*
	* @param pnt is the point whose coordinates are assigned to the current point
	*/
#if GEOM_PSEUDO3D==GEOM_TRUE
	void set(const Point2& pnt)
	{
		pnt.xyz(coordX,coordY,coordZ);
		pAssociatedTriangle=NULL;
		this->customIndex=pnt.customIndex;
	}
#else
	void set(const Point2& pnt)
	{
		pnt.xy(coordX,coordY);
		pAssociatedTriangle=NULL;
		this->customIndex=pnt.customIndex;
	}
#endif



	/**
	* @brief Sets a custom index for the Point2.
	*
	* This method allows you to assign an **arbitrary** index to the Point2, which can be used to
	* associate it with custom data structures in your software. To retrieve the index later,
	* you can use the `getCustomIndex()` method.
	*
	* @param customIndex_ The custom index to be assigned to the Point2.
	*
	* @note This method is provided for the user's convenience to associate a `Point2` with
	* their own data structures. It is not related to the internal data structures of Fade 2D,
	* and its usage is optional. By default, the \c customIndex is set to `-1`.
	*
	* @see int getCustomIndex()
	*/
	void setCustomIndex(int customIndex_)
	{
		customIndex=customIndex_;
	}
	/**
	* @brief Retrieves the custom index of the Point2.
	*
	* @return The custom index of the Point2.
	*
	* @note The custom index defaults to `-1`. It is not related to the point's index in
	* the triangulation (since such an index does not exist), but rather an arbitrary value
	* that can be assigned by the user for associating the Point2 with their own data structures.
	*
	* @see void setCustomIndex(int customIndex_)
	*
	*/
	int getCustomIndex() const
	{
		return customIndex;
	}

	/**
	 * @private Associate a triangle with the point.
	 *
	 * This method associates a specific triangle with the Point2
	 *
	 * @param pT Pointer to the triangle to associate
	 */
	void setIncidentTriangle(Triangle2* pT)
	{
		pAssociatedTriangle=pT;
	}

/**
* @brief Create a Vector2 by subtracing another Point2 from the current one.
*
* Creates a Vector2, pointing from Point2 \p other to the current Point2.
*
* @param other The other Point2.
*
* @return The resulting Vector2 pointing from Point2 \p other to the current Point2.
*/
Vector2 operator-(const Point2& other) const
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	double xdiff(x()-other.x());
	double ydiff(y()-other.y());
	double zdiff(z()-other.z());
	return Vector2(xdiff,ydiff,zdiff);
#else
	double xdiff(x()-other.x());
	double ydiff(y()-other.y());
	return Vector2(xdiff,ydiff);
#endif


}

/**
 * @brief Add a Vector2 to the Point2.
 *
 * Adds a Vector2 `vec` to the current Point2 and returns the resulting Point2.
 *
 * @param vec The vector to be added.
 * @return The resulting Point2
 */
Point2 operator+(const Vector2& vec) const
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Point2(x()+vec.x(),y()+vec.y(),z()+vec.z());
#else
	return Point2(x()+vec.x(),y()+vec.y());
#endif
}
/**
* @brief Subtract a Vector2 from the Point2
*
 * Subtracts a Vector2 from the Point2 and returns the resulting Point2.
 *
* @param vec The vector to be subtracted.
* @return The resulting Point2
*/
Point2 operator-(const Vector2& vec) const
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	return Point2(x()-vec.x(),y()-vec.y(),z()-vec.z());
#else
	return Point2(x()-vec.x(),y()-vec.y());
#endif
}
friend std::ostream &operator<<(std::ostream &stream, const Point2& pnt);
friend std::istream &operator>>(std::istream &stream, Point2& pnt);

protected:
	friend class Dt2;///< @private
	double coordX;///< x-coordinate
	double coordY;///< y-coordinate
#if GEOM_PSEUDO3D==GEOM_TRUE
	double coordZ;///< z-coordinate
#endif
	Triangle2* pAssociatedTriangle; ///< @private Associated triangle (internal use)
	int customIndex;///< User-defined index
}; // End of class

/** @brief Print to output stream.
 *
 * Prints a Point2 to an output stream.
 *
 * @param stream The output stream.
 * @param pnt The Point2 to be printed.
 * @return The output stream.
 */
inline std::ostream &operator<<(std::ostream &stream, const Point2& pnt)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	stream << "Point2 ("<<&pnt<<",ci="<<pnt.customIndex<<"): "<<pnt.x()<<", "<<pnt.y()<<", "<<pnt.z();
	//#ifndef NDEBUG
		//stream<<", anyT="<<pnt.getIncidentTriangle();
	//#endif
#else
	stream << "Point2 ("<<&pnt<<","<<pnt.customIndex<<"): "<<pnt.x()<<", "<<pnt.y();
#endif


	return stream;
}

/** @brief Read coordinates from an input stream into a Point2.
 *
 * Reads coordinates from a stream into a Point2.
 *
 * @param stream The input stream.
 * @param pnt The Point2 to be populated with data.
 * @return The input stream.
 */
inline std::istream &operator>>(std::istream &stream, Point2& pnt)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	stream >> pnt.coordX >> pnt.coordY >> pnt.coordZ;
#else
	stream >> pnt.coordX >> pnt.coordY;
#endif
	return stream;
}

// Free functions

/** \brief Get the squared 2D distance between two points
*
 * Computes the **squared** distance between two points
 *
 * @param p0,p1 The Point2 objects whose squared distance is to be computed
 * @return The squared 2D distance between \p p0 and \p p1.
 *
* @if DOC25D
* @note This method returns the 2D distance, it does **not** consider the z-coordinate
* @endif
*/
inline
double sqDistance2D(const Point2& p0,const Point2& p1)
{
	double deltaX=p1.x()-p0.x();
	double deltaY=p1.y()-p0.y();
	return (deltaX*deltaX+deltaY*deltaY);
}


/** \brief Get the squared 2D distance between two points
*
 * Computes the **squared** distance between two points
 *
 * @param x0,y0 The coordinates of the first point
 * @param p1 The second point
 * @return The squared 2D distance between the specified points.
 *
* @if DOC25D
* @note This method returns the 2D distance, it does **not** consider the z-coordinate
* @endif
*/
inline
double sqDistance2D(const double x0,const double y0,const Point2& p1)
{
	double deltaX=p1.x()-x0;
	double deltaY=p1.y()-y0;
	return (deltaX*deltaX+deltaY*deltaY);
}

#if GEOM_PSEUDO3D==GEOM_TRUE

/** \brief Get the squared 2.5D distance between two points
*
* Computes the **squared** 2.5D distance between two points.
*
* @param p0,p1 The Point2 objects whose squared 2.5D distance is to be computed
* @return The squared 2.5D distance between \p p0 and \p p1
*/
inline
double sqDistance25D(const Point2& p0,const Point2& p1)
{
	double deltaX=p1.x()-p0.x();
	double deltaY=p1.y()-p0.y();
	double deltaZ=p1.z()-p0.z();
	double result(deltaX*deltaX+deltaY*deltaY+deltaZ*deltaZ);

	if(result!=result)
	{
		GCOUT<<"warning, sqDistance25D, nan value, no distance"<<std::endl;
		GCOUT<<"p0="<<p0<<std::endl;
		GCOUT<<"p1="<<p1<<std::endl;
	}

	return result;
}


/** \brief Get the squared 2.5D distance between two points
*
 * Computes the **squared** 2.5D distance between two points
 *
 * @param x0,y0,z0 The coordinates of the first point
 * @param p1 The second point
 * @return The squared 2.5D distance between the specified points.
 *
*/
inline
double sqDistance25D(const double x0,const double y0,const double z0,const Point2& p1)
{
	double deltaX=p1.x()-x0;
	double deltaY=p1.y()-y0;
	double deltaZ=p1.z()-z0;
	double result(deltaX*deltaX+deltaY*deltaY+deltaZ*deltaZ);

	if(result!=result)
	{
		GCOUT<<"warning, sqDistance25D, nan value, no distance"<<std::endl;
		GCOUT<<"x0="<<x0<<", y0="<<y0<<", z0="<<z0<<std::endl;
		GCOUT<<"p1="<<p1<<std::endl;
	}

	return result;

}
#endif


/**
 * @brief Compute the midpoint of points p0 and p1.
 *
 * This function calculates the midpoint between two given points, \p p0 and \p p1.

 * @param p0 First point
 * @param p1 Second point
 * @return The computed midpoint
 *
 * @note The exact midpoint of \p p0 and \p p1 may not exist in floating-point numbers
 * due to limited precision. As a result, the returned point may not be perfectly collinear
 * with \p p0 and \p p1.
 */
inline
Point2 center(const Point2& p0,const Point2& p1)
{
#if GEOM_PSEUDO3D==GEOM_TRUE
	Point2 center((p0.x()+p1.x())/2.0,(p0.y()+p1.y())/2.0,(p0.z()+p1.z())/2.0);

#else
	Point2 center((p0.x()+p1.x())/2.0,(p0.y()+p1.y())/2.0);
#endif
	return center;
}

/** \brief Compute the midpoint of p0 and p1 and adapt it
*
* Experimental new function that may change in the future. Thought
* for specific applications.
*
* This function works like center() but additionally it adapts
* the midpoint to the segment (p0,p1) such that it is 'as
* collinear as possible' with p0 and p1 in the x/y plane.
* Bounds for the shift are 0.01 and 1 % of the range in x-
* and y-direction.
*/


/**
 * @private Compute the midpoint of points p0 and p1 (with shift, deprecated)
 */
CLASS_DECLSPEC
Point2 centerWithShift(const Point2& p0,const Point2& p1);

#if GEOM_PSEUDO3D==GEOM_TRUE
/// \private Functor to sort points lexicographically
struct CLASS_DECLSPEC Func_ltPointXYZ
{
	bool operator()(const Point2& p0,const Point2& p1) const
	{
		if(p0.x()<p1.x()) return true;
		if(p0.x()>p1.x()) return false;
		if(p0.y()<p1.y()) return true;
		if(p0.y()>p1.y()) return false;
		return p0.z()<p1.z();
	}
};
/// \private Functor to compare two points in 3 dimensions
struct CLASS_DECLSPEC Func_samePointXYZ
{
	bool operator()(const Point2& p0,const Point2& p1) const
	{
		return p0.x()==p1.x() && p0.y()==p1.y() && p0.z()==p1.z();
	}
};
#endif

} // (namespace)
