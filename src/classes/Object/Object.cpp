#include "Object.hpp"

void Object::addVertex(const glm::vec4& vertex)
{
    verticies.push_back(vertex);
}

void Object::addVertex(glm::vec4 &&vertex)
{
    verticies.push_back(std::move(vertex));
}

void Object::addNormal(const glm::vec3& normal)
{
    normals.push_back(normal);
}

void Object::addNormal(glm::vec3 &&normal)
{
    normals.push_back(std::move(normal));
}

void Object::addTexture(const glm::vec3 &texture)
{
    textures.push_back(texture);
}

void Object::addTexture(glm::vec3 &&texture)
{
    textures.push_back(std::move(texture));
}

void Object::addFace(const std::array<std::tuple<size_t, size_t, size_t>, 3>& face)
{
    faces.push_back(face);
}

