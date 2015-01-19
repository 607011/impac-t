// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __GROUND_H_
#define __GROUND_H_

#include "Body.h"
#include "Breakout.h"

namespace Breakout {

  class Ground : public Body
  {
  public:
    Ground(Game *game);
    virtual ~Ground();

    virtual void setBody(b2Body *body)
    {
      Body::setBody(body);
      body->SetUserData(this);
    }

    // Body implementation
    virtual void onUpdate(float elapsedSeconds) { /* ... */ }
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const  { /* ... */ }
  };

}

#endif // __GROUND_H_

