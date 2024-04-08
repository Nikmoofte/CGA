#include "ShaderType.hpp"

#include <unordered_map>
using namespace std;

namespace Viewer
{
    unordered_map<string, decltype(ShaderType::type)> stringToType
    {
        {"vertex", ShaderType::VERTEX_SHADER},
        {"fragment", ShaderType::FRAGMENT_SHADER}
    };

    ShaderType::ShaderType(GLenum t)
    {
        type = static_cast<decltype(type)>(t);
    }

    ShaderType &ShaderType::operator=(ShaderType other)
    {
        type = other.type;
        return *this;
    }

    ShaderType &ShaderType::operator=(GLenum other)
    {
        type = static_cast<decltype(type)>(other);
        return *this;
    }

    std::string ShaderType::toString() const
    {
        std::string result = "none";

        switch (type)
        {
        case VERTEX_SHADER:
            result =  "vertex shader";
            break;
        case FRAGMENT_SHADER:
            result =  "fragment shader";
            break;
        case NONE:
        default:
            break;  
        };

        return result;
    }

    void ShaderType::fromString(const std::string &type)
    {
        this->type = stringToType[type];
    }
}