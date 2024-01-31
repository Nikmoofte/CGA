#include "ShaderProg.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include "Shader/Shader.hpp"

ShaderProg::ShaderProg(const std::string &vertexFilepath, const std::string &fragmentFilepath)
{
    id = CreateProgram(GetSourceFromFile(vertexFilepath), GetSourceFromFile(fragmentFilepath));
}

ShaderProg::ShaderProg(const ShaderProg &other)
{
    this->id = other.id;
}

ShaderProg::ShaderProg()
{

}

ShaderProg::~ShaderProg()
{
    glDeleteProgram(this->id);
}

void ShaderProg::Use() const
{
    glUseProgram(this->id);
}

void ShaderProg::UnUse() const
{
    glUseProgram(0);
}

void ShaderProg::createFromFile(const std::string &vertexFilepath, const std::string &fragmentFilepath)
{
    create(GetSourceFromFile(vertexFilepath), GetSourceFromFile(fragmentFilepath));
}


void ShaderProg::create(const std::string &vertexSource, const std::string &fragmentSource)
{
    clear();
    id = CreateProgram(vertexSource, fragmentSource);
}

void ShaderProg::attachShader(ShaderType type, const std::string &source)
{
    shaders[lastFreeSpot++] = Shader(type, source);
}

GLint ShaderProg::GetLocation(const std::string &name)
{ 
    if(UniformLocCache.find(name) != UniformLocCache.end())
        return UniformLocCache[name];

    GLint location = glGetUniformLocation(id, name.c_str());
    if(location == -1)
        std::cerr << "Uniform " + name + " not found!" << std::endl;
    else
        UniformLocCache[name] = location;
    return location;
}


unsigned ShaderProg::CreateProgram(const std::string &vertexSource, const std::string &fragmentSource)
{
    unsigned int shaderProgram = glCreateProgram();
    
    attachShader(GL_VERTEX_SHADER, vertexSource);
    attachShader(GL_FRAGMENT_SHADER, fragmentSource);

    for(size_t i = 0; i < lastFreeSpot; ++i)
        glAttachShader(shaderProgram, shaders[i].getID());


    glLinkProgram(shaderProgram);

    int result;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if(!result)
    {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &result);
        char* log = new char[result];
        glGetProgramInfoLog(shaderProgram, result, &result, log);
        std::string message("Program failed to link: \n");
        message += log;
        message += vertexSource + "\n\n";    
        message += fragmentSource + "\n\n";
        throw std::runtime_error(message);
    }
    
    return shaderProgram;
}

std::string ShaderProg::GetSourceFromFile(const std::string &filePath)
{
    std::ifstream in(filePath);
    std::stringstream sIn;
    sIn << in.rdbuf();
    return sIn.str();
}

void ShaderProg::clear()
{
    if(id != (GLuint)-1)
        glDeleteProgram(id);
    UniformLocCache.clear();
    lastFreeSpot = 0;
}
