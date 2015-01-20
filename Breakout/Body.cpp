// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4503)

namespace Breakout {

  Body::Body(BodyType type, Game *game)
    : mType(type)
    , mID(0xffffffffU)
    , mAlive(true)
    , mVisible(true)
    , mMaxAge(sf::milliseconds(0))
    , mZIndex(0)
    , mBody(nullptr)
  {
    setGame(game);
    mSpawned.restart();
  }


  Body::~Body()
  {
    remove();
  }


  void Body::setGame(Game *game)
  {
    mGame = game;
    if (mGame != nullptr)
      doOnKilled(boost::bind(&Game::onBodyKilled, mGame, this));
  }


  Game *Body::game(void)
  {
    return mGame;
  }


  boost::signals2::connection Body::doOnKilled(const KilledSlotType &slot)
  {
    return signalKilled.connect(slot);
  }


  void Body::update(float elapsedSeconds)
  {
    onUpdate(elapsedSeconds);
  }


  void Body::draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    onDraw(target, states);
  }


  void Body::setPosition(float x, float y)
  {
    mBody->SetTransform(b2Vec2(x + 0.5f * mTexture.getSize().x / mGame->tileWidth(), y + 0.5f * mTexture.getSize().y / mGame->tileHeight()), mBody->GetAngle());
  }


  const b2Vec2 &Body::position(void) const
  {
    return mBody->GetPosition();
  }


  void Body::setLifetime(const sf::Time &lifetime)
  {
    mMaxAge = lifetime;
  }


  const sf::Time &Body::lifetime(void) const
  {
    return mMaxAge;
  }


  const sf::Time &Body::age(void) const
  {
    return mSpawned.getElapsedTime();
  }


  bool Body::overAge(void) const
  {
    return age() > lifetime();
  }


  Body::BodyType Body::type(void) const
  {
    return mType;
  }


  void Body::setType(Body::BodyType type)
  {
    mType = type;
  }


  const sf::Texture &Body::texture(void) const
  {
    return mTexture;
  }

  
  void Body::remove(void)
  {
    if (mBody) {
      b2World *world = mBody->GetWorld();
      world->DestroyBody(mBody);
      mBody = nullptr;
    }
  }


  void Body::kill(void)
  {
    mAlive = false;
    setVisible(false);
    signalKilled(this);
  }


  bool Body::isAlive(void) const
  {
    return mAlive;
  }


  void Body::setVisible(bool visible)
  {
    mVisible = visible;
  }


  bool Body::isVisible(void) const
  {
    return mVisible;
  }


  void Body::setZIndex(int z)
  {
    mZIndex = z;
  }


  int Body::zIndex(void) const
  {
    return mZIndex;
  }


  void Body::setId(uint32_t id)
  {
    mID = id;
  }


  uint32_t Body::id(void) const
  {
    return mID;
  }

  b2Body *Body::body(void)
  {
    return mBody;
  }


  void Body::setBody(b2Body *body)
  {
    mBody = body;
  }
}
