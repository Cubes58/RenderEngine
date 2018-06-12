#include "Model.h"

#include "Log.h"

Model::Model(Win32OpenGL *p_Win32OpenGL, const std::string &p_ModelFileLocation, const std::string &p_TextureFileLocation, bool p_Mipmaps,
	const glm::vec3 &p_Position, const glm::vec3 &p_Rotation, const glm::vec3 &p_Scale) 
	: m_Win32OpenGL(p_Win32OpenGL), m_ModelFileLocation(p_ModelFileLocation), m_TextureFileLocation(p_TextureFileLocation), m_Mipmaps(p_Mipmaps),
	m_Position(p_Position), m_Rotation(p_Rotation), m_Scale(p_Scale), m_Prepared(false), m_LoadedADrawableModel(true) {

}

Model::~Model() {

}

void Model::LoadModel() {
	ModelLoader m_ModelLoader;
	// Attempt to load the model binary.
	if (!m_ModelLoader.LoadModelBinary(m_ModelFileLocation)) {
		// If the model binary doesn't load try to use the obj and load that, saving it as a binary file, for next time.
		if (!m_ModelLoader.LoadModelObj(m_ModelFileLocation)) {
			// If it cannot find/use the obj file then it'll attempt to load the Default model binary.
			if (!m_ModelLoader.LoadModelBinary("Models\\Default")) {
				// If it cannot find/use the .bin file then it'll attempt to load the Default model obj.
				if (!m_ModelLoader.LoadModelObj("Models\\Default")) {
					// If the default cannot be loaded then print an error message to the user. 
					Log::AppendToLogFile("Couldn't find the default model .obj!");
					m_LoadedADrawableModel = false;
				}
			}
		}
	}
	if (m_LoadedADrawableModel) {
		// Success - Load the information into the model object!
		m_Vertices = m_ModelLoader.GetVertices();
		m_Normals = m_ModelLoader.GetNormals();
		m_TextureUVs = m_ModelLoader.GetTextureUVs();
	}
}

bool Model::LoadTexture() {
	if (TextureLoader::LoadBMP(m_TextureFileLocation, m_TextureID, m_Mipmaps) != 0) {
		Log::AppendToLogFile("Couldn't load texture file: " + m_TextureFileLocation);
		if (TextureLoader::LoadBMP("Textures\\Default.bmp", m_TextureID, m_Mipmaps) != 0) {
			Log::AppendToLogFile("Couldn't load the default texture file: Textures\\Default.bmp");
			return false;
		}
	}

	return true;
}

void Model::Prepare() {
	LoadModel();

	// Before we prepare the model check to make sure we have a model we're able to load a texture on.
	if (m_LoadedADrawableModel) 
		m_LoadedADrawableModel = LoadTexture();		// If we're able to load a texture on it, check to make sure a texture is loaded on it - not drawable without one.

	// Now we'll prepare the model, if something drawable has been loaded, whether that be the actual model and texture or the defaults.
	if (m_LoadedADrawableModel) {
		// GLuint VBOVertices;
		glGenBuffers(1, &m_VBOVertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(GLfloat), &m_Vertices[0], GL_STATIC_DRAW);

		// GLuint VBONormals;
		glGenBuffers(1, &m_VBONormals);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormals);
		glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(GLfloat), &m_Normals[0], GL_STATIC_DRAW);

		// GLuint VBOTextures;
		glGenBuffers(1, &m_VBOTextureUV);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOTextureUV);
		glBufferData(GL_ARRAY_BUFFER, m_TextureUVs.size() * sizeof(GLfloat), &m_TextureUVs[0], GL_STATIC_DRAW);

		// GLuint VAO;
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOTextureUV);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		m_Prepared = true;	// It's been prepared.
	}
}

void Model::Draw(GLuint p_ShaderProgram) {
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	// Translate last:
	modelMatrix = glm::translate(modelMatrix, m_Position);
	// Rotate second:
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	// Scale first:
	modelMatrix = glm::scale(modelMatrix, m_Scale);

	// Send to the shader.
	Win32OpenGL::SendUniformMatrixToShader(p_ShaderProgram, modelMatrix, "model_matrix");

	glBindVertexArray(m_VAO);
	GLuint numberOfElements = m_Vertices.size() / 3;

	glDrawArrays(GL_TRIANGLES, 0, numberOfElements);

	glBindVertexArray(NULL);
	// Unbind
	glBindTexture(GL_TEXTURE_2D, NULL);
}

GLuint Model::GetTextureID() const {
	return m_TextureID;
}

bool Model::GetPreparedState() const {
	return m_Prepared;
}

glm::vec3 Model::GetPosition() const {
	return m_Position;
}

void Model::SetPosition(glm::vec3 p_Position) {
	m_Position = p_Position;
}

glm::vec3 Model::GetRotation() const {
	return m_Rotation;
}

void Model::SetRotation(glm::vec3 p_Rotation) {
	m_Rotation = p_Rotation;
}

glm::vec3 Model::GetScale() const {
	return m_Scale;
}

void Model::SetScale(glm::vec3 p_Scale) {
	m_Scale = p_Scale;
}

bool Model::GetIsModelDrawable() {
	return m_LoadedADrawableModel;
}