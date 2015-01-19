// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BLOCK_H_
#define __BLOCK_H_

#include "Body.h"
#include "Breakout.h"
#include "Destructive.h"

namespace Breakout {

  class Block : public Body
  {
  public:
    Block(int index, Game *game);
    virtual ~Block() {}

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;

    void hit(float impulse);

  };

}

#endif // __BLOCK_H_

