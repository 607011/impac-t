// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  Pad::Pad(Game *game)
    : Body(Body::BodyType::Pad, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = "Pad";
    mTexture = TextureCache::texture(mName);

    const float sx = 1.f / mGame->tileWidth();
    const float sy = 1.f / mGame->tileHeight();

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    {
      b2BodyDef bd;
      bd.position.Set(20.f, 23.f);
      bd.type = b2_dynamicBody;
      bd.linearDamping = 0.f;
      bd.gravityScale = 0.f;
      bd.allowSleep = true;
      bd.awake = true;
      bd.userData = this;
      bd.fixedRotation = true;
      mBody = mGame->world()->CreateBody(&bd);
    }

    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      //  bd.angularDamping = 0.21f;
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

    b2PrismaticJointDef pjd;
    pjd.bodyA = mGame->ground();
    pjd.bodyB = mBody;
    pjd.collideConnected = false;
    pjd.localAxisA.Set(1.f, 0.f);
    pjd.localAnchorA.Set(25.f, 23.f);
    pjd.localAnchorB.SetZero();
    mGame->world()->CreateJoint(&pjd);

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


  void Pad::setPosition(float x, float y)
  {
    Body::setPosition(x, y);
    const b2Transform &tx = mBody->GetTransform();
    mTeetingBody->SetTransform(tx.p, tx.q.GetAngle());
  }


  void Pad::moveLeft(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2(-30.f, 0.f));
  }


  void Pad::moveRight(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2(+30.f, 0.f));
  }


  void Pad::stopMotion(void)
  {
    mTeetingBody->SetLinearVelocity(b2Vec2_zero);
  }


  void Pad::kickLeft(void)
  {
    mJoint->SetMotorSpeed(-300.f);
  }


  void Pad::kickRight(void)
  {
    mJoint->SetMotorSpeed(+300.f);
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

}
