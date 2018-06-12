#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <tuple>

#include "Win32OpenGL.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Light.h"

typedef std::tuple<std::string, Model*> ModelGroupObject;
#define GETMODELOBJECT static_cast<int>(ModelObjectInformation::MODEL_OBJECT)
#define GETMODELGROUP static_cast<int>(ModelObjectInformation::GROUP)
enum class ModelObjectInformation : unsigned int {	
	GROUP = 0,
	MODEL_OBJECT = 1
};

typedef std::tuple<std::string, float, float> GroupLighting;
enum class GroupLightingInformation : unsigned int {
	GROUP = 0,						// First element type in the GroupLighting tuple.
	SPECULAR_INTENSITY = 1,
	SPECULAR_EXPONENT = 2
};

class ModelManager {
private:
	Win32OpenGL *m_Win32OpenGL;

	// Stores the model's name as the unique key, then the group of the model and the model as the element[s].
	std::map<std::string, ModelGroupObject> m_Models;

	// Stores the modified lighting properties of groups - if any.
	std::vector<GroupLighting> m_GroupLightingProperties;

	bool m_ModelsPrepared;		// This tells new models (being added) whether the model manager has prepared the models already, 
								// because if they have then that model needs to be prepared before being added.
	
	bool LoadScene(const std::string &p_SceneFileLocation);		// Loads (scene) information from a file.
public:
	ModelManager(Win32OpenGL *p_Win32OpenGL);
	~ModelManager();

	void PrepareModels();
	void DrawModels(GLuint p_ShaderProgram, Light &p_Light);

	bool AddModel(const std::string &p_Name, const std::string &p_Group, const std::string &p_ModelFileLocation, const std::string &p_TextureFileLocation, bool p_Mipmaps,
		const glm::vec3 &p_Position, const glm::vec3 &p_Rotation, const glm::vec3 &p_Scale = glm::vec3(1.0f, 1.0f, 1.0f));
	bool AddModel(const std::string &p_Name, const std::string &p_Group, Model &p_Model);

	bool RemoveModel(const std::string &p_Name);
	Model *GetModel(const std::string &p_Name);
	std::string GetGroupName(const std::string &p_Name);
};