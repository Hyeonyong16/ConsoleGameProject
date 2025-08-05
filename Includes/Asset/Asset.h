#pragma once

#include "Core.h"

class Engine_API Asset
{
public:
	Asset(const char* _key, int _width, int _height, const char* _image);
	~Asset();

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline const char* GetImage() const { return image; }
	inline const char* GetKey() const { return key; }

private:
	char* key = nullptr;

	int width = 0;
	int height = 0;
	char* image = nullptr;
};