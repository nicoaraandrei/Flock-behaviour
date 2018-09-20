#include "TextureCache.h"
#include "ImageLoader.h"

#include <iostream>

TextureCache::TextureCache()
{
}


TextureCache::~TextureCache()
{
}

GLTexture TextureCache::getTexture(std::string texturePath)
{
	auto mapit =  _textureMap.find(texturePath);

	//check if the texture is not in the map
	if (mapit == _textureMap.end())
	{
		GLTexture newTexture = ImageLoader::loadPNG(texturePath);

		_textureMap.insert(make_pair(texturePath, newTexture));

		std::cout << "Used created texture\n";

		return newTexture;
	}

	std::cout << "Used cached texture\n";

	return mapit->second;
}
