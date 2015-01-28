/*  

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


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
    inline const sf::Color &backgroundColor(void) const
    {
      return mBackgroundColor;
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
    sf::Color mBackgroundColor;
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
