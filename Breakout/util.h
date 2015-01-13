// Copyright (c) 2014 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __UTIL_H_
#define __UTIL_H_

#include <string>
#include <cstdint>

#define UNUSED(x) (void)(x)


namespace Breakout {

  static const float _PI = 3.14159265358979f;
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
