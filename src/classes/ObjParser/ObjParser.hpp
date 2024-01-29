#ifndef OBJPARCER_HPP
#define OBJPARCER_HPP

#include <string>

#include "Object/Object.hpp"

class ObjParser
{
public:
    ObjParser() = default;
    Object operator()(const std::string& pathToFile);
};

#endif