#ifndef SHADERPROG_H
#define SHADERPROG_H

#include <glad/glad.h>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <array>

#include "Viewer/Shader/Shader.hpp"
#include "Viewer/Shader/ShaderType.hpp"

namespace Viewer
{
    class ShaderProg
    {
    public:
        ShaderProg(const std::string& vertexFilepath, const std::string& fragmentFilepath);
        ShaderProg(const ShaderProg&);
        ShaderProg();
        ~ShaderProg();

        void Use() const;
        void UnUse() const;

        void createFromFile(const std::string& vertexFilepath, const std::string& fragmentFilepath);
        void create(const std::string& vertexSource, const std::string& fragmentSource);
        void createNoexept(const std::string& vertexSource, const std::string& fragmentSource) noexcept;
        void attachShader(ShaderType type, const std::string &source);

        GLint GetLocation(const std::string& name);
    private:
        GLuint CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
        GLuint CreateProgramNoexcept(const std::string &vertexSource, const std::string &fragmentSource) noexcept;
        std::string GetSourceFromFile(const std::string& filePath);
        void clear();

        size_t lastFreeSpot = 0;
        std::array<Shader, 2> shaders;
        std::unordered_map<std::string, int> UniformLocCache;
        GLuint id = -1;
    };
}

#endif