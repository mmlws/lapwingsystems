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


/// @file common.h

#pragma once
#include <assert.h>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <exception>
#include "Lg.h"
#include "fadeVersion.h"

#ifdef _WIN32
	#ifdef DEVMODE
		pragma error "DEVMODE IN WIN32"
	#endif
#endif

// DLL IMPORT/EXPORT MACRO
#if defined (_WIN32)
	// ** WINDOWS **
	#if defined(FADE2D_EXPORT)
		#define  CLASS_DECLSPEC __declspec(dllexport)
		//#define  CLASS_DECLSPEC extern "C" __declspec(dllexport)
	#else
		#define  CLASS_DECLSPEC __declspec(dllimport)
	#endif
#else
	// ** LINUX / MAC **
	#define CLASS_DECLSPEC
#endif

// Development only: Performance counters
// #define GEOM_BENCHCOUNTING
#ifdef GEOM_BENCHCOUNTING
		#define BC(x) benchCounter(x);
		#define BN(x,y) benchNums(x,y);
#else
		#define BC(x)
		#define BN(x,y)
#endif


// NAMESPACE (for Doxygen)
#if GEOM_PSEUDO3D==GEOM_TRUE
/**
 * @namespace GEOM_FADE25D
 * GEOM_FADE25D is the namespace of the 2.5D version of Fade
 */
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
/**
 * @namespace GEOM_FADE2D
 * GEOM_FADE2D is the namespace of the 2D version of Fade
 */
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

// Development only: Get stack traces quickly
void goToHell(const std::string& msg);

/// \brief Enum specifying the location or area type of a zone.
enum ZoneLocation
{
	ZL_GLOBAL,
	ZL_INSIDE,
	ZL_OUTSIDE,
	ZL_GROW,
	ZL_RESULT_UNION,
	ZL_RESULT_INTERSECTION,
	ZL_RESULT_DIFFERENCE,
	ZL_RESULT_SYM_DIFFERENCE,
	ZL_BOUNDED
};

/// \brief Enum representing the orientation of three points in 2D
enum Orientation2
{
	ORIENTATION2_COLLINEAR,
	ORIENTATION2_CCW,
	ORIENTATION2_CW
};



template<class T_PTR> struct func_ltDerefPtr
{
	bool operator()(const T_PTR p0,const T_PTR p1) const {return *p0<*p1;}
};


////////////////////////////////////////////////////////////////////////
//                             Exceptions                             //
////////////////////////////////////////////////////////////////////////

/** \brief Fade Exception
 *
 * This exception is thrown when there is a bug or incorrect usage of the Fade library.
 * If you believe this is a bug, please report the issue, and it will be addressed promptly.
 */
struct FadeException: public std::exception
{
  virtual const char* what() const throw()
  {
		return "\n  FADE EXCEPTION due to a bug _or_ wrong usage of Fade. If you think it is a \n  bug then please report this incident and it will be fixed quickly: bkorn@geom.at";
  }
};

/** \brief Time Limit Exception
 *
 * This exception is thrown when an unlicensed feature is accessed. The free student version has certain limitations,
 * but it provides enough functionality for most use cases.
 */
struct FadeLicenseException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Fade2D License Exception: Your program used an unlicensed feature. Interested in a license? Contact: bkorn@geom.at\n";
	}
};


/** \brief Time Limit Exception
 *
 * This exception is thrown if a time limit specified via setGlobalTimeLimit_constraints() has been exceeded.
 */
struct FadeTimeLimitException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Fade2D Time Limit Exception: The time limit specified via setGlobalTimeLimit_constraints() has been exceeded\n";
	}
};




} // (namespace)

