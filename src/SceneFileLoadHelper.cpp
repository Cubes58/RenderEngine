#include "SceneFileloadHelper.h"

void SceneFileLoadHelper::ReadVec3(std::istringstream &p_ISSModelInfo, glm::vec3 &p_Vec, float p_DefaultErrorValue) {
	float temp(0.0f);
	if (p_ISSModelInfo >> temp)		// If a float can be found load it in, otherwise set it to p_DefaultErrorValue.
		p_Vec.x = temp;
	else
		p_Vec.x = p_DefaultErrorValue;

	if (p_ISSModelInfo >> temp)
		p_Vec.y = temp;
	else
		p_Vec.y = p_DefaultErrorValue;

	if (p_ISSModelInfo >> temp)
		p_Vec.z = temp;
	else
		p_Vec.z = p_DefaultErrorValue;
}

LineType SceneFileLoadHelper::GetLineType(const std::string &p_LineType) {
	if (p_LineType == "m" || p_LineType == "M")
		return LineType::MODEL;
	else if (p_LineType == "l" || p_LineType == "L")
		return LineType::LIGHTING;

	// If the line doesn't start with m or M then assume it's a comment.
	return LineType::COMMENT;
}

void SceneFileLoadHelper::DealWithComment(void) {
	return;		// Leave blank, don't process comments - move on to the next line.
}

bool SceneFileLoadHelper::DealWithModel(std::istringstream &p_ISSModelInfo, std::string &p_Name, std::string &p_Group, std::string &p_ModelLocation,
	std::string &p_TextureLocation, bool &p_Mipmaps, glm::vec3 &p_Position, glm::vec3 &p_Rotation, glm::vec3 &p_Scale) {
	const static unsigned int requiredPiecesOfInfo(5);
	std::string currentString("");

	// Defaults for rotation, scale, and group.
	p_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	p_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	p_Group = "Default";

	int requiredPiecesOfInfoCounter(0);
	while (p_ISSModelInfo >> currentString) {		// While there's information on the current line, to process, process it.
		if (currentString == "name-" || currentString == "Name-") {
			if (p_ISSModelInfo >> p_Name)
				requiredPiecesOfInfoCounter++;
			else
				Log::AppendToLogFile("Couldn't find the name of the model.");
		}
		else if (currentString == "group-" || currentString == "Group-") {
			if (p_ISSModelInfo >> p_Group)
				;
			else
				Log::AppendToLogFile("No group given, using Default.");
		}
		else if (currentString == "modelLocation-" || currentString == "ModelLocation-") {
			if (p_ISSModelInfo >> p_ModelLocation)
				requiredPiecesOfInfoCounter++;
			else 
				Log::AppendToLogFile("No model location was given.");
		}
		else if (currentString == "textureLocation-" || currentString == "TextureLocation-") {
			if (p_ISSModelInfo >> p_TextureLocation)
				requiredPiecesOfInfoCounter++;
			else 
				Log::AppendToLogFile("No Texture location was given.");
		}
		else if (currentString == "mipmaps-" || currentString == "Mipmaps-") {
			std::string mipmaps;
			if (p_ISSModelInfo >> mipmaps) {
				if (mipmaps == "true" || mipmaps == "True" || mipmaps == "yes" || mipmaps == "Yes")
					p_Mipmaps = true;
				else
					p_Mipmaps = false;
				requiredPiecesOfInfoCounter++;
			}
			else
				Log::AppendToLogFile("No mipmaps information was given.");
		}
		else if (currentString == "position-" || currentString == "Position-") {
			ReadVec3(p_ISSModelInfo, p_Position);
			requiredPiecesOfInfoCounter++;
		}
		else if (currentString == "rotation-" || currentString == "Rotation-") {
			ReadVec3(p_ISSModelInfo, p_Rotation);
		}
		else if (currentString == "scale-" || currentString == "Scale-") {
			ReadVec3(p_ISSModelInfo, p_Scale, 1.0f);
		}
	}

	// Make sure there's enough information to create a model - if there is return true.
	if (requiredPiecesOfInfoCounter == requiredPiecesOfInfo)
		return true;

	return false;
}

bool SceneFileLoadHelper::DealWithGroupLighting(std::istringstream &p_ISSModelInfo, std::string &p_Group, float &p_SpecularIntensity, float &p_LightExponent) {
	const static unsigned int requiredPiecesOfInfo(2);

	p_Group = "NA";
	p_SpecularIntensity = 1;
	p_LightExponent = 1000;

	std::string currentString("");

	int requiredPiecesOfInfoCounter(0);
	while (p_ISSModelInfo >> currentString) {
		if (currentString == "group-" || currentString == "Group-") {
			if (p_ISSModelInfo >> p_Group)
				requiredPiecesOfInfoCounter++;
			else
				Log::AppendToLogFile("Couldn't find the group- you wanted to modify.");
		}
		else if (currentString == "specularIntensity-" || currentString == "SpecularIntensity-") {
			if (p_ISSModelInfo >> p_SpecularIntensity) {
				requiredPiecesOfInfoCounter++;
			}
			else 
				Log::AppendToLogFile("Couldn't find the specularIntensity- value.");
		}
		else if (currentString == "specularExponent- " || currentString == "specularExponent- ") {
			if (p_ISSModelInfo >> p_LightExponent) {
				requiredPiecesOfInfoCounter++;
			}
			else 
				Log::AppendToLogFile("Couldn't find the specularExponent- value.");
		}
	}

	if (requiredPiecesOfInfoCounter >= requiredPiecesOfInfo)
		return true;

	return false;
}