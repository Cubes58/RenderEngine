#pragma once

#include "stdafx.h"
#include "Win32OpenGL.h"
#include "TextureLoader.h"
#include "ModelLoader.h"

class Model {
private:
	Win32OpenGL *m_Win32OpenGL;

	std::string m_ModelFileLocation;
	std::string m_TextureFileLocation;

	GLuint m_TextureID;

	std::vector<GLfloat> m_Vertices;
	std::vector<GLfloat> m_Normals;
	std::vector<GLfloat> m_TextureUVs;

	GLuint m_VAO;
	GLuint m_VBOVertices;
	GLuint m_VBONormals;
	GLuint m_VBOTextureUV;

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;
	bool m_Mipmaps;			// Whether the model has mipmaps or not.

	bool m_Prepared;		// Used to check whether the model needs to be prepared, before being drawn.
	bool m_LoadedADrawableModel;

	void LoadModel();		// Loads a model, if a model loads properly m_LoadedADrawableModel is set to true.
	bool LoadTexture();		// Loads texture, if a texture loaded it returns true.
public:
	Model(Win32OpenGL *p_Win32OpenGL, 
		const std::string &p_ModelFileName, 
		const std::string &p_TextureFileLocation, bool p_Mipmaps,
		const glm::vec3 &p_Position,
		const glm::vec3 &p_Rotation = glm::vec3(0.0f, 0.0f, 0.0f), 
		const glm::vec3 &p_Scale = glm::vec3(1.0f, 1.0f, 1.0f));
	~Model();

	void Prepare();							// Send information to the GPU, to get everything ready.
	void Draw(GLuint p_ShaderProgram);		// Use the information on the GPU to draw the models.

	// Accessor methods.
	GLuint GetTextureID() const;
	bool GetPreparedState() const;

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 p_Position);

	glm::vec3 GetRotation() const;
	void SetRotation(glm::vec3 p_Rotation);

	glm::vec3 GetScale() const;
	void SetScale(glm::vec3 p_Scale);

	bool GetIsModelDrawable();
};