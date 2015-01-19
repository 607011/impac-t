// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __LEVEL_H_
#define __LEVEL_H_

#include "Body.h"
#include "globals.h"

#include <map>
#include <string>
#include <zlib.h>


namespace Breakout {

  class Level {
  public:
    Level(void);
    ~Level();

    void clear(void);
    void set(int level);

    uint32_t firstGID(void) const;
    const sf::Texture &texture(int index) const;
    const sf::Texture &texture(const std::string &name) const;
    int bodyIndexByTextureName(const std::string &name) const;
    void addTexture(const std::string &filename, int index, const std::string &tileName);
    const std::string &textureName(int index);
    const sf::Sprite &backgroundSprite(void) const;
    uint32_t mapData(int x, int y) const;
    uint32_t *const mapDataScanLine(int y) const;
    int score(int index) const;

    int width(void) const;
    int height(void) const;
    int tileWidth(void) const;
    int tileHeight(void) const;

  private:
    void load(void);

    float mBackgroundImageOpacity;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    int mLevelNum;
    uint32_t *mMapData;
    uLongf mMapDataSize;
    int mNumTilesX;
    int mNumTilesY;
    int mTileWidth;
    int mTileHeight;
    uint32_t mFirstGID;

    std::map<int, int> mScores;
    std::map<int, std::string> mTextureNames;
    std::map<int, sf::Texture> mTextures;
    std::map<int, Body::BodyType> mTileMapping;
    sf::Texture load(const std::string &filename);
  };

}

#endif // __LEVEL_H_
