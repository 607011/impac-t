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

#ifndef __LEVELTIMER_H_
#define __LEVELTIMER_H_

#include <SFML/System.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

namespace Impact {
class LevelTimer {
  public:
    LevelTimer(void)
    {
      restart();
      mActive = false;
    }
    inline void restart(void)
    {
      mClock.restart();
      mTime = sf::Time::Zero;
      mActive = true;
    }
    inline void pause(void)
    {
#ifndef NDEBUG
      std::cout << "LevelTimer is paused." << std::endl;
#endif
      mTime += mClock.restart();
      mActive = false;
    }
    inline void resume(void)
    {
#ifndef NDEBUG
      std::cout << "LevelTimer is active." << std::endl;
#endif
      mActive = true;
      mClock.restart();
    }
    inline const sf::Time &total(void) const
    {
      return mTime;
    }
    inline int accumulatedSeconds(void) const
    {
      const sf::Time &accumulatedTime = mActive ? mClock.getElapsedTime() + mTime : mTime;
      return accumulatedTime.asMilliseconds() / 1000;
    }
    inline bool isActive(void) const
    {
      return mActive;
    }
  private:
    sf::Clock mClock;
    sf::Time mTime;
    bool mActive;
  };

}

#endif // __LEVELTIMER_H_
