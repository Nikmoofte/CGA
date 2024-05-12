#include "Renderer.hpp"

#include "Assets/Material/Material.hpp"
#include "Assets/Texture/Texture.hpp"

#include "Engine/Scene/Scene.hpp"
#include "Engine/Buffers/ColorBuffer/ColorBuffer.hpp"
#include "Engine/Buffers/DepthBuffer/DepthBuffer.hpp"
#include "Engine/Camera/Camera.hpp"

#include "Engine/Settings.hpp"

#include "perpDot.hpp"

#include <glm/glm.hpp>

namespace Engine
{
    Renderer::Renderer(Scene &scene) : 
    scene(scene)
    {

        resetBuffers();
    }

    void Renderer::clearBuffers()
    {
        colorBuffer->clear();
        depthBuffer->clear(1.0f);
    }
    void Renderer::resetBuffers()
    {
        glm::ivec2 screenDimentions = scene.getCamera().getScreenDimentions();
        projectedVertices.resize(scene.getVertecies().size());
        int trianglesCount = scene.getTriangleCount();
        const auto numCores = std::min(unsigned(trianglesCount), std::thread::hardware_concurrency());
        coresID.resize(numCores);
        for(int i = 0; i < coresID.size(); i++)
            coresID[i] = i;
        coreInterval = trianglesCount / numCores;
        coreIntervalBuffer = screenDimentions.x * screenDimentions.y / numCores; 
        triangles.resize(trianglesCount);

        colorBuffer = std::make_unique<Buffers::ColorBuffer>(screenDimentions);
        depthBuffer = std::make_unique<Buffers::DepthBuffer>(screenDimentions);
        clearBuffers();
    }
    void *Renderer::getColorBuffer()
    {
        return colorBuffer->data();
    }
    void Renderer::render()
    {
        clearBuffers();

        vertexShaderStage();
        triangleAssing();
        clippingStage();       
        rasterisationStage();
        fragmentShaderStage();
        
    }
    void Renderer::vertexShaderStage()
    {
        auto& camera = scene.getCamera();
        auto& verticies = scene.getVertecies();

        glm::mat4 total = camera.getViewportMatrix() * camera.getProjectionMatrix() * camera.getViewMatrix();
        std::for_each(std::execution::par, verticies.cbegin(), verticies.cend(), [&total, this](const Assets::Vertex& inVert)
        {
            Assets::Vertex outVert{inVert};
            outVert.projectedPosition = total * glm::vec4(outVert.position, 1.0f); 
            outVert.perspectiveDivide();
            projectedVertices[inVert.id] = std::move(outVert);
        }
        );
    }
    void Renderer::clippingStage()
    {
        auto& cam = scene.getCamera();
        std::for_each(std::execution::par, triangles.begin(), triangles.end(), [&cam, this](Triangle& triangle)
        {
            // if(triangle.isBackFace(cam.getPos()))
            //     triangle.Clip();
        }   
        );
    }
    void Renderer::rasterisationStage()
    {
        using namespace glm;
        std::for_each(std::execution::par, triangles.begin(), triangles.end(), [this](Triangle& triangle)
        {
            if(!triangle.isClipped())
            {   
                auto& triangleIndicies = triangle.getVertInds();
                auto& v0 = projectedVertices[triangleIndicies[0]];    
                auto& v1 = projectedVertices[triangleIndicies[1]];
                auto& v2 = projectedVertices[triangleIndicies[2]];

                vec2 bbmin = glm::min(v0.projectedPosition, glm::min(v1.projectedPosition,  v2.projectedPosition));
                vec2 bbmax = glm::max(v0.projectedPosition, glm::max(v1.projectedPosition,  v2.projectedPosition));

                ivec2 min = glm::max({0, 0}, glm::min(Settings::Get().windowSize - 1, glm::ivec2{glm::floor(bbmin)}));
                ivec2 max = glm::max({0, 0}, glm::min(Settings::Get().windowSize - 1, glm::ivec2{glm::ceil(bbmax)}));

                for(int y = min.y; y <= max.y; ++y)
                {
                    for(int x = min.x; x <= max.x; ++x)
                    {
                        glm::vec3 bary = triangle.getBarycentric({x, y});
                        if(bary.x > 0.0f && bary.y > 0.0f && bary.z > 0.0f)
                        {
                            float z = glm::dot(bary, vec3(v0.projectedPosition.z, v1.projectedPosition.z, v2.projectedPosition.z));
                            if(z < depthBuffer->get({x, y}))
                            {
                                depthBuffer->set({x, y}, z);

                                colorBuffer->set({x, y}, triangle.getId());
                            }
                        }
                    }
                }
            }

        }
        );
    }

    inline glm::vec3 CalcBumpedNormal(const glm::vec3& norm, const glm::vec3& tangent, glm::vec3 BumpMapNormal)
    {
        glm::vec3 Normal = glm::normalize(norm);
        glm::vec3 Tangent = glm::normalize(tangent);
        Tangent = glm::normalize(Tangent - glm::dot(Tangent, Normal) * Normal);
        glm::vec3 Bitangent = glm::cross(Tangent, Normal);
        glm::mat3 TBN = glm::mat3(Tangent, Bitangent, Normal);
        BumpMapNormal = 2.0f * BumpMapNormal - 1.0f;
        glm::vec3 NewNormal;
        NewNormal = TBN * BumpMapNormal;
        NewNormal = glm::normalize(NewNormal);
        return NewNormal;
    }
    void Renderer::fragmentShaderStage()
    {
        using namespace glm;
        auto& lights = scene.getLights();
        const auto& cam = scene.getCamera();
        int width = cam.getScreenDimentions().x;
        float invWidth = (float)1 / width;
        std::for_each(std::execution::par, coresID.begin(), coresID.end(), [this, &lights, &cam, &invWidth, width](int id)
        {
            int startInd = id * coreIntervalBuffer;
            int endInd = (id + 1) * coreIntervalBuffer;
            for(int i = startInd; i < endInd; ++i)
            {
                int y = invWidth * i;
                int x = i - y * width;
                Assets::Color4b& pixel = colorBuffer->get({x, y});
                int val = *(int*)&pixel;
                if(val != 0)
                {
                    auto& triangle = triangles[val - 1];
                    int materialId = triangle.getMaterialId();
                    auto& material = scene.getMaterial(materialId);
                    auto& triangleIndicies = triangle.getVertInds();
                    auto& v0 = projectedVertices[triangleIndicies[0]];    
                    auto& v1 = projectedVertices[triangleIndicies[1]];
                    auto& v2 = projectedVertices[triangleIndicies[2]];
                    auto bary = triangle.getBarycentric({x, y});

                    vec3 FragPos = bary.x * v0.position + bary.y * v1.position + bary.z * v2.position;
                    float invW =  1 / glm::dot(bary, vec3(v0.invW, v1.invW, v2.invW));
                    vec2 texCoords = (bary.x * v0.texCoords + bary.y * v1.texCoords + bary.z * v2.texCoords) * invW;
                    auto& normalTexName = material->getNormalTextureName();
                    vec3 Normal;
                    if(normalTexName.empty())
                        Normal = bary.x * v0.normal + bary.y * v1.normal + bary.z * v2.normal;
                    else
                        Normal = glm::normalize (scene.getTexture(normalTexName)->GetPixel(texCoords));
                    vec3 Tangent = bary.x * v0.tangent + bary.y * v1.tangent + bary.z * v2.tangent;
                    auto& bumpTextName = material->getBumpTextureName();
                    if(!bumpTextName.empty())
                    {
                        auto& bumpTex = scene.getTexture(bumpTextName);
                        Normal = CalcBumpedNormal(Normal, Tangent, bumpTex->GetPixel(texCoords));
                    }
                    //Actual shader code
                    float ambientStrength;
                    float specularStrength;

                    auto& ambientTexName = material->getAmbientTextureName();
                    if(ambientTexName.empty())
                        ambientStrength = Settings::Get().ambientStrength;
                    else
                        ambientStrength = scene.getTexture(ambientTexName)->GetPixel(texCoords).a;
                    
                    auto& specTexName = material->getSpecularTextureName();
                    if(specTexName.empty())
                        specularStrength = Settings::Get().specularStrength;
                    else
                        specularStrength = scene.getTexture(specTexName)->GetPixel(texCoords).a;
                    
                    vec3 ambient{}, diffuse{}, specular{};
                    auto& diffTex = scene.getTexture(material->getDiffuseTextureName());
                    for(auto& light : lights)
                    {
                        ambient += ambientStrength * light.color;

                        // diffuse 
                        vec3 lightDir = normalize(light.pos - FragPos);
                        float diff = std::max(dot(Normal, lightDir), 0.0f);
                        diffuse += diff * light.color;

                        // specular
                        vec3 viewDir = normalize(cam.getPos() - FragPos);
                        vec3 reflectDir = reflect(-lightDir, Normal);  
                        float spec = pow(std::max(dot(viewDir, reflectDir), 0.0f), 32);
                        
                        specular += specularStrength * spec * light.color;  
                    }
                    vec3 col{1.0f};
                    col *= ambient + diffuse  + specular;
                    Assets::Color4b color{};
                    //color.fromVec({z, 0.0f, 0.0f, 1.0f});
                    glm::vec3 coltemp = diffTex->GetPixel(texCoords);
                    color.fromVec({col * glm::vec3(coltemp.b, coltemp.r, coltemp.g), 1.0f});
                    colorBuffer->set({x, y}, color);
                }
            }
        }
        );
    }

    void Renderer::triangleAssing()
    {
        auto& indicies = scene.getIndecies();
        std::for_each(std::execution::par, coresID.cbegin(), coresID.cend(), [indicies, this](int id)
        {
            int startInd = id * coreInterval;
            int endInd = (id + 1) * coreInterval;
            for(int i = startInd; i < endInd; i++)
            {
                int index = i * 3;
                triangles[i] = Triangle(
                    i + 1, 
                    {indicies[index + 0], indicies[index + 1], indicies[index + 2]}, 
                    {
                        projectedVertices[indicies[index + 0]], 
                        projectedVertices[indicies[index + 1]], 
                        projectedVertices[indicies[index + 2]]
                    }
                );
                triangles[i].setMaterialId(scene.getMaterialId(index));
            }            
        });
    }

    glm::vec3 Renderer::getBarycentric(const glm::vec2 &p, const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c)
    {
        glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float denom = 1 / (d00 * d11 - d01 * d01);
        float v = (d11 * d20 - d01 * d21) * denom;
        float w = (d00 * d21 - d01 * d20) * denom;
        float u = 1.0f - v - w;
        return {u, v, w};
    }
}