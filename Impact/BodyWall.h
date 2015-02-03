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


#ifndef __BODYWALL_H_
#define __BODYWALL_H_

#include "Body.h"
#include "Impact.h"
#include "Destructive.h"

#include <iostream>

namespace Impact {

  class Wall : public Body
  {
  public:
    Wall(int index, Game *game);
#ifndef NDEBUG
    ~Wall() {
      std::cout << "~dtor of " << typeid(this).name() << std::endl;
    }
#endif

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Wall; }

    virtual void setPosition(int x, int y);
  };

}

#endif // __BODYWALL_H_

