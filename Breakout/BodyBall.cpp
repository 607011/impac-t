// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  Ball::Ball(Game *game)
    : Body(Body::BodyType::Ball, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = "Ball";
    mTexture = TextureCache::texture(mName);

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.angle = 0.f;
    bd.linearDamping = 0.5f;
    bd.angularDamping = 0.21f;
    bd.gravityScale = 1.f;
    bd.allowSleep = false;
    bd.awake = true;
    bd.fixedRotation = false;
    bd.linearVelocity = b2Vec2(10.f - 20.f * std::cos(0.5f * _PI * float(std::rand()) / RAND_MAX), -40.f);
    bd.bullet = true;
    bd.active = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    const float sx = 1.f / game->tileWidth();
    const float sy = 1.f / game->tileHeight();

    b2CircleShape circle;
    circle.m_p.SetZero();
    circle.m_radius = W * sx * 0.5f;

    b2FixtureDef fd;
    fd.shape = &circle;
    fd.density = 11.f;
    fd.friction = 0.7f;
    fd.restitution = 1.f;
    mBody->CreateFixture(&fd);
  }


  void Ball::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(mGame->tileWidth() * mBody->GetPosition().x, mGame->tileHeight() * mBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mBody->GetAngle()));
  }


  void Ball::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }

}
