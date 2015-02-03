/*  

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef __DESTRUCTIBLE_H_
#define __DESTRUCTIBLE_H_

#include <algorithm>
#include <limits.h>

namespace Impact {

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
