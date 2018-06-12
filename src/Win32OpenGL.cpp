/*

	Win32OpenGL.cpp

*/

/* 

Win32OpenGL.cpp provides a group of support functions for interacting with open gl

In essence this is like a small library except the code is visible and you can see how it works

*/

#include "stdafx.h"
#include "Win32OpenGL.h"
//#include <glm/glm.hpp>  // if transformations needed

Win32OpenGL::Win32OpenGL()
{
	// nothing yet in the constructor
}

Win32OpenGL::~Win32OpenGL()
{

}

#pragma region creation

bool Win32OpenGL::CreateGLContext(HWND hWnd)		// use HWND not HDC so we can generate message boxes
{
	HDC hdc = GetDC(hWnd);
	// ask for a pixel format
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (nPixelFormat == 0 )
	{
		MessageBoxA(hWnd, "Can't get Pixel format", "Error", MB_OK);
		return false;
	}

	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);
	if (!bResult)
	{
		MessageBoxA(hWnd, "Can't set Pixel Format", "Error", MB_OK);
		return false;
	}
	
	HGLRC tempContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBoxA(hWnd, "Glew init failed", "Error", MB_OK);
		Log::AppendToLogFile("Glew init failed");
		return false;
	}

	// we want open gl 4.x for the shader code
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		m_hrc = wglCreateContextAttribsARB(hdc, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hdc, m_hrc);
	}
	else
	{	//It's not possible to make a GL 4.x context. Use the old style context (GL 2.1 and before)
		//throw new exception("Can't setup Open GL 4 - falling back to 2.1");
		m_hrc = tempContext;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
#if 0
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
#endif
	if (!m_hrc)
	{
		return false;
	}
	return true;
}

void Win32OpenGL::TearDownGLContext()
{
	// we delete the shaders and enclosing program
	// then remove the context

	if (m_program)
	{
		//glDetachShader(m_program, m_vertexShader);
		//glDetachShader(m_program, m_fragmentShader);

		//glDeleteShader(m_vertexShader);
		//glDeleteShader(m_fragmentShader);
		glDeleteProgram(m_program);
	}
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void Win32OpenGL::Reshape(HDC hdc, int w, int h)
{
	if (m_hrc)
	{
		glViewport(0, 0, w, h);
	}
}

#pragma endregion

#pragma region shaders

//non static version - use if no shader manager

bool Win32OpenGL::CreateShadersAndProgram(string baseName)
{
	return Win32OpenGL::CreateShadersAndProgram(baseName, m_program);
}


// static version
// revised for 2017-18 - no class variables for vertex and frag shaders - only local as cleared by end
bool Win32OpenGL::CreateShadersAndProgram(string baseName, GLuint& program)
{
	// local vars for vertex and frag shaders
	GLuint vertexShader{ 0 };
	GLuint fragmentShader{ 0 };

	string vertexShaderName = "Shaders\\";
	vertexShaderName += baseName;
	vertexShaderName += ".vert";

	if (!CreateShader(vertexShaderName.c_str(), &vertexShader, GL_VERTEX_SHADER))
	{
		// tidy up
		DeleteVertexAndFragmentShaders(program, vertexShader, fragmentShader);
		return false;
	}

	string fragmentShaderName = "Shaders\\";
	fragmentShaderName += baseName;
	fragmentShaderName += ".frag";

	if (!CreateShader(fragmentShaderName.c_str(), &fragmentShader, GL_FRAGMENT_SHADER))
	{
		// tidy up
		DeleteVertexAndFragmentShaders(program, vertexShader, fragmentShader);
		return false;
	}

	if (!CreateProgram(vertexShader, fragmentShader, &program))
	{
		// tidy up
		DeleteVertexAndFragmentShaders(program, vertexShader, fragmentShader);
		return false;
	}
	glLinkProgram(program);
	glUseProgram(program);
	// tidy up
	GetError();
	DeleteVertexAndFragmentShaders(program, vertexShader, fragmentShader);
	GetError();
	return true;
}


void Win32OpenGL::DeleteVertexAndFragmentShaders(GLuint m_program, GLuint vertexShader, GLuint fragmentShader)
{
	if (vertexShader != 0)
	{
		glDetachShader(m_program, vertexShader);
		//glDeleteShader(vertexShader); no - generates error
		vertexShader = 0;
	}
	if (fragmentShader != 0)
	{
		glDetachShader(m_program, fragmentShader);
		//glDeleteShader(fragmentShader); no - generates error
		fragmentShader = 0;
	}
}

void Win32OpenGL::UseProgram(GLuint program)
{
	if (program == 0)
	{
		Log::AppendToLogFile("Can't use shader 0)");
		DebugBreak();
	}
	glUseProgram(program);
}

/*-----------------------------------SHADERS----------------------------------*/

char* Win32OpenGL::ParseFileIntoString(std::string filename)
{
	char* shaderString = nullptr;

	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		std::ostringstream oss;
		oss << "Failed to open shader file: " << filename;
		Log::AppendToLogFile(oss.str());
		std::cerr << oss.str();
		//throw new std::runtime_error("File not found");
		// or test for nullptr
		DebugBreak();
	}

	file.seekg(0, std::ios::end);    // go to the end
	int length = static_cast<int>(file.tellg());        // report location (this is the length)
	file.seekg(0, std::ios::beg);						// go back to the beginning
	shaderString = new char[length + 1];				// allocate memory for a buffer with a trailing /0
	file.read(shaderString, length);					// read the whole file into the buffer

	shaderString[length] = '\0';						// append a trailing /0

	file.close();										// close file handle
	return shaderString;
}


bool Win32OpenGL::CreateShader(const char* fileName, GLuint* shader, GLenum type) 
{
	// log this
	std::ostringstream oss;
	oss << "Creating shader: " << fileName;
	Log::AppendToLogFile(oss.str());

	char* shaderString = ParseFileIntoString(fileName);

	if (shaderString == nullptr)
	{
		Log::AppendToLogFile("Shader not found");
		//throw new std::runtime_error("Shader not found");
		return false;
	}
	*shader = glCreateShader(type);
	const GLchar* p = (const GLchar*)shaderString;
	glShaderSource(*shader, 1, &p, NULL);
	glCompileShader(*shader);
	// check for compile errors
	int parameters = GL_FALSE;			//
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &parameters);

	if (GL_TRUE != parameters) 
	{
		Log::AppendToLogFile("Shader compilation failed");
		LogShaderInfo(*shader, type);
		//throw new std::runtime_error("Shader compilation failed");
		return false;
	}
	Log::AppendToLogFile("Shader compiled");
	//tidy up....

	if (shaderString != nullptr)
	{
		delete[] shaderString;
	}
	return true;
}

void Win32OpenGL::LogShaderInfo(GLuint shaderIndex, GLenum type)
{
	int max_length = 2048;
	int actualLength = 0;
	char errorMessage[2048];
	glGetShaderInfoLog(shaderIndex, max_length, &actualLength, errorMessage);

	ostringstream oss;

	if (type == GL_VERTEX_SHADER)
	{
		oss << "Error in Vertex Shader" << endl;
	}
	else if (type == GL_FRAGMENT_SHADER)
	{
		oss << "Error in Fragment Shader" << endl;
	}
	oss << errorMessage << endl;
	Log::AppendToLogFile(oss.str());
}



bool Win32OpenGL::CreateProgram(GLuint vert, GLuint frag, GLuint* program)
{
	*program = glCreateProgram();
	glAttachShader(*program, vert);
	glAttachShader(*program, frag);
	// link the shader programme. if binding input attributes do that before link
	glLinkProgram(*program);
	GLint parameters = GL_FALSE;
	glGetProgramiv(*program, GL_LINK_STATUS, &parameters);
	if (GL_TRUE != parameters) 
	{
		Log::AppendToLogFile("Error - could not link shader program");
		throw new std::runtime_error("Shader link failed");
		return false;
	}
	if (!IsProgramValid(*program))
	{
		Log::AppendToLogFile("Error - Program not valid");
		throw new std::runtime_error("Program not valid");
		return false;

	}
	// delete shaders here to free memory
	glDeleteShader(vert);
	glDeleteShader(frag);
	Log::AppendToLogFile("Program created");
	return true;
}

bool Win32OpenGL::IsProgramValid(GLuint sp) {
	glValidateProgram(sp);
	GLint parameters = GL_FALSE;
	glGetProgramiv(sp, GL_VALIDATE_STATUS, &parameters);
	if (GL_TRUE != parameters)
	{
		Log::AppendToLogFile("GL_VALIDATE_STATUS == GL_FALSE for program");
		return false;
	}
	Log::AppendToLogFile("Program valid");
	return true;
}

void Win32OpenGL::LogProgramInfo(GLuint sp)
{
	int max_length = 2048;
	int actual_length = 0;
	char errorMessage[2048];
	glGetProgramInfoLog(sp, max_length, &actual_length, errorMessage);

	ostringstream oss;
	oss << "Program link failed" << endl;
	oss << errorMessage << endl;
	Log::AppendToLogFile(oss.str());
}



GLuint Win32OpenGL::GetShaderProgram()
{
	return m_program;
}


void Win32OpenGL::SendUniformFloatToShader(GLuint program, GLfloat floatData, std::string uniformName)
{
	glUseProgram(program);
	int vec3Location = glGetUniformLocation(program, uniformName.c_str());

	if (vec3Location != -1)
	{
		glUniform1fv(vec3Location, 1, &floatData);
	}
	else
	{
		// we don't treat this as an error as different shaders can be sent different uniforms!!!!!
		// this is documented as safe... if the uniform isn't in the shader we can exit

		return;
		// don't write to log here unless we are exiting - otherwise we write to log at 50fps!!!!
		std::ostringstream oss;
		oss << "Warning: Uniform float: " << uniformName << "not found in shader";
		Log::AppendToLogFile(oss.str());
		//throw std::invalid_argument(oss.str());
	}
}
// send matrix data to shader as uniform.

void  Win32OpenGL::SendUniformMatrixToShader(GLuint program, glm::mat4 matrix, std::string uniformName)
{
	glUseProgram(program);
	int matrixLocation = glGetUniformLocation(program, uniformName.c_str());

	if (matrixLocation != -1)
	{
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	else
	{
		std::ostringstream oss;
		oss << "Uniform matrix: " << uniformName << " not found in shader";
		Log::AppendToLogFile(oss.str());
		throw new std::runtime_error(oss.str());
	}
}

void  Win32OpenGL::SendUniformVector3ToShader(GLuint program, glm::vec3 vector, std::string uniformName)
{
	glUseProgram(program);
	int vec3Location = glGetUniformLocation(program, uniformName.c_str());

	if (vec3Location != -1)
	{
		glUniform3fv(vec3Location, 1, glm::value_ptr(vector));
	}
	else
	{
		// we don't treat this as an error as different shaders can be sent different uniforms!!!!!
		// this is documented as safe... if the uniform isn't in the shader we can exit

		return;
		// don't write to log here unless we are exiting - otherwise we write to log at 50fps!!!!
		std::ostringstream oss;
		oss << "Warning: Uniform vec3: " << uniformName << "not found in shader";
		Log::AppendToLogFile(oss.str());
		//throw new std::runtime_error(oss.str());
	}
}

#pragma endregion

#pragma region vbo_and_vao

void Win32OpenGL::CreateVAOWithTextures(
	GLuint& vao, GLuint& vboVertices, GLuint& vboNormals, GLuint& vboTextures, 
	vector<float>& vertices, vector<float>& normals, vector<float>& textureCoordinates
	)
{
	//GLuint vboVertices;
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof (GLfloat), &vertices[0], GL_STATIC_DRAW);

	//GLuint vboNormals;
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof (GLfloat), &normals[0], GL_STATIC_DRAW);

	//GLuint vboTextures;
	glGenBuffers(1, &vboTextures);
	glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
	glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof (GLfloat), &textureCoordinates[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO
	// normals element 1
	// textures element 2
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Win32OpenGL::CreateVAO(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals, vector<float>& vertices, vector<float>& normals)
{
	//GLuint vboVertices;
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof (GLfloat), &vertices[0], GL_STATIC_DRAW);

	//GLuint vboNormals;
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof (GLfloat), &normals[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO
	// normals element 1
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
} 


void Win32OpenGL::CreateVAO(GLuint& vao, GLuint& vboVertices, vector<float>& vertices)
{
	//GLuint vboVertices;
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof (GLfloat), &vertices[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO (the only element)
	glEnableVertexAttribArray(0);

}

void Win32OpenGL::DestroyVAO(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals)
{
	// unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	// release the buffers!!!!!
	glDeleteBuffers(1, &vboVertices);
	glDeleteBuffers(1, &vboNormals);
#if 0 //WITH_TEXTURE
	glDeleteBuffers(1, &m_vertexTextureCoordBufferID);
#endif
	glDeleteVertexArrays(1, &vao);
	// mark unused...
	vboVertices = 0;
	vboNormals = 0;
	vao = 0;
}


void Win32OpenGL::DestroyVAO(GLuint& vao, GLuint& vboVertices)
{
	// unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	// release the buffers!!!!!
	glDeleteBuffers(1, &vboVertices);
	glDeleteVertexArrays(1, &vao);

	// mark unused...
	vboVertices = 0;
	vao = 0;

}

void Win32OpenGL::DestroyVAOWithTextures(GLuint& vao, GLuint& vboVertices, GLuint& vboNormals, GLuint& vboTextures)
{
	// unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	// release the buffers!!!!!
	glDeleteBuffers(1, &vboVertices);
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboTextures);

	glDeleteVertexArrays(1, &vao);
	// mark unused...
	vboVertices = 0;
	vboNormals = 0;
	vboTextures = 0;
	vao = 0;
}

#pragma endregion

#pragma region setup

void Win32OpenGL::SetupDisplay()
{
	glClearColor(0.5f, 0.5f, 0.7f, 0.0);	//  a colour of your choice here

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK);	// cull back face
	glFrontFace(GL_CCW);	// GL_CCW for counter clock-wise
}

#pragma endregion

#pragma region draw

void Win32OpenGL::ClearGLDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Win32OpenGL::Draw(GLuint vao, int numberOfElements)
{
	glBindVertexArray(vao);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, numberOfElements);	// draw first object
	// unbind (mode to tidyup later?)
	glBindVertexArray(0);


}

void Win32OpenGL::DrawLines(GLuint vao, int numberOfElements)
{
	glBindVertexArray(vao);		// select first VAO
	glDrawArrays(GL_LINES, 0, numberOfElements);	// draw first object
	// unbind (mode to tidyup later?)
	glBindVertexArray(0);

}

void Win32OpenGL::FinishedDrawing()
{
	// anything to do at end of draw??
	glBindVertexArray(0);
}

#pragma endregion

#pragma region get_error

string Win32OpenGL::GetError()
{
	// opengl doesn't report error conditions, you have
	// to ask for any errors - this will return
	// a string representing any open gl errors found
	// if all is ok you get the empty string

	string s = "";
	GLenum err;
	err = glGetError();

	if (err == GL_NO_ERROR)		
	{
		return s;
	}	

					// loop scanning for errors and appending them to a string
	while ((err != GL_NO_ERROR) && (s.length() < 128)) // stop infinite loop!!
	{
		switch (err)
		{
		case GL_NO_ERROR:
			//"No error has been recorded. The value of this symbolic constant is guaranteed to be 0.";
			break;

		case GL_INVALID_ENUM:
			s = s + "GL_INVALID_ENUM "; //"An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
			break;

		case GL_INVALID_VALUE:
			s = s + "GL_INVALID_VALUE "; //"A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
			break;

		case GL_INVALID_OPERATION:
			s = s + "GL_INVALID_OPERATION "; //"The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
			break;

			//case GL_INVALID_FRAMEBUFFER_OPERATION:
			//    s = "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
			//    break;

		case GL_OUT_OF_MEMORY:
			s = s + "GL_OUT_OF_MEMORY "; //"There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;

		case GL_STACK_UNDERFLOW:
			s = s + "GL_STACK_UNDERFLOW";	//"An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;

		case GL_STACK_OVERFLOW:
			s = s + "GL_STACK_OVERFLOW"; //"An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		// get next error, if any
		err = glGetError();
	}

	DebugBreak();	//want to know about the error - set a breakpoint here while debugging
	// return the error messages as a single string
	return s;
}

#pragma endregion

