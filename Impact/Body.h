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

#ifndef __BODY_H_
#define __BODY_H_

#include <boost/signals2.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "Destructible.h"
#include "TileParam.h"

#include <cstdint>
#include <vector>

namespace Impact {

  class Game;


  class Body : public sf::Drawable, public Destructible {
  public:
    typedef enum _BodyType {
      // misc
      Nobody,
      Bumper,
      Block,
      BlockGreen,
      BlockYellow,
      BlockLight,
      BlockDark,
      BlockRed,
      BlockBlue,
      Ball,
      Racket,
      Ground,
      Particle,
      Text,
      Wall,
      LeftBoundary,
      TopBoundary,
      RightBoundary,
      BottomBoundary
    } BodyType;

    static const int16 DefaultCollisionGroup = 1;
    static const uint16 BlockMask = 1 << 0;
    static const uint16 BallMask = 1 << 1;
    static const uint16 RacketMask = 1 << 2;
    static const uint16 ParticleMask = 1 << 3;
    static const uint16 GroundMask = 1 << 4;
    static const uint16 BoundaryMask = 1 << 5;
    static const uint16 WallMask = 1 << 6;

    Body(BodyType, Game *game);
    virtual ~Body();

    typedef boost::signals2::signal<void (Body*)> killed_signal_t;
    typedef killed_signal_t::slot_type KilledSlotType;

    boost::signals2::connection doOnKilled(const KilledSlotType &slot);

    void update(float elapsedSeconds);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    virtual void setRestitution(float32);
    virtual void setFriction(float32);
    virtual void setDensity(float32);
    virtual void setLinearDamping(float32);
    virtual void setAngularDamping(float32);
    virtual void setFixedRotation(bool);

    virtual void setPosition(const b2Vec2 &);
    virtual void setPosition(float32 x, float32 y);
    virtual void setPosition(int x, int y);
    virtual inline const b2Vec2 &position(void) const
    {
      return mBody->GetPosition();
    }

    void setLifetime(const sf::Time &);
    inline const sf::Time &lifetime(void) const
    {
      return mMaxAge;
    }

    inline const sf::Time &age(void) const
    {
      return mSpawned.getElapsedTime();
    }

    inline bool overAge(void) const
    {
      return age() > lifetime();
    }

    virtual BodyType type(void) const = 0;

    inline const sf::Texture &texture(void) const
    {
      return mTexture;
    }

    virtual void remove(void);
    virtual void kill(void);

    inline bool isAlive(void) const
    {
      return mAlive;
    }

    void setVisible(bool);
    inline bool isVisible(void) const
    {
      return mVisible;
    }

    void setSmooth(bool);

    virtual void setGame(Game *);
    inline Game *game(void)
    {
      return mGame;
    }

    virtual void setBody(b2Body *);
    virtual b2Body *body(void)
    {
      return mBody;
    }

    void setTileParam(const TileParam &tileParam);
    const TileParam &tileParam(void) const { return mTileParam; }

  protected:
    Body::killed_signal_t signalKilled;

    sf::Texture mTexture;
    sf::Sprite mSprite;
    sf::Shader mShader;
    b2Body *mBody;

    int mZIndex;
    sf::Clock mSpawned; // milliseconds
    sf::Time mMaxAge;
    Game *mGame;

    std::string mName;

    virtual void onUpdate(float elapsedSeconds) = 0;
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const = 0;

    TileParam mTileParam;

  private:
    bool mAlive;
    bool mVisible;

  };


  typedef std::vector<Body*> BodyList;
  typedef std::vector<const Body*> ConstBodyList;

}

#endif // __BODY_H_
