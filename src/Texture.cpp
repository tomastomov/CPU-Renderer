#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include <external/stb_image.h>

namespace CPURenderer {
	Texture::Texture(const char* path)
	{
		_data = stbi_load(path, &_width, &_height, &_channels, 4);
	}

	Texture::~Texture()
	{
		stbi_image_free(_data);
	}
	int Texture::GetWidth()
	{
		return _width;
	}
	int Texture::GetHeight()
	{
		return _height;
	}
}