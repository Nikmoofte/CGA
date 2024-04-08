#include "Shader.hpp"

#include <stdexcept>
namespace Viewer
{
    Shader &Shader::operator=(Shader &&other)
    {
        Shader temp(std::move(other));
        swap(temp);
        return *this;
    }   
    GLuint Shader::create(const std::string &source)
    {
        if(type == ShaderType::NONE)
            throw std::runtime_error("shader type has not been specified");
        

        id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, NULL);
        glCompileShader(id);
        
        if(!isCompiled())
        {
            auto log = getCompileLog();
            throw std::runtime_error(
                type.toString() + 
                " id " + 
                std::to_string(id) + 
                " error log: \n" + 
                log
            );
        }
        
        return id;
    }

    GLuint Shader::createNoException(const std::string &source) noexcept
    {
        if(type != ShaderType::NONE)
        {
            id = glCreateShader(type);
            const char* src = source.c_str();
            glShaderSource(id, 1, &src, NULL);
            glCompileShader(id);
            
            if(!isCompiled())
            {
                glDeleteShader(id);
                id = -1;
            }
        }
        return id;
    }
    inline ShaderType Shader::getType()
    {
        return type;
    }

    inline void Shader::setType(ShaderType type)
    {
        this->type = type;
    }

    void Shader::swap(Shader &other)
    {
        std::swap(type, other.type);
        std::swap(id, other.id);
    }

    inline bool Shader::isCompiled() 
    {
        GLint result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        return result;
    }

    std::string Shader::getCompileLog()
    {
        GLint result;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &result);
        char* buff = new char[result];
        glGetShaderInfoLog(id, result, &result, buff);
        std::string log;
        log += buff;
        return log;
    }
}