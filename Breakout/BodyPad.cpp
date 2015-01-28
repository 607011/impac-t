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

namespace Breakout {

  Pad::Pad(Game *game)
    : Body(Body::BodyType::Pad, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Pad");
    mTexture = mGame->level()->texture(mName);
    const sf::Vector2f &origin = .5f * sf::Vector2f(float(mTexture.getSize().x), float(mTexture.getSize().y));
    mSprite.setTexture(mTexture);
    mSprite.setOrigin(origin);
    mCenter = Game::InvScale * b2Vec2(origin.x, origin.y);

    // pad
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.gravityScale = 0.f;
      bd.allowSleep = true;
      bd.awake = true;
#ifdef ENABLE_MOUSEMODE
      bd.bullet = true;
      bd.fixedRotation = true;
      mBody = mGame->world()->CreateBody(&bd);
#else
      bd.bullet = false;
      bd.fixedRotation = false;
      mTeetingBody = mGame->world()->CreateBody(&bd);
#endif

      b2PolygonShape polygon;
      b2Vec2 vertices[8];
      vertices[0] = Game::InvScale * b2Vec2(-32, -6);
      vertices[1] = Game::InvScale * b2Vec2(-32,  6);
      vertices[2] = Game::InvScale * b2Vec2(-30,  8);
      vertices[3] = Game::InvScale * b2Vec2( 30,  8);
      vertices[4] = Game::InvScale * b2Vec2( 32,  6);
      vertices[5] = Game::InvScale * b2Vec2( 32, -6);
      vertices[6] = Game::InvScale * b2Vec2( 30, -8);
      vertices[7] = Game::InvScale * b2Vec2(-32, -8);
      polygon.Set(vertices, 8);

      b2FixtureDef fd;
      fd.shape = &polygon;
      fd.density = 10.f;
      fd.friction = .7f;
      fd.restitution = .99f;
      fd.userData = this;
#ifdef ENABLE_MOUSEMODE
      mBody->CreateFixture(&fd);
#else
      mTeetingBody->CreateFixture(&fd);
      // hinge
      {
        b2BodyDef bd;
        bd.position.Set(0.f, 1.5f);
        bd.type = b2_dynamicBody;
        bd.gravityScale = 0.f;
        bd.allowSleep = true;
        bd.awake = true;
        bd.userData = this;
        bd.fixedRotation = true;
        mBody = mGame->world()->CreateBody(&bd);

        b2RevoluteJointDef jd;
        jd.Initialize(mBody, mTeetingBody, b2Vec2_zero);
        jd.enableMotor = true;
        jd.maxMotorTorque = 20000.0f;
        jd.enableLimit = true;
        jd.motorSpeed = 0.f;
        jd.lowerAngle = deg2rad(-17.5f);
        jd.upperAngle = deg2rad(+17.5f);
        mJoint = reinterpret_cast<b2RevoluteJoint*>(mGame->world()->CreateJoint(&jd));
      }
#endif
    }


#ifndef ENABLE_MOUSEMODE
    // x-axis constraint
    {
      b2BodyDef bd;
      bd.position.Set(0.f, float32(mGame->level()->height()));
      b2Body *xAxis = mGame->world()->CreateBody(&bd);

      b2PrismaticJointDef pjd;
      pjd.bodyA = xAxis;
      pjd.bodyB = mBody;
      pjd.collideConnected = false;
      pjd.localAxisA.Set(1.f, 0.f);
      pjd.localAnchorA.SetZero();
      pjd.localAnchorB.Set(W * sx * 0.5f, H * sy * 0.5f);
      pjd.lowerTranslation = 0.f;
      pjd.upperTranslation = W;
      mGame->world()->CreateJoint(&pjd);
    }
#endif
  }


  void Pad::setPosition(float x, float y)
  {
    Body::setPosition(x, y);
    const b2Transform &tx = mBody->GetTransform();
#ifdef ENABLE_MOUSEMODE
    mBody->SetTransform(tx.p - mCenter, tx.q.GetAngle());
#else
    mTeetingBody->SetTransform(tx.p - mCenter, tx.q.GetAngle());
#endif
  }


  void Pad::applyLinearVelocity(const b2Vec2 &v)
  {
#ifdef ENABLE_MOUSEMODE
    mBody->SetLinearVelocity(v);
#else
    mTeetingBody->SetLinearVelocity(v);
#endif
  }


#ifndef ENABLE_MOUSEMODE
  void Pad::moveLeft(void)
  {
    applyLinearVelocity(b2Vec2(-25.f, 0.f));
  }


  void Pad::moveRight(void)
  {
    applyLinearVelocity(b2Vec2(+25.f, 0.f));
  }


  void Pad::stopMotion(void)
  {
    applyLinearVelocity(b2Vec2_zero);
  }


  void Pad::kickLeft(void)
  {
    mJoint->SetMotorSpeed(-500.f);
  }


  void Pad::kickRight(void)
  {
    mJoint->SetMotorSpeed(+500.f);
  }


  void Pad::stopKick(void)
  {
    mJoint->SetMotorSpeed(0.f);
  }
#endif


  void Pad::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
#ifdef ENABLE_MOUSEMODE
    mSprite.setPosition(mGame->tileWidth() * mBody->GetPosition().x, mGame->tileHeight() * mBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mBody->GetAngle()));
#else
    mSprite.setPosition(mGame->tileWidth() * mTeetingBody->GetPosition().x, mGame->tileHeight() * mTeetingBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mTeetingBody->GetAngle()));
#endif
  }


  void Pad::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  const b2Vec2 &Pad::position(void) const
  {
#ifdef ENABLE_MOUSEMODE
    return mBody->GetPosition();
#else
    return mTeetingBody->GetPosition();
#endif
  }

}
