#include "Camera.hpp"

#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>
extern unsigned RenderDist;

Camera::Camera(const glm::vec3& vPos, uint16_t uScreenWidth, uint16_t uScreenHeight)
{
	CameraPos = vPos;
	ChangeView(uScreenWidth, uScreenHeight);
}

void Camera::ChangeView(uint16_t uScreenWidth, uint16_t uScreenHeight)
{
	this->uScreenHeight = uScreenHeight;
	this->uScreenWidth = uScreenWidth;
	halfHeight = uScreenHeight / 2;
	halfWidth = uScreenWidth / 2;
	proj = glm::perspective(glm::radians(60.f), static_cast<float>(uScreenWidth) / uScreenHeight, 0.5f, 1000.0f);
	
	viewport = glm::mat4(
		halfWidth,  0, 			 0, 0,
		0, 		    -halfHeight, 0, 0,
		0, 			0, 			 1, 0,
		halfWidth,  halfHeight,  0, 1
	);

}

void Camera::RecalcView()
{
	view = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void Camera::MouseControl(GLFWwindow* window)
{
	using namespace glm;

	float fSensivity = 0.005f;

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (bFirst || (x > uScreenWidth - 100) || (x < 100))
	{
		glfwSetCursorPos(window, uScreenWidth / 2, y);
		uLastX = uScreenWidth / 2;
		uLastY = y;
		bFirst = false;
	}

	int iDeltaX = x - uLastX;
	int iDeltaY = uLastY - y;
	uLastX = x;
	uLastY = y;

	fYaw += iDeltaX * fSensivity;
	fPitch += iDeltaY * fSensivity;

	if (fPitch > pi<float>() / 4)
		fPitch = pi<float>() / 4;
	if (fPitch < -pi<float>() / 4)
		fPitch = -pi<float>() / 4;

	if (fYaw > 2 * pi<float>())
		fYaw -= 2 * pi<float>();
	if (fYaw < 2 * pi<float>())
		fYaw += 2 * pi<float>();

	vec3 direction;
	direction.x = cos(fYaw) * cos(fPitch);
	direction.y = sin(fPitch);
	direction.z = sin(fYaw) * cos(fPitch);

	CameraFront = normalize(direction);
}

void Camera::SetPos(const glm::vec3 &vNewPos)
{
	CameraPos = vNewPos;
}

const glm::mat4 &Camera::GetProjMat() const
{
	return proj;
}

const glm::mat4& Camera::GetViewMat() const
{
	return view;
}

const glm::mat4 &Camera::GetViewportMat() const
{
	return viewport;
}

const glm::vec3 &Camera::GetPos() const
{
    return CameraPos;
}

const glm::vec3 &Camera::GetCameraFront() const
{
    return CameraFront;
}

void Camera::KeyboardControl(GLFWwindow* window, float deltaTime)
{
	float fSpeed = 1.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		fSpeed = 5.0f;

	float fMovSpeed = fSpeed * deltaTime;


	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		CameraPos += fMovSpeed * CameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		CameraPos -= fMovSpeed * CameraUp;
	}
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		CameraPos += fMovSpeed * CameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		CameraPos -= fMovSpeed * CameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));

		CameraPos += fMovSpeed * right;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));
		CameraPos -= fMovSpeed * right;
	}

}

