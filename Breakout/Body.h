// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BODY_H_
#define __BODY_H_

#include <boost/signals2.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "Destructible.h"

#include <cstdint>
#include <vector>

namespace Breakout {

  class Game;


  class Body : public sf::Drawable, public Destructible {
  public:
    typedef enum _BodyType {
      // misc
      Nobody,
      Block,
      BlockGreen,
      BlockYellow,
      BlockLight,
      BlockDark,
      BlockRed,
      BlockBlue,
      Ball,
      Pad,
      Ground,
      Particle,
      Text
    } BodyType;

    typedef enum _ZIndex {
      Background = -100000,
      Intermediate = 0,
      Foreground = +100000
    } ZIndex;

    typedef enum _Mask {
      DefaultMask = 0x0001,
      BallMask = 0x0002,
      PadMask = 0x0004,
      ParticleMask = 0x0008
    } BodyMask;

    Body(BodyType, Game *game);
    virtual ~Body();

    typedef boost::signals2::signal<void (Body*)> killed_signal_t;
    typedef killed_signal_t::slot_type KilledSlotType;

    boost::signals2::connection doOnKilled(const KilledSlotType &slot);

    void update(float elapsedSeconds);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    virtual void setPosition(float x, float y);
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

    void remove(void);
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

    virtual void setZIndex(int);
    inline virtual int zIndex(void) const
    {
      return mZIndex;
    }

    virtual void setGame(Game *);
    inline Game *game(void)
    {
      return mGame;
    }

    inline uint32_t id(void) const
    {
      return mID;
    }

    virtual void setBody(b2Body *);
    inline b2Body *body(void)
    {
      return mBody;
    }

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

    void setId(uint32_t);
    friend class Game;

    std::string mName;

    virtual void onUpdate(float elapsedSeconds) = 0;
    virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

  private:
    uint32_t mID;
    bool mAlive;
    bool mVisible;

  };


  typedef std::vector<Body*> BodyList;
  typedef std::vector<const Body*> ConstBodyList;

}

#endif // __BODY_H_
