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

#include <SFML/System.hpp>
#include <vector>
#include <string>
#include "Body.h"
#include "globals.h"
#include "TileParam.h"

#include "../zip-utils/unzip.h"

namespace Impact {

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
    Level(int num);
    Level(const Level &);
    ~Level();

    static const float32 DefaultGravity;

    void clear(void);
    bool set(int level, bool doLoad);
    bool gotoNext(void);

    const sf::Texture &texture(const std::string &name) const;
    int bodyIndexByTextureName(const std::string &name) const;
    uint32_t *const mapDataScanLine(int y);
    const TileParam &tileParam(int index) const;
    inline bool isAvailable(void) const
    {
      return mSuccessfullyLoaded;
    }
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
    inline int tileWidth(void) const
    {
      return mTileWidth;
    }
    inline int tileHeight(void) const
    {
      return mTileHeight;
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
    inline b2Vec2 size(void) const {
      return b2Vec2(float32(mNumTilesX), float32(mNumTilesY));
    }
    inline const Boundary &boundary(void) const
    {
      return mBoundary;
    }
    inline float32 gravity(void) const
    {
      return mGravity;
    }
    inline bool explosionParticlesCollideWithBall(void) const
    {
      return mExplosionParticlesCollideWithBall;
    }
    inline int killingsPerKillingSpree(void) const
    {
      return mKillingsPerKillingSpree;
    }
    inline int killingSpreeBonus(void) const
    {
      return mKillingSpreeBonus;
    }
    inline const sf::Time &killingSpreeInterval(void) const
    {
      return mKillingSpreeInterval;
    }
    inline const std::string &credits(void) const
    {
      return mCredits;
    }
    inline const std::string &author(void) const
    {
      return mAuthor;
    }
    inline const std::string &copyright(void) const
    {
      return mCopyright;
    }
    inline const std::string &name(void) const
    {
      return mName;
    }
    inline const std::string &base62name(void) const
    {
      return mBase62Name;
    }
    inline const std::string &info(void) const
    {
      return mInfo;
    }
    inline const std::string &hash(void) const
    {
      return mSHA1;
    }
    inline sf::Music *music(void)
    {
      return mMusic;
    }

    void load(void);
    void loadZip(const std::string &zipFilename);

  private:
    bool mSuccessfullyLoaded;
    std::string mSHA1;
    float mBackgroundImageOpacity;
    sf::Color mBackgroundColor;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    int mLevelNum;
    std::vector<uint32_t> mMapData;
    int mNumTilesX;
    int mNumTilesY;
    int mTileWidth;
    int mTileHeight;
    uint32_t mFirstGID;
    Boundary mBoundary;
    float32 mGravity;
    bool mExplosionParticlesCollideWithBall;
    int mKillingsPerKillingSpree;
    int mKillingSpreeBonus;
    sf::Time mKillingSpreeInterval;
    std::string mBase62Name;
    std::string mName;
    std::string mInfo;
    std::string mCredits;
    std::string mAuthor;
    std::string mCopyright;
    sf::Music *mMusic;

    std::vector<TileParam> mTiles;

    bool calcSHA1(const std::string &filename);
  };

}

#endif // __LEVEL_H_
