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

namespace Impact {

  TextBody::TextBody(const TextBodyDef &def)
    : Body(Body::BodyType::Text, def.game)
  {
    setLifetime(def.maxAge);
    mText.setCharacterSize(def.size);
    mText.setFont(def.font);
    mText.setString(sf::String(def.text));
    mText.setOrigin(.5f * mText.getLocalBounds().width, -.5f * mText.getLocalBounds().height);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(std::ceil(def.pos.x - 0.5f * Game::InvScale * mSprite.getGlobalBounds().width), std::ceil(def.pos.y - 0.5f * Game::InvScale * mSprite.getGlobalBounds().height));
    bd.gravityScale = -1.f;
    bd.fixedRotation = true;
    mBody = mGame->world()->CreateBody(&bd);
  }


  void TextBody::onUpdate(float elapsedSeconds)
  {
    mText.setPosition(Game::Scale * mBody->GetPosition().x, Game::Scale * mBody->GetPosition().y);
    if (overAge())
      this->kill();
  }


  void TextBody::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mText);
  }

}
