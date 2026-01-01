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
#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


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
	// ** LINUX / MAC / RPI **
	#define CLASS_DECLSPEC
#endif

namespace GEOM_TOOLS
{

class CLASS_DECLSPEC Log
{
public:
	static Log& get()
	{
		static Log instance;
		return instance;
	}
	void setActive(bool b)
	{
		bActive = b;
	}
	void setTarget(std::ostream& output)
	{
		target = &output;
	}
	Log& operator<<(std::ostream& (*func)(std::ostream&))
	{
		if (bActive)
		{
			func(*target);
		}
		return *this;
	}
	template <typename T>
	Log& operator<<(const T& value)
	{
		if (bActive)
		{
			*target << value;
		}
		return *this;
	}
	std::streamsize precision() const
	{
		return target->precision();
	}

private:
	Log():target(&std::cout), bActive(true) {}
	Log(const Log&);
	Log& operator=(const Log&);

	std::ostream* target;
	bool bActive;
};

} // (namespace)

#define GCOUT GEOM_TOOLS::Log::get()

