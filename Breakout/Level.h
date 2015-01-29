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

  class LevelTimer {
  public:
    LevelTimer(void)
    {
      restart();
      mActive = false;
    }
    inline void restart(void)
    {
      mClock.restart();
      mTime = sf::Time::Zero;
      mActive = true;
    }
    inline void pause(void)
    {
#ifndef NDEBUG
      std::cout << "LevelTimer is paused." << std::endl;
#endif
      mTime += mClock.restart();
      mActive = false;
    }
    inline void resume(void)
    {
#ifndef NDEBUG
      std::cout << "LevelTimer is active." << std::endl;
#endif
      mActive = true;
      mClock.restart();
    }
    inline const sf::Time &total(void) const
    {
      return mTime;
    }
    inline int accumulatedSeconds(void) const
    {
      const sf::Time &accumulatedTime = mActive ? mClock.getElapsedTime() + mTime : mTime;
      return accumulatedTime.asMilliseconds() / 1000;
    }
    inline bool isActive(void) const
    {
      return mActive;
    }
  private:
    sf::Clock mClock;
    sf::Time mTime;
    bool mActive;
  };

  struct Boundary {
    Boundary(void)
      : left(0)
      , top(0)
      , right(0)
      , bottom(0)
      , valid(false)
    { /* ... */ }
    Boundary(int top, int right, int bottom, int left)
      : top(0)
      , right(0)
      , bottom(0)
      , left(0)
      , valid(false)
    { /* ... */ }
    int top;
    int right;
    int bottom;
    int left;
    bool valid;
  };

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
    /// level number
    inline int level(void) const
    {
      return mLevelNum;
    }
    /// number of tiles in horizontal direction
    inline int width(void) const
    {
      return mNumTilesX;
    }
    /// number of tiles in vertical direction
    inline int height(void) const
    {
      return mNumTilesY;
    }
    inline const Boundary &boundary(void) const
    {
      return mBoundary;
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
    Boundary mBoundary;

    std::map<int, int> mScores;
    std::map<int, std::string> mTextureNames;
    std::map<int, sf::Texture> mTextures;
    std::map<int, Body::BodyType> mTileMapping;
    sf::Texture load(const std::string &filename);
  };

}

#endif // __LEVEL_H_
