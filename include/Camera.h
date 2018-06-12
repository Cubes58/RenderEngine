#pragma once

#include "Win32OpenGL.h"

class Camera {
private:
	// Store the Camera's position.
	const float m_kiStartX = 0.0f;
	const float m_kiStartY = 20.0f;
	const float m_kiStartZ = 60.0f;

	float m_fXPosition{ m_kiStartX };
	float m_fYPosition{ m_kiStartY };
	float m_fZPosition{ m_kiStartZ };

	// Store the Camera's orientation.
	float m_fAngleXY{ 0 };
	float m_fAngleXZ{ 0 };
	float m_fAngleYZ{ 0 };	

	// Create a look at vector. 
	float m_fDirectionX{ 0 };
	float m_fDirectionY{ 0 };
	float m_fDirectionZ{ 0 };

	// Compute left and right of the camera.
	glm::vec3 m_RightAngleDirection{ 0, 0, 0 };

	glm::mat4 m_ViewMatrix;
public:
	Camera();

	void ComputeViewMatrixUsingLookAt();
	void ComputeDirectionVector();
	void ComputerRightAngledDirectionVector();
	void UpdateCameraPosition(float p_fDistanceMoved);
	void SetViewMatrix(GLuint p_Program);

	void HandleInput(unsigned char p_uiVirtualKeyCode);
	void HandleMouseInput(int xPos, int yPos);

	glm::mat4 &GetViewMatrix();
};