// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BODYGROUND_H_
#define __BODYGROUND_H_

#include "Body.h"
#include "Breakout.h"

namespace Breakout {

  class Ground : public Body
  {
  public:
    Ground(Game *game, float width);
#ifndef NDEBUG
    ~Ground() {
      std::cout << "~dtor of " << typeid(this).name() << std::endl;
    }
#endif

    // Body implementation
    virtual void onUpdate(float) { /* ... */ }
    virtual void onDraw(sf::RenderTarget &, sf::RenderStates) const  { /* ... */ }
    virtual BodyType type(void) const { return Body::BodyType::Ground; }

  };

}

#endif // __BODYGROUND_H_

