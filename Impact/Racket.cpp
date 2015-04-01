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

  const std::string Racket::Name = "Racket";
  const float32 Racket::DefaultDensity = 5.f;
  const float32 Racket::DefaultFriction = .71f;
  const float32 Racket::DefaultRestitution = .1f;

  Racket::Racket(Game *game, const b2Vec2 &pos, b2Body *ground)
    : Body(Body::BodyType::Racket, game)
  {
    mName = Name;
    mTexture = mGame->level()->texture(mName);
    mSprite.setTexture(mTexture);
    mSprite.setOrigin(sf::Vector2f(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y));

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.gravityScale = 0.f;
    bd.bullet = true;
    bd.allowSleep = true;
    mTeetingBody = mGame->world()->CreateBody(&bd);

    b2PolygonShape polygon;
    const float32 hs = .5f * Game::InvScale;
    const float32 hh = hs * mTexture.getSize().y;
    const float32 xoff = hs * (mTexture.getSize().x - mTexture.getSize().y);
    polygon.SetAsBox(xoff, hh);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = DefaultDensity;
    fdBox.friction = DefaultFriction;
    fdBox.restitution = DefaultRestitution;
    fdBox.userData = this;
    mTeetingBody->CreateFixture(&fdBox);

    b2CircleShape circleL;
    circleL.m_p.Set(-xoff, 0.f);
    circleL.m_radius = hh;

    b2FixtureDef fdCircleL;
    fdCircleL.shape = &circleL;
    fdCircleL.density = DefaultDensity;
    fdCircleL.friction = DefaultFriction;
    fdCircleL.restitution = DefaultRestitution;
    fdCircleL.userData = this;
    mTeetingBody->CreateFixture(&fdCircleL);

    b2CircleShape circleR;
    circleR.m_p.Set(xoff, 0.f);
    circleR.m_radius = hh;

    b2FixtureDef fdCircleR;
    fdCircleR.shape = &circleR;
    fdCircleR.density = DefaultDensity;
    fdCircleR.friction = DefaultFriction;
    fdCircleR.restitution = DefaultRestitution;
    fdCircleR.userData = this;
    mTeetingBody->CreateFixture(&fdCircleR);

    b2BodyDef bdHinge;
    bdHinge.position.Set(0.f, 1.5f);
    bdHinge.type = b2_dynamicBody;
    bdHinge.bullet = true;
    bdHinge.gravityScale = 0.f;
    bdHinge.allowSleep = false;
    bdHinge.awake = true;
    bdHinge.userData = this;
    bdHinge.fixedRotation = true;
    mBody = mGame->world()->CreateBody(&bdHinge);

    b2RevoluteJointDef rjd;
    rjd.Initialize(mBody, mTeetingBody, b2Vec2_zero);
    rjd.enableMotor = true;
    rjd.maxMotorTorque = 20000.0f; //MOD Drehmoment
    rjd.enableLimit = true;
    rjd.motorSpeed = 0.f;
    rjd.lowerAngle = deg2rad(-17.5f); //MOD Anschlagswinkel
    rjd.upperAngle = deg2rad(+17.5f); //MOD Anschlagswinkel 
    mJoint = reinterpret_cast<b2RevoluteJoint*>(mGame->world()->CreateJoint(&rjd));

    b2MouseJointDef mjd;
    mjd.bodyA = ground;
    mjd.bodyB = mTeetingBody;
    mjd.target = mBody->GetPosition();
    mjd.collideConnected = true;
    mjd.frequencyHz = 6.f; //MOD
    mjd.dampingRatio = .95f; //MOD
    mjd.maxForce = 1000.f * mTeetingBody->GetMass(); //MOD
    mMouseJoint = reinterpret_cast<b2MouseJoint*>(mGame->world()->CreateJoint(&mjd));

    setPosition(pos);
  }


  void Racket::setRestitution(float32 restitution)
  {
    for (b2Fixture *f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetRestitution(restitution);
  }


  void Racket::setFriction(float32 friction)
  {
    for (b2Fixture *f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetFriction(friction);
  }


  void Racket::setDensity(float32 density)
  {
    for (b2Fixture *f = mTeetingBody->GetFixtureList(); f != nullptr; f = f->GetNext())
      f->SetDensity(density);
    mTeetingBody->ResetMassData();
  }


  void Racket::setPosition(float32 x, float32 y)
  {
    setPosition(b2Vec2(x, y));
  }


  void Racket::setPosition(const b2Vec2 &pos)
  {
    Body::setPosition(pos);
    const b2Transform &tx = mBody->GetTransform();
    mTeetingBody->SetTransform(tx.p, tx.q.GetAngle());
  }


  void Racket::moveTo(const b2Vec2 &target)
  {
    mBody->SetAwake(true);
    mTeetingBody->SetAwake(true);
    mMouseJoint->SetTarget(target);
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
    const float32 s = .5f * Game::InvScale;
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
    return mTeetingBody;
  }


  const b2AABB &Racket::aabb(void) const
  {
    b2Transform t;
    t.SetIdentity();
    mAABB.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
    mAABB.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
    b2Fixture* fixture = mTeetingBody->GetFixtureList();
    while (fixture != nullptr) {
      const b2Shape *shape = fixture->GetShape();
      const int childCount = shape->GetChildCount();
      for (int child = 0; child < childCount; ++child) {
        const b2Vec2 r(shape->m_radius, shape->m_radius);
        b2AABB shapeAABB;
        shape->ComputeAABB(&shapeAABB, t, child);
        shapeAABB.lowerBound = shapeAABB.lowerBound + r;
        shapeAABB.upperBound = shapeAABB.upperBound - r;
        mAABB.Combine(shapeAABB);
      }
      fixture = fixture->GetNext();
    }
    return mAABB;
  }


  void Racket::kickLeft(void)
  {
    mJoint->SetMotorSpeed(-1700.f);
  }


  void Racket::kickRight(void)
  {
    mJoint->SetMotorSpeed(+1700.f);
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
