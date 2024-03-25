#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include <filesystem>

#include <glm/glm.hpp>

namespace Loader
{
	class SceneBase
	{
	public:
		virtual ~SceneBase() {};
		virtual int addMesh(const std::string& path, glm::mat4&&) = 0;
		virtual int addLight(const glm::vec3& pos, const glm::vec3& color) = 0;
		virtual int addCamera(const glm::vec3 &pos, const glm::vec3 &front, float fov, glm::ivec2 screenDimentions) = 0;
	};

	bool LoadSceneFromFile(
		const std::filesystem::path& filename,
		SceneBase& scene
	);
}

#endif