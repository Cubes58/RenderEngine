#pragma once
//#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class ModelLoader {
private:
	std::vector<float> m_IndexedVertices;
	std::vector<float> m_IndexedUnitNormals;
	std::vector<float> m_IndexedTextureUVs;

	std::vector<unsigned int> m_VerticesIndices;
	std::vector<unsigned int> m_UnitNormalIndices;
	std::vector<unsigned int> m_TextureUVIndices;

	std::vector<float> m_Vertices;
	std::vector<float> m_UnitNormals;
	std::vector<float> m_TextureUVs;

	void HandleFileExtension(std::string &p_FileName, const std::string &p_FileExtension);

	bool LoadInformationFromObjFile(std::string p_ModelName);
	bool SetModelInformationUsingObjFile(std::string p_ModelName);

	bool SaveModelObjAsBinary(std::string p_FileName);
public:
	ModelLoader();
	~ModelLoader();

	bool LoadModelObj(std::string p_ModelName);
	bool LoadModelBinary(std::string p_FileName);

	std::vector<float>& GetVertices();
	std::vector<float>& GetNormals();
	std::vector<float>& GetTextureUVs();
};