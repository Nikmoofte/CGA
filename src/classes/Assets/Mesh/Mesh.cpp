#include "Mesh.hpp"
#include <unordered_map>
#include "tiny_obj_loader.h"

#include "Engine/Scene/Scene.hpp"

namespace Assets
{

    Mesh::Mesh(const std::string &path, ::Engine::Scene& scene)
    {
        loader = std::async(std::launch::async, &Mesh::Load, this, path, std::ref(scene));
    }

    void Mesh::Wait()
    {
        if(loader.valid())
            loader.wait();
    }

	void Mesh::Load(const std::string& path, ::Engine::Scene& scene)
	{
        tinyobj::ObjReaderConfig reader_config;

		tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) 
        {
            throw std::runtime_error(reader.Error() + reader.Warning());
        }

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		auto& attrib = reader.GetAttrib();
		auto& materials = reader.GetMaterials();
		for(auto & material : materials)
		{
			scene.addMaterial(material);
		}
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
			lastMaterialIndex.push_back(indices.size());
		}
	}
}