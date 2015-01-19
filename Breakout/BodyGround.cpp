// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {


  Ground::Ground(Game *game)
    : Body(Body::BodyType::Ground, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Ground");

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

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

    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * (W - 8) * sx, 0.5f * H * sy);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = 800.f;
    fdBox.friction = mGame->world()->GetGravity().y;
    fdBox.restitution = 0.04f;
    mBody->CreateFixture(&fdBox);

  }


  Ground::~Ground()
  {
    // ...
  }

}
