// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {


  Block::Block(int index, Game *game)
    : Body(Body::BodyType::Block, game)
  {
    setZIndex(Body::ZIndex::Intermediate + 0);
    setEnergy(100);
    mTexture = mGame->level()->texture(index);
    mName = std::string("Block");

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.angle = 0.f;
    bd.linearDamping = 0.f;
    bd.angularDamping = 0.61f;
    bd.gravityScale = 0.f;
    bd.allowSleep = true;
    bd.awake = false;
    bd.fixedRotation = false;
    bd.bullet = false;
    bd.active = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    const float sx = 1.f / game->tileWidth();
    const float sy = 1.f / game->tileHeight();

// #define USE_POLYGON_AS_BLOCK_SHAPE
#ifdef USE_POLYGON_AS_BLOCK_SHAPE
    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * W * sx, 0.5f * H * sy);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = 800.f;
    fdBox.friction = mGame->world()->GetGravity().y;
    fdBox.restitution = 0.04f;
    fdBox.userData = nullptr;
    mBody->CreateFixture(&fdBox);
#else
    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * (W - 8) * sx, 0.5f * H * sy);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = 800.f;
    fdBox.friction = mGame->world()->GetGravity().y;
    fdBox.restitution = 0.04f;
    fdBox.userData = nullptr;
    mBody->CreateFixture(&fdBox);

    b2CircleShape circleL;
    circleL.m_p.Set(-8.f * sx, 0.f);
    circleL.m_radius = 8.f * sx;

    b2FixtureDef fdCircleL;
    fdCircleL.shape = &circleL;
    fdCircleL.density = 800.f;
    fdCircleL.friction = mGame->world()->GetGravity().y;
    fdCircleL.restitution = 0.04f;
    mBody->CreateFixture(&fdCircleL);

    b2CircleShape circleR;
    circleR.m_p.Set(+8.f * sx, 0.f);
    circleR.m_radius = 8.f * sx;

    b2FixtureDef fdCircleR;
    fdCircleR.shape = &circleR;
    fdCircleR.density = 800.f;
    fdCircleR.friction = mGame->world()->GetGravity().y;
    fdCircleR.restitution = 0.04f;
    mBody->CreateFixture(&fdCircleR);
#endif
  }


  void Block::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    const b2Transform &tx = mBody->GetTransform();
    mSprite.setPosition(mGame->tileWidth() * tx.p.x, mGame->tileHeight() * tx.p.y);
    mSprite.setRotation(rad2deg(tx.q.GetAngle()));
  }


  void Block::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  void Block::hit(float impulse)
  {
    bool destroyed = Body::hit(int(impulse));
    if (destroyed)
      mBody->SetGravityScale(2.f);
  }

}
