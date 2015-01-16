// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __PAD_H_
#define __PAD_H_

#include <Box2D/Box2D.h>
#include "Body.h"
#include "Breakout.h"

namespace Breakout {

  class Pad : public Body
  {
  public:
    Pad(Game *game);
    virtual ~Pad() {}

    void moveLeft(void);
    void moveRight(void);
    void stopMotion(void);
    void kickLeft(void);
    void kickRight(void);
    void stopKick(void);

    virtual void setPosition(float x, float y);

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;

  private:
    b2RevoluteJoint* mJoint;
    b2Body *mTeetingBody;
  };

}

#endif // __PAD_H_
