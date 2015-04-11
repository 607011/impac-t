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

#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <string>
#include <map>
#include <boost/archive/xml_oarchive.hpp>
#include <memory>

namespace Impact {

  class SettingsPrivate;

  class Settings {
  public:
    Settings(void);

    bool save(void);
    bool load(void);

    void setUseShaders(bool);
    bool useShaders(void) const;
    void setUseShadersForExplosions(bool);
    bool useShadersForExplosions(void) const;
    void setLastOpenDir(std::string);
    const std::string &lastOpenDir(void) const;
    const std::string &levelsDir(void) const;
    const std::string &musicDir(void) const;
    const std::string &soundFXDir(void) const;
    void setMusicVolume(float);
    float musicVolume(void) const;
    void setSoundFXVolume(float);
    float soundFXVolume(void) const;
    void setParticlesPerExplosion(unsigned int);
    unsigned int particlesPerExplosion(void) const;
    void setLastCampaignLevel(int) const;
    int lastCampaignLevel(void) const;
    void setFramerateLimit(int);
    int framerateLimit(void) const;
    void setPositionIterations(int);
    int positionIterations(void) const;
    void setVelocityIterations(int);
    int velocityIterations(void) const;

  private:
    std::shared_ptr<SettingsPrivate> d;

    friend class boost::serialization::access;
    template<class archive>
    void serialize(archive& ar, const unsigned int version);
  };

}

#endif // __SETTINGS_H_
