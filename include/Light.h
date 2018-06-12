#pragma once

#include <memory>

#include "Win32OpenGL.h"
#include "Model.h"

class Light {
private:
	Win32OpenGL *m_Win32OpenGL;
	GLuint m_LightShaderProgram;			// Light shader.

	// Shader uniform variables.
	glm::vec3 m_LightColourSpecular{ 1.0f, 1.0f, 1.0f };
	glm::vec3 m_LightColourDiffuse{ 0.8f, 0.8f, 0.8f };
	glm::vec3 m_LightColourAmbient{ 0.3f, 0.3f, 0.3f };
	glm::vec3 m_LightPosition{ 25.0f, 200.0f, 5.0f };

	float m_specularIntensity{ 1.0f };	
	float m_specularExponent{ 3000.0f };
	
	// Sphere model put on the light, to show its location - like a sun.
	std::unique_ptr<Model> m_Light = std::unique_ptr<Model>(new Model(m_Win32OpenGL, "Models\\Light", "Textures\\LightTA.bmp", false, m_LightPosition));
public:
	Light(Win32OpenGL *p_Win32OpenGL, GLuint p_ShaderProgram);
	~Light();

	void PrepareLight();
	void Draw(GLuint p_ShaderProgram);

	// Accessor methods (getters/setters), allows for easy changing of the light's properties.
	glm::vec3 GetLightColourSpecular() const;
	void SetLightColourSpecular(const glm::vec3 &p_LightColourSpecular);

	glm::vec3 GetLightColourDiffuse() const;
	void SetLightColourDiffuse(const glm::vec3 &p_LightColourDiffuse);

	glm::vec3 GetLightColourAmbient() const;
	void SetLightColourAmbient(const glm::vec3 &p_LightColourAmbient);

	glm::vec3 GetLightPosition() const;
	void SetLightPosition(const glm::vec3 &p_LightPosition);

	float GetSpecularIntensity() const;
	void SetSpecularIntensity(float p_SpecularIntensity);

	float GetSpecularExponent() const;
	void SetSpecularExponent(float p_SpecularExponent);
};