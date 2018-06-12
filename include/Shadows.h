#pragma once

#include "Win32OpenGL.h"

class Shadows {
private:
	static const GLuint s_m_ShadowWidth = 1024;
	static const GLuint s_m_ShadowHeight = 1024;

	GLuint &m_DepthShader;

	GLuint m_DepthMapFBO;
	GLuint m_DepthMap;
	
	float m_NearPlane = 1.0f;
	float m_FarPlane = 1200.0f;
	// Using an orthographic projection because we don't want any perspective deform.
	glm::vec3 m_LightPosition;

	glm::mat4 m_LightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, m_NearPlane, m_FarPlane);
	glm::mat4 m_LightView = glm::lookAt(m_LightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 m_LightSpaceMatrix = m_LightProjection * m_LightView;

public:
	Shadows(GLuint &p_DepthShader, glm::vec3 &p_LightPosition);
	~Shadows();

	void Prepare();
	void Draw(float p_WindowWidth, float p_WindowHeight, GLuint p_ModelShader);
};