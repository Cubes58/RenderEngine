#pragma once

#include "Win32OpenGL.h"
#include "TextureLoader.h"

class Skybox {
private:
	GLuint m_CubeMapTextureID;
	
	GLuint m_SkyBoxVBO;
	GLuint m_SkyBoxVAO;	

	static const GLfloat m_SkyboxSize;	// Instead of changing every number, for the size of the skybox, we can just change this single variable.
	static const GLuint m_SkyboxNumberOfVertices = 108;	
	GLfloat m_SkyboxVertices[m_SkyboxNumberOfVertices];		// Manually create the cube vertices - easier to keep track of/modify.
public:
	Skybox();

	void Prepare();
	void Draw(GLuint p_ShaderProgram, glm::mat4 &p_ViewMatrix, const glm::mat4 &p_ProjectionMatrix);
};