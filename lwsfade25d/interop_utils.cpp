#include "interop_utils.h"

std::string marshal_as_string(String^ path) {
    return msclr::interop::marshal_as<std::string>(path);
}