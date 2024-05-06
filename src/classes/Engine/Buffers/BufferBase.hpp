#ifndef BUFFERBASE_HPP
#define BUFFERBASE_HPP

#include <vector>
#include <algorithm>
#include <execution>
namespace Engine
{
    namespace Buffers
    {
        template<class T>
        class BufferBase
        {
        public:
            BufferBase(size_t size) : buffer(size)
            {

            }
            virtual void clear(T&& clearVal = T{})
            {
                std::fill(std::execution::par, buffer.begin(), buffer.end(), clearVal);
            }
            size_t size()
            {
                return buffer.size();
            }
            void resize(size_t newSize)
            {
                return buffer.resize(newSize);
            }
            T& get(size_t index)
            {
                return buffer[index];
            }
            T* data() 
            {
                return reinterpret_cast<T*>(buffer.data());
            }
        protected:
            std::vector<T> buffer;
        };
    }
}

#endif