#include "stdafx.h"
#include "Skybox.h"

const GLfloat Skybox::m_SkyboxSize(800);	// Smaller than the far clipping plane.

Skybox::Skybox() : m_SkyboxVertices{
	// Vertices' positions.
	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,

	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,

	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,

	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,

	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize,  m_SkyboxSize,
	-m_SkyboxSize,  m_SkyboxSize, -m_SkyboxSize,

	-m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize, -m_SkyboxSize,
	-m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize,
	m_SkyboxSize, -m_SkyboxSize,  m_SkyboxSize } {

}

void Skybox::Prepare() {
	glGenVertexArrays(1, &m_SkyBoxVAO);
	glGenBuffers(1, &m_SkyBoxVBO);
	glBindVertexArray(m_SkyBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, Skybox::m_SkyboxNumberOfVertices * sizeof(GLfloat), &m_SkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);

	// Add the faces to a vector, so they can be passed and processed.
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures\\SkyBox\\right.bmp");			
	skyboxFaces.push_back("Textures\\SkyBox\\left.bmp");
	skyboxFaces.push_back("Textures\\SkyBox\\top.bmp");
	skyboxFaces.push_back("Textures\\SkyBox\\bottom.bmp");
	skyboxFaces.push_back("Textures\\SkyBox\\front.bmp");
	skyboxFaces.push_back("Textures\\SkyBox\\back.bmp");

	// Process the faces - creating the cubemap.
	m_CubeMapTextureID = TextureLoader::LoadCubeMap(skyboxFaces);
}

void Skybox::Draw(GLuint p_ShaderProgram, glm::mat4 &p_ViewMatrix, const glm::mat4 &p_ProjectionMatrix) {
	glDepthFunc(GL_LEQUAL);		// The incoming depth value is less than or equal to the stored depth value.
	glUseProgram(p_ShaderProgram);	// Make sure we're using the correct shader.

	p_ViewMatrix = glm::mat4(glm::mat3(p_ViewMatrix));
	// Setting two uniform objects, in the shader.
	glUniformMatrix4fv(glGetUniformLocation(p_ShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(p_ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(p_ShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(p_ProjectionMatrix));

	glBindVertexArray(m_SkyBoxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}