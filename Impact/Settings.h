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

namespace Impact {

  class Settings {
  public:
    Settings(void)
      : useShaders(false)
      , particlesPerExplosion(50)
    {
      useShaders &= sf::Shader::isAvailable();
    }

    bool save(void);
    bool load(void);

    bool useShaders;
    unsigned int particlesPerExplosion;




  };

}

#endif // __SETTINGS_H_
