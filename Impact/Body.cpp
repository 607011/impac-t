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


#include "stdafx.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4503)

namespace Impact {

  Body::Body(BodyType type, Game *game)
    : mAlive(true)
    , mVisible(true)
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


  void Body::setRestitution(float32 restitution)
  {
    for (b2Fixture* f = mBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetRestitution(restitution);
  }


  void Body::setFriction(float32 friction)
  {
    for (b2Fixture* f = mBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetFriction(friction);
  }


  void Body::setDensity(float32 density)
  {
    for (b2Fixture* f = mBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetDensity(density);
  }


  void Body::setLinearDamping(float32 linearDamping)
  {
    mBody->SetLinearDamping(linearDamping);
  }


  void Body::setAngularDamping(float32 angularDamping)
  {
    mBody->SetAngularDamping(angularDamping);
  }


  void Body::setFixedRotation(bool fixedRotation)
  {
    mBody->SetFixedRotation(fixedRotation);
  }


  void Body::setPosition(float x, float y)
  {
    mBody->SetTransform(b2Vec2(x + 0.5f * mTexture.getSize().x * Game::InvScale, y + 0.5f * mTexture.getSize().y * Game::InvScale), mBody->GetAngle());
  }


  void Body::setPosition(int x, int y)
  {
    this->setPosition(float(x), float(y));
  }


  void Body::setPosition(const b2Vec2 &p)
  {
    this->setPosition(p.x, p.y);
  }
  
  
  void Body::setPosition(const sf::Vector2u &p)
  {
    this->setPosition(p.x * Game::Scale, p.y * Game::Scale);
  }


  void Body::setLifetime(const sf::Time &lifetime)
  {
    mMaxAge = lifetime;
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


  void Body::setVisible(bool visible)
  {
    mVisible = visible;
  }


  void Body::setZIndex(int z)
  {
    mZIndex = z;
  }


  void Body::setBody(b2Body *body)
  {
    remove();
    mBody = body;
  }
}
