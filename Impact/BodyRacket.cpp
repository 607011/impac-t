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

namespace Impact {

  Racket::Racket(Game *game, const b2Vec2 &pos)
    : Body(Body::BodyType::Racket, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Racket");
    mTexture = mGame->level()->texture(mName);

    const float32 W = float32(mTexture.getSize().x);
    const float32 H = float32(mTexture.getSize().y);

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
      fd.density = 1.1f;
      fd.friction = .7f;
      fd.userData = this;
      fd.restitution = 1.f;
      mTeetingBody->CreateFixture(&fd);

      b2BodyDef bdHinge;
      bdHinge.position.Set(0.f, 1.5f);
      bdHinge.type = b2_dynamicBody;
      bdHinge.gravityScale = 0.f;
      bdHinge.allowSleep = true;
      bdHinge.awake = true;
      bdHinge.userData = this;
      bdHinge.fixedRotation = true;
      mBody = mGame->world()->CreateBody(&bdHinge);

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

    setPosition(pos);
  }


  void Racket::setRestitution(float32 restitution)
  {
    for (b2Fixture* f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetRestitution(restitution);
  }


  void Racket::setFriction(float32 friction)
  {
    for (b2Fixture* f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetFriction(friction);
  }


  void Racket::setDensity(float32 density)
  {
    for (b2Fixture* f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetDensity(density);
  }


  void Racket::setPosition(float32 x, float32 y)
  {
    setPosition(b2Vec2(x, y));
  }


  void Racket::setPosition(const b2Vec2 &pos)
  {
    Body::setPosition(pos);
    const b2Transform &tx = mBody->GetTransform();
    mTeetingBody->SetTransform(tx.p - mCenter, tx.q.GetAngle());
  }


  void Racket::applyLinearVelocity(const b2Vec2 &v)
  {
    mBody->SetLinearVelocity(v);
  }


  void Racket::setXAxisConstraint(float32 y)
  {
    const float32 W = float32(mTexture.getSize().x);
    const float32 H = float32(mTexture.getSize().y);
    b2BodyDef bd;
    bd.position.Set(0.f, y);
    b2Body *xAxis = mGame->world()->CreateBody(&bd);
    b2PrismaticJointDef pjd;
    pjd.bodyA = xAxis;
    pjd.bodyB = mBody;
    pjd.collideConnected = false;
    pjd.localAxisA.Set(1.f, 0.f);
    pjd.localAnchorA.SetZero();
    const float32 s = .5f * Game::InvScale * 0.5f;
    pjd.localAnchorB.Set(s * W, s * H);
    pjd.lowerTranslation = 0.f;
    pjd.upperTranslation = W;
    mGame->world()->CreateJoint(&pjd);
  }


  const b2Vec2 &Racket::position(void) const
  {
    return mTeetingBody->GetPosition();
  }


  b2Body *Racket::body(void)
  {
    return mBody;
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
    mJoint->SetMotorSpeed(-10.f);
  }


  void Racket::kickRight(void)
  {
    mJoint->SetMotorSpeed(+10.f);
  }


  void Racket::stopKick(void)
  {
    mJoint->SetMotorSpeed(mTeetingBody->GetAngle() > 0.f ? -1.f : 1.f);
  }


  void Racket::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(Game::Scale * mTeetingBody->GetPosition().x, Game::Scale * mTeetingBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mTeetingBody->GetAngle()));
  }


  void Racket::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


}
