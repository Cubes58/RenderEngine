#include "stdafx.h"
#include "Camera.h"

Camera::Camera() {
	
}

void Camera::ComputeViewMatrixUsingLookAt() {
	static const int fullCircleAngle = 360;
	static const int halfCircleAngle = 180;
	static const int quarterCircleAngle = 90;
	//	float eyeX = m_x;
	//	float eyeY = m_y;
	//	float eyeZ = m_z;

	//	float centerX = m_x + m_directionX;
	//	float centerY = m_y + m_directionY;
	//	float centerZ = m_z + m_directionZ;

	float upX = 0;
	float upY = 1;
	float upZ = 0;

	// Keep the angle bounds within a single circle.
	if (m_fAngleYZ > halfCircleAngle)
		m_fAngleYZ -= fullCircleAngle;
	else if (m_fAngleYZ < -halfCircleAngle)
		m_fAngleYZ += fullCircleAngle;

	if (m_fAngleYZ == quarterCircleAngle)
		m_fAngleYZ = 89.0f;
	if (m_fAngleYZ == -quarterCircleAngle)
		m_fAngleYZ = -89.0f;

	if (m_fAngleYZ > -quarterCircleAngle && m_fAngleYZ < quarterCircleAngle)
		upY = 1;
	else
		upY = -1;

	// vec3 forward = vec3(centerX - eyeX, centerY - eyeY, centerZ - eyeZ); - simplified now...
	// we already store centreX - eyeX as m_directionX
	glm::vec3 forward = glm::vec3(m_fDirectionX, m_fDirectionY, m_fDirectionZ);
	glm::vec3 up = glm::vec3(upX, upY, upZ);
	glm::vec3 forwardNormalised = glm::normalize(forward);		//normalize(forward);
	glm::vec3 side = glm::vec3(forwardNormalised);				// copy cross(forward, up, side);
	side = glm::cross(side, up);
	glm::vec3 sideNormalised = glm::normalize(side);				//normalize(side);
	up = glm::vec3(sideNormalised);
	up = glm::cross(up, forwardNormalised);					// no need to normalise as generated from 2 normalised vectors of unit length
															// now we build the matrix... (column major)
															// this is the transpose for the V matrix as  Vinverse == VT for rotation matrices

														// row 0
	m_ViewMatrix[0][0] = sideNormalised[0];				//0
	m_ViewMatrix[1][0] = sideNormalised[1];				//4
	m_ViewMatrix[2][0] = sideNormalised[2];				//8
	m_ViewMatrix[3][0] = 0;								//12
														// row 1
	m_ViewMatrix[0][1] = up[0];							//1
	m_ViewMatrix[1][1] = up[1];							//5
	m_ViewMatrix[2][1] = up[2];							//9
	m_ViewMatrix[3][1] = 0;								//13
														// row 2
	m_ViewMatrix[0][2] = -(forwardNormalised[0]);		//2
	m_ViewMatrix[1][2] = -(forwardNormalised[1]);		//6
	m_ViewMatrix[2][2] = -(forwardNormalised[2]);		//10
	m_ViewMatrix[3][2] = 0;								//14
														// row 3
	m_ViewMatrix[0][3] = 0;								//3
	m_ViewMatrix[1][3] = 0;								//7
	m_ViewMatrix[2][3] = 0;								//11
	m_ViewMatrix[3][3] = 1;								//15
														// now apply translation to camera position

	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(-m_fXPosition, -m_fYPosition, -m_fZPosition));

	m_ViewMatrix = m_ViewMatrix * cameraTranslation;  // pre or post multiply - - definitely POST!!!
}

void Camera::ComputeDirectionVector() {
	float angleYZRadians = 3.14156f * m_fAngleYZ / 180.0f;
	
	m_fDirectionY = sin(angleYZRadians);

	float m_other = cos(angleYZRadians);

	float angleXZRadians = 3.14156f * m_fAngleXZ / 180.0f;
	m_fDirectionX = m_other * sin(angleXZRadians);
	m_fDirectionZ = m_other * -cos(angleXZRadians);
}

void Camera::ComputerRightAngledDirectionVector() {
	float m_other = 1.0f;

	float angleXZRadians = 3.14156f * (m_fAngleXZ + 90) / 180.0f;
	m_RightAngleDirection.x = m_other * sin(angleXZRadians);
	m_RightAngleDirection.z = m_other * -cos(angleXZRadians);

	m_RightAngleDirection.y = 0;
}


void Camera::UpdateCameraPosition(float p_fDistanceMoved) {
	// later might use matrices to calculate this....
	// for now use angle in xz and stay on ground....
	// if moved update position

	m_fXPosition += p_fDistanceMoved * m_fDirectionX;
	m_fYPosition += p_fDistanceMoved * m_fDirectionY;
	m_fZPosition += p_fDistanceMoved * m_fDirectionZ;
}

void Camera::SetViewMatrix(GLuint p_Program) {
	Win32OpenGL::SendUniformMatrixToShader(p_Program, m_ViewMatrix, "view_matrix");
}

void Camera::HandleInput(unsigned char p_uiVirtualKeyCode) {
	static float movementSpeed = 0.5f;
	float distanceMoved = 0.0f;
	float distanceMovedSideways = 0.0f;
	float angleStep = 2.5f;

	// Basic movement.
	if (p_uiVirtualKeyCode == VK_LEFT || p_uiVirtualKeyCode == 0x41) 				// A - 0x41
		distanceMovedSideways = -movementSpeed;
	else if (p_uiVirtualKeyCode == VK_RIGHT || p_uiVirtualKeyCode == 0x44) 			// D - 0x44
		distanceMovedSideways = movementSpeed;
	else if (p_uiVirtualKeyCode == VK_UP || p_uiVirtualKeyCode == 0x57) 			// W - 0x57
		distanceMoved = movementSpeed;
	else if (p_uiVirtualKeyCode == VK_DOWN || p_uiVirtualKeyCode == 0x53) 			// S - 0x53
		distanceMoved = -movementSpeed;

	// Direct up/down movement.
	else if (p_uiVirtualKeyCode == VK_ADD) 
		m_fYPosition += movementSpeed;
	else if (p_uiVirtualKeyCode == VK_SUBTRACT) 
		m_fYPosition -= movementSpeed;

	// Speed boost.
	p_uiVirtualKeyCode == VK_SHIFT ? movementSpeed = 0.75f : movementSpeed = 0.35f;

	// Move camera and update view matrix
	ComputeDirectionVector();

	if (distanceMoved != 0.0f) {
		UpdateCameraPosition(distanceMoved);
	}

	if (distanceMovedSideways != 0.0f)
	{
		ComputerRightAngledDirectionVector();

		m_fXPosition += distanceMovedSideways * m_RightAngleDirection.x;
		m_fYPosition += distanceMovedSideways * m_RightAngleDirection.y;
		m_fZPosition += distanceMovedSideways * m_RightAngleDirection.z;

	}
	ComputeViewMatrixUsingLookAt();
}

void Camera::HandleMouseInput(int xPos, int yPos) {
	const float kfXSensitivity = 10;
	const float kfYSensitivity = -10;

	m_fAngleXZ += (float)xPos / kfXSensitivity;
	m_fAngleYZ += (float)yPos / kfYSensitivity;
	
	ComputeDirectionVector();
	ComputeViewMatrixUsingLookAt();
}

glm::mat4 &Camera::GetViewMatrix() {
	return m_ViewMatrix;
}