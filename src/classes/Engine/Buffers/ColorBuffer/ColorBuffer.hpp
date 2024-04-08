#ifndef COLORBUFFER_HPP
#define COLORBUFFER_HPP

#include "../BufferBase.hpp"
#include <Assets/Color4b.hpp>

namespace Engine
{
    namespace Buffers
    {
        class ColorBuffer : public BufferBase<Assets::Color4b>
        {

        };
    }   
}

#endif