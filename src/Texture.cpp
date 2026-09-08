#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include <external/stb_image.h>
#include <Log.h>

namespace CPURenderer {
	Texture::Texture(const char* path)
	{
		_data = stbi_load(path, &_width, &_height, &_channels, 4);

		if (_data == nullptr) {
			CPURenderer::Log("Failed to load texture {}: {}", path, stbi_failure_reason());

			_width = 0;
			_height = 0;
			_channels = 0;
			_size = 0;
			return;
		}

		_size = _width * _height * 4;
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
	int Texture::GetSize()
	{
		return _size;
	}
	Pixel Texture::GetPixel(int x, int y)
	{
		int offset = (y * _width + x) * 4;

		if (offset >= _size) {
			return {};
		}

		return Pixel{
			_data[offset],
			_data[offset + 1],
			_data[offset + 2],
			_data[offset + 3]
		};
	}

	Pixel Texture::GetPixelUV(float u, float v)
	{
		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		int x = static_cast<int>(u * (_width - 1));
		int y = static_cast<int>(v * (_height - 1));

		return GetPixel(x, y);
	}
}