#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <tuple>
#include <array>
#include <glm/glm.hpp>

class Object
{
public:
    Object() = default;
    void addVertex(const glm::vec4& vertex);
    void addVertex(glm::vec4&& vertex);
    void addNormal(const glm::vec3& normal);
    void addNormal(glm::vec3&& normal);
    void addTexture(const glm::vec3& texture);
    void addTexture(glm::vec3&& texture);

    void addFace(const std::array<std::tuple<size_t, size_t, size_t>, 3>& face);

    constexpr static size_t NONE = static_cast<size_t>(-1);
    friend class App;
private:

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> textures;
    std::vector<std::array<std::tuple<size_t, size_t, size_t>, 3>> faces;
};

#endif