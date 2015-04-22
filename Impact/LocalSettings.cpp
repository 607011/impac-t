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
#include <boost/serialization/version.hpp>
#include <boost/serialization/map.hpp>

#if defined(WIN32)
#include <ShlObj.h>
#endif

#if defined(LINUX_AMD64)
#include <X11/Xlib.h>
#endif

BOOST_CLASS_VERSION(Impact::LocalSettings, 1)

namespace Impact {

  class LocalSettingsPrivate {
  public:
    LocalSettingsPrivate(void)
      : useShaders(false)
      , particlesPerExplosion(50U)
      , lastCampaignLevel(1)
      , campaignHighscore(0LL)
      , musicVolume(50)
      , soundfxVolume(100)
      , framerateLimit(0)
      , velocityIterations(32)
      , positionIterations(64)
    { /* ... */ }
    bool useShaders;
    bool useShadersForExplosions;
    unsigned int particlesPerExplosion;
    std::string lastOpenDir;
    int lastCampaignLevel;
    int64_t campaignHighscore;
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

    std::map<int, int64_t> highscores;
  };

  LocalSettings& gLocalSettings() {
     static LocalSettings* localSettings = new LocalSettings();
     return *localSettings;
  }

  LocalSettings::LocalSettings(void)
    : d(new LocalSettingsPrivate)
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
      load();
    }
#elif defined(LINUX_AMD64)
    XInitThreads(); // workaround for SFML threading issue, need to call this as early as possible
    // see also: http://en.sfml-dev.org/forums/index.php?topic=14853.0
    // this constructor is a good candidate for early, as gLocalSettings() is called from everywhere ;-)

    char* home = getenv("HOME");
    d->appData = home;
    d->appData += "/.impact";
    d->settingsFile = d->appData + "/settings.xml";
    d->levelsDir = d->appData + "/levels";
    d->soundFXDir = d->appData + "/soundfx";
    d->musicDir = d->appData + "/music";
#ifndef NDEBUG
    std::cout << "settingsFile = '" << d->settingsFile << "'" << std::endl;
#endif
    load();
#endif //LINUX_AMD64
  }


  bool LocalSettings::save(void)
  {
    bool ok = true;
    std::ofstream ofs(d->settingsFile);
    unsigned int flags = boost::archive::no_header | boost::archive::no_tracking | boost::archive::no_xml_tag_checking;
    boost::archive::xml_oarchive xml(ofs, flags);
    xml << boost::serialization::make_nvp("impact", *this);
    return ok;
  }


#pragma warning(disable : 4503)
  bool LocalSettings::load(void)
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
      d->campaignHighscore = pt.get<int64_t>("impact.campaign-highscore", 0ULL);
      d->soundfxVolume = b2Clamp(pt.get<float>("impact.soundfx-volume", 100.f), 0.f, 100.f);
      d->musicVolume = b2Clamp(pt.get<float>("impact.music-volume", 50.f), 0.f, 100.f);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

    try {
#ifndef NDEBUG
      std::cout << "****HIGHSCORES****" << std::endl;
#endif
      const boost::property_tree::ptree &highscores = pt.get_child("impact.highscores");
      boost::property_tree::ptree::const_iterator pi;
      for (pi = highscores.begin(); pi != highscores.end(); ++pi) {
        boost::property_tree::ptree property = pi->second;
        if (pi->first == "item") {
          const int level = pi->second.get<int>("first", 0);
          const int64_t highscore = pi->second.get<int64_t>("second", 0LL);
          d->highscores[level] = highscore;
#ifndef NDEBUG
          std::cout << "level " << std::dec << std::setw(4) << level << ": " << std::setw(15) << highscore << std::endl;
#endif
        }
      }
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

    d->useShaders &= sf::Shader::isAvailable();
    d->useShadersForExplosions &= d->useShaders;
    return ok;
  }


  template<class archive>
  void LocalSettings::serialize(archive& ar, const unsigned int version)
  {
    UNUSED(version);
    ar & boost::serialization::make_nvp("use-shaders", d->useShaders);
    ar & boost::serialization::make_nvp("use-shaders-for-explosions", d->useShadersForExplosions);
    ar & boost::serialization::make_nvp("explosion-particle-count", d->particlesPerExplosion);
    ar & boost::serialization::make_nvp("highscores", d->highscores);
    ar & boost::serialization::make_nvp("frame-rate-limit", d->framerateLimit);
    ar & boost::serialization::make_nvp("velocity-iterations", d->velocityIterations);
    ar & boost::serialization::make_nvp("position-iterations", d->positionIterations);
    ar & boost::serialization::make_nvp("last-open-dir", d->lastOpenDir);
    ar & boost::serialization::make_nvp("campaign-last-level", d->lastCampaignLevel);
    ar & boost::serialization::make_nvp("campaign-highscore", d->campaignHighscore);
    ar & boost::serialization::make_nvp("music-volume", d->musicVolume);
    ar & boost::serialization::make_nvp("soundfx-volume", d->soundfxVolume);
  }


  void LocalSettings::setUseShaders(bool use)
  {
    d->useShaders = use;
  }


  bool LocalSettings::useShaders(void) const
  {
    return d->useShaders;
  }


  void LocalSettings::setUseShadersForExplosions(bool use)
  {
    d->useShadersForExplosions = use;
  }


  bool LocalSettings::useShadersForExplosions(void) const
  {
    return d->useShadersForExplosions;
  }


  void LocalSettings::setLastOpenDir(std::string lastOpenDir)
  {
    d->lastOpenDir = lastOpenDir;
  }


  const std::string &LocalSettings::lastOpenDir(void) const
  {
    return d->lastOpenDir;
  }


  const std::string &LocalSettings::levelsDir(void) const
  {
    return d->levelsDir;
  }


  const std::string &LocalSettings::musicDir(void) const
  {
    return d->musicDir;
  }


  const std::string &LocalSettings::soundFXDir(void) const
  {
    return d->soundFXDir;
  }


  void LocalSettings::setMusicVolume(float volume)
  {
    d->musicVolume = volume;
  }


  float LocalSettings::musicVolume(void) const
  {
    return d->musicVolume;
  }


  void LocalSettings::setSoundFXVolume(float volume)
  {
    d->soundfxVolume = volume;
  }


  float LocalSettings::soundFXVolume(void) const
  {
    return d->soundfxVolume;
  }


  void LocalSettings::setParticlesPerExplosion(unsigned int n)
  {
    d->particlesPerExplosion = n;
  }


  unsigned int LocalSettings::particlesPerExplosion(void) const
  {
    return d->particlesPerExplosion;
  }


  void LocalSettings::setLastCampaignLevel(int level) const
  {
    d->lastCampaignLevel = level;
  }


  int LocalSettings::lastCampaignLevel(void) const
  {
    return d->lastCampaignLevel;
  }


  void LocalSettings::setFramerateLimit(int limit)
  {
    d->framerateLimit = limit;
  }


  int LocalSettings::framerateLimit(void) const
  {
    return d->framerateLimit;
  }


  void LocalSettings::setPositionIterations(int n)
  {
    d->positionIterations = n;
  }


  int LocalSettings::positionIterations(void) const
  {
    return d->positionIterations;
  }


  void LocalSettings::setVelocityIterations(int n)
  {
    d->velocityIterations = n;
  }


  int LocalSettings::velocityIterations(void) const
  {
    return d->velocityIterations;
  }


  void LocalSettings::setHighscore(int level, int64_t score)
  {
    d->highscores[level] = score;
  }


  int64_t LocalSettings::highscore(int level) const
  {
    return d->highscores[level];
  }


  void LocalSettings::setHighscore(int64_t score)
  {
    d->campaignHighscore = score;
  }


  int64_t LocalSettings::highscore(void) const
  {
    return d->campaignHighscore;
  }


  bool LocalSettings::isHighscore(int64_t score) const
  {
    return score > d->campaignHighscore;
  }


  bool LocalSettings::isHighscore(int level, int64_t score) const
  {
    std::map<int, int64_t>::const_iterator h = d->highscores.find(level);
    if (h == d->highscores.end())
      return true;
    return score > h->second;
  }


}
