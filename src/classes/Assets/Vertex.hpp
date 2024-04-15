#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

namespace Assets
{
    class Vertex 
    {
    public:
        // Projected position in homogenous space for clipping
		glm::vec4 projectedPosition{};

		// Index with relative position to all vertices buffer
		uint32_t id;

		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 texCoords{};

		void perspectiveDivide()
		{
			auto invW = 1.0f / projectedPosition.w;
			projectedPosition *= invW;
		}

		bool operator==(const Vertex& other) const
		{
			return position == other.position &&
				   normal == other.normal &&
				   texCoords == other.texCoords;
		}
    };
}

namespace std
{
	template<> struct hash<Assets::Vertex>
	{
		size_t operator()(Assets::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^
					(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
					(hash<glm::vec2>()(vertex.texCoords) << 1);
		}
	};
}

#endif
