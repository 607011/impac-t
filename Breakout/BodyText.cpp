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


#include "stdafx.h"

namespace Breakout {

  TextBody::TextBody(Game *game, const std::string &text, unsigned int size, const sf::Time &maxAge)
    : Body(Body::BodyType::Text, game)
  {
    setZIndex(Body::ZIndex::Foreground + 100);
    setLifetime(maxAge);
    setCharacterSize(size);
    mText.setString(sf::String(text));
  }


  void TextBody::setPosition(float x, float y)
  {
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x - 0.5f * Game::InvScale * mText.getGlobalBounds().width , y - 0.5f * Game::InvScale * mText.getGlobalBounds().height);
    bd.gravityScale = -2.f;
    bd.fixedRotation = true;
    mBody = mGame->world()->CreateBody(&bd);
  }


  void TextBody::setCharacterSize(unsigned int size)
  {
    mText.setCharacterSize(size);
  }


  void TextBody::setFont(const sf::Font &font)
  {
    mText.setFont(font);
  }


  void TextBody::onUpdate(float elapsedSeconds)
  {
    sf::Uint8 alpha = sf::Uint8(Easing<float>::quadEaseInOut(age().asSeconds(), 0, 255, mMaxAge.asSeconds()));
    mText.setColor(sf::Color(255, 255, 255, alpha));
    mText.setPosition(mGame->tileWidth() * mBody->GetPosition().x, mGame->tileHeight() * mBody->GetPosition().y);
    if (overAge())
      this->kill();
  }


  void TextBody::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mText, states);
  }

}
