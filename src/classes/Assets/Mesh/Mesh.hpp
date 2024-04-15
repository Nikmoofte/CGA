#ifndef MESH_HPP
#define MESH_HPP

#include <future>
#include <vector>
#include <string>

#include "Assets/Vertex.hpp"

#include <glm/mat4x4.hpp>

namespace Assets 
{
    class Mesh 
    {
        public:
		Mesh(const std::string& path);
		Mesh(const std::string& path, glm::mat4&& modelTransform);

		void Wait();
		
		std::vector<Vertex>& getVertices() { return vertices; }
		uint32_t getVerticesSize() const { return vertices.size() * sizeof(Vertex); }
		std::vector<uint32_t>& getIndecies() { return indices; }
		uint32_t getIndeciesSize() const { return indices.size(); }

		const glm::mat4& getModelMat() const { return modelTransform; }
		const glm::mat4& setModelMat(const glm::mat4& model) { return modelTransform = model; }
		
	private:
		glm::mat4 modelTransform = glm::mat4(1.f);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::future<void> loader{};

		void Load(const std::string& path);
    };
}

#endif