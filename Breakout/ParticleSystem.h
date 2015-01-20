// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __PARTICLESYSTEM_H_
#define __PARTICLESYSTEM_H_

#include <Box2D/Box2D.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Body.h"
#include "Breakout.h"

namespace Breakout {


  struct SimpleParticle 
  {
    b2Body *body;
    sf::Time lifeTime;
  };


  class ParticleSystem : public Body
  {
  public:
    ParticleSystem(Game *game, int count = 100);
    virtual ~ParticleSystem();

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;

    virtual void setPosition(float x, float y);
    virtual void setColor(const sf::Color &);

  protected:
    static const sf::Time sMaxAge;
    std::vector<SimpleParticle> mParticles;
    sf::Color mColor;
    sf::VertexArray mVertices;
  };

}

#endif // __PARTICLESYSTEM_H_
