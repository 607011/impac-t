// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __DESTRUCTIBLE_H_
#define __DESTRUCTIBLE_H_

#include <algorithm>
#include <limits.h>

namespace Breakout {

  class Destructible {
  public:
    Destructible(int energy = INT_MAX)
      : mEnergy(energy)
    { /* ... */ }
    virtual ~Destructible()
    { /* ... */ }
    virtual int getScore(void) const
    {
      return 0;
    }
    virtual bool hit(int energy)
    {
      mEnergy = std::max(mEnergy - energy, 0);
      return 0 == mEnergy;
    }
    virtual void setEnergy(int energy)
    {
      mEnergy = energy;
    }
    int energy(void) const
    {
      return mEnergy;
    }

  private:
    int mEnergy;
  };

}


#endif // __DESTRUCTIBLE_H_
