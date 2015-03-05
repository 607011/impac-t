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

namespace Impact {

  Settings gSettings;


  Settings::Settings(void)
    : useShaders(ENABLE_SHADERS)
    , particlesPerExplosion(50)
  {
    load();
  }


  void Settings::save(void)
  {
    // TODO ...
  }


  LONG GetDWORDRegKey(HKEY hKey, const std::wstring &strValueName, DWORD &nValue, DWORD nDefaultValue)
  {
    nValue = nDefaultValue;
    DWORD dwBufferSize(sizeof(DWORD));
    DWORD nResult(0);
    LONG nError = ::RegQueryValueExW(hKey,
      strValueName.c_str(),
      0,
      NULL,
      reinterpret_cast<LPBYTE>(&nResult),
      &dwBufferSize);
    if (nError == ERROR_SUCCESS) {
      nValue = nResult;
    }
    return nError;
  }


  LONG GetBoolRegKey(HKEY hKey, const std::wstring &strValueName, bool &bValue, bool bDefaultValue)
  {
    DWORD nDefValue((bDefaultValue) ? 1 : 0);
    DWORD nResult(nDefValue);
    LONG nError = GetDWORDRegKey(hKey, strValueName.c_str(), nResult, nDefValue);
    if (nError == ERROR_SUCCESS) {
      bValue = (nResult != 0) ? true : false;
    }
    return nError;
  }


  LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
  {
    strValue = strDefaultValue;
    WCHAR szBuffer[512];
    DWORD dwBufferSize = sizeof(szBuffer);
    ULONG nError;
    nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    if (nError == ERROR_SUCCESS) {
      strValue = szBuffer;
    }
    return nError;
  }

  void Settings::load(void)
  {
    // TODO ...
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Impact", 0, KEY_READ, &hKey);
#ifndef NDEBUG
    std::cout << "RegOpenKeyExW('HKLM\\SOFTWARE\\Impact') -> " << lRes << std::endl;
#endif
    if (lRes == ERROR_SUCCESS) {
      bool useShaders;
      GetBoolRegKey(hKey, L"useShaders", useShaders, true);
#ifndef NDEBUG
      std::cout << "HKLM\\SOFTWARE\\Impact\\useShaders -> " << useShaders << std::endl;
#endif
    }
  }


}
