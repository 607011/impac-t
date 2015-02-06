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


#ifndef __UTIL_H_
#define __UTIL_H_

#include <string>
#include <cstdint>

#define UNUSED(x) (void)(x)
#define tr(x) std::string(x)

namespace Impact {

  static const float _PI =  3.14159265359f;
  static const float _2PI = 6.28318530718f;
  static const float _DEG2RAD = _PI / 180.f;
  static const float _RAD2DEG = 180.f / _PI;

  template <typename T> inline T deg2rad(T angle) {
    return angle * T(_DEG2RAD);
  }
  template <typename T> inline T rad2deg(T angle) {
    return angle * T(_RAD2DEG);
  }
  template <typename T> inline int sign(T val) {
    return (T(0) < val) - (val < T(0));
  }
  template <class T>
  inline void safeDelete(T &a) {
    if (a)
      delete a;
    a = nullptr;
  }
  template <class T>
  inline void safeFree(T &a) {
    if (a)
      free(a);
    a = nullptr;
  }

  template <class T>
  inline void safeDeleteArray(T &a) {
    if (a)
      delete [] a;
    a = nullptr;
  }

  template <class T>
  inline void safeRenew(T &a, T obj) {
    if (a)
      delete a;
    a = obj;
  }

  extern bool base64_decode(std::string, uint8_t *&, unsigned long &);

}

#endif // __UTIL_H_
