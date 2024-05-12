#ifndef MESH_HPP
#define MESH_HPP

#include <future>
#include <vector>
#include <string>

#include "Assets/Vertex.hpp"

#include <glm/mat4x4.hpp>

namespace Engine
{
	class Scene;
}

namespace Assets 
{
    class Mesh 
    {
        public:
		Mesh(const std::string& path, Engine::Scene& scene);

		void Wait();
		
		std::vector<Vertex>& getVertices() { return vertices; }
		uint32_t getVerticesSize() const { return vertices.size(); }
		std::vector<uint32_t>& getIndecies() { return indices; }
		uint32_t getIndeciesSize() const { return indices.size(); }
		
		//Last index in indicies for    material
		std::vector<std::pair<uint32_t, uint32_t>> lastMaterialIndex;
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::future<void> loader{};

		void Load(const std::string& path, Engine::Scene& scene);
    };
}

#endif