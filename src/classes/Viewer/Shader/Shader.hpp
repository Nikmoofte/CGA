#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ShaderType.hpp"
namespace Viewer
{
    class Shader
    {

    public:
        Shader() = default;
        Shader(ShaderType type) : type(type) {}
        Shader(ShaderType type, const std::string& source) : type(type) { create(source); } 
        Shader(Shader&& other) : type(other.type), id(other.id) { other.id = -1; } 
        ~Shader() 
        { 
            glDeleteShader(id); 
        } 

        Shader& operator=(Shader&& other);

        GLuint create(const std::string& source);
        GLuint createNoException(const std::string& source) noexcept;

        inline GLuint getID() { return id; } ;
        inline ShaderType getType();
        inline void setType(ShaderType type);

        void swap(Shader& other);

    private:
        inline bool isCompiled();
        std::string getCompileLog();

        ShaderType type = ShaderType::NONE;
        GLuint id = -1;
    };
}
#endif