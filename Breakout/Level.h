// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __LEVEL_H_
#define __LEVEL_H_

#include "Body.h"
#include "globals.h"

#include <map>
#include <string>


namespace Breakout {

  class Level {
  public:
    Level(void);
    ~Level();

    void clear(void);
    bool set(int level);
    bool gotoNext(void);

    const sf::Texture &texture(int index) const;
    const sf::Texture &texture(const std::string &name) const;
    int bodyIndexByTextureName(const std::string &name) const;
    void addTexture(const std::string &filename, int index, const std::string &tileName);
    const std::string &textureName(int index);
    uint32_t mapData(int x, int y) const;
    uint32_t *const mapDataScanLine(int y) const;
    int score(int index) const;
    inline int num(void) const
    {
      return mLevelNum;
    }
    inline uint32_t firstGID(void) const
    {
      return mFirstGID;
    }
    inline const sf::Sprite &backgroundSprite(void) const
    {
      return mBackgroundSprite;
    }
    inline int level(void) const
    {
      return mLevelNum;
    }
    inline int width(void) const
    {
      return mNumTilesX;
    }
    inline int height(void) const
    {
      return mNumTilesY;
    }
    inline int tileWidth(void) const
    {
      return mTileWidth;
    }
    inline int tileHeight(void) const
    {
      return mTileHeight;
    }

  private:
    bool load(void);

    float mBackgroundImageOpacity;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    int mLevelNum;
    uint32_t *mMapData;
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
