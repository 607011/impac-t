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

#include <ShlObj.h>

namespace Impact {

  Settings gSettings;

  Settings::Settings(void)
    : useShaders(ENABLE_SHADERS)
    , particlesPerExplosion(50U)
    , lastCampaignLevel(1)
    , campaignScore(0)
    , musicVolume(50)
    , soundfxVolume(100)
    , framerateLimit(0)
    , velocityIterations(16)
    , positionIterations(64)
  {
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
      this->appData = szPath;
      this->appData += "\\Impact";
      this->settingsFile = this->appData + "\\settings.xml";
      this->levelsDir = this->appData + "\\levels";
      this->soundFXDir = this->appData + "\\soundfx";
      this->musicDir = this->appData + "\\music";
#ifndef NDEBUG
      std::cout << "settingsFile = '" << settingsFile << "'" << std::endl;
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
    boost::property_tree::ptree pt;
    pt.put("impact.use-shaders", this->useShaders);
    pt.put("impact.frame-rate-limit", this->framerateLimit);
    pt.put("impact.velocity-iterations", this->velocityIterations);
    pt.put("impact.position-iterations", this->positionIterations);
    pt.put("impact.explosion.use-shaders", this->useShadersForExplosions);
    pt.put("impact.explosion.particle-count", this->particlesPerExplosion);
    pt.put("impact.last-open-dir", this->lastOpenDir);
    pt.put("impact.campaign.last-level", this->lastCampaignLevel);
    pt.put("impact.campaign.score", this->campaignScore);
    pt.put("impact.volume.music", this->musicVolume);
    pt.put("impact.volume.soundfx", this->soundfxVolume);
    try {
      boost::property_tree::xml_parser::write_xml(settingsFile, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    return ok;
  }


#pragma warning(disable : 4503)
  bool Settings::load(void)
  {
    bool ok = fileExists(settingsFile);
    if (!ok)
      return true;
    boost::property_tree::ptree pt;
    try {
      boost::property_tree::xml_parser::read_xml(settingsFile, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    if (!ok)
      return false;

    try {
      this->useShaders = pt.get<bool>("impact.use-shaders", true);
      this->useShadersForExplosions = pt.get<bool>("impact.explosion.use-shaders", true);
      this->particlesPerExplosion = pt.get<unsigned int>("impact.explosion.particle-count", 50U);
      this->velocityIterations = pt.get<unsigned int>("impact.velocity-iterations", 16);
      this->positionIterations = pt.get<unsigned int>("impact.position-iterations", 64);
      this->framerateLimit = pt.get<unsigned int>("impact.frame-rate-limit", 0U);
      this->lastOpenDir = pt.get<std::string>("impact.last-open-dir", levelsDir);
      this->lastCampaignLevel = pt.get<int>("impact.campaign.last-level", 1);
      if (this->lastCampaignLevel < 1)
        this->lastCampaignLevel = 1;
      this->campaignScore = pt.get<int>("impact.campaign.score", 0);
      this->soundfxVolume = b2Clamp(pt.get<float>("impact.volume.soundfx", 100.f), 0.f, 100.f);
      this->musicVolume = b2Clamp(pt.get<float>("impact.volume.music", 50.f), 0.f, 100.f);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

#ifndef NDEBUG
    std::cout << "useShaders: " << this->useShaders << std::endl;
    std::cout << "useShadersForExplosions: " << this->useShadersForExplosions << std::endl;
    std::cout << "particlesPerExplosion: " << this->particlesPerExplosion << std::endl;
    std::cout << "framerateLimit: " << this->framerateLimit << std::endl;
    std::cout << "velocityIterations: " << this->velocityIterations << std::endl;
    std::cout << "positionIterations: " << this->positionIterations << std::endl;
    std::cout << "lastOpenDir: " << this->lastOpenDir << std::endl;
    std::cout << "lastCampaignLevel: " << this->lastCampaignLevel << std::endl;
    std::cout << "campaignScore: " << this->campaignScore << std::endl;
    std::cout << "soundfxVolume: " << this->soundfxVolume << std::endl;
    std::cout << "musicVolume: " << this->musicVolume << std::endl;
#endif

    this->useShaders &= sf::Shader::isAvailable();
    this->useShadersForExplosions &= this->useShaders;

    return ok;
  }

}
