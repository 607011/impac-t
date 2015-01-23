// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4503)

namespace Breakout {

  Body::Body(BodyType type, Game *game)
    : mID(0xffffffffU)
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


  void Body::setLifetime(const sf::Time &lifetime)
  {
    mMaxAge = lifetime;
  }

  
  void Body::remove(void)
  {
#ifndef NDEBUG
    std::cout << "Body::remove() " << mName << std::endl;
#endif
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


  void Body::setVisible(bool visible)
  {
    mVisible = visible;
  }


  void Body::setZIndex(int z)
  {
    mZIndex = z;
  }


  void Body::setId(uint32_t id)
  {
    mID = id;
  }


  void Body::setBody(b2Body *body)
  {
    remove();
    mBody = body;
  }
}
