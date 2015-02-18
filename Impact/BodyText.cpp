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
    sf::Text text;
    text.setCharacterSize(def.size);
    text.setFont(def.font);
    text.setString(sf::String(def.text));
    const float W = text.getLocalBounds().width;
    const float H = text.getLocalBounds().height;
    const unsigned int W2 = 2 * unsigned int(W);
    const unsigned int H2 = 2 * unsigned int(H);
    text.setOrigin(-.5f * W, .5f * H);
    text.setPosition(0.f, .5f * H);
    sf::RenderTexture renderTexture;
    renderTexture.create(W2, H2);
    renderTexture.draw(text);
    mTexture = renderTexture.getTexture();
    mTexture.setSmooth(true);
    mSprite.setTexture(mTexture);
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(def.pos.x - 0.5f * Game::InvScale * mSprite.getGlobalBounds().width, def.pos.y - 0.5f * Game::InvScale * mSprite.getGlobalBounds().height);
    bd.gravityScale = -1.f;
    bd.fixedRotation = true;
    mBody = mGame->world()->CreateBody(&bd);
    mShader.loadFromMemory(def.fragmentShaderCode, sf::Shader::Fragment);
    mShader.setParameter("uResolution", sf::Vector2f(float(W2 * 2), float(H2 * 2)));
    mShader.setParameter("uMaxT", def.maxAge.asSeconds());
  }


  void TextBody::onUpdate(float elapsedSeconds)
  {
    mShader.setParameter("uT", age().asSeconds());
    mSprite.setPosition(Game::Scale * mBody->GetPosition().x, Game::Scale * mBody->GetPosition().y);
    if (overAge())
      this->kill();
  }


  void TextBody::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    states.shader = &mShader;
    target.draw(mSprite, states);
  }

}
