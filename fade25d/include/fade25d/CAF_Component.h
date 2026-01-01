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

/// \file CAF_Component.h

#pragma once
#include <map>
#include "common.h"
#include "Segment2.h"



// Only defined for 2.5D
#if GEOM_PSEUDO3D==GEOM_TRUE
namespace GEOM_FADE25D {


/** \enum CAFTYP
 *
 * Enum representing the possible Cut-and-Fill (CAF) types for a component.
 * - CT_NULL: The first surface corresponds to the second one.
 * - CT_CUT: The first surface is above the second one (earth needs to be cut).
 * - CT_FILL: The first surface is below the second one (earth needs to be filled).
 */
enum CAFTYP
{
	CT_NULL,///< the first surface corresponds to the second one
	CT_CUT,	///< the first surface is above the second one
	CT_FILL ///< the first surface is below the second one
};

class Triangle2;
class Point2;
class Zone2;



/** \brief CAF_Component represents a connected area of the surface.
 *
 * A CAF_Component is a region of the surface where:
 * - The first surface is below the second one (CAFTYP = CT_FILL),
 * - The first surface is above the second one (CAFTYP = CT_CUT),
 * - The first surface corresponds exactly to the second one (CAFTYP = CT_NULL).
 *
 * The component can be either a cut, fill, or a null (no change) area depending
 * on the relationship between the two surfaces.
 */
class CLASS_DECLSPEC CAF_Component
{
public:
	/** \brief Constructor
	* @internal
	*/
	CAF_Component(
		std::vector<Triangle2*>& vT_,
		std::map<Point2*,std::pair<double,double> >& mVtx2BeforeAfter_,
		int label_
		);
	/** \brief Destructor
	* @internal
	*/
	~CAF_Component();

/** \brief Get Cut&Fill-Type
 *
 * \return The CAFTYP (Cut&Fill type) of the component:
 * - `CT_CUT` indicates that earth must be dug off to transform the first surface into the second one.
 * - `CT_FILL` indicates that earth must be added to fill the difference between the two surfaces.
 * - `CT_NULL` is returned when the first surface corresponds exactly to the second one (no difference).
 */
	CAFTYP getCAFType() const;


/** \brief Get the volume of the component.
 *
 * This function computes the volume of the component. The volume is an absolute
 * value. Its sign (cut or fill) can be determined using the `getCAFType()` function.
 *
 * \return The absolute volume of the component.
 *
 * \note The volume is computed using unitless coordinates. Ensure that the
 * x, y, and z-coordinates are consistent (in the same unit system) when using this method.
 */
	double getVolume() const;

/** \brief Get the label of the component.
*
* Each component is assigned a unique label (an integer).
*
* \return The label of the component.
*/
	int getLabel() const;

/** \brief Get the triangles of the component.
*
* This function returns the triangles that define the component.
*
* \param [out] vTrianglesOut A vector to store Triangle2 pointers that make up the component.
*
* @note  The z-coordinates of the returned triangle corners represent the
* height differences between the two input surfaces.
*/
	void getTriangles(std::vector<Triangle2*>& vTrianglesOut) const;

/** \brief Get the border segments of the component.
 *
 * This function returns the border segments of the component. The border is represented
 * as a collection of segments in arbitrary order.
 *
 * \param [out] vBorderSegments A vector to store the border segments of the component.
 */
	void getBorder(std::vector<Segment2>& vBorderSegments) const;

private:
	CAF_Component(const CAF_Component&);
	CAF_Component& operator=(const CAF_Component&);
protected:
	/// @private
	void init(std::map<Point2*,std::pair<double,double> >& mVtx2BeforeAfter);
	/// @private
	void showGeomview(const char* name,std::vector<Point2>* pvBeforeT, std::vector<Point2>* pvAfterT, std::vector<Point2>* pvWallT) const;
	/// @private
	void setVolume(std::vector<Point2>* pvBeforeT, std::vector<Point2>* pvAfterT, std::vector<Point2>* pvWallT);
	std::vector<Triangle2*>* pVT;
	CAFTYP caftype;
	double volume;
	int label;
};



/** \brief Output stream operator for CAF_Component.
 *
 * This operator allows printing the component information (label, type, and volume) to the output stream.
 *
 * \param stream The output stream
 * \param c The CAF_Component object to be printed.
 * \return The output stream
 */
inline std::ostream &operator<<(std::ostream &stream, const CAF_Component& c)
{
	stream<<"Component "<<c.getLabel();
	switch(c.getCAFType())
	{
		case CT_NULL: stream<<", Type: NULL";break;
		case CT_CUT:  stream<<", Type: CUT ";break;
		case CT_FILL: stream<<", Type: FILL";break;
	}
	stream<<", Volume: "<<c.getVolume();
	return stream;
}


} // (namespace)


#elif GEOM_PSEUDO3D==GEOM_FALSE
#else
	#error GEOM_PSEUDO3D is not defined
#endif
