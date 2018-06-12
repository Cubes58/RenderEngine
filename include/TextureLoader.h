#pragma once

#include "Win32OpenGL.h"

class TextureLoader {
private:
	// No objects - only static methods
	TextureLoader();
	~TextureLoader();
public:
	static int LoadBMP(const string location, GLuint &texture, bool withMipMaps);
	static GLuint LoadCubeMap(vector<string> p_SkyBoxFaces);
};

