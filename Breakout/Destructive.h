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
