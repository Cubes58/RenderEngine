/*

	Win32OpenGL.h

*/
#pragma once
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

// TODO: reference additional headers your program requires here
//#define GL_GLEXT_PROTOTYPES 1		// ALLOW USE OF OPEN GL 1.5+ functionality!!!
//--- OpenGL --- 
// we don't want to have to use the glew.dll so use static and link to glews.lib
#define GLEW_STATIC

#include "glew.h"
#include "wglew.h"

#include <windows.h>		//before gl.h
#include <GL/GL.h>

// Include GLM core features
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
// Include GLM extensions
#include <glm/gtc/matrix_transform.hpp>
// allow access to floats
#include <glm/gtc/type_ptr.hpp>

// c  and stl libraries
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Log.h"

//  for M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;	// is this a good idea?

class Win32OpenGL
{
public:

	Win32OpenGL();
	virtual ~Win32OpenGL();

	//bool Initialised();

	bool CreateGLContext(HWND hWnd);	// HDC hdc); 	// Creates OpenGL Rendering Context
	void TearDownGLContext();
	bool CreateShadersAndProgram(string baseName);

	// static version can be called by a shader manager
	static bool CreateShadersAndProgram(string baseName, GLuint& program);
	static void DeleteVertexAndFragmentShaders(GLuint m_program, GLuint vertexShader, GLuint fragmentShader);
	static void UseProgram(GLuint program);

	void Reshape(HDC hdc, int w, int h);	// Changing viewport

	GLuint GetShaderProgram();		//
	
	static void SendUniformFloatToShader(GLuint program, GLfloat floatData, std::string uniformName);
	static void SendUniformMatrixToShader(GLuint program, glm::mat4 matrix, std::string uniformName);
	static void SendUniformVector3ToShader(GLuint program, glm::vec3 vector, std::string uniformName);
	static void CreateVAO(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals, vector<float>& vertices, vector<float>& normals);
	static void CreateVAO(GLuint& vao, GLuint& vboVertices, vector<float>& vertices);					// only vertices!
	static void CreateVAOWithTextures(GLuint&, GLuint&, GLuint&, GLuint&, vector<float>&, vector<float>& , vector<float>&);
	
	
	
	static void SetupDisplay();
	static void ClearGLDisplay();
	static void Draw(GLuint vao, int numberOfElements);
	static void DrawLines(GLuint vao, int numberOfElements);
	static void FinishedDrawing();
	static void DestroyVAO(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals);
	static void DestroyVAO(GLuint& vao, GLuint& vboVertices);
	static void DestroyVAOWithTextures(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals, GLuint& vboTextures);
	static string GetError();

protected:
	// shader stuff
	static char* ParseFileIntoString(std::string file_name);
	
	static bool CreateShader(const char* file_name, GLuint* shader, GLenum type);
	static void LogShaderInfo(GLuint shader_index, GLenum type);

	static bool CreateProgram(GLuint vert, GLuint frag, GLuint* program);
	static bool IsProgramValid(GLuint sp);
	static void LogProgramInfo(GLuint sp);

protected:
	HGLRC m_hrc{ nullptr };             // OpenGL Rendering Context 

	GLuint m_program;					// the shader program
};

