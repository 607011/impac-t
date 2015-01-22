// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __BODYTEXT_H_
#define __BODYTEXT_H_

#include "Body.h"
#include "Breakout.h"
#include "Destructive.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>

namespace Breakout {

  class TextBody : public Body
  {
  public:
    TextBody(Game *game, const std::string &text, unsigned int size = 22U, const sf::Time &maxAge = sf::milliseconds(500));
#ifndef NDEBUG
    ~TextBody() {
      std::cout << "~dtor of " << typeid(this).name() << std::endl;
    }
#endif

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

