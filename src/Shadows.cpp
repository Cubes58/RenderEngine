#include "Shadows.h"

Shadows::Shadows(GLuint &p_DepthShader, glm::vec3 &p_LightPosition) 
	: m_DepthShader(p_DepthShader), m_LightPosition(p_LightPosition) {

}

Shadows::~Shadows() {

}

void Shadows::Prepare() {
	glGenFramebuffers(1, &m_DepthMapFBO);

	// Now generate a 2D texture that'll be used as the framebuffer's depth buffer.
	glGenTextures(1, &m_DepthMap);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, s_m_ShadowWidth, s_m_ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
	// FBOs must have a draw and read buffer, but we don't need them.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadows::Draw(float p_WindowWidth, float p_WindowHeight, GLuint p_ModelShader) {	
	// I also passed the Draw method in and rendered the scene twice - once for depth and then a normal render.
	// First render to the depth map.
	glUseProgram(m_DepthShader);
	glUniformMatrix4fv(glGetUniformLocation(m_DepthShader, "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(m_LightSpaceMatrix));

	glViewport(0, 0, s_m_ShadowWidth, s_m_ShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	// SET SHADER[S] AND MATRICES			// USE THE LIGHTS SET MATRICES METHOD.
	// DRAW SCENE						// PASS IN THE DRAW METHOD - DRAW
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Then render the scene as normal with shadow mapping (using the depth map).
	glViewport(0, 0, (GLsizei)p_WindowWidth, (GLsizei)p_WindowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// SET SHADER[S] AND MATRICES	
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	glUseProgram(p_ModelShader);
	// DRAW SCENE

	//glm::mat4 projection = glm::perspective(glm::radians(), (float)p_WindowWidth / (float)p_WindowHeight, 0.1f, 100.0f);
}