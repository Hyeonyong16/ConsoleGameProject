#include "Asset.h"

#include <iostream>

Asset::Asset(const char* _key, int _width, int _height, const char* _image)
	: width(_width), height(_height)
{
	size_t length = strlen(_key) + 1;
	key = new char[length];
	strcpy_s(key, length, _key);

	length = strlen(_image) + 1;
	image = new char[length];
	strcpy_s(image, length, _image);
}

Asset::~Asset()
{
	SafeDeleteArray(key);
	SafeDeleteArray(image);
}
