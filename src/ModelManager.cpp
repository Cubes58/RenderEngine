#include "ModelManager.h"

#include "SceneFileLoadHelper.h"

ModelManager::ModelManager(Win32OpenGL *p_Win32OpenGL) : m_Win32OpenGL(p_Win32OpenGL), m_ModelsPrepared(false) {
	LoadScene("Scenes\\Airport.txt");	// Load all the models for the airport scene.
}

ModelManager::~ModelManager() {
	// If there's memory used, by the models being stored on the heap free the memory.
	// Check to make sure it's still pointing to the memory it was allocated, if it is delete the memory, and set the pointer to null.
	for (auto i : m_Models) {
		if (std::get<GETMODELOBJECT>(i.second) != nullptr) {
			delete std::get<GETMODELOBJECT>(i.second);
			std::get<GETMODELOBJECT>(i.second) = nullptr;
		}
	}	
}

bool ModelManager::LoadScene(const std::string &p_SceneFileLocation) {
	static int lineFailedOn(1);	// Start on line 1 - not 0
	std::fstream sceneFile(p_SceneFileLocation, std::ios_base::in);

	if (!sceneFile) {
		Log::AppendToLogFile("Couldn't find the scene file: " + p_SceneFileLocation + " to load from.");
		return false;
	}

	// The information a model needs.
	std::string name;
	std::string group;
	std::string modelLocation;
	std::string textureLocation;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	bool mipmaps;

	// The extra information the lighting needs - uses group and extra:
	float specularIntensity;
	float lightExponent;

	try {
		std::string line("");
		if (sceneFile.is_open()) {
			while (std::getline(sceneFile, line)) {			// Get the line from the file, if there is one.
				std::string currentStringOfInfo("");
				std::istringstream stringStreamLine(line);	// The line we're currently processing.

				stringStreamLine >> currentStringOfInfo;
				LineType lineType = SceneFileLoadHelper::GetLineType(currentStringOfInfo);	// Get the line type.
				switch (lineType) {													// Check the line type.
				case LineType::COMMENT:
					SceneFileLoadHelper::DealWithComment();
					lineFailedOn++;				// Allows us to print out an error message with the line number the error occured on.
					break;
				case LineType::MODEL:
					if (SceneFileLoadHelper::DealWithModel(stringStreamLine, name, group, modelLocation, textureLocation, mipmaps, position, rotation, scale)) {
						// Create the model.
						Model *model = new Model(m_Win32OpenGL, modelLocation, textureLocation, mipmaps, position, rotation, scale);

						// Create the modelGroupObject - model that's tied to a group.
						auto modelObject = std::make_tuple(group, model);

						// Add it to the managed models (m_Models).
						m_Models.insert(std::pair<std::string, ModelGroupObject>(name, modelObject));
					}
					else
						Log::AppendToLogFile("Error on line: " + std::to_string(lineFailedOn) + " not enough information to load a model object with. Scene file: " + p_SceneFileLocation);
					lineFailedOn++;
					break;
				case LineType::LIGHTING:
					if (SceneFileLoadHelper::DealWithGroupLighting(stringStreamLine, group, specularIntensity, lightExponent)) {
						auto groupLighting = std::make_tuple(group, specularIntensity, lightExponent);

						// Add it to the vector, storing all the group lighting properties.
						m_GroupLightingProperties.push_back(groupLighting);
					}
					else {
						Log::AppendToLogFile("Couldn't set groups lighting: " + group + "\nLight values read in: specular intensity = "
							+ std::to_string(specularIntensity) + "		light exponent = " + std::to_string(lightExponent));
					}
					lineFailedOn++;
					break;
				}
			}
		}
		sceneFile.close();
	}
	catch (const std::runtime_error &rumtimeError) {
		std::string rumtimeErrorInfo = rumtimeError.what();
		Log::AppendToLogFile("An error occured, while loading the information from the scene file: " + p_SceneFileLocation);
		Log::AppendToLogFile("Standard runtime error was caught, with the message: " + rumtimeErrorInfo);

		if (sceneFile.is_open())
			sceneFile.close();

		return false;
	}
	catch (const std::exception &exception) {
		std::string exceptionInfo = exception.what();		// Inform the user of the issues, by logging it.
		Log::AppendToLogFile("An exception has occured, while loading information from the scene file: " + p_SceneFileLocation);
		Log::AppendToLogFile("Standard exception was caught, with the message: " + exceptionInfo);

		if (sceneFile.is_open())
			sceneFile.close();

		return false;
	}

	return true;
}

void ModelManager::PrepareModels() {
	m_ModelsPrepared = true;

	// Get every model and prepare them.
	for (auto it = m_Models.begin(); it != m_Models.end(); /* No increment. Done below */) {
		std::get<GETMODELOBJECT>(it->second)->Prepare();

		//If the model cannot be prepared delete it from the map, so its not drawn and taking up system resources.
		if (!std::get<GETMODELOBJECT>(it->second)->GetIsModelDrawable()) {
			delete std::get<GETMODELOBJECT>(it->second);
			std::get<GETMODELOBJECT>(it->second) = nullptr;

			m_Models.erase(it++->first);
		}
		else
			++it;
	}
}

void ModelManager::DrawModels(GLuint p_ShaderProgram, Light &p_Light) {
	// Go through all the models, drawing them. Change the specular component if it's a certain model - remove the shine.
	for (auto &i : m_Models) {
		for (auto &j : m_GroupLightingProperties) {
			// Check if the model's group has certain properties set, for the lighting - check every modified group.
			if (std::get<(int)(ModelObjectInformation::GROUP)>(i.second) == std::get<(int)(GroupLightingInformation::GROUP)>(j)) {
				// If the group has been modified set the specular properties to that found in the file.
				p_Light.SetSpecularIntensity(std::get<(int)(GroupLightingInformation::SPECULAR_INTENSITY)>(j));
				p_Light.SetSpecularExponent(std::get<(int)(GroupLightingInformation::SPECULAR_EXPONENT)>(j));
			}
			else {
				p_Light.SetSpecularIntensity(1.0f);
				p_Light.SetSpecularExponent(1000.0f);
			}
		}

		std::get<GETMODELOBJECT>(i.second)->Draw(p_ShaderProgram);
	}
}

bool ModelManager::AddModel(const std::string &p_Name, const std::string &p_Group, const std::string &p_ModelFileLocation, const std::string &p_TextureFileLocation, bool p_Mipmaps,
	const glm::vec3 &p_Position, const glm::vec3 &p_Rotation, const glm::vec3 &p_Scale) {
	Model *model = new Model(m_Win32OpenGL, p_ModelFileLocation, p_TextureFileLocation, p_Mipmaps, p_Position, p_Rotation, p_Scale);

	// If the models in the map have been prepared prepare the model, so it's ready to be drawn.
	if (m_ModelsPrepared)
		model->Prepare();

	// If the model cannot be drawn then do not add it, and return true.
	if (model->GetIsModelDrawable()) {
		// Add the model.
		auto modelObject = std::make_tuple(p_Group, model);
		m_Models.insert(std::pair<std::string, ModelGroupObject>(p_Name, modelObject));
		return true;
	}
	return false;
}

bool ModelManager::AddModel(const std::string &p_Name, const std::string &p_Group, Model &p_Model) {
	// If the models in the map have been prepared prepare the model, so it's ready to be drawn.
	if (m_ModelsPrepared && !p_Model.GetPreparedState())
		p_Model.Prepare();

	if (p_Model.GetIsModelDrawable()) {
		// Add the model.
		auto modelObject = std::make_tuple(p_Group, &p_Model);
		m_Models.insert(std::pair<std::string, ModelGroupObject>(p_Name, modelObject));

		return true;
	}
	return false;
}

bool ModelManager::RemoveModel(const std::string &p_Name) {
	std::map<std::string, ModelGroupObject>::iterator iter = m_Models.find(p_Name);

	// If the model has been found, delete the memory allocated to it, and remove it from the map.
	if (iter != m_Models.end()) {
		delete std::get<GETMODELOBJECT>(iter->second);

		std::get<GETMODELOBJECT>(iter->second) = nullptr;

		m_Models.erase(p_Name);

		// True, because it hasn't failed to find the model and has deleted it.
		return true;
	}

	// Couldn't find the model, so nothing was deleted.
	return false;
}

Model *ModelManager::GetModel(const std::string &p_Name) {
	std::map<std::string, ModelGroupObject>::iterator iter = m_Models.find(p_Name);

	// If the model has been found return it.
	if (iter != m_Models.end())
		return std::get<GETMODELOBJECT>(iter->second);

	// If the model hasn't been found log the error, to inform the user and return a nullptr.
	Log::AppendToLogFile("Couldn't find the model: " + p_Name);
	return nullptr;
}

std::string ModelManager::GetGroupName(const std::string &p_Name) {
	std::map<std::string, ModelGroupObject>::iterator iter = m_Models.find(p_Name);

	if (iter != m_Models.end()) 
		return std::get<GETMODELGROUP>(iter->second);

	Log::AppendToLogFile("Couldn't find the model: " + p_Name + " to give you its group name.");
	return "NO_GROUP_FOUND";
}