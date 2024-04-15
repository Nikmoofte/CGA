#include "Mesh.hpp"
#include <unordered_map>
#include "tiny_obj_loader.h"

namespace Assets
{

    Mesh::Mesh(const std::string &path)
    {
        loader = std::async(std::launch::async, &Mesh::Load, this, path);
    }

    Mesh::Mesh(const std::string &path, glm::mat4 &&modelTransform) : Mesh(path)
    {
		this->modelTransform = modelTransform;
    }

    void Mesh::Wait()
    {
        if(loader.valid())
            loader.wait();
    }

	void Mesh::Load(const std::string& path)
	{
        tinyobj::ObjReaderConfig reader_config;

		tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) 
        {
            throw std::runtime_error(reader.Error() + reader.Warning());
        }

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		auto& attrib = reader.GetAttrib();
		for (const auto& shape : reader.GetShapes())
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};
				size_t vertInd = 3 * index.vertex_index;
				vertex.position = {
					attrib.vertices[vertInd + 0],
					attrib.vertices[vertInd + 1],
					attrib.vertices[vertInd + 2]
				};

				vertInd = 3 * index.normal_index;
				if (!attrib.normals.empty())
				{
					vertex.normal = {
						attrib.normals[vertInd + 0],
						attrib.normals[vertInd + 1],
						attrib.normals[vertInd + 2]
					};
				}
				
				vertInd = 2 * index.texcoord_index;
				if (!attrib.texcoords.empty() && index.texcoord_index >= 0)
				{
					vertex.texCoords = {
						attrib.texcoords[vertInd + 0],
						1.0f - attrib.texcoords[vertInd + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}