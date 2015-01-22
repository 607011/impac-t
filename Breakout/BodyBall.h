// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BODYBALL_H_
#define __BODYBALL_H_

#include "Body.h"
#include "Breakout.h"
#include "Destructive.h"

namespace Breakout {

  class Ball : public Body
  {
  public:
    Ball(Game *game);
#ifndef NDEBUG
    ~Ball() {
      std::cout << "~dtor of " << typeid(this).name() << std::endl;
    }
#endif


    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Ball; }
  };

}

#endif // __BODYBALL_H_

