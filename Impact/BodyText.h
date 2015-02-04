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


#ifndef __BODYTEXT_H_
#define __BODYTEXT_H_

#include "Body.h"
#include "Impact.h"
#include "Destructive.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>

namespace Impact {

  class TextBody : public Body
  {
  public:
    TextBody(Game *game, const std::string &text, unsigned int size = 22U, const sf::Time &maxAge = sf::milliseconds(500));

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Text; }

    virtual void setPosition(float x, float y);

    void setCharacterSize(unsigned int size);
    void setText(const char *);
    void setText(const std::string &);
    void setFont(const sf::Font &);

  private:
    sf::Text mText;

  };

}

#endif // __BODYTEXT_H_

