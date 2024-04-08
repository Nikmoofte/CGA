#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

namespace Engine
{
    class Camera
    {
    public:
        Camera(glm::vec3 eye, glm::vec3 lookAt, float fov, glm::ivec2 screenDimensions);

        glm::mat4 getProjectionMatrix() const;
        glm::mat4 getViewMatrix() const;
        glm::mat4 getViewportMatrix() const;

        const glm::vec3& getPos() const;
        const glm::vec3& getFront() const;
        glm::ivec2 getScreenDimentions() const 
        {
            return screenDimensions;
        }
        size_t getPixelCount() const
        {
            return screenDimensions.x * screenDimensions.y;
        }

        void OnKeyChanged(int key, int scanCode, int action, int mod);
        void OnMouseButtonChanged(int button, int action, int mods);
        bool OnCursorPositionChanged(double xpos, double ypos);
        void OnBeforeRender();
        void move();
        void update();
        void resize(float fov, glm::ivec2 screenDimensions);
    private:
        const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 pos;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

        glm::ivec2 screenDimensions;
        glm::ivec2 halfScreenDimensions;
        float aspect;
        float fov;
        float yaw, pitch;

        const float speed = 0.05f; 

        bool isCameraLeft = false;
		bool isCameraRight = false;
		bool isCameraUp = false;
		bool isCameraDown = false;
        bool isCameraForward = false;
        bool isCameraBackward = false;
		bool isMousePressed = false;

        glm::mat4 viewport;
    };
}

#endif