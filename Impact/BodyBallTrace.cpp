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

  const sf::Time BallTrace::sMaxAge = sf::milliseconds(100);
  const sf::Color BallTrace::sColor = sf::Color::White;

  BallTrace::BallTrace(Game *game)
    : Body(Body::BodyType::Ball, game)
    , mTraces(10)
  {
    mName = std::string("Ball");
    setLifetime(sMaxAge);
    mTexture = mGame->level()->texture(mName);
    mTexture.setRepeated(false);
    mTexture.setSmooth(false);

    mCurrentTraceIndex = 0;

    const int N = mTraces.size();
    for (int i = 0; i < N; ++i) {
      SimpleBallTrace &p = mTraces[i];
      p.sprite.setTexture(mTexture);
      p.sprite.setOrigin(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y);
    }
  }


  void BallTrace::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    const int N = mTraces.size();
    for (int i = 0; i < N; ++i) {
       SimpleBallTrace &p = mTraces[i];
       const sf::Time &age = p.age.getElapsedTime();
       const sf::Uint8 alpha = age < sMaxAge
         ? 0xff - sf::Uint8(Easing<float>::quadEaseIn(age.asSeconds(), 0.0f, 255.0f, sMaxAge.asSeconds()))
         : 0x00;
       p.sprite.setColor(sf::Color(0xff, 0xff, 0xff, alpha));
    }
  }


  void BallTrace::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    for (std::vector<SimpleBallTrace>::const_iterator p = mTraces.cbegin(); p != mTraces.cend(); ++p)
      if (p->age.getElapsedTime() < sMaxAge)
        target.draw(p->sprite, states);
  }


  void BallTrace::addMarker(const b2Vec2 &pos, float32 angle)
  {
    SimpleBallTrace &trace = mTraces[mCurrentTraceIndex];
    trace.age.restart();
    trace.sprite.setPosition(pos.x * Game::Scale, pos.y * Game::Scale);
    trace.sprite.setRotation(rad2deg(angle));
    mCurrentTraceIndex = (mCurrentTraceIndex + 1) % mTraces.size();
  }


  void BallTrace::addMarker(float x, float y, float32 angle)
  {
    addMarker(b2Vec2(x, y), angle);
  }


}
