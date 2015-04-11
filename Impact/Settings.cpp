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

#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/map.hpp>

#if defined(WIN32)
#include <ShlObj.h>
#endif

namespace Impact {

  class SettingsPrivate {
  public:
    SettingsPrivate(void)
      : useShaders(false)
      , particlesPerExplosion(50U)
      , lastCampaignLevel(1)
      , campaignScore(0)
      , musicVolume(50)
      , soundfxVolume(100)
      , framerateLimit(0)
      , velocityIterations(16)
      , positionIterations(64)
    { /* ... */ }
    bool useShaders;
    bool useShadersForExplosions;
    unsigned int particlesPerExplosion;
    std::string lastOpenDir;
    int lastCampaignLevel;
    int campaignScore;
    float musicVolume;
    float soundfxVolume;
    unsigned int framerateLimit;
    int velocityIterations;
    int positionIterations;

    std::string appData;
    std::string settingsFile;
    std::string levelsDir;
    std::string soundFXDir;
    std::string musicDir;

    std::map<int, int> highscores;
  };


  Settings gSettings;

  Settings::Settings(void)
    : d(new SettingsPrivate)
  {
#if defined(WIN32)
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
      d->appData = szPath;
      d->appData += "\\Impact";
      d->settingsFile = d->appData + "\\settings.xml";
      d->levelsDir = d->appData + "\\levels";
      d->soundFXDir = d->appData + "\\soundfx";
      d->musicDir = d->appData + "\\music";
#ifndef NDEBUG
      std::cout << "settingsFile = '" << d->settingsFile << "'" << std::endl;
#endif
#endif
      load();
    }
  }


  bool Settings::save(void)
  {
    bool ok = true;
#ifndef NDEBUG
    std::cout << "Settings::save()" << std::endl;
#endif
    std::ofstream ofs(d->settingsFile);
    unsigned int flags = boost::archive::no_header | boost::archive::no_tracking | boost::archive::no_xml_tag_checking;
    boost::archive::xml_oarchive xml(ofs, flags);
    xml << boost::serialization::make_nvp("impact", this);
    return ok;
  }


#pragma warning(disable : 4503)
  bool Settings::load(void)
  {
    bool ok = fileExists(d->settingsFile);
    if (!ok)
      return true;
    boost::property_tree::ptree pt;
    try {
      boost::property_tree::xml_parser::read_xml(d->settingsFile, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    if (!ok)
      return false;

    try {
      d->useShaders = pt.get<bool>("impact.use-shaders", true);
      d->useShadersForExplosions = pt.get<bool>("impact.use-shaders-for-explosions", true);
      d->particlesPerExplosion = pt.get<unsigned int>("impact.explosion-particle-count", 50U);
      d->velocityIterations = pt.get<unsigned int>("impact.velocity-iterations", 16);
      d->positionIterations = pt.get<unsigned int>("impact.position-iterations", 64);
      d->framerateLimit = pt.get<unsigned int>("impact.frame-rate-limit", 0U);
      d->lastOpenDir = pt.get<std::string>("impact.last-open-dir", d->levelsDir);
      d->lastCampaignLevel = pt.get<int>("impact.campaign-last-level", 1);
      if (d->lastCampaignLevel < 1)
        d->lastCampaignLevel = 1;
      d->campaignScore = pt.get<int>("impact.campaign-highscore", 0);
      d->soundfxVolume = b2Clamp(pt.get<float>("impact.soundfx-volume", 100.f), 0.f, 100.f);
      d->musicVolume = b2Clamp(pt.get<float>("impact.music-volume", 50.f), 0.f, 100.f);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

    try {

    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

#ifndef NDEBUG
    std::cout << "useShaders: " << d->useShaders << std::endl;
    std::cout << "useShadersForExplosions: " << d->useShadersForExplosions << std::endl;
    std::cout << "particlesPerExplosion: " << d->particlesPerExplosion << std::endl;
    std::cout << "framerateLimit: " << d->framerateLimit << std::endl;
    std::cout << "velocityIterations: " << d->velocityIterations << std::endl;
    std::cout << "positionIterations: " << d->positionIterations << std::endl;
    std::cout << "lastOpenDir: " << d->lastOpenDir << std::endl;
    std::cout << "lastCampaignLevel: " << d->lastCampaignLevel << std::endl;
    std::cout << "campaignScore: " << d->campaignScore << std::endl;
    std::cout << "soundfxVolume: " << d->soundfxVolume << std::endl;
    std::cout << "musicVolume: " << d->musicVolume << std::endl;
#endif

    d->useShaders &= sf::Shader::isAvailable();
    d->useShadersForExplosions &= d->useShaders;
    return ok;
  }


  template<class archive>
  void Settings::serialize(archive& ar, const unsigned int version)
  {
    ar & boost::serialization::make_nvp("use-shaders", d->useShaders);
    ar & boost::serialization::make_nvp("use-shaders-for-explosions", d->useShadersForExplosions);
    ar & boost::serialization::make_nvp("explosion-particle-count", d->particlesPerExplosion);
    ar & boost::serialization::make_nvp("highscores", d->highscores);
    ar & boost::serialization::make_nvp("frame-rate-limit", d->framerateLimit);
    ar & boost::serialization::make_nvp("velocity-iterations", d->velocityIterations);
    ar & boost::serialization::make_nvp("position-iterations", d->positionIterations);
    ar & boost::serialization::make_nvp("last-open-dir", d->lastOpenDir);
    ar & boost::serialization::make_nvp("campaign-last-level", d->lastCampaignLevel);
    ar & boost::serialization::make_nvp("campaign-highscore", d->campaignScore);
    ar & boost::serialization::make_nvp("music-volume", d->musicVolume);
    ar & boost::serialization::make_nvp("soundfx-volume", d->soundfxVolume);
  }


  void Settings::setUseShaders(bool use)
  {
    d->useShaders = use;
  }


  bool Settings::useShaders(void) const
  {
    return d->useShaders;
  }


  void Settings::setUseShadersForExplosions(bool use)
  {
    d->useShadersForExplosions = use;
  }


  bool Settings::useShadersForExplosions(void) const
  {
    return d->useShadersForExplosions;
  }


  void Settings::setLastOpenDir(std::string lastOpenDir)
  {
    d->lastOpenDir = lastOpenDir;
  }


  const std::string &Settings::lastOpenDir(void) const
  {
    return d->lastOpenDir;
  }


  const std::string &Settings::levelsDir(void) const
  {
    return d->levelsDir;
  }


  const std::string &Settings::musicDir(void) const
  {
    return d->musicDir;
  }


  const std::string &Settings::soundFXDir(void) const
  {
    return d->soundFXDir;
  }


  void Settings::setMusicVolume(float volume)
  {
    d->musicVolume = volume;
  }


  float Settings::musicVolume(void) const
  {
    return d->musicVolume;
  }


  void Settings::setSoundFXVolume(float volume)
  {
    d->musicVolume = volume;
  }


  float Settings::soundFXVolume(void) const
  {
    return d->soundfxVolume;
  }


  void Settings::setParticlesPerExplosion(unsigned int n)
  {
    d->particlesPerExplosion = n;
  }


  unsigned int Settings::particlesPerExplosion(void) const
  {
    return d->particlesPerExplosion;
  }


  void Settings::setLastCampaignLevel(int level) const
  {
    d->lastCampaignLevel = level;
  }


  int Settings::lastCampaignLevel(void) const
  {
    return d->lastCampaignLevel;
  }


  void Settings::setFramerateLimit(int limit)
  {
    d->framerateLimit = limit;
  }


  int Settings::framerateLimit(void) const
  {
    return d->framerateLimit;
  }


  void Settings::setPositionIterations(int n)
  {
    d->positionIterations = n;
  }


  int Settings::positionIterations(void) const
  {
    return d->positionIterations;
  }


  void Settings::setVelocityIterations(int n)
  {
    d->velocityIterations = n;
  }


  int Settings::velocityIterations(void) const
  {
    return d->velocityIterations;
  }

}
