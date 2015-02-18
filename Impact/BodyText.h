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

  struct TextBodyDef {
    TextBodyDef(Game *game, const std::string &text, const sf::Font &font, const b2Vec2 &pos)
      : game(game)
      , pos(pos)
      , size(24U)
      , text(text)
      , font(font)
      , maxAge(sf::milliseconds(500))
    { /* ... */ }
    Game *game;
    b2Vec2 pos;
    const std::string &text;
    unsigned int size;
    const sf::Font &font;
    sf::Time maxAge;
    std::string fragmentShaderCode;
  };

  class TextBody : public Body
  {
  public:
    TextBody(const TextBodyDef &);

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Text; }

  private:
    sf::Shader mShader;
  };

}

#endif // __BODYTEXT_H_

