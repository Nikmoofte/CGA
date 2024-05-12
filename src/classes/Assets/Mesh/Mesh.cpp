#include "Mesh.hpp"
#include <unordered_map>
#include <queue>
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
		std::vector<int> materialIndices;
		for(auto & material : materials)
		{
			materialIndices.push_back(scene.addMaterial(material));
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
			lastMaterialIndex.emplace_back(indices.size(), materialIndices.empty() ? 0 : materialIndices[shape.mesh.material_ids[0]]);
		}

		for(int i = 0; i < indices.size(); i += 3)
		{
			Vertex& v0 = vertices[indices[i + 0]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];

			glm::vec3 Edge1 = v1.position - v0.position;
			glm::vec3 Edge2 = v2.position - v0.position;

			float DeltaU1 = v1.texCoords.x - v0.texCoords.x;
			float DeltaV1 = v1.texCoords.y - v0.texCoords.y;
			float DeltaU2 = v2.texCoords.x - v0.texCoords.x;
			float DeltaV2 = v2.texCoords.y - v0.texCoords.y;

			float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			glm::vec3 Tangent;

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

			v0.tangent += Tangent;
			v1.tangent += Tangent;
			v2.tangent += Tangent;
		}
		for(auto& vert : vertices)
			vert.tangent = glm::normalize(vert.tangent);
	}
}