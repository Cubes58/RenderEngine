#pragma once

#include "Win32OpenGL.h"

// The different line types.
enum class LineType : unsigned int {
	COMMENT,
	MODEL,
	LIGHTING
};

class SceneFileLoadHelper {
private:
	static void ReadVec3(std::istringstream &p_ISSModelInfo, glm::vec3 &p_Vec, float p_DefaultErrorValue = 0.0f);
public:
	static LineType GetLineType(const std::string &p_LineType);

	static void DealWithComment(void);

	static bool DealWithModel(std::istringstream &p_ISSModelInfo, std::string &p_Name, std::string &p_Group, std::string &p_ModelLocation,
		std::string &p_TextureLocation, bool &p_Mipmaps, glm::vec3 &p_Position, glm::vec3 &p_Rotation, glm::vec3 &p_Scale);

	static bool DealWithGroupLighting(std::istringstream &p_ISSModelInfo, std::string &p_Group, float &p_SpecularIntensity, float &p_LightExponent);
};