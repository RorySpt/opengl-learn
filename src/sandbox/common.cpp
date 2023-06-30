#include "stdafx.h"
#include "common.h"
#include "stb_image.h"


static std::map<std::string_view, unsigned int> s_textureMap;

unsigned comm::loadTexture(std::string_view path)
{
	if (s_textureMap.contains(path))
		return s_textureMap.at(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrCommponents;
	unsigned char* data = stbi_load(path.data(), &width, &height, &nrCommponents, 0);
	if(data)
	{
		GLenum format(GL_RGB);
		if(nrCommponents == 1)
		{
			format = GL_RED;
		}
		else if (nrCommponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrCommponents == 4)
		{
			format = GL_RGBA;
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
	s_textureMap.insert_or_assign(path, textureID);
	return textureID;
}

unsigned comm::loadTexture(std::string_view fileName, std::string_view directory)
{
	return loadTexture(std::string(directory).append("/").append(fileName));
}
