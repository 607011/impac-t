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
      , mScore(0)
    { /* ... */ }
    virtual ~Destructible()
    { /* ... */ }
    virtual int getScore(void) const
    {
      return mScore;
    }
    virtual void setScore(int score)
    {
      mScore = score;
    }
    virtual bool hit(int energy)
    {
      mEnergy = std::max(mEnergy - energy, 0);
      return 0 == mEnergy;
    }
    virtual void lethalHit(void)
    {
      setEnergy(0);
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
    int mScore;
  };

}


#endif // __DESTRUCTIBLE_H_
