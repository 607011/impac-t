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

  Racket::Racket(Game *game)
    : Body(Body::BodyType::Racket, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Pad");
    mTexture = mGame->level()->texture(mName);
    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);
    const sf::Vector2f &origin = .5f * sf::Vector2f(W, H);
    mSprite.setTexture(mTexture);
    mSprite.setOrigin(origin);
    mCenter = Game::InvScale * b2Vec2(origin.x, origin.y);

    // racket
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.gravityScale = 0.f;
      bd.allowSleep = true;
      bd.awake = true;
      bd.bullet = true;
      bd.fixedRotation = false;
      mTeetingBody = mGame->world()->CreateBody(&bd);

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
      fd.userData = this;
      fd.restitution = .99f;
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
    }

    if (!mGame->mouseModeEnabled()) {
      // x-axis constraint
      b2BodyDef bd;
      bd.position.Set(0.f, float32(mGame->level()->height()));
      b2Body *xAxis = mGame->world()->CreateBody(&bd);
      b2PrismaticJointDef pjd;
      pjd.bodyA = xAxis;
      pjd.bodyB = mBody;
      pjd.collideConnected = false;
      pjd.localAxisA.Set(1.f, 0.f);
      pjd.localAnchorA.SetZero();
      pjd.localAnchorB.Set(W * Game::InvScale * 0.5f, H * Game::InvScale * 0.5f);
      pjd.lowerTranslation = 0.f;
      pjd.upperTranslation = W;
      mGame->world()->CreateJoint(&pjd);
    }
  }


  void Racket::setPosition(float x, float y)
  {
    Body::setPosition(x, y);
    const b2Transform &tx = mBody->GetTransform();
    mTeetingBody->SetTransform(tx.p - mCenter, tx.q.GetAngle());
  }


  void Racket::applyLinearVelocity(const b2Vec2 &v)
  {
    mTeetingBody->SetLinearVelocity(v);
  }


  void Racket::moveLeft(void)
  {
    applyLinearVelocity(b2Vec2(-25.f, 0.f));
  }


  void Racket::moveRight(void)
  {
    applyLinearVelocity(b2Vec2(+25.f, 0.f));
  }


  void Racket::stopMotion(void)
  {
    applyLinearVelocity(b2Vec2_zero);
  }


  void Racket::kickLeft(void)
  {
    mJoint->SetMotorSpeed(-500.f);
  }


  void Racket::kickRight(void)
  {
    mJoint->SetMotorSpeed(+500.f);
  }


  void Racket::stopKick(void)
  {
    if (mGame->mouseModeEnabled())
      mJoint->SetMotorSpeed(mTeetingBody->GetAngle() > 0.f ? -1.f : 1.f);
    else
      mJoint->SetMotorSpeed(0.f);
  }


  void Racket::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(mGame->tileWidth() * mTeetingBody->GetPosition().x, mGame->tileHeight() * mTeetingBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mTeetingBody->GetAngle()));
  }


  void Racket::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  const b2Vec2 &Racket::position(void) const
  {
    return mTeetingBody->GetPosition();
  }

}
