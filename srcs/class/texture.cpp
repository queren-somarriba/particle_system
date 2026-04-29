#include "texture.hpp"
#include <cstdint>
#include <fstream>


Texture::Texture(const char* img_path,	GLenum slot) : id(0), slot(slot)
{
	this->slot = slot;

	std::ifstream file(img_path, std::ios::binary);
	if (!file)
	{
		std::cerr << "Error: Could not open file " << img_path << std::endl;
		return;
	}

	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;

	file.read((char*)&header, sizeof(header));
	file.read((char*)&info, sizeof(info));

	if (header.bfType != 0x4D42)
	{
		std::cerr << "Error: " << img_path << " is not a valid BMP" << std::endl;
        return;
	}

	int channels = info.biBitCount / 8;
	uint32_t size = info.biSizeImage;
	if (size == 0)
		size = info.biWidth * info.biHeight * channels;

	unsigned char* pixels = new unsigned char[size];
	file.seekg(header.bfOffBits);
	file.read((char*)pixels, size);
	file.close();

	glGenTextures(1, &(this->id));
	glActiveTexture(this->slot);
	glBindTexture(GL_TEXTURE_2D, this->id);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLint internalFormat = (channels == 4) ? GL_RGBA : GL_RGB;
	GLenum externalFormat = (channels == 4) ? GL_BGRA : GL_BGR;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, info.biWidth, info.biHeight, 0, externalFormat, GL_UNSIGNED_BYTE, pixels);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pixels;
}

Texture::~Texture()
{
	if (this->id)
	{
		glDeleteTextures(1, &this->id);
		this->id = 0;
	}
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.id, uniform);
	shader.use();
	glUniform1i(texUni, unit);
}

void  Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}