// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __DESTRUCTIVE_H_
#define __DESTRUCTIVE_H_

namespace Breakout {

  class Destructive {
  public:
    virtual ~Destructive() {}
    virtual int getDestructiveEnergy(void) const = 0;
    virtual void setDestructiveEnergy(int energy)
    {
      mDestructiveEnergy = energy;
    }

  protected:
    int mDestructiveEnergy;
  };

}


#endif // __DESTRUCTIVE_H_
