// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  Pad::Pad(Game *game)
    : Body(Body::BodyType::Pad, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Pad");
    mTexture = mGame->level()->texture(mName);

    const float sx = 1.f / mGame->tileWidth();
    const float sy = 1.f / mGame->tileHeight();

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    // pad
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.gravityScale = 0.f;
      bd.allowSleep = true;
      bd.awake = true;
      bd.fixedRotation = false;
      mTeetingBody = mGame->world()->CreateBody(&bd);

      b2PolygonShape polygon;
      b2Vec2 vertices[8];
      vertices[0] = b2Vec2(sx * -32, sy * -6);
      vertices[1] = b2Vec2(sx * -32, sy *  6);
      vertices[2] = b2Vec2(sx * -30, sy *  8);
      vertices[3] = b2Vec2(sx *  30, sy *  8);
      vertices[4] = b2Vec2(sx *  32, sy *  6);
      vertices[5] = b2Vec2(sx *  32, sy * -6);
      vertices[6] = b2Vec2(sx *  30, sy * -8);
      vertices[7] = b2Vec2(sx * -32, sy * -8);
      polygon.Set(vertices, 8);

      b2FixtureDef fd;
      fd.shape = &polygon;
      fd.density = 11.f;
      fd.friction = .7f;
      fd.restitution = .99f;
      mTeetingBody->CreateFixture(&fd);
    }

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
      pjd.localAnchorB.SetZero();
      mGame->world()->CreateJoint(&pjd);
    }


    //// left spring
    //{
    //  b2BodyDef bd;
    //  bd.position.Set(0.f, 1.5f);
    //  bd.type = b2_dynamicBody;
    //  bd.allowSleep = true;
    //  bd.awake = true;
    //  b2Body *spring = mGame->world()->CreateBody(&bd);

    //  b2DistanceJointDef djd;
    //  djd.bodyA = spring;
    //  djd.bodyB = mTeetingBody;
    //  djd.localAnchorA.SetZero();
    //  djd.localAnchorB.Set(-16.f, 0.f);
    //  b2Vec2 d = djd.bodyB->GetWorldPoint(djd.localAnchorB) - djd.bodyA->GetWorldPoint(djd.localAnchorA);
    //  djd.length = d.Length();
    //  std::cout << djd.length << std::endl;
    //  djd.dampingRatio = .1f;
    //  djd.frequencyHz = 20.f;
    //  mGame->world()->CreateJoint(&djd);
    //}

    //// right spring
    //{
    //  b2BodyDef bd;
    //  bd.position.Set(0.f, 1.5f);
    //  bd.type = b2_dynamicBody;
    //  b2Body *spring = mGame->world()->CreateBody(&bd);

    //  b2DistanceJointDef djd;
    //  djd.bodyA = spring;
    //  djd.bodyB = mTeetingBody;
    //  djd.localAnchorA.SetZero();
    //  djd.localAnchorB.Set(+16.f, 0.f);
    //  b2Vec2 d = djd.bodyB->GetWorldPoint(djd.localAnchorB) - djd.bodyA->GetWorldPoint(djd.localAnchorA);
    //  djd.length = d.Length();
    //  std::cout << djd.length << std::endl;
    //  djd.dampingRatio = .1f;
    //  djd.frequencyHz = 20.f;
    //  mGame->world()->CreateJoint(&djd);
    //}

  }


  void Pad::setPosition(float x, float y)
  {
    Body::setPosition(x, y);
    const b2Transform &tx = mBody->GetTransform();
    mTeetingBody->SetTransform(tx.p, tx.q.GetAngle());
  }


  void Pad::moveLeft(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2(-25.f, 0.f));
  }


  void Pad::moveRight(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2(+25.f, 0.f));
  }


  void Pad::stopMotion(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2_zero);
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


  void Pad::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(mGame->tileWidth() * mTeetingBody->GetPosition().x, mGame->tileHeight() * mTeetingBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mTeetingBody->GetAngle()));
  }


  void Pad::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  const b2Vec2 &Pad::position(void) const
  {
    return mTeetingBody->GetPosition();
  }

}
