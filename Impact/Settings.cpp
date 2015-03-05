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
    , verticalSync(false)
    , antialiasing(16U)
    , particlesPerExplosion(50)
  {
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
      appData = szPath;
      appData += "\\Impact";
      settingsFile = appData + "\\settings.xml";
#ifndef NDEBUG
      std::cout << "settingsFile = '" << settingsFile << "'" << std::endl;
#endif
      load();
    }
  }


  bool Settings::save(void)
  {
    // TODO ...
    return false;
  }


  /**
  <impact>
    <vertical-sync>true</vertical-sync>

  </impact>
  */

  bool Settings::load(void)
  {
    bool ok;
    ok = fileExists(settingsFile);
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
      useShaders = pt.get<bool>("impact.use-shaders", true);
      verticalSync = pt.get<bool>("impact.vertical-sync", false);
      particlesPerExplosion = pt.get<unsigned int>("impact.particles-per-explosion", 50);
      antialiasing = pt.get<unsigned int>("impact.antialiasing", 16U);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }


#ifndef NDEBUG
    std::cout << "useShaders: " << useShaders << std::endl;
    std::cout << "verticalSync: " << verticalSync << std::endl;
    std::cout << "particlesPerExplosion: " << particlesPerExplosion << std::endl;
    std::cout << "antialiasing: " << antialiasing << std::endl;
#endif

    useShaders &= sf::Shader::isAvailable();

    return true;
  }

}
