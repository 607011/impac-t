// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __TEXTURECACHE_H_
#define __TEXTURECACHE_H_

#include <SFML/Graphics/Texture.hpp>
#include "Body.h"
#include <map>
#include <string>

namespace Breakout {

  class TextureCache {
  public:
    TextureCache(void);
    static const sf::Texture &texture(int index);
    static const sf::Texture &texture(std::string name);
    static int bodyIndexByName(std::string name);
    static void clear(void);
    static void add(const std::string &filename, int index, const std::string &tileName);
    static const std::string &textureName(int index);

  private:
    static std::map<int, std::string> mTextureNames;
    static std::map<int, sf::Texture> mTextures;
    static std::map<int, Body::BodyType> mTileMapping;

    static sf::Texture load(const std::string &filename);
  };

}

#endif // __TEXTURECACHE_H_
