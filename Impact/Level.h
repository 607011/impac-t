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

  struct Tile {
    Tile(void)
      : score(0)
      , fixed(false)
      , friction(.5f)
      , restitution(.9f)
      , density(800.f)
      , gravityScale(2.f)
      , smooth(true)
      , minimumHitImpulse(0)
      , minimumKillImpulse(50)
    { /* ... */ }
    int score;
    std::string textureName;
    sf::Texture texture;
    bool fixed;
    float32 friction;
    float32 restitution;
    float32 density;
    float32 gravityScale;
    bool smooth;
    int minimumHitImpulse;
    int minimumKillImpulse;
  };

  class Level {
  public:
    Level(void);
    ~Level();

    static const float32 DefaultGravity;

    void clear(void);
    bool set(int level);
    bool gotoNext(void);

    const sf::Texture &texture(const std::string &name) const;
    int bodyIndexByTextureName(const std::string &name) const;
    uint32_t mapData(int x, int y) const;
    uint32_t *const mapDataScanLine(int y) const;
    const Tile &tile(int index) const;
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
    float32 mGravity;
    bool mExplosionParticlesCollideWithBall;
    int mKillingsPerKillingSpree;
    int mKillingSpreeBonus;
    sf::Time mKillingSpreeInterval;

    std::vector<Tile> mTiles;
    sf::Texture load(const std::string &filename);
  };

}

#endif // __LEVEL_H_
