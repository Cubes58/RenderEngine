#include "stdafx.h"
#include "ModelLoader.h"

#include <exception>
#include <string>

#include "Log.h"

ModelLoader::ModelLoader() {

}

ModelLoader::~ModelLoader() {

}

void ModelLoader::HandleFileExtension(std::string &p_FileName, const std::string &p_FileExtension) {	
	// If a dot has been found assume it's a file extension - either .obj or .bin
	// Because they're both 4 characters long, and the only types used replace 4 characters with the requested file extension passed in.
	// If there's no file extension then add the one passed in.

	size_t posFound = p_FileName.find(".");	
	if (posFound < p_FileName.size())		
		p_FileName.replace(p_FileName.find("."), p_FileExtension.length(), p_FileExtension);
	else
		 p_FileName.append(p_FileExtension);
}

bool ModelLoader::LoadInformationFromObjFile(std::string p_ModelName) {
	HandleFileExtension(p_ModelName, ".obj");
	
	std::fstream modelfile(p_ModelName, std::ios_base::in);

	if (!modelfile) {
		Log::AppendToLogFile("Cannot find the model file: " + p_ModelName);
		return false;
	}

	try {
		std::string line = "";
		if (modelfile.is_open()) {
			while (getline(modelfile, line)) {
				std::string s;
				std::istringstream iss(line);

				iss >> s; // Read to first whitespace
				if (s == "f") {
					// Process face line
					unsigned int faceIndex;

					while (iss >> faceIndex) {
						faceIndex--;
						m_VerticesIndices.push_back(faceIndex);

						static const int forwardSlash = 0x2F;
						int lookAhead = iss.peek();  	// Peek character
						if (lookAhead == forwardSlash) {
							iss.get();

							lookAhead = iss.peek();
							// Normals
							if (lookAhead == forwardSlash) {
								iss.get();

								unsigned int normalIndex;
								iss >> normalIndex;
								normalIndex--;
								m_UnitNormalIndices.push_back(normalIndex);
							}
							else // Textures coordinates + Normals
							{
								// Get texture
								unsigned int textureCoordinateIndex;
								iss >> textureCoordinateIndex;
								textureCoordinateIndex--;
								m_TextureUVIndices.push_back(textureCoordinateIndex);

								// Remove the slashes
								lookAhead = iss.peek();
								while (lookAhead == forwardSlash) {
									iss.get();
									lookAhead = iss.peek();
								}
								// Get normals
								unsigned int normalIndex;
								iss >> normalIndex;
								normalIndex--;
								m_UnitNormalIndices.push_back(normalIndex);
							}
						}
						else {
							// Vertices: 
							// Already done (At the top - m_VerticesIndices.push_back(faceIndex);)
						}
					}
				}
				else if (s == "v") {
					float indexedVertices;
					while (iss >> indexedVertices) {
						m_IndexedVertices.push_back(indexedVertices);
					}
				}
				else if (s == "vn") {
					float indexedUnitNormals;
					while (iss >> indexedUnitNormals) {
						m_IndexedUnitNormals.push_back(indexedUnitNormals);
					}
				}
				else if (s == "vt") {
					float indexedTextureCoordinates;
					while (iss >> indexedTextureCoordinates) {
						m_IndexedTextureUVs.push_back(indexedTextureCoordinates);
					}
				}
			}
			modelfile.close();
		}
	}
	catch (const std::runtime_error &rumtimeError) {
		std::string rumtimeErrorInfo = rumtimeError.what();
		Log::AppendToLogFile("An error occured, while saving the information to the binary file: " + p_ModelName);
		Log::AppendToLogFile("Standard runtime error was caught, with the message: " + rumtimeErrorInfo);

		if (modelfile.is_open())	// If the file is still open - close it.
			modelfile.close();

		return false;
	}
	catch (const std::exception &exception) {
		std::string exceptionInfo = exception.what();		// Inform the user of the issues, by logging it.
		Log::AppendToLogFile("An exception has occured, while loading information from the binary file: " + p_ModelName);
		Log::AppendToLogFile("Standard exception was caught, with the message: " + exceptionInfo);

		if (modelfile.is_open())
			modelfile.close();

		return false;
	}

	if (m_IndexedVertices.size() <= 0 || m_IndexedUnitNormals.size() <= 0 || m_IndexedTextureUVs.size() <= 0) {
		Log::AppendToLogFile("The model obj file: " + p_ModelName + " doesn't have m_IndexedVertices, m_IndexedUnitNormals, or m_IndexedTextureUVs");
		return false;
	}

	return true;
}

bool ModelLoader::SetModelInformationUsingObjFile(std::string p_ModelName) {
	const int pointIndexOffset = 3;	// Each point has an X, Y, and Z
	// Vertices
	for (auto vertexIndex : m_VerticesIndices) {
		vertexIndex *= pointIndexOffset;
		for (unsigned int point = vertexIndex; point < vertexIndex + pointIndexOffset; ++point)
			m_Vertices.push_back(m_IndexedVertices[point]);
	}

	// Unit normals.
	for (auto unitNormalIndex : m_UnitNormalIndices) {
		unitNormalIndex *= pointIndexOffset;
		for (unsigned int normal = unitNormalIndex; normal < unitNormalIndex + pointIndexOffset; ++normal)
			m_UnitNormals.push_back(m_IndexedUnitNormals[normal]);
	}

	// TextureUVs come in twos.
	const int pointIndexOffsetT = 2;
	for (auto textureCoordinateIndex : m_TextureUVIndices) {
		textureCoordinateIndex *= pointIndexOffsetT;
		for (unsigned int textureCoordinate = textureCoordinateIndex; textureCoordinate < textureCoordinateIndex + pointIndexOffsetT; ++textureCoordinate)
			m_TextureUVs.push_back(m_IndexedTextureUVs[textureCoordinate]);
	}

	// Check to make sure there's enough information in the model loaded to draw it, 
	// using the current system we use (model must have vertices, normals and texturs).
	if (m_Vertices.size() <= 0 || m_UnitNormals.size() <= 0 || m_TextureUVs.size() <= 0) {
		Log::AppendToLogFile("Model didn't have any Vertices, Normals or TextureUVs. Check the model .obj " + p_ModelName);
		return false;
	}

	return SaveModelObjAsBinary(p_ModelName);
}

bool ModelLoader::LoadModelObj(std::string p_ModelName) {
	if (LoadInformationFromObjFile(p_ModelName))
		return SetModelInformationUsingObjFile(p_ModelName);

	return false;
}

bool ModelLoader::SaveModelObjAsBinary(std::string p_FileName) {
	HandleFileExtension(p_FileName, ".bin");

	std::ofstream file(p_FileName, std::ios::binary | std::ios::out);
	try {
		if (!file) {
			Log::AppendToLogFile("Failed to save Binary file: " + p_FileName);
			return false;
		}

		// The size of each vector.
		size_t sizeOfVertices(sizeof(float) * m_Vertices.size());
		size_t sizeOfNormals(sizeof(float) * m_UnitNormals.size());
		size_t sizeOfTextureCoordinates(sizeof(float) * m_TextureUVs.size());

		// Store the size of the vectors.
		file.write((char*)&sizeOfVertices, sizeof(size_t));
		file.write((char*)&sizeOfNormals, sizeof(size_t));
		file.write((char*)&sizeOfTextureCoordinates, sizeof(size_t));

		// Store the vectors.
		file.write((char*)&m_Vertices[0], sizeOfVertices);
		file.write((char*)&m_UnitNormals[0], sizeOfNormals);
		file.write((char*)&m_TextureUVs[0], sizeOfTextureCoordinates);

		// Close the file.
		file.close();
	}
	catch (const std::runtime_error &rumtimeError) {
		std::string rumtimeErrorInfo = rumtimeError.what();
		Log::AppendToLogFile("An error occured, while saving the information to the binary file: " + p_FileName);
		Log::AppendToLogFile("Standard runtime error was caught, with the message: " +  rumtimeErrorInfo);

		if (file.is_open())	// If the file is still open - close it.
			file.close();

		return false;
	}
	catch (const std::exception &exception) {
		std::string exceptionInfo = exception.what();
		Log::AppendToLogFile("An exception has occured, while saving the information to the binary file: " + p_FileName);
		Log::AppendToLogFile("Standard exception was caught, with the message: " + exceptionInfo);

		if (file.is_open())
			file.close();

		return false;
	}

	return true;
}

bool ModelLoader::LoadModelBinary(std::string p_FileName) {
	HandleFileExtension(p_FileName, ".bin");

	std::ifstream file(p_FileName, std::ios::binary | std::ios::in);
	/* Try to load the model, if an error occures, catch it and inform the user, using the log file, while trying to recover, and closing the file. */
	try {
		if (!file) {
			Log::AppendToLogFile("Failed to open Binary file: " + p_FileName);
			return false;
		}

		// The size of each vector.
		size_t sizeOfVertices;
		size_t sizeOfNormals;
		size_t sizeOfTextureCoordinates;

		// Read in the size of each vector.
		file.read((char*)&sizeOfVertices, sizeof(size_t));
		file.read((char*)&sizeOfNormals, sizeof(size_t));
		file.read((char*)&sizeOfTextureCoordinates, sizeof(size_t));

		// Resize the vectors to the number of elements they will hold, so the data can be read straight in.
		m_Vertices.resize(sizeOfVertices / sizeof(float));
		m_UnitNormals.resize(sizeOfNormals / sizeof(float));
		m_TextureUVs.resize(sizeOfTextureCoordinates / sizeof(float));

		// Read in the the information into vectors.	
		file.read((char*)&m_Vertices[0], sizeOfVertices);
		file.read((char*)&m_UnitNormals[0], sizeOfNormals);
		file.read((char*)&m_TextureUVs[0], sizeOfTextureCoordinates);

		// Close the file.
		file.close();
	}
	catch (const std::runtime_error &rumtimeError) {
		std::string rumtimeErrorInfo = rumtimeError.what();
		Log::AppendToLogFile("An error occured, while loading the information from the binary file: " + p_FileName);
		Log::AppendToLogFile("Standard runtime error was caught, with the message: " + rumtimeErrorInfo);

		if (file.is_open())
			file.close();

		return false;
	}
	catch (const std::exception &exception) {
		std::string exceptionInfo = exception.what();		// Inform the user of the issues, by logging it.
		Log::AppendToLogFile("An exception has occured, while loading information from the binary file: " + p_FileName);
		Log::AppendToLogFile("Standard exception was caught, with the message: " + exceptionInfo);

		if (file.is_open())
			file.close();

		return false;
	}

	// The model loaded successfully.
	return true;
}

std::vector<float>& ModelLoader::GetVertices() {
	return m_Vertices;
}

std::vector<float>& ModelLoader::GetNormals() {
	return m_UnitNormals;
}

std::vector<float>& ModelLoader::GetTextureUVs() {
	return m_TextureUVs;
}