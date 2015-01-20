// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BALL_H_
#define __BALL_H_

#include "Body.h"
#include "Breakout.h"
#include "Destructive.h"

namespace Breakout {

  class Ball : public Body
  {
  public:
    Ball(Game *game);
    virtual ~Ball();

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
  };

}

#endif // __BALL_H_

