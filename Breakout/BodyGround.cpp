// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {


  Ground::Ground(Game *game, float width)
    : Body(Body::BodyType::Ground, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("Ground");

    {
      b2BodyDef bd;
      bd.userData = this;
      mBody = mGame->world()->CreateBody(&bd);

      b2EdgeShape bottomBoundary;
      bottomBoundary.Set(b2Vec2_zero, b2Vec2(width, 0.f));
      mBody->CreateFixture(&bottomBoundary, 0.f);
    }

  }


  Ground::~Ground()
  {
    mGame->world()->DestroyBody(mBody);
  }


}
