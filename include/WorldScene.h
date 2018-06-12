#pragma once
//  for M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>

#include "Win32OpenGL.h"
#include "resource.h"
#include "Log.h"

#include "ModelManager.h"
#include "Camera.h"
#include "Skybox.h"
#include "Light.h"
#include "Shadows.h"

class WorldScene {
	// OpenGL helper class.
	Win32OpenGL m_win32OpenGL;

	std::unique_ptr<Light> m_Light;
	Camera m_Camera;
	Skybox m_Skybox;
	std::unique_ptr<ModelManager> m_ModelManager;

	/* Shaders. */
	GLuint m_ModelShader;
	GLuint m_SkyboxShader;
	//GLuint m_SimpleDepthShader;

	glm::mat4 m_ProjectionMatrix;

	float m_aspectRatio;
public:
	WorldScene();
	~WorldScene();

	void CreateGLWindow(HWND hWnd, RECT rect);
	void DestroyGLWindow();

	void PrepareToDraw();
	void Draw();

	// Passed a function pointer, for key pressed.
	void Update(bool(*p_IsKeyPressedPointer)(unsigned char p_KeyCode));

	void HandleInput(unsigned char virtualKeyCode);
	void HandleMouseInput(int xPos, int yPos);

	void Resize(HDC hdc, RECT rect);
	void ComputeProjectionMatrix();
	bool SimpleLoadTexture(string fileName, char* imageData);
};