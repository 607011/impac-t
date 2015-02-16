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


#ifndef __PARTICLESYSTEM_H_
#define __PARTICLESYSTEM_H_

#include <Box2D/Box2D.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Body.h"
#include "Impact.h"

namespace Impact {
  
  struct SimpleParticle 
  {
    b2Body *body;
    sf::Time lifeTime;
    bool dead;
    sf::Sprite sprite;
  };


  struct ParticleSystemDef
  {
    ParticleSystemDef(Game *game, const b2Vec2 &pos, bool ballCollisionEnabled = false, int count = 50)
      : game(game)
      , pos(pos)
      , ballCollisionEnabled(ballCollisionEnabled)
      , count(count)
      , minLifetime(sf::milliseconds(500))
      , maxLifetime(sf::milliseconds(1000))
      , minSpeed(2.f)
      , maxSpeed(5.f)
      , gravityScale(5.f)
      , linearDamping(.2f)
      , radius(1e-6f)
      , density(1.f)
      , friction(0.f)
      , restitution(.8f)
    { /* ... */ }
    Game *game;
    b2Vec2 pos;
    bool ballCollisionEnabled;
    int count;
    sf::Time minLifetime;
    sf::Time maxLifetime;
    float32 minSpeed;
    float32 maxSpeed;
    float32 gravityScale;
    float32 linearDamping;
    float32 radius;
    float32 density;
    float32 friction;
    float32 restitution;
    sf::Texture texture;
    std::string fragmentShaderCode;
  };


  class ParticleSystem : public Body
  {
  public:
    ParticleSystem(const ParticleSystemDef &);
    virtual ~ParticleSystem();

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Particle; }

  protected:
    std::vector<SimpleParticle> mParticles;
  };

}

#endif // __PARTICLESYSTEM_H_
