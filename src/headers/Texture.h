#pragma once

namespace CPURenderer {
	class Texture {
	private:
		unsigned char* _data;
		int _width;
		int _height;
		int _channels;
	public:
		Texture(const char* path);
		~Texture();
	};
}