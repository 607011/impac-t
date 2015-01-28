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

namespace Breakout {

  // from http://www.adp-gmbh.ch/cpp/common/base64.html

  static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  static inline bool is_base64(unsigned char c) {
    bool yes = (isalnum(c) || (c == '+') || (c == '/'));
    return yes;
  }

  // trim from end
  static inline std::string &rtrim(std::string &s) {
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end() );
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end() );
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end() );
    return s;
  }


  bool base64_decode(std::string base64, uint8_t *&buf, unsigned long &sz) {
    std::string encodedString = rtrim(base64);
    std::size_t inLen = encodedString.size();
    int i = 0;
    int j = 0;
    int idx = 0;
    unsigned char char_array_4[4];
    unsigned char char_array_3[3];
    std::vector<uint8> ret;
    while (inLen-- && (encodedString[idx] != '=') && is_base64(encodedString[idx])) {
      char_array_4[i++] = encodedString[idx++];
      if (i == 4) {
        for (i = 0; i < 4; ++i)
          char_array_4[i] = base64_chars.find(char_array_4[i]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (i = 0; (i < 3); ++i)
          ret.push_back(char_array_3[i]);
        i = 0;
      }
    }
    if (i) {
      for (j = i; j < 4; ++j)
        char_array_4[j] = 0;
      for (j = 0; j  <4; ++j)
        char_array_4[j] = base64_chars.find(char_array_4[j]);
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
      for (j = 0; (j < i - 1); ++j)
        ret.push_back(char_array_3[j]);
    }
    sz = ret.size();
    if (sz == 0) {
      buf = nullptr;
      return false;
    }
    buf = new uint8_t[sz];
    for (std::size_t i = 0; i < sz; ++i)
      buf[i] = ret.at(i);
    return true;
  }


}
