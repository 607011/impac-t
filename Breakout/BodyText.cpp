// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  TextBody::TextBody(Game *game, const std::string &text, unsigned int size, const sf::Time &maxAge)
    : Body(Body::BodyType::Text, game)
  {
    //mName = std::string("Text");
    setZIndex(Body::ZIndex::Foreground + 100);
    setLifetime(maxAge);
    setCharacterSize(size);
    mText.setString(sf::String(text));
  }


  void TextBody::setPosition(float x, float y)
  {
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y);
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
