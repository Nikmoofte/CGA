#ifndef COLORBUFFER_HPP
#define COLORBUFFER_HPP

#include "../BufferBase.hpp"
#include <Assets/Color4b.hpp>
#include <glm/glm.hpp>


namespace Engine
{
    namespace Buffers
    {
        class ColorBuffer : public BufferBase<Assets::Color4b>
        {
        public:
            ColorBuffer(glm::ivec2 screenDimentions) : BufferBase<Assets::Color4b>(screenDimentions.x * screenDimentions.y), screenDimentions(screenDimentions) {}
            const Assets::Color4b& get(glm::ivec2 pos) const
            {
                return buffer[pos.y * screenDimentions.x + pos.x];
            }
            Assets::Color4b& get(glm::ivec2 pos)
            {
                return buffer[pos.y * screenDimentions.x + pos.x];
            }
            void set(glm::ivec2 pos, Assets::Color4b val)
            {
                buffer[pos.y * screenDimentions.x + pos.x] = val;
            }
            void set(glm::ivec2 pos, int triangleIndex)
            {
                //well i need to store int:/ sizeof(int) == sizeof(Assets::Color4b)
                buffer[pos.y * screenDimentions.x + pos.x] = *(Assets::Color4b*)&triangleIndex;
            }

        private:
            glm::ivec2 screenDimentions{};
        };
    }   
}

#endif