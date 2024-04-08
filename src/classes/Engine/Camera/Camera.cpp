#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <numbers>

namespace Engine
{
    Camera::Camera(glm::vec3 eye, glm::vec3 lookAt, float fov, glm::ivec2 screenDimensions) : pos(eye), fov(fov), screenDimensions(screenDimensions), aspect((float)screenDimensions.x / screenDimensions.y)     
    {
        glm::vec3 dir = normalize(lookAt - pos);
		pitch = asin(dir.y);
		yaw = atan2(dir.z, dir.x);

        halfScreenDimensions = screenDimensions >> 1;

        viewport = glm::mat4(
            halfScreenDimensions.x,  0, 			          0, 0,
            0, 		                 -halfScreenDimensions.y, 0, 0,
            0, 			             0, 			          1, 0,
            halfScreenDimensions.x,  halfScreenDimensions.y,  0, 1
        );

        update();
    }
    
    bool Camera::OnCursorPositionChanged(double xpos, double ypos)
	{
        using glm::pi;
        static constexpr float SENSITIVITY = 0.01f;

        static bool first = true; 
        static glm::vec2 lastPos;

		if (first)
		{
			lastPos.x = xpos;
			lastPos.y = ypos;
			first = false;
			return false;
		}

		float xoffset = xpos - lastPos.x;
		float yoffset = lastPos.y - ypos;

		lastPos.x = xpos;
		lastPos.y = ypos;

		if (isMousePressed)
        {
            xoffset *= SENSITIVITY;
            yoffset *= SENSITIVITY;

            yaw += xoffset;
            pitch += yoffset;
            static const float piTemp = pi<float>() / 4; 
            static const float piTemp2 = pi<float>() * 2; 
            if (pitch > pi<float>() / 4)
                pitch = pi<float>() / 4;
            if (pitch < -pi<float>() / 4)
                pitch = -pi<float>() / 4;

            if (yaw > 2 * pi<float>())
                yaw -= 2 * pi<float>();
            if (yaw < 2 * pi<float>())
                yaw += 2 * pi<float>();

            update();
        }
		return isMousePressed;
	}

    void Camera::OnBeforeRender()
    {
        move();
    }

    void Camera::move()
    {
        pos += speed * (forward * (float)isCameraForward  
                        - forward * (float)isCameraBackward
                        + right * (float)isCameraRight
                        - right * (float)isCameraLeft
                        + up * (float)isCameraUp
                        - up * (float)isCameraDown);
    }



    void Camera::OnKeyChanged(int key, int scanCode, int action, int mod)
	{
		switch (key)
		{
		case GLFW_KEY_S:
			isCameraBackward = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_W:
			isCameraForward = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_A:
			isCameraLeft = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_D:
			isCameraRight = action != GLFW_RELEASE;
			break;
        case GLFW_KEY_SPACE:
			isCameraUp = action != GLFW_RELEASE;
			break;
        case GLFW_KEY_LEFT_CONTROL:
			isCameraDown = action != GLFW_RELEASE;
			break;
		}
	}

    void Camera::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			isMousePressed = action == GLFW_PRESS;
		}
	}

    void Camera::update()
    {
        glm::vec3 direction;

		direction.x = cos(yaw) * cos(pitch);
		direction.y = sin(pitch);
		direction.z = sin(yaw) * cos(pitch);

		forward = normalize(direction);
		right = normalize(cross(forward, worldUp));
		up = normalize(cross(right, forward));
    }

    void Camera::resize(float fov, glm::ivec2 screenDimensions)
    {
        this->screenDimensions = screenDimensions;
        fov = fov;
        aspect = (float)screenDimensions.x / screenDimensions.y;
        
        halfScreenDimensions = screenDimensions >> 1;

        viewport = glm::mat4(
            halfScreenDimensions.x,  0, 			          0, 0,
            0, 		                 -halfScreenDimensions.y, 0, 0,
            0, 			             0, 			          1, 0,
            halfScreenDimensions.x,  halfScreenDimensions.y,  0, 1
        );
    }

    glm::mat4 Camera::getProjectionMatrix() const
    {
        return glm::perspective(fov, aspect, 0.1f, 100.0f);
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(pos, pos + forward, up);
    }
    glm::mat4 Camera::getViewportMatrix() const
    {
        return viewport;
    }
    const glm::vec3 &Camera::getPos() const
    {
        return pos;
    }
    const glm::vec3 &Camera::getFront() const
    {
        return forward;
    }
}