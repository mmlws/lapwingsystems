#pragma once
#include <string>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace Runtime::InteropServices;

std::string marshal_as_string(String^ path);


