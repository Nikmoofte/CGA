#include "Camera.hpp"

#include <chrono>
#include <iostream>
#include <Windows.h>

extern unsigned RenderDist;

Camera::Camera(const glm::vec3& vPos, uint16_t uScreenWidth, uint16_t uScreenHeight)
{
	CameraPos = vPos;
	ChangeView(uScreenWidth, uScreenHeight);
}

void Camera::ChangeView(uint16_t uScreenWidth, uint16_t uSreenHeight)
{
	this->uScreenHeight = uScreenHeight;
	this->uScreenWidth = uScreenWidth;
	halfHeight = uScreenHeight / 2;
	halfWidth = uScreenWidth / 2;
	proj = glm::perspective(glm::radians(45.f), static_cast<float>(uScreenWidth) / uScreenHeight, 0.1f, 100.0f);
	viewport = glm::mat4(
		halfWidth,  0, 			0, 0,
		0, 		    -halfHeight, 0, 0,
		0, 			0, 			1, 0,
		halfWidth,  uScreenHeight, 0, 1
	);
	view = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void Camera::MouseControl()
{
	using namespace glm;

	float fSensivity = 0.005f;

	POINT pos;
	GetCursorPos(&pos);

	if (bFirst || (pos.x > uScreenWidth - 100) || (pos.x < 100))
	{
		SetCursorPos(uScreenWidth / 2, pos.y);
		uLastX = uScreenWidth / 2;
		uLastY = pos.y;
		bFirst = false;
	}

	int iDeltaX = pos.x - uLastX;
	int iDeltaY = uLastY - pos.y;
	uLastX = pos.x;
	uLastY = pos.y;



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
	view = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
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

void Camera::KeyboardControl()
{
	float fSpeed = 0.0001f;

	if (GetKeyState(VK_SHIFT) & 0x8000)
		fSpeed = 0.0005f;

	float fMovSpeed = fSpeed;


	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CameraPos += fMovSpeed * CameraUp;
	}
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		CameraPos -= fMovSpeed * CameraUp;
	}
	
	if (GetKeyState('W') & 0x8000)
	{
		CameraPos += fMovSpeed * CameraFront;
	}
	if (GetKeyState('S') & 0x8000)
	{
		CameraPos -= fMovSpeed * CameraFront;
	}

	if (GetKeyState('D') & 0x8000)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));

		CameraPos += fMovSpeed * right;
	}
	if (GetKeyState('A') & 0x8000)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));
		CameraPos -= fMovSpeed * right;
	}

}

