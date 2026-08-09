#pragma once
#include <Pixel.h>

namespace CPURenderer {
	class Texture {
	private:
		unsigned char* _data;
		int _width;
		int _height;
		int _channels;
		int _size;
	public:
		Texture(const char* path);
		~Texture();

		int GetWidth();
		int GetHeight();
		int GetSize();

		Pixel GetPixel(int x, int y);
	};
}