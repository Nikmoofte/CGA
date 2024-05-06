#ifndef DEPTHBUFFER_HPP
#define DEPTHBUFFER_HPP

#include  "../BufferBase.hpp"
#include <glm/glm.hpp>

namespace Engine
{
    namespace Buffers
    {
        class DepthBuffer : public BufferBase<float>
        {
        public:
            DepthBuffer(glm::ivec2 screenDimentions) : BufferBase<float>(screenDimentions.x * screenDimentions.y), screenDimentions(screenDimentions) {}

            inline float get(glm::ivec2 pos) const
            {
                return buffer[pos.y * screenDimentions.x + pos.x];
            }
            inline void set(glm::ivec2 pos, float val)
            {
                buffer[pos.y * screenDimentions.x + pos.x] = val;
            }
        private:
            glm::ivec2 screenDimentions{};
        };
    }
}

#endif