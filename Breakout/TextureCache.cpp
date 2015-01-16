// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  std::map<int, std::string> TextureCache::mTextureNames;
  std::map<int, sf::Texture> TextureCache::mTextures;
  std::map<int, Body::BodyType> TextureCache::mTileMapping;

  static TextureCache sTextureCache;


  TextureCache::TextureCache(void)
  {
    // ...
  }


  void TextureCache::clear(void)
  {
    mTextures.clear();
    mTileMapping.clear();
  }


  void TextureCache::add(const std::string &filename, int index, const std::string &tileName)
  {
    mTextures[index] = load(filename);
    mTextureNames[index] = tileName;
  }


  int TextureCache::bodyIndexByName(std::string name)
  {
    int result = -1;
    std::map<int, std::string>::const_iterator i;
    for (i = mTextureNames.cbegin(); i != mTextureNames.cend(); ++i) {
      if (i->second == name)
        return i->first;
    }
    return result;
  }


  const sf::Texture &TextureCache::texture(int index)
  {
    return mTextures.at(index);
  }


  const sf::Texture &TextureCache::texture(std::string name)
  {
    return texture(bodyIndexByName(name));
  }


  const std::string &TextureCache::textureName(int index)
  {
    return mTextureNames.at(index);
  }


  sf::Texture TextureCache::load(const std::string &filename)
  {
    sf::Texture texture;
    texture.loadFromFile(filename);
    return texture;
  }

}
