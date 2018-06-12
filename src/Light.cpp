#include "Light.h"

Light::Light(Win32OpenGL *p_Win32OpenGL, GLuint p_ShaderProgram) 
	: m_Win32OpenGL(p_Win32OpenGL), m_LightShaderProgram(p_ShaderProgram) {
}

Light::~Light() {

}

// Send the necessary information to the shader - set the shader uniforms.
void Light::PrepareLight() {
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightPosition, "light_position_world");
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourSpecular, "light_colour_specular");
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourDiffuse, "light_colour_diffuse");
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourAmbient, "light_colour_ambient");
	Win32OpenGL::SendUniformFloatToShader(m_LightShaderProgram, m_specularIntensity, "specular_intensity");		
	Win32OpenGL::SendUniformFloatToShader(m_LightShaderProgram, m_specularExponent, "specular_exponent");	

	m_Light->Prepare();
}

void Light::Draw(GLuint p_ShaderProgram) {
	static glm::vec3 s_DefaultLightColourAmbient = GetLightColourAmbient();

	SetLightColourAmbient(glm::vec3(10.0f, 10.0f, 10.0f));	// Light the sphere up, to make it look like a light.

	m_Light->Draw(p_ShaderProgram);

	// Reset the shader's uniform type.
	SetLightColourAmbient(s_DefaultLightColourAmbient);
}

glm::vec3 Light::GetLightColourSpecular() const {
	return m_LightColourSpecular;
}

// Every time we set the a property of the light send the new information to the shader.
void ::Light::SetLightColourSpecular(const glm::vec3 &p_LightColourSpecular) {
	m_LightColourSpecular = p_LightColourSpecular;
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourSpecular, "light_colour_specular");
}

glm::vec3 Light::GetLightColourDiffuse() const {
	return m_LightColourDiffuse;
}

void Light::SetLightColourDiffuse(const glm::vec3 &p_LightColourDiffuse) {
	m_LightColourDiffuse = p_LightColourDiffuse;
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourDiffuse, "light_colour_diffuse");
}

glm::vec3 Light::GetLightColourAmbient() const {
	return m_LightColourAmbient;
}

void Light::SetLightColourAmbient(const glm::vec3 &p_LightColourAmbient) {
	m_LightColourAmbient = p_LightColourAmbient;
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightColourAmbient, "light_colour_ambient");
}

glm::vec3 Light::GetLightPosition() const {
	return m_LightPosition;
}

void Light::SetLightPosition(const glm::vec3 &p_LightPosition) {
	m_LightPosition = p_LightPosition;
	m_Light->SetPosition(m_LightPosition);		// We want to set the model's position the same as the lights position - so it follows it.
	Win32OpenGL::SendUniformVector3ToShader(m_LightShaderProgram, m_LightPosition, "light_position_world");
}

float Light::GetSpecularIntensity() const {
	return m_specularIntensity;
}

void Light::SetSpecularIntensity(float p_SpecularIntensity) {
	m_specularIntensity = p_SpecularIntensity;
	Win32OpenGL::SendUniformFloatToShader(m_LightShaderProgram, m_specularIntensity, "specular_intensity");
}

float Light::GetSpecularExponent() const {
	return m_specularExponent;
}

void Light::SetSpecularExponent(float p_SpecularExponent) {
	m_specularExponent = p_SpecularExponent;
	Win32OpenGL::SendUniformFloatToShader(m_LightShaderProgram, m_specularExponent, "specular_exponent");
}