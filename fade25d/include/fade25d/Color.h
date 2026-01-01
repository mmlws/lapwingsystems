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


/// \file Color.h


#pragma once

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

/** \brief Predefined colors for convenience
*/
enum Colorname
{
	CRED,         /**< Red: (1.0, 0.0, 0.0) */
	CGREEN,       /**< Green: (0.0, 1.0, 0.0) */
	CBLUE,        /**< Blue: (0.0, 0.0, 1.0) */
	CBLACK,       /**< Black: (0.0, 0.0, 0.0) */
	CYELLOW,      /**< Yellow: (1.0, 1.0, 0.0) */
	CPINK,        /**< Pink: (1.0, 0.4, 0.7) */
	CGRAY,        /**< Gray: (0.5, 0.5, 0.5) */
	CWHITE,       /**< White: (1.0, 1.0, 1.0) */
	CORANGE,      /**< Orange: (1.0, 0.627, 0.0) */
	CLIGHTBLUE,   /**< Light Blue: (0.0, 0.749, 1.0) */
	CLIGHTBROWN,  /**< Light Brown: (0.823, 0.705, 0.549) */
	CDARKBROWN,   /**< Dark Brown: (0.545, 0.271, 0.074) */
	CPURPLE,      /**< Purple: (0.627, 0.125, 0.941) */
	COLIVE,       /**< Olive: (0.5, 0.5, 0.0) */
	CLAWNGREEN,   /**< Clawngreen: (0.49, 0.99, 0.0) */
	CPALEGREEN,   /**< Pale Green: (0.60, 0.98, 0.6) */
	CCYAN,        /**< Cyan: (0.0, 1.0, 1.0) */
	CTRANSPARENT  /**< Transparent: (-1.0, -1.0, -1.0, no drawing) */
};

/** \brief Color for visualization
 *
 * This class represents a color for Postscript and PDF visualization used
 * together with Visualizer2.
 *
 * @see Visualizer2
 */
class CLASS_DECLSPEC Color
{
public:
	/**
	* @param [in] r, g, b Red, green, blue components of the color (0.0 to 1.0).
	* @param width Line width for visualization (default: 0.001).
	* @param bFill If `true`, objects with an area (e.g., `Triangle2`, `Circle2`, `VoroCell2`) will be filled with the specified color while x-marks will be placed at the endpoints of line segments.
	*              If `false`, the objects will not be filled, and line segments will have x-marks at their endpoints.
	*/
	Color(double r,double g,double b,double width,bool bFill=false);

	/**
	* @param[in]  c Predefined color name
	* @param width Line width for visualization (default: 0.001).
	* @param bFill If `true`, objects with an area (e.g., `Triangle2`, `Circle2`, `VoroCell2`) will be filled with the specified color while x-marks will be placed at the endpoints of line segments.
	*              If `false`, the objects will not be filled, and line segments will have x-marks at their endpoints.
	*/
	explicit Color(Colorname c,float width=0.001,bool bFill=false);

	/** Default constructor for black color (no fill). */
	Color();

	bool operator<(const Color& other) const;
	bool operator!=(const Color& other) const;
	bool operator==(const Color& other) const;
	float r; ///< Red component
	float g; ///< Green component
	float b; ///< Blue component
	float width; ///< Line width
	bool bFill; ///< Fill the shape or not

	/**
	* \brief Stream output for color information.
	*
	* This function outputs the color information (red, green, blue, linewidth, and fill status)
	* of a `Color` object to the provided output stream.
	*
	* \param[in] stream The output stream to which the color information will be printed.
	* \param[in] c The `Color` object whose details are to be printed.
	*
	* \return The output stream with the color information appended.
	*/
	friend std::ostream &operator<<(std::ostream &stream, const Color& c);
	/** \brief Get the next available predefined color name.
	 *
	 * This function returns the next color name in the enumeration.
	 */
	static Colorname getNextColorName();
};

inline std::ostream &operator<<(std::ostream &stream, const Color& c)
{
	stream<<"Color (r,g,b): "<<c.r<<","<<c.g<<","<<c.b<<", linewidth="<<c.width<<", fill="<<c.bFill;
	return stream;
}

} // (namespace)
