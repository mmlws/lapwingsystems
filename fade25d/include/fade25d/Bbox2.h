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
/// \file Bbox2.h

#pragma once
#include "Segment2.h"
#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class GeomTest; // FWD

/**
 * @brief A 2D axis-aligned bounding box.
 *
 * The `Bbox2` class represents a 2D axis-aligned bounding box with minimum and maximum
 * coordinates in the X and Y directions.
 */
class CLASS_DECLSPEC Bbox2
{
public:
/**
* @brief Default constructor
*
* Initializes the bounding box with `minX` and `minY` set to `DBL_MAX` and `maxX` and `maxY`
* set to `-DBL_MAX`.
*/
	explicit Bbox2(GeomTest* pGeomTest_=NULL):
		minX(DBL_MAX),minY(DBL_MAX),
		maxX(-DBL_MAX),maxY(-DBL_MAX),
		maxAbsCoord(0),
		bValid(false),pGeomTest(pGeomTest_)
	{
	}
	~Bbox2();


	/**
	 * @brief Resets the bounds.
	 *
	 * Resets the bounding box to default values.
	 */
	void reset()
	{
		minX=DBL_MAX;
		minY=DBL_MAX;
		maxX=-DBL_MAX;
		maxY=-DBL_MAX;
		bValid=false;
	}

	/**
	 * @brief Checks if the bounds are valid.
	 *
	 * The bounds are valid when at least one point has been added or
	 * when set-methods have been used to set `minX <= maxX` and `minY <= maxY`.
	 */
	bool isValid() const
	{
		return minX<DBL_MAX;
	}

	/**
	 * @brief Retrieves the corners of the bounding box.
	 *
	 * Convenience function: Returns the 4 corners of the box.
	 *
	 * @param vBoxCorners A vector to store the 4 corners of the box.
	 */
	void getCorners(std::vector<Point2>& vBoxCorners) const;

	/**
	 * @brief Retrieves the boundary of the bounding box.
	 *
	 * Convenience function: Returns 4 border segments.
	 *
	 * @param vBoundary A vector to store the 4 boundary segments.
	 */
	void getBoundary(std::vector<Segment2>& vBoundary) const;

	/**
	 * @brief Retrieves the offset corners of the bounding box.
	 *
	 * Returns the 4 corners of an enlarged box. The box is enlarged by `offset` in each direction.
	 *
	 * @param offset The enlargement value for each direction.
	 * @param vBoxCorners A vector to store the 4 offset corners of the box.
	 */
	void getOffsetCorners(double offset,std::vector<Point2>& vBoxCorners) const;


	/**
	 * @brief Checks if this bounding box intersects with another.
	 *
	 * Checks if this bounding box intersects the Bbox2 \p other.
	 *
	 * @param other The other bounding box to check for intersection.
	 * @return `true` if the boxes intersect, `false` otherwise.
	 */
	bool doIntersect(const Bbox2& other) const;


	/**
	 * @brief Adds points to the bounding box.
	 *
	 * Adds points, extends the 2D bounding box if required.
	 *
	 * @param start_it,end_it Iterator range containing the points to add
	 * @return `true` if the bounding box changes, `false` otherwise.
	 */
	bool add(std::vector<Point2*>::const_iterator start_it,std::vector<Point2*>::const_iterator end_it)
	{
		if(start_it==end_it) return false;
		if(bValid)
		{
			double oldMinX(minX),oldMinY(minY),oldMaxX(maxX),oldMaxY(maxY);
			for(;start_it!=end_it;++start_it) treatPointForValidBox(**start_it);
			updateMaxAbs();
			if(oldMinX!=minX || oldMinY!=minY || oldMaxX!=maxX || oldMaxY!=maxY ) return true;
			return false;
		}
		else
		{
			treatPointForInvalidBox(**start_it);
			++start_it;
			for(;start_it!=end_it;++start_it) treatPointForValidBox(**start_it);
			updateMaxAbs();
			return true;
		}

	}

	/**
	 * @brief Adds points to the bounding box.
	 *
	 * Extends the 2D bounding box if required.
	 *
	 * @param start_it An iterator pointing to the first point to add.
	 * @param end_it An iterator pointing to one past the last point to add.
	 * @return `true` if the bounding box changes, `false` otherwise.
	 */
	bool add(std::vector<Point2>::const_iterator start_it,std::vector<Point2>::const_iterator end_it)
	{
		if(start_it==end_it) return false;
		if(bValid)
		{
			double oldMinX(minX),oldMinY(minY),oldMaxX(maxX),oldMaxY(maxY);
			for(;start_it!=end_it;++start_it) treatPointForValidBox(*start_it);
			updateMaxAbs();
			if(oldMinX!=minX || oldMinY!=minY || oldMaxX!=maxX || oldMaxY!=maxY ) return true;
			return false;
		}
		else
		{
			treatPointForInvalidBox(*start_it);
			++start_it;
			for(;start_it!=end_it;++start_it) treatPointForValidBox(*start_it);
			updateMaxAbs();
			return true;
		}
	}

	/**
	 * @brief Adds points to the bounding box.
	 *
	 * Extends the 2D bounding box if required.
	 *
	 * @param numPoints The number of points to add.
	 * @param coordinates A pointer to an array of coordinates (x, y).
	 * @return `true` if the bounding box changes, `false` otherwise.
	 */
	bool add(size_t numPoints,double * coordinates)
	{
#if GEOM_PSEUDO3D==GEOM_TRUE
		const int NUMCOMPONENTS(3);
#else
		const int NUMCOMPONENTS(2);
#endif

		if(numPoints==0) return false;
		double oldMinX(minX),oldMinY(minY),oldMaxX(maxX),oldMaxY(maxY);
		double firstX(coordinates[0]);
		double firstY(coordinates[1]);
		if(firstX<minX) minX=firstX;
		if(firstX>maxX) maxX=firstX;
		if(firstY<minY) minY=firstY;
		if(firstY>maxY) maxY=firstY;

		for(size_t i=0;i<numPoints;++i)
		{
			double x(coordinates[NUMCOMPONENTS*i]);
			double y(coordinates[NUMCOMPONENTS*i+1]);
			if(x<minX) minX=x;
				else if(x>maxX) maxX=x;
			if(y<minY) minY=y;
				else if(y>maxY) maxY=y;
		}
		bValid=true;
		updateMaxAbs();
		if(oldMinX!=minX || oldMinY!=minY || oldMaxX!=maxX || oldMaxY!=maxY ) return true;
			else return false;
	}

	/**
	 * @brief Adds a single point to the bounding box.
	 *
	 * Extends the 2D bounding box if required.
	 *
	 * @param p The point to add.
	 * @return `true` if the bounding box changes, `false` otherwise.
	 */
	bool add(const Point2& p)
	{
		//GCOUT<<"Add point: "<<p<<std::endl;
		if(bValid)
		{
			double oldMinX(minX),oldMinY(minY),oldMaxX(maxX),oldMaxY(maxY);
			treatPointForValidBox(p);
			updateMaxAbs();
			if(oldMinX!=minX || oldMinY!=minY || oldMaxX!=maxX || oldMaxY!=maxY ) return true;
				else return false;
		}
		else
		{
			treatPointForInvalidBox(p);
			updateMaxAbs();
			return true;
		}
	}

	/**
	 * @brief Adds another bounding box to this one.
	 *
	 * Extends the 2D bounding box if required.
	 *
	 * @param other The bounding box to add.
	 * @return `true` if the bounding box changes, `false` otherwise.
	 */	bool add(const Bbox2& other)
	{
		bool bRet(false);
		if(other.minX<minX) {minX=other.minX;bRet=true;}
		if(other.minY<minY) {minY=other.minY;bRet=true;}
		if(other.maxX>maxX) {maxX=other.maxX;bRet=true;}
		if(other.maxY>maxY) {maxY=other.maxY;bRet=true;}
		updateMaxAbs();
		return bRet;
	}

	/**
	 * @brief Checks if a point is inside the bounding box.
	 *
	 * Checks if the query point \p p is inside the box.
	 *
	 * @param p The point to check.
	 * @return `true` if the point is inside the box, `false` otherwise.
	 */
	bool isInBox(const Point2& p) const;


	/**
	 * @brief Computes the 2D midpoint of the bounding box.
	 *
	 * Computes the center of the bounding box.
	 *
	 * @return The center point of the bounding box.
	 */
	Point2 computeCenter() const;

	/**
	 * @brief Adds another bounding box to this one.
	 *
	 * Extends the 2D bounding box and returns the resulting bounding box.
	 *
	 * @param b The bounding box to add.
	 * @return The resulting bounding box after the addition.
	 */
	Bbox2 operator+(const Bbox2& b);


	/**
	 * @brief Retrieves the minimum corner point of the bounding box.
	 *
	 * Retrieves the minimum corner point of the bounding box.
	 *
	 * @return The 2D corner point with the minimum coordinates.
	 */
	Point2 getMinPoint() const
	{
#if GEOM_PSEUDO3D==GEOM_TRUE
		return Point2(minX,minY,0);
#else
		return Point2(minX,minY);
#endif
	}


	/**
	 * @brief Retrieves the maximum corner point of the bounding box.
	 *
	 * @return The 2D corner point with the maximum coordinates.
	 */

	Point2 getMaxPoint() const
	{
#if GEOM_PSEUDO3D==GEOM_TRUE
		return Point2(maxX,maxY,0);
#else
		return Point2(maxX,maxY);
#endif
	}

	/**
	 * @brief Retrieves the smallest coordinate value.
	 *
	 * @return The minimum of `minX` and `minY`.
	 */

	double getMinCoord() const
	{
		return (std::min)(minX,minY);
	}

	/**
	 * @brief Retrieves the largest coordinate value.
	 *
	 * @return The maximum of `maxX` and `maxY`.
	 */
	double getMaxCoord() const
	{
		return (std::max)(maxX,maxY);
	}


	/**
	 * @brief Retrieves the X-range of the bounding box.
	 *
	 * @return The difference between `maxX` and `minX`.
	 */
	double getRangeX() const
	{
		return maxX-minX;
	}

	/**
	 * @brief Retrieves the Y-range of the bounding box.
	 *
	 * @return The difference between `maxY` and `minY`.
	 */
	double getRangeY() const
	{
		return maxY-minY;
	}

	/**
	 * @brief Retrieves the largest range of the bounding box.
	 *
	 * @return The larger of the X-range and Y-range.
	 */
	double getMaxRange() const
	{
		double range0=getRangeX();
		double range1=getRangeY();
		if(range0>range1) return range0;
		return range1;
	}

	/**
	 * @brief Retrieves the `minX` value.
	 *
	 * @return The `minX` coordinate.
	 */
	double get_minX() const {return minX;}
	/**
     * @brief Retrieves the `minY` value.
     *
     * @return The `minY` coordinate.
     */
	double get_minY() const {return minY;}

	/**
	 * @brief Retrieves the `maxX` value.
	 *
	 * @return The `maxX` coordinate.
	 */
	double get_maxX() const {return maxX;}

	/**
	 * @brief Retrieves the `maxY` value.
	 *
	 * @return The `maxY` coordinate.
	 */
	double get_maxY() const {return maxY;}

	/**
	 * @brief Retrieves the bounding box coordinates.
	 *
	 * Retrieves the bounding box coordinates.
	 *
	 * @param minX_ A reference to store the `minX` value.
	 * @param maxX_ A reference to store the `maxX` value.
	 * @param minY_ A reference to store the `minY` value.
	 * @param maxY_ A reference to store the `maxY` value.
	 */
	void getBounds(double& minX_,double& maxX_,double& minY_,double& maxY_) const;



	/**
	 * @brief Doubles the size of the bounding box.
	 *
	 * Changes the bounds such that the box grows in each direction by half
	 * the previous range.
	 */
	void doubleTheBox();

	/**
	 * @brief Sets the `minX` value.
	 *
	 * @param val The new value for `minX`.
	 */
	void setMinX(double val)
	{
		minX=val;
		updateMaxAbs();
		if(minX<=maxX && minY<=maxY) bValid=true;
	}

	/**
	 * @brief Sets the `maxX` value.
	 *
	 * @param val The new value for `maxX`.
	 */
	void setMaxX(double val)
	{
		maxX=val;
		updateMaxAbs();
		if(minX<=maxX && minY<=maxY) bValid=true;
	}

	/**
	 * @brief Sets the `minY` value.
	 *
	 * @param val The new value for `minY`.
	 */
	void setMinY(double val)
	{
		minY=val;
		updateMaxAbs();
		if(minX<=maxX && minY<=maxY) bValid=true;
	}

	/**
	 * @brief Sets the `maxY` value.
	 *
	 * @param val The new value for `maxY`.
	 */
	void setMaxY(double val)
	{
		maxY=val;
		updateMaxAbs();
		if(minX<=maxX && minY<=maxY) bValid=true;
	}


	/**
	 * @brief Enlarges the bounding box's ranges symmetrically.
	 *
	 * This method updates `minX`, `minY`, `maxX`, and `maxY` symmetrically
	 * to enlarge the X- and Y-ranges of the bounding box.
	 *
	 * @param factor The factor by which the existing ranges `rangeX` and `rangeY` shall grow.
	 * @param bUseMaxRange If `true`, a square bounding box with side length
	 *        `factor * max(rangeX, rangeY)` is created.
	 * @param minRange The minimum allowable range to avoid a degenerate box.
	 */
	void enlargeRanges(double factor,bool bUseMaxRange,double minRange);

	/**
	 * @brief Inflates the bounding box if degenerate.
	 *
	 * If the bounding box is degenerate (i.e., it only contains one point or
	 * has identical `x` and/or `y` coordinates), this method inflates the box by adding
	 * a value (`val`) to `maxX` and/or `maxY`.
	 *
	 * @param val The value to inflate the bounding box.
	 */
	void inflateIfDegenerate(double val)
	{
		if(bValid)
		{
			if(minX==maxX) maxX+=val;
			if(minY==maxY) maxY+=val;
		}
		updateMaxAbs();
	}
protected:
	inline void treatPointForValidBox(const Point2& p)
	{
		double x,y;
		p.xy(x,y);
		if(x<minX) minX=x;
			else if(x>maxX) maxX=x;
		if(y<minY) minY=y;
			else if(y>maxY) maxY=y;
		updateMaxAbs();
	}
	inline void treatPointForInvalidBox(const Point2& p)
	{
		// Individual bounds may have been set already. Keep them!
		if(minX==DBL_MAX) minX=p.x();
		if(minY==DBL_MAX) minY=p.y();
		if(maxX==-DBL_MAX) maxX=p.x();
		if(maxY==-DBL_MAX) maxY=p.y();
		updateMaxAbs();
		bValid=true;
	}
	friend std::ostream &operator<<(std::ostream &stream, const Bbox2& pC);
protected:
	double minX,minY;
	double maxX,maxY;
	double maxAbsCoord;
	bool bValid;
	GeomTest* pGeomTest;
	bool updateMaxAbs();
	friend Bbox2 intersection(const Bbox2& a,const Bbox2& b);
};

/**
 * @brief Computes the bounding box of a set of points.
 *
 * @param vP A vector of `Point2` objects.
 * @return The bounding box that contains all points.
 */
Bbox2 getBox(std::vector<Point2>& vP);

/**
 * @brief Computes the bounding box of a set of points.
 *
 * @param vP A vector of `Point2` pointers.
 * @return The bounding box that contains all points.
 */
 Bbox2 getBox(std::vector<Point2*>& vP);

 /**
 * @brief Computes the intersection of two bounding boxes.
 *
 * This function calculates the intersection of two 2D axis-aligned bounding boxes, `a` and `b`.
 * The intersection is the region where both bounding boxes overlap. If there is no overlap,
 * the resulting bounding box is marked as invalid (`bValid = false`).
 *
 * @param a The first bounding box.
 * @param b The second bounding box.
 * @return A `Bbox2` object representing the intersection of `a` and `b`.
 *         If the boxes do not intersect, the resulting bounding box is invalid.
 */
inline Bbox2 intersection(const Bbox2& a, const Bbox2& b)
{
	Bbox2 ret(a);
	if(b.minX > ret.minX) ret.minX=b.minX;
	if(b.maxX < ret.maxX) ret.maxX=b.maxX;
	if(b.minY > ret.minY) ret.minY=b.minY;
	if(b.maxY < ret.maxY) ret.maxY=b.maxY;
	if( (ret.minX>ret.maxX) || (ret.minY>ret.maxY) )
	{
		ret.bValid=false;
		ret.minX=DBL_MAX;
		ret.minY=DBL_MAX;
		ret.maxX=-DBL_MAX;
		ret.maxY=-DBL_MAX;
	}
	else
	{
		ret.bValid = true;
	}
	return ret;
}


/**
 * @brief Prints the bounding box to a stream.
 *
 * @param stream The output stream
 * @param pC The bounding box to print.
 * @return The output stream
 */
inline std::ostream &operator<<(std::ostream &stream, const Bbox2& pC)
{
	stream<<"Bbox2: xy("<<pC.minX<<","<<pC.minY<<") -> xy("<<pC.maxX<<","<<pC.maxY<<"), rangeX="<<pC.getRangeX()<<", rangeY="<<pC.getRangeY();
	return stream;
}



} // (namespace)
