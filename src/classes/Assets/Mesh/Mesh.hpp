#ifndef MESH_HPP
#define MESH_HPP

#include <future>
#include <vector>
#include <string>

#include "Assets/Vertex.hpp"

namespace Assets 
{
    class Mesh 
    {
        public:
		Mesh(const std::string& path);
		Mesh(const std::string& path, glm::mat4&& modelTransform);

		void Wait();

		std::vector<Vertex>& GetVertices() { return vertices; }
		uint32_t GetVerticesSize() const { return vertices.size(); }
		const std::vector<uint32_t>& GetIndecies() const { return indices; }
		uint32_t GetIndeciesSize() const { return indices.size(); }

		const glm::mat4& GetModelTransform() const { return modelTransform; }
		void SetModelTransform(const glm::mat4& model) { modelTransform = model; }

	private:
		glm::mat4 modelTransform = glm::mat4(1.f);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::future<void> loader{};

		constexpr static char* pathToMaterinals = "./";

		void Load(const std::string& path);
    };
}

#endif