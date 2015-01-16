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
#include <list>

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
      Ground
    } BodyType;

    typedef enum _ZIndex {
      Background = -100000,
      Intermediate = 0,
      Foreground = +100000
    } ZIndex;

    Body(BodyType, Game *game);
    virtual ~Body();

    typedef boost::signals2::signal<void (Body*)> killed_signal_t;
    typedef killed_signal_t::slot_type KilledSlotType;

    boost::signals2::connection doOnKilled(const KilledSlotType &slot);

    void update(float elapsedSeconds);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    virtual void setPosition(float x, float y);

    void setLifetime(const sf::Time &);
    const sf::Time &lifetime(void) const;
    const sf::Time &age(void) const;
    bool overAge(void) const;

    BodyType type(void) const;
    void setType(BodyType);

    void remove(void);
    virtual void kill(void);
    bool isAlive(void) const;

    void setVisible(bool);
    bool isVisible(void) const;

    virtual void setZIndex(int);
    virtual int zIndex(void) const;

    virtual void setGame(Game *);
    Game *game(void);

    uint32_t id(void) const;

    b2Body *body(void);
    virtual void setBody(b2Body *);

  protected:
    Body::killed_signal_t signalKilled;

    sf::Texture mTexture;
    sf::Sprite mSprite;
    sf::Shader mShader;
    b2Body *mBody;

    int mZIndex;
    sf::Clock mSpawned; // milliseconds
    sf::Time mMaxAge;
    BodyType mType;
    Game *mGame;

    void setId(uint32_t);
    friend class Game;

    std::string mName;


  private:
    virtual void onUpdate(float elapsedSeconds) = 0;
    virtual void onDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

    uint32_t mID;
    bool mAlive;
    bool mVisible;

  };


  typedef std::list<Body*> BodyList;
  typedef std::list<const Body*> ConstBodyList;

}

#endif // __BODY_H_
