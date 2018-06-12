#include "stdafx.h"
#include "TextureLoader.h"
//#include "IMAT2902_Ex07.h"	// for error messages

#include "stb_image.h"

TextureLoader::TextureLoader()
{
}

TextureLoader::~TextureLoader()
{
}

// Original code from
//
// http://www.cplusplus.com/articles/GwvU7k9E/
//
// Modified and extended PC

int TextureLoader::LoadBMP(string location, GLuint &texture, bool withMipMaps)  //const char* location 
{
	// get the bit depth 24 or 32, 32 is better!!!!
	Win32OpenGL::GetError();

	// this is a version using c++ file i/o rather than c.
	unsigned char* datBuff[2] = { nullptr, nullptr }; // Header buffers
	unsigned char* pixels = nullptr; // Pixels

									 // windows knows the relevant header structures
	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	BITMAPINFOHEADER* bmpInfo = nullptr; // Info

										 // The file... We open it with its constructor
	std::ifstream file(location, std::ios::binary);
	if (!file)
	{
		//DebugBreak();
		Log::AppendToLogFile("Couldn't open texture file. It doesn't exist: " + location);
		return 1;
	}
	// Allocate byte memory that will hold the two headers
	datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];

	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));

	// Construct the values from the buffers
	bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
	bmpInfo = (BITMAPINFOHEADER*)datBuff[1];



	// Check if the file is an actual BMP file 0x42 = 'B'  4D = 'M'
	if (bmpHeader->bfType != 0x4D42)
	{
		//DebugBreak();
		Log::AppendToLogFile("Expected a .bmp file.");
		return 2;
	}

	int numberOfBits = bmpInfo->biBitCount;
	int bytesPerPixel = numberOfBits / 8;		// or >>3 should be 3 or 4...

	// we expect 24 or 32 bit files
	if (numberOfBits != 24 && numberOfBits != 32)
	{
		//DebugBreak();
		Log::AppendToLogFile("Expected 24 or 32 bit files.");
		return 2;
	}

	// First allocate pixel memory
	unsigned int imageSize = bmpInfo->biSizeImage;

	if ((imageSize == 0) || ((imageSize % bytesPerPixel) != 0))
	{
		imageSize = bmpInfo->biWidth * bmpInfo->biHeight * bytesPerPixel;
	}

	pixels = new unsigned char[imageSize];

	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)pixels, imageSize);

	// fixed for DMU lab machines - 9/10/17
	// added code to re-jig BGRA to RGBA in CPU manually - rather than opengl...
	GLint format = (bytesPerPixel == 4) ? GL_RGBA : GL_RGB;  //GL_BGRA_EXT                 // Set the mode (see https://msdn.microsoft.com/en-us/library/windows/desktop/dd368638(v=vs.85).aspx )

	GLint internalFormat = (bytesPerPixel == 4) ? GL_RGBA : GL_RGB;  //GL_BGRA_EXT             // GL_BGRA is windows mode for DIB -- could rejig??

	// We're almost done. We have our image loaded, however it's not in the right format.
	// .bmp files store image data in the BGR format, and if its 24 bit we have to convert it to RGB.
	// Since we have the value in bytes, this shouldn't be to hard to accomplish

	// Last, most image file formats on disk(TGA, BMP, most JPG loading libraries, etc) 
	// store / generate the texture data in BGRA format in memory, because that's how 
	// the hardware and screen device wants it (on x86). 
	// Last, a BGRA pixel (again, on x86) will read as 0xAARRGGBB as a hex longword, 
	// so if you generate pixel values, this is typically what you expect.
	// All in all, if there's no particular reason to use another format, you will usually be happiest with BGRA in memory. 
	// Unfortunately a 24bit BMP stores the data a BGR but open gl will not accept this - we need to convert to RGB by swapping the r/b pixels
	// similarly a 32bit bmp is started as BGRA and open gl needs RGBA
	// so in both cases we need to swop the 0th and 2nd pixel values (B<->R)

	// this IF is redundant then as we always swop pixels 0 and 2 to fixup windows image formats...

	// single loop now fr both formats
	if ((bytesPerPixel == 4) || ((bytesPerPixel == 3) && (format == GL_RGB)))
	{
		unsigned char tmpRGB = 0; // Swap buffer
		for (unsigned long i = 0; i < imageSize; i += bytesPerPixel)
		{
			tmpRGB = pixels[i];
			pixels[i] = pixels[i + 2];
			pixels[i + 2] = tmpRGB;
		}
	}

	// Set width and height to the values loaded from the file
	GLuint w = bmpInfo->biWidth;
	GLuint h = bmpInfo->biHeight;

	/*******************GENERATING TEXTURES*******************/

	Win32OpenGL::GetError();

	glGenTextures(1, &texture);             // Generate a texture
	glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily

	Win32OpenGL::GetError();

	// Create the texture. We get the offsets from the image, then we use it with the image's
	// pixel data to create it.
	// target level of detail internal format width height border format type data*
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
	Win32OpenGL::GetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// or GL_CLAMP
	Win32OpenGL::GetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	Win32OpenGL::GetError();

	if (withMipMaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   // or GL_LINEAR if no mipmaps...
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
	}
	Win32OpenGL::GetError();
	// could try clamp instead of wrap for the texture
#if 0
		// -----------------------------------------------------------
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//------------------------------------------------------------
#endif
	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, NULL);

	// Output a successful message
	std::cout << "Texture \"" << location << "\" successfully loaded.\n";

	// Delete the buffers.
	delete[] datBuff[0];
	delete[] datBuff[1];
	delete[] pixels;

	return 0; // Return success code 	
}

GLuint TextureLoader::LoadCubeMap(vector<string> p_SkyBoxFaces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int imageWidth, imageHeight, nrChannels;
	unsigned char *imageData;	

	// Load in the textures - setting them to each face of the cube.
	for (GLuint i = 0; i < p_SkyBoxFaces.size(); i++) {
		imageData = stbi_load(p_SkyBoxFaces[i].c_str(), &imageWidth, &imageHeight, &nrChannels, 0);

		if (imageData) 
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		else 
			Log::AppendToLogFile("Cubemap texture failed to load at path: " + p_SkyBoxFaces[i]);

		stbi_image_free(imageData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

#if 0

TextureLoader::TextureLoader()
{
}


TextureLoader::~TextureLoader()
{
}



// Original code from
//
// http://www.cplusplus.com/articles/GwvU7k9E/
//
// Modified and extended PC

int TextureLoader::LoadBMP(string location, GLuint &texture)  //const char* location
{
	// get the bit depth 24 or 32 32 is better!!!!


	// this is a version using c++ file i/o rather than c.
	unsigned char* datBuff[2] = { nullptr, nullptr }; // Header buffers
	unsigned char* pixels = nullptr; // Pixels

	// windows knows the relevant header structures
	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	BITMAPINFOHEADER* bmpInfo = nullptr; // Info

	// The file... We open it with it's constructor
	std::ifstream file(location, std::ios::binary);
	if (!file)
	{
		DebugBreak();
		stringstream oss;
		oss << "File \"" << location << "\" not found\n";
		DisplayMessageBox(oss.str(), "Error");
		return 1;
	}
	// Allocate byte memory that will hold the two headers
	datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];

	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));

	// Construct the values from the buffers
	bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
	bmpInfo = (BITMAPINFOHEADER*)datBuff[1];



	// Check if the file is an actual BMP file 0x42 = 'B'  4D = 'M'
	if (bmpHeader->bfType != 0x4D42)
	{
		DebugBreak();
		stringstream oss;
		oss << "File \"" << location << "\" isn't a bitmap file\n";
		DisplayMessageBox(oss.str(), "Error");
		return 2;
	}

	int numberOfBits = bmpInfo->biBitCount;
	int bytesPerPixel = numberOfBits / 8;		// or >>3

	if (bytesPerPixel == 4)
	{
		int z = 1;
	}

	if (numberOfBits != 24 && numberOfBits != 32)
	{
		DebugBreak();
		stringstream oss;
		oss << "File \"" << location << "\" not 24 or 32 bit bitmap\n";
		DisplayMessageBox(oss.str(), "Error");
		return 2;
	}

	// First allocate pixel memory
	unsigned int imageSize = bmpInfo->biSizeImage;


	if ((imageSize == 0) || ((imageSize % bytesPerPixel) != 0))
	{
		imageSize = bmpInfo->biWidth * bmpInfo->biHeight * bytesPerPixel;
	}

	pixels = new unsigned char[imageSize];

	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)pixels, imageSize);

	GLint mode = (bytesPerPixel == 4) ? GL_BGRA : GL_RGB;                   // Set the mode

	// We're almost done. We have our image loaded, however it's not in the right format.
	// .bmp files store image data in the BGR format, and if its 24 bit we have to convert it to RGB.
	// Since we have the value in bytes, this shouldn't be to hard to accomplish


	//Last, most image file formats on disk(TGA, BMP, most JPG loading libraries, etc) 
	//store / generate the texture data in BGRA format in memory, because that's how 
	//the hardware and screen device wants it (on x86). 
	//Last, a BGRA pixel (again, on x86) will read as 0xAARRGGBB as a hex longword, 
	//so if you generate pixel values, this is typically what you expect.
	//All in all, if there's no particular reason to use another format, you will usually be happiest with BGRA in memory. 
	//Unfortunately a 24bit BMP stores the data a BGR but open gl will not accept this - we need to convert to RGB by swapping the r/b pixels

	if (bytesPerPixel == 4)
	{

		// sort later!!!

	}
	else
	{
		if (mode == GL_RGB)
		{
			unsigned char tmpRGB = 0; // Swap buffer
			for (unsigned long i = 0; i < imageSize; i += 3)
			{
				tmpRGB = pixels[i];
				pixels[i] = pixels[i + 2];
				pixels[i + 2] = tmpRGB;
			}
		}
	}
	// Set width and height to the values loaded from the file
	GLuint w = bmpInfo->biWidth;
	GLuint h = bmpInfo->biHeight;

	/*******************GENERATING TEXTURES*******************/

	glGenTextures(1, &texture);             // Generate a texture
	glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily





	// Create the texture. We get the offsets from the image, then we use it with the image's
	// pixel data to create it.
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, pixels);

	// 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// or GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   // or GL_LINEAR if no mipmaps...
	//glGenerateMipmap(GL_TEXTURE_2D);

	glGetError();

#if 0
	// -----------------------------------------------------------
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//------------------------------------------------------------
#endif
	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, NULL);

	// Output a successful message
	std::cout << "Texture \"" << location << "\" successfully loaded.\n";

	// Delete the buffers.
	delete[] datBuff[0];
	delete[] datBuff[1];
	delete[] pixels;

	return 0; // Return success code 	
}

#endif