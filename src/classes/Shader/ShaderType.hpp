#ifndef SHADERTYPE_HPP
#define SHADERTYPE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <string>

struct ShaderType
{
    enum 
    {
        NONE = -1,
        VERTEX_SHADER = GL_VERTEX_SHADER,
        FRAGMENT_SHADER = GL_FRAGMENT_SHADER
    } type;
    
    ShaderType() = default;
    ShaderType(GLenum t);
    ShaderType(const ShaderType& t) : type(t.type) {}
    ShaderType(const std::string& type) { fromString(type); }
    ShaderType& operator=(ShaderType other); 
    ShaderType& operator=(GLenum other); 

    operator GLenum() { return type; } 

    std::string toString() const;
    void fromString(const std::string&);
};

namespace std
{
    template<>
    class hash<ShaderType>
    {
    public:
        size_t operator()(const ShaderType& shaderType) const
        {
            return hash<int>()(shaderType.type);
        }
    };
};

#endif