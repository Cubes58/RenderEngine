#include "stdafx.h"
#include "WorldScene.h"

#include "TextureLoader.h"
#include "P16204361_Airport.h"

WorldScene::WorldScene() {
	// we don't use RAII currently so no action in constructor
}

WorldScene::~WorldScene() {

}

void WorldScene::CreateGLWindow(HWND hWnd, RECT rect) {
	m_win32OpenGL.CreateGLContext(hWnd);	// may throw an exception!!!
	// MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
	m_aspectRatio = static_cast<float>(rect.right / rect.bottom);

	// Shaders. 
	m_win32OpenGL.CreateShadersAndProgram("phongTextureScaled2", m_ModelShader);
	m_win32OpenGL.CreateShadersAndProgram("skybox", m_SkyboxShader);
	//m_win32OpenGL.CreateShadersAndProgram("simpleDepth", m_SimpleDepthShader);

	m_win32OpenGL.SetupDisplay();
	m_win32OpenGL.GetError();			// Check all is ok
}

void WorldScene::DestroyGLWindow() {
	m_win32OpenGL.TearDownGLContext();
}

void WorldScene::PrepareToDraw() {
	ComputeProjectionMatrix();

	m_Camera.ComputeDirectionVector();
	m_Camera.ComputeViewMatrixUsingLookAt();

	// Send the matrices to the shader.
	m_Camera.SetViewMatrix(m_ModelShader);
	Win32OpenGL::SendUniformMatrixToShader(m_ModelShader, m_ProjectionMatrix, "projection_matrix");
	
	// Create the light, now OpenGL is set up, and send the uniform variables to the shader.
	m_Light = std::unique_ptr<Light>(new Light(&m_win32OpenGL, m_ModelShader));
	m_Light->PrepareLight();

	// Prepare Skybox.
	m_Skybox.Prepare();

	// Create a model manager, to create/store/manage models.
	m_ModelManager = std::unique_ptr<ModelManager>(new ModelManager(&m_win32OpenGL));

	// Prepare the models for drawing.
	m_ModelManager->PrepareModels();
}

	
void WorldScene::Draw() {
	m_win32OpenGL.ClearGLDisplay();

	// Draw the skybox.
	m_Skybox.Draw(m_SkyboxShader, m_Camera.GetViewMatrix(), m_ProjectionMatrix);

	// Draw the light (sphere around the light).
	m_Light->Draw(m_ModelShader);

	// Draw the models, stored in the model manager.
	m_ModelManager->DrawModels(m_ModelShader, *m_Light);

	m_win32OpenGL.FinishedDrawing();
}

void WorldScene::Update(bool(*p_IsKeyPressedPointer)(unsigned char p_KeyCode)) {
	// Constantly update the movement, with the keys pressed.
	if (p_IsKeyPressedPointer(VK_LEFT) || p_IsKeyPressedPointer(0x41)) 		// 0x41 = A
		HandleInput(VK_LEFT);
	if (p_IsKeyPressedPointer(VK_RIGHT) || p_IsKeyPressedPointer(0x44)) 	// 0x44 = D
		HandleInput(VK_RIGHT);
	if (p_IsKeyPressedPointer(VK_UP) || p_IsKeyPressedPointer(0x57)) 		// 0x57 = W
		HandleInput(VK_UP);
	if (p_IsKeyPressedPointer(VK_DOWN) || p_IsKeyPressedPointer(0x53)) 		// 0x53 = S
		HandleInput(VK_DOWN);

	// Add and subtract keys to move up/down.
	if (p_IsKeyPressedPointer(VK_ADD))
		HandleInput(VK_ADD);
	if (p_IsKeyPressedPointer(VK_SUBTRACT))
		HandleInput(VK_SUBTRACT);

	// Movement speed boost.
	if (p_IsKeyPressedPointer(VK_SHIFT))
		HandleInput(VK_SHIFT);
}

void WorldScene::HandleInput(unsigned char p_VirtualKeyCode) {
	m_Camera.HandleInput(p_VirtualKeyCode);	
	m_Camera.SetViewMatrix(m_ModelShader);
}

void WorldScene::HandleMouseInput(int p_XPosition, int p_YPosition) {
	m_Camera.HandleMouseInput(p_XPosition, p_YPosition);
	m_Camera.SetViewMatrix(m_ModelShader);
}

void WorldScene::Resize(HDC hdc, RECT rect) {
	// if the display is resized then OpenGL needs to know about the apect ratio
	// to compute the correct projection matrix
	m_win32OpenGL.Reshape(hdc, rect.right, rect.bottom);
	m_aspectRatio = (float)rect.right / rect.bottom;
	ComputeProjectionMatrix();
	Win32OpenGL::SendUniformMatrixToShader(m_ModelShader, m_ProjectionMatrix, "projection_matrix");
}

void WorldScene::ComputeProjectionMatrix() {
	// we will look at this later in the course
	// in Modern OpenGL we must supply a projection matrix
	const float oneDegreeInRadians = static_cast<float>((2.0 * M_PI) / 360.0);
	//ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

	// input variables
	float zNear = 0.1f;		// clipping plane
	float zFar = 1400.0f;	// clipping plane
	float fov = static_cast<float>(67.0f * oneDegreeInRadians); // convert 67 degrees to radians

	float range = tan(fov * 0.5f) * zNear;
	float Sx = (2.0f * zNear) / (range * m_aspectRatio + range * m_aspectRatio);
	float Sy = zNear / range;
	float Sz = -(zFar + zNear) / (zFar - zNear);
	float Pz = -(2.0f * zFar * zNear) / (zFar - zNear);
	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};
	m_ProjectionMatrix = glm::make_mat4(proj_mat);
}

bool WorldScene::SimpleLoadTexture(string fileName, char *imageData) {
	ifstream texFile;
	texFile.open(fileName, ios::binary);
	if (!texFile) {
		std::cout << "Error: Opening texture" << fileName << std::endl;
		return false;
	}
	texFile.read(imageData, 256 * 256 * 3);
	texFile.close();

	return true;
}