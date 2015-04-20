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

  Racket::Racket(Game *game, const b2Vec2 &pos, b2Body *ground, const TileParam &tileParam)
    : Body(Body::BodyType::Racket, game, tileParam)
  {
    mName = Name;
    mTexture = mGame->level()->texture(mName);
    setSmooth(mTileParam.smooth);
    mSprite.setTexture(mTexture);
    mSprite.setOrigin(sf::Vector2f(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y));

    setHalfTextureSize(mTexture);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.gravityScale = 0.f;
    mTiltingBody = mGame->world()->CreateBody(&bd);

    b2PolygonShape polygon;
    const float32 hs = .5f * Game::InvScale;
    const float32 hh = hs * mTexture.getSize().y;
    const float32 xoff = hs * (mTexture.getSize().x - mTexture.getSize().y);
    polygon.SetAsBox(xoff, hh);

    const float32 density = tileParam.density.isValid() ? tileParam.density.get() : DefaultDensity;
    const float32 friction = tileParam.friction.isValid() ? tileParam.friction.get() : DefaultFriction;
    const float32 restitution = tileParam.restitution.isValid() ? tileParam.restitution.get() : DefaultRestitution;

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = density;
    fdBox.friction = friction;
    fdBox.restitution = restitution;
    fdBox.userData = this;
    mTiltingBody->CreateFixture(&fdBox);

    b2CircleShape circleL;
    circleL.m_p.x = -xoff;
    circleL.m_radius = hh;

    b2FixtureDef fdCircleL;
    fdCircleL.shape = &circleL;
    fdCircleL.density = density;
    fdCircleL.friction = friction;
    fdCircleL.restitution = restitution;
    fdCircleL.userData = this;
    mTiltingBody->CreateFixture(&fdCircleL);

    b2CircleShape circleR;
    circleR.m_p.x = xoff;
    circleR.m_radius = hh;

    b2FixtureDef fdCircleR;
    fdCircleR.shape = &circleR;
    fdCircleR.density = density;
    fdCircleR.friction = friction;
    fdCircleR.restitution = restitution;
    fdCircleR.userData = this;
    mTiltingBody->CreateFixture(&fdCircleR);

    b2BodyDef bdHinge;
    bdHinge.type = b2_dynamicBody;
    mBody = mGame->world()->CreateBody(&bdHinge);

    b2RevoluteJointDef rjd;
    rjd.Initialize(mBody, mTiltingBody, b2Vec2_zero);
    rjd.enableMotor = true;
    rjd.maxMotorTorque = 20000.0f; //MOD Drehmoment
    rjd.enableLimit = true;
    rjd.lowerAngle = deg2rad(-17.5f); //MOD Anschlagswinkel in Grad
    rjd.upperAngle = deg2rad(+17.5f); //MOD Anschlagswinkel in Grad
    mJoint = reinterpret_cast<b2RevoluteJoint*>(mGame->world()->CreateJoint(&rjd));

    b2MouseJointDef mjd;
    mjd.bodyA = ground;
    mjd.bodyB = mTiltingBody;
    mjd.collideConnected = true;
    mjd.frequencyHz = 6.f; //MOD
    mjd.dampingRatio = .95f; //MOD
    mjd.maxForce = 1000.f * mTiltingBody->GetMass(); //MOD
    mMouseJoint = reinterpret_cast<b2MouseJoint*>(mGame->world()->CreateJoint(&mjd));

    setPosition(pos);
  }


  void Racket::setPosition(const b2Vec2 &pos)
  {
    Body::setPosition(pos);
    const b2Transform &tx = mBody->GetTransform();
    mTiltingBody->SetTransform(tx.p, tx.q.GetAngle());
  }


  void Racket::moveTo(const b2Vec2 &target)
  {
    mMouseJoint->SetTarget(target);
  }


  void Racket::setXAxisConstraint(float32 y)
  {
    const float32 W = float32(mTexture.getSize().x);
    const float32 H = float32(mTexture.getSize().y);
    b2BodyDef bd;
    bd.position.y = y;
    b2Body *xAxis = mGame->world()->CreateBody(&bd);
    b2PrismaticJointDef pjd;
    pjd.bodyA = xAxis;
    pjd.bodyB = mBody;
    pjd.collideConnected = false;
    pjd.localAxisA.x = 1.f;
    const float32 s = .5f * Game::InvScale;
    pjd.localAnchorB.Set(s * W, s * H);
    pjd.lowerTranslation = 0.f;
    pjd.upperTranslation = W;
    mGame->world()->CreateJoint(&pjd);
  }


  const b2Vec2 &Racket::position(void) const
  {
    return mTiltingBody->GetPosition();
  }


  b2Body *Racket::body(void)
  {
    return mTiltingBody;
  }


  const b2AABB &Racket::aabb(void) const
  {
    b2Transform t;
    t.SetIdentity();
    mAABB.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
    mAABB.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
    b2Fixture* fixture = mTiltingBody->GetFixtureList();
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
    mJoint->SetMotorSpeed(mTiltingBody->GetAngle() > 0.f ? -1.f : 1.f);
  }


  void Racket::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(Game::Scale * mTiltingBody->GetPosition().x, Game::Scale * mTiltingBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mTiltingBody->GetAngle()));
  }


  void Racket::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


}
