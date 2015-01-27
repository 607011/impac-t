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
  
#define PARTICLES_WITH_SPRITES

  struct SimpleParticle 
  {
    b2Body *body;
    sf::Time lifeTime;
    bool dead;
#ifdef PARTICLES_WITH_SPRITES
    sf::Sprite sprite;
#endif
  };


  class ParticleSystem : public Body
  {
  public:
    ParticleSystem(Game *game, const b2Vec2 &pos, int count = 50);
    virtual ~ParticleSystem();

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Particle; }

  protected:
    static const float sHalfSize;
    static const sf::Time sMaxAge;
    std::vector<SimpleParticle> mParticles;
#ifndef PARTICLES_WITH_SPRITES
    sf::VertexArray mVertices;
#endif
  };

}

#endif // __PARTICLESYSTEM_H_
