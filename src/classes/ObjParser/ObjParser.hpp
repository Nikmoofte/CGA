#ifndef OBJPARCER_HPP
#define OBJPARCER_HPP

#include <string>

#include "Object/Object.hpp"
#include "BS_thread_pool.hpp"

class ObjParser
{
public:
    ObjParser() = default;
    Object operator()(const std::string& pathToFile);
    Object operator()(const std::string& pathToFile, BS::thread_pool& threads);
};

#endif